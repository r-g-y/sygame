#include "UITextArea.h"
#include "cocoa/CCString.h"
#include "platform/platform.h"
#include "cocoa/CCDictionary.h"
#include "CCConfiguration.h"
#include "CCDrawingPrimitives.h"
#include "sprite_nodes/CCSprite.h"
#include "support/CCPointExtension.h"
#include "platform/CCFileUtils.h"
#include "support/data_support/uthash.h"
#include "CCDirector.h"
#include "textures/CCTextureCache.h"
NS_CC_BEGIN
static int cc_wcslen(const unsigned short* str)
{
    int i=0;
    while(*str++) i++;
    return i;
}

typedef struct _FontDefHashElement
{
    unsigned int    key;        // key. Font Unicode value
    ccBMFontDef        fontDef;    // font definition
    UT_hash_handle    hh;
} tFontDefHashElement;
/**
 * 创建UITextArea
 */
UITextArea * UITextArea::create(const char *str, const char *fntFile)
{
	UITextArea *pRet = new UITextArea();
    if(pRet && pRet->initWithString(str, fntFile, kCCLabelAutomaticWidth, kCCTextAlignmentLeft, CCPointZero))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

void UITextArea::createFontChars()
{
    int nextFontPositionX = 0;
    int nextFontPositionY = 0;
    //unsigned short prev = -1;
    int kerningAmount = 0;

    CCSize tmpSize = CCSizeZero;

    int longestLine = 0;
    unsigned int totalHeight = 0;

    unsigned int quantityOfLines = 1;
	if (!m_sString) return;
    unsigned int stringLen = cc_wcslen(m_sString);
    if (stringLen == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < stringLen - 1; ++i)
    {
        unsigned short c = m_sString[i];
        if (c == '\n')
        {
            quantityOfLines++;
        }
    }

    totalHeight = m_pConfiguration->m_nCommonHeight * quantityOfLines;
    nextFontPositionY = 0-(m_pConfiguration->m_nCommonHeight - m_pConfiguration->m_nCommonHeight * quantityOfLines);

    for (unsigned int i= 0; i < stringLen; i++)
    {
        unsigned short c = m_sString[i];

        if (c == '\n')
        { 
            nextFontPositionX = 0;
            nextFontPositionY -= m_pConfiguration->m_nCommonHeight;
            continue;
        }

        tFontDefHashElement *element = NULL;

        // unichar is a short, and an int is needed on HASH_FIND_INT
        unsigned int key = c;
        HASH_FIND_INT(m_pConfiguration->m_pFontDefDictionary, &key, element);
        CCAssert(element, "FontDefinition could not be found!");

        ccBMFontDef fontDef = element->fontDef;

        CCRect rect = fontDef.rect;
        rect = CC_RECT_PIXELS_TO_POINTS(rect);

        rect.origin.x += m_tImageOffset.x;
        rect.origin.y += m_tImageOffset.y;

        CCSprite *fontChar;

        fontChar = (CCSprite*)(this->getChildByTag(i));
        if( ! fontChar )
        {
            fontChar = new CCSprite();
            fontChar->initWithTexture(m_pobTextureAtlas->getTexture(), rect);
            this->addChild(fontChar, 0, i);
            fontChar->release();
        }
        else
        {
            // reusing fonts
            fontChar->setTextureRect(rect, false, rect.size);

            // restore to default in case they were modified
            fontChar->setVisible(true);
            fontChar->setOpacity(255);
        }

        // See issue 1343. cast( signed short + unsigned integer ) == unsigned integer (sign is lost!)
        int yOffset = m_pConfiguration->m_nCommonHeight - fontDef.yOffset;
        CCPoint fontPos = ccp( (float)nextFontPositionX + fontDef.xOffset + fontDef.rect.size.width*0.5f + kerningAmount,
            (float)nextFontPositionY + yOffset - rect.size.height*0.5f * CC_CONTENT_SCALE_FACTOR() );
        fontChar->setPosition(CC_POINT_PIXELS_TO_POINTS(fontPos));

        // update kerning
        nextFontPositionX += fontDef.xAdvance + kerningAmount;
        //prev = c;

        // Apply label properties
        fontChar->setOpacityModifyRGB(m_bIsOpacityModifyRGB);
        // Color MUST be set before opacity, since opacity might change color if OpacityModifyRGB is on
		if ( i < colors.size())
		{
			fontChar->setColor(colors.at(i));
		}
		else
			fontChar->setColor(m_tColor);

        // only apply opacity if it is different than 255 )
        // to prevent modifying the color too (issue #610)
        if( m_cOpacity != 255 )
        {
            fontChar->setOpacity(m_cOpacity);
        }

        if (longestLine < nextFontPositionX)
        {
            longestLine = nextFontPositionX;
        }
    }

    tmpSize.width  = (float) longestLine;
    tmpSize.height = (float) totalHeight;

    this->setContentSize(CC_SIZE_PIXELS_TO_POINTS(tmpSize));
    
}
/**
 * 设置字体颜色
 */
void UITextArea::setColors(int startId,int endId,const ccColor3B& color)
{
	for (int index = startId; index <= endId;index++)
	{
		if (index >= colors.size())
		{
			colors.resize(index + 1);
		}
		colors[index] = color;
	}
}


UIMultiLineArea *UIMultiLineArea::create(const char *str,float width,float fontSize,const ccColor3B& color)
{
	UIMultiLineArea *pRet = new UIMultiLineArea();
    if(pRet && pRet->init(str,width,fontSize,color))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool UIMultiLineArea::init(const char *str,float width,float fontSize,const ccColor3B& color)
{
	if (width == 0) width = 10;
	this->width = width;
	this->fontSize = fontSize;
	this->color = color;
	int len = strlen(str);
	int line = (len / width) ;
	char *buffer = new char[(int)width+1];
	int nowLen = 0;
	int nowY = (line+(len % (int)width)?1:0) * fontSize;
	while (line)
	{
		memset(buffer,0,width+1);
		strncpy(buffer,str+nowLen,width);
		CCLabelTTF * font = CCLabelTTF::create(buffer,"",fontSize);
		this->addChild(font);
		font->setAnchorPoint(ccp(0,0));
		font->setColor(color);
		font->setPositionY(nowY);
		nowY -= fontSize;
		nowLen += width;
		line--;
	}
	if (len % (int)width)
	{
		memset(buffer,0,(int)width+1);
		strncpy(buffer,str+nowLen,len % (int)width);
		CCLabelTTF * font = CCLabelTTF::create(buffer,"",fontSize);
		font->setAnchorPoint(ccp(0,0));
		this->addChild(font);
		font->setPositionY(nowY);
		font->setColor(color);
	}
	return true;
}
void UIMultiLineArea::update(const std::string &content)
{
	this->removeAllChildrenWithCleanup(true);
	const char *str = content.c_str();
	int len = strlen(str);
	int line = (len / width) ;
	char *buffer = new char[(int)width+1];
	int nowLen = 0;
	int nowY = (line+(len % (int)width)?1:0) * fontSize;
	while (line)
	{
		memset(buffer,0,width+1);
		strncpy(buffer,str+nowLen,width);
		CCLabelTTF * font = CCLabelTTF::create(buffer,"",fontSize);
		this->addChild(font);
		font->setAnchorPoint(ccp(0,0));
		font->setColor(color);
		font->setPositionY(nowY);
		nowY -= fontSize;
		nowLen += width;
		line--;
	}
	if (len % (int)width)
	{
		memset(buffer,0,(int)width+1);
		strncpy(buffer,str+nowLen,len % (int)width);
		CCLabelTTF * font = CCLabelTTF::create(buffer,"",fontSize);
		font->setAnchorPoint(ccp(0,0));
		this->addChild(font);
		font->setPositionY(nowY);
		font->setColor(color);
	}
	return ;
}
NS_CC_END