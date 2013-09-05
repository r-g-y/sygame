#include "LineBlockMap.h"

NS_CC_BEGIN
/**
 * 测试是否可以将物体限制在线段围成的区域里
 */
bool LineBlockMap::checkCollide(const CCRect& rect)
{
	CCPoint one = ccp(rect.origin.x,rect.origin.y);
	CCPoint two = ccp(rect.origin.x + rect.size.width,rect.origin.y);
	CCPoint three = ccp(rect.origin.x,rect.origin.y + rect.size.height);
	CCPoint four = ccp(rect.origin.x + rect.size.width,rect.origin.y + rect.size.height);
	std::vector<Line> locs;
	locs.resize(4);
	locs[0] = Line(one,two);
	locs[1] = Line(one,three);
	locs[2] = Line(three,four);
	locs[3] = Line(two,four);
	for (int i = 0; i < 4;i++)
	{
		Line &src = locs[i];
		for (std::vector<Line>::iterator iter = _lines.begin(); iter != _lines.end(); ++iter)
		{
			Line &dest = *iter;
			if (rect.containsPoint(dest.src) || rect.containsPoint(dest.dest))
			{
				return true;
			}
			if (LineCollide::checkCollide(src,dest))
			{
				return true;
			}
		}
	}
	return false;
}
bool LineBlockMap::loadFromXml(const char*fileName)
{
	return script::tixmlCode::init(fileName);
}
bool LineBlockMap::checkSpriteNextPointValid(CCSprite *nowNode,const CCPoint &dest)
{
	if (!nowNode) return false;
	CCPoint nowPoint = nowNode->getPosition();
	float dx = dest.x;
	float dy = dest.y;
	float oldx = nowPoint.x - (nowNode->getContentSize().width/2);
	float oldy = nowPoint.y - (nowNode->getContentSize().height/2);
	float oldmaxx = oldx +  nowNode->getContentSize().width;
	float oldmaxy = oldy + nowNode->getContentSize().height;
	float x = dx - (nowNode->getContentSize().width/2);
	float y = dy - (nowNode->getContentSize().height/2);
	float maxx = x + nowNode->getContentSize().width;
	float maxy = y + nowNode->getContentSize().height;
	x = oldx < x ? oldx : x;
	y = oldy < y ? oldy : y;
	maxx = oldmaxx > maxx ? oldmaxx:maxx;
	maxy = oldmaxy > maxy ? oldmaxy : maxy;
	float width = maxx - x ;
	float height = maxy - y;
	CCRect rect = CCRectMake(x ,y ,width,height);
	return checkCollide(rect);
}

void LineBlockMap::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("lineblock"))
	{
		script::tixmlCodeNode lineNode = node->getFirstChildNode("line");
		while (lineNode.isValid())
		{
			float sx  = 0;
			float sy = 0;
			float dx = 0;
			float dy = 0;
			lineNode.getAttr("sx",sx);
			lineNode.getAttr("sy",sy);
			lineNode.getAttr("dx",dx);
			lineNode.getAttr("dy",dy);
			Line line(sx,sy,dx,dy);
			_lines.push_back(line);
			lineNode = lineNode.getNextNode("line");
		}
	}
}


void LineBlockMap::draw()
{
			
	glLineWidth( 10);
	for (std::vector<Line>::iterator iter = _lines.begin(); iter != _lines.end();++iter)
	{
		ccDrawLine(iter->src,iter->dest);
	}		//画线段，也可以使用点
	glLineWidth(1);
}
NS_CC_END