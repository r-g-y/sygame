#include "MapBackGroud.h"
NS_CC_BEGIN

/**
 * 创建一个地表
 */
MapBackGroud* MapBackGroud::create(const char *fileName,int width,int height,int eachWidth,int eachHeight)
{
	MapBackGroud *mapb = new MapBackGroud();
	mapb->_width = width;
	mapb->_height = height;
	mapb->_eachWidth = eachWidth;
	mapb->_eachHeight = eachHeight;
    if (mapb && mapb->initWithFile(fileName))
    {
        mapb->autorelease();
        return mapb;
    }
    CC_SAFE_DELETE(mapb);
    return NULL;
}

/**
 * 使用纹理初始化
 */
bool MapBackGroud::initWithTexture(CCTexture2D *pTexture, const CCRect& rect, bool rotated)
{
	 m_pobBatchNode = NULL;
    // shader program
    setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));

    m_bRecursiveDirty = false;
    setDirty(false);

    m_bOpacityModifyRGB = true;
    m_nOpacity = 255;
    m_sColor = m_sColorUnmodified = ccWHITE;

    m_sBlendFunc.src = CC_BLEND_SRC;
    m_sBlendFunc.dst = CC_BLEND_DST;

    m_bFlipX = m_bFlipY = false;

    // default transform anchor: center
    setAnchorPoint(ccp(0.5f, 0.5f));

    // zwoptex default values
    m_obOffsetPosition = CCPointZero;

    m_bHasChildren = false;
    
    // clean the Quad
    memset(&m_sQuad, 0, sizeof(m_sQuad));

    // update texture (calls updateBlendFunc)
    setTexture(pTexture);
    setTextureRect(rect, rotated, rect.size);

    // by default use "Self Render".
    // if the sprite is added to a batchnode, then it will automatically switch to "batchnode Render"
    setBatchNode(NULL);

    return true;
}
/**
 * 设置数据
 */
 void MapBackGroud::setTextureRect(const CCRect& rect, bool rotated, const CCSize& untrimmedSize)
 {
	m_bRectRotated = rotated;

    setContentSize(untrimmedSize);
    setVertexRect(rect);
    setTextureCoords(rect);  // 设置纹理坐标

    CCPoint relativeOffset = m_obUnflippedOffsetPositionFromCenter;

    // issue #732
    if (m_bFlipX)
    {
        relativeOffset.x = -relativeOffset.x;
    }
    if (m_bFlipY)
    {
        relativeOffset.y = -relativeOffset.y;
    }

    m_obOffsetPosition.x = relativeOffset.x + (m_tContentSize.width - m_obRect.size.width) / 2;
    m_obOffsetPosition.y = relativeOffset.y + (m_tContentSize.height - m_obRect.size.height) / 2;

    // rendering using batch node
    if (m_pobBatchNode)
    {
        // update dirty_, don't update recursiveDirty_
        setDirty(true);
    }
    else
    {
        // self rendering
        
        // Atlas: Vertex
        float x = 0 + m_obOffsetPosition.x;
        float y = 0 + m_obOffsetPosition.y;
		// Atlas: Color
		ccColor4B tmpColor = { 255, 255, 255, 255 };
	
		// 设置顶点坐标
        // Don't update Z.
  
		int eachWidth = _eachWidth;
		int eachHeight = _eachHeight;
		if (eachWidth > _width) eachWidth = _width;
		if (eachHeight > _height) eachHeight = _height;

		std::vector<ccV3F_C4B_T2F> tpoints;
		int widthSize =	_width / eachWidth ;
		int heightSize = _height/ eachHeight;
		for (int i = 0; i < widthSize;i++)
			for (int j = 0; j < heightSize;j++)
			{
				ccV3F_C4B_T2F point;
				point.vertices = vertex3(i * eachWidth + x,j * eachHeight + y,0);
				tpoints.push_back(point);
			} // 生成顶点集合
		for (int i = 0; i < widthSize;i++)
			for (int j = 0; j < heightSize;j++)
			{
				ccVertex3F point = tpoints[i* widthSize + j].vertices; // 生成顶点编号
				static ccV3F_C4B_T2F pointList[4];
				pointList[0].vertices = vertex3(point.x,point.y,point.z);
				pointList[2].vertices = vertex3(point.x + eachWidth,point.y,point.z);
				pointList[1].vertices= vertex3(point.x ,point.y + eachHeight,point.z);
				pointList[3].vertices= vertex3(point.x + eachWidth ,point.y + eachHeight,point.z);
				// 确定纹理映射
				static int indexs[] = {0,1,2,1,2,3}; // 固态的指定渲染的引索号 
				static float textures[] = {0,1,0,0,1,1,1,0}; // 纹理映射点
				for (int z = 0; z < 6 ;z++)
				{
					ccV3F_C4B_T2F p;
					p.vertices = pointList[indexs[z]].vertices;
					p.colors = tmpColor;
					p.texCoords.u = textures[indexs[z]*2];
					p.texCoords.v = textures[indexs[z]*2+1];
					points.push_back(p);
				}
				//增加到plist中
			} // 生成三角形集合
		}
 }
 void MapBackGroud::updateTransform(void)
 {
	for (int i = 0; i < points.size();++i)
	{
		float x1 = m_obOffsetPosition.x;
        float y1 = m_obOffsetPosition.y;
        float x = m_transformToBatch.tx;
        float y = m_transformToBatch.ty;
		points[i].vertices = vertex3(x1 + x,y1 +y , 0);
	}
 }
/**
 * 绘制
 */
 void MapBackGroud::draw(void)
 {
	  CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");

    CCAssert(!m_pobBatchNode, "If CCSprite is being rendered by CCSpriteBatchNode, CCSprite#draw SHOULD NOT be called");

    CC_NODE_DRAW_SETUP();

    ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );

    if (m_pobTexture != NULL)
    {
        ccGLBindTexture2D( m_pobTexture->getName() );
    }
    else
    {
        ccGLBindTexture2D(0);
    }
    
    //
    // Attributes
    //

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );

#define kQuadSize sizeof(m_sQuad.bl)
	if (points.size())
	{
		long offset = (long)&points[0];

		// vertex
		int diff = offsetof( ccV3F_C4B_T2F, vertices);
		glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (void*) (offset + diff));

		// texCoods
		diff = offsetof( ccV3F_C4B_T2F, texCoords);
		glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (void*)(offset + diff));

		// color
		diff = offsetof( ccV3F_C4B_T2F, colors);
		glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (void*)(offset + diff));


		glDrawArrays(GL_TRIANGLES, 0, points.size());
	}
    CHECK_GL_ERROR_DEBUG();

    CC_INCREMENT_GL_DRAWS(1);

    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "CCSprite - draw");
 }
 bool MapBackGroud::checkIn(float x,float y)
 {
	int width = (_width - 1 ) ;
	int height = (_height - 1) ;
	CCRect rect = CCRectMake(
			 m_obOffsetPosition.x,
			 m_obOffsetPosition.y,
			width,
			height);
	if (rect.containsPoint(ccp(x,y)))
	{
		return true;
	}
	return false;
 }
 NS_CC_END