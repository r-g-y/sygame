#include "UIRichTextArea.h"
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
/* Code from GLIB gutf8.c starts here. */

#define UTF8_COMPUTE(Char, Mask, Len)        \
  if (Char < 128)                \
    {                        \
      Len = 1;                    \
      Mask = 0x7f;                \
    }                        \
  else if ((Char & 0xe0) == 0xc0)        \
    {                        \
      Len = 2;                    \
      Mask = 0x1f;                \
    }                        \
  else if ((Char & 0xf0) == 0xe0)        \
    {                        \
      Len = 3;                    \
      Mask = 0x0f;                \
    }                        \
  else if ((Char & 0xf8) == 0xf0)        \
    {                        \
      Len = 4;                    \
      Mask = 0x07;                \
    }                        \
  else if ((Char & 0xfc) == 0xf8)        \
    {                        \
      Len = 5;                    \
      Mask = 0x03;                \
    }                        \
  else if ((Char & 0xfe) == 0xfc)        \
    {                        \
      Len = 6;                    \
      Mask = 0x01;                \
    }                        \
  else                        \
    Len = -1;

#define UTF8_LENGTH(Char)            \
  ((Char) < 0x80 ? 1 :                \
   ((Char) < 0x800 ? 2 :            \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :            \
      ((Char) < 0x4000000 ? 5 : 6)))))


#define UTF8_GET(Result, Chars, Count, Mask, Len)    \
  (Result) = (Chars)[0] & (Mask);            \
  for ((Count) = 1; (Count) < (Len); ++(Count))        \
    {                            \
      if (((Chars)[(Count)] & 0xc0) != 0x80)        \
    {                        \
      (Result) = -1;                \
      break;                    \
    }                        \
      (Result) <<= 6;                    \
      (Result) |= ((Chars)[(Count)] & 0x3f);        \
    }

#define UNICODE_VALID(Char)            \
  ((Char) < 0x110000 &&                \
   (((Char) & 0xFFFFF800) != 0xD800) &&        \
   ((Char) < 0xFDD0 || (Char) > 0xFDEF) &&    \
   ((Char) & 0xFFFE) != 0xFFFE)


static const char utf8_skip_data[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5,
  5, 5, 5, 6, 6, 1, 1
};

static const char *const g_utf8_skip = utf8_skip_data;

#define cc_utf8_next_char(p) (char *)((p) + g_utf8_skip[*(unsigned char *)(p)])

/*
 * @str:    the string to search through.
 * @c:        the character to find.
 * 
 * Returns the index of the first occurrence of the character, if found.  Otherwise -1 is returned.
 * 
 * Return value: the index of the first occurrence of the character if found or -1 otherwise.
 * */
static unsigned int cc_utf8_find_char(std::vector<unsigned short> str, unsigned short c)
{
    unsigned int len = str.size();

    for (unsigned int i = 0; i < len; ++i)
        if (str[i] == c) return i;

    return -1;
}

/*
 * @str:    the string to search through.
 * @c:        the character to not look for.
 * 
 * Return value: the index of the last character that is not c.
 * */
static unsigned int cc_utf8_find_last_not_char(std::vector<unsigned short> str, unsigned short c)
{
    int len = str.size();

    int i = len - 1;
    for (; i >= 0; --i)
        if (str[i] != c) return i;

    return i;
}

/*
 * @str:    the string to trim
 * @index:    the index to start trimming from.
 * 
 * Trims str st str=[0, index) after the operation.
 * 
 * Return value: the trimmed string.
 * */
static void cc_utf8_trim_from(std::vector<unsigned short>* str, int index)
{
    int size = str->size();
    if (index >= size || index < 0)
        return;

    str->erase(str->begin() + index, str->begin() + size);
}

/*
 * @ch is the unicode character whitespace?
 * 
 * Reference: http://en.wikipedia.org/wiki/Whitespace_character#Unicode
 * 
 * Return value: weather the character is a whitespace character.
 * */
static bool isspace_unicode(unsigned short ch)
{
    return  (ch >= 0x0009 && ch <= 0x000D) || ch == 0x0020 || ch == 0x0085 || ch == 0x00A0 || ch == 0x1680
        || (ch >= 0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029 || ch == 0x202F
        ||  ch == 0x205F || ch == 0x3000;
}

static void cc_utf8_trim_ws(std::vector<unsigned short>* str)
{
    int len = str->size();

    if ( len <= 0 )
        return;

    int last_index = len - 1;

    // Only start trimming if the last character is whitespace..
    if (isspace_unicode((*str)[last_index]))
    {
        for (int i = last_index - 1; i >= 0; --i)
        {
            if (isspace_unicode((*str)[i]))
                last_index = i;
            else
                break;
        }

        cc_utf8_trim_from(str, last_index);
    }
}

/*
 * g_utf8_strlen:
 * @p: pointer to the start of a UTF-8 encoded string.
 * @max: the maximum number of bytes to examine. If @max
 *       is less than 0, then the string is assumed to be
 *       null-terminated. If @max is 0, @p will not be examined and
 *       may be %NULL.
 *
 * Returns the length of the string in characters.
 *
 * Return value: the length of the string in characters
 **/
static long
cc_utf8_strlen (const char * p, int max)
{
  long len = 0;
  const char *start = p;

  if (!(p != NULL || max == 0))
  {
      return 0;
  }

  if (max < 0)
    {
      while (*p)
    {
      p = cc_utf8_next_char (p);
      ++len;
    }
    }
  else
    {
      if (max == 0 || !*p)
    return 0;

      p = cc_utf8_next_char (p);

      while (p - start < max && *p)
    {
      ++len;
      p = cc_utf8_next_char (p);
    }

      /* only do the last len increment if we got a complete
       * char (don't count partial chars)
       */
      if (p - start == max)
    ++len;
    }

  return len;
}

/*
 * g_utf8_get_char:
 * @p: a pointer to Unicode character encoded as UTF-8
 *
 * Converts a sequence of bytes encoded as UTF-8 to a Unicode character.
 * If @p does not point to a valid UTF-8 encoded character, results are
 * undefined. If you are not sure that the bytes are complete
 * valid Unicode characters, you should use g_utf8_get_char_validated()
 * instead.
 *
 * Return value: the resulting character
 **/
static unsigned int
cc_utf8_get_char (const char * p)
{
  int i, mask = 0, len;
  unsigned int result;
  unsigned char c = (unsigned char) *p;

  UTF8_COMPUTE (c, mask, len);
  if (len == -1)
    return (unsigned int) - 1;
  UTF8_GET (result, p, i, mask, len);

  return result;
}

/*
 * cc_utf16_from_utf8:
 * @str_old: pointer to the start of a C string.
 * 
 * Creates a utf8 string from a cstring.
 * 
 * Return value: the newly created utf8 string.
 * */
static unsigned short* cc_utf16_from_utf8(const char* str_old)
{
    int len = cc_utf8_strlen(str_old, -1);

    unsigned short* str_new = new unsigned short[len + 1];
    str_new[len] = 0;

    for (int i = 0; i < len; ++i)
    {
        str_new[i] = cc_utf8_get_char(str_old);
        str_old = cc_utf8_next_char(str_old);
    }

    return str_new;
}

static std::vector<unsigned short> cc_utf16_vec_from_utf16_str(const unsigned short* str)
{
    int len = cc_wcslen(str);
    std::vector<unsigned short> str_new;

    for (int i = 0; i < len; ++i)
    {
        str_new.push_back(str[i]);
    }
    return str_new;
}
/**
 * 创建富信息文本域
 **/
UIRichTextArea* UIRichTextArea::createFromFile(CCNode *parent,const char *fileName)
{
	UIRichTextArea *pRet = new UIRichTextArea();
    if(pRet)
    {
		pRet->node = parent;
		if (pRet->initWithXml(fileName))
		{
			pRet->autorelease();
			return pRet;
		}
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
UIRichTextArea* UIRichTextArea::createFromString(CCNode *parent,const char *content)
{
	UIRichTextArea *pRet = new UIRichTextArea();
    if(pRet)
    {
		pRet->node = parent;
		if (pRet->initFromString(content))
		{
			pRet->autorelease();
			return pRet;
		}
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool UIRichTextArea::initFromString(const char *content)
{
	if (script::tixmlCode::initFromString(content))
	{
		setContentSize(CCSizeMake(width,height));
		if (fntFileName == "") return false;
		initWithString("", fntFileName.c_str(), kCCLabelAutomaticWidth, kCCTextAlignmentLeft, CCPointZero);
		return true;
	}
	return false;
}
bool UIRichTextArea::initWithXml(const char *fileName)
{
	if (script::tixmlCode::init(fileName))
	{
		setContentSize(CCSizeMake(width,height));
		if (fntFileName == "") return false;
		initWithString("", fntFileName.c_str(), kCCLabelAutomaticWidth, kCCTextAlignmentLeft, CCPointZero);
		return true;
	}
	return false;
}
void UIRichTextArea::update(const std::string& name,const std::string& content)
{
	for (CONTENTS_ITER iter = _contents.begin(); iter != _contents.end();++iter)
	{
		if (iter->name == name)
		{
			iter->content = content;
		}
	}
	 updateString(true);
}
std::string UIRichTextArea::getValueByName(const std::string& name)
{
	for (CONTENTS_ITER iter = _contents.begin(); iter != _contents.end();++iter)
	{
		if (iter->name == name)
		{
			return iter->content;
		}
	}
	return "";
}
void UIRichTextArea::createFontChars()
{	
	for (CONTENTS_ITER iter = _contents.begin(); iter != _contents.end(); ++iter)
	{
		stUIRichString & content = *iter;
		if (content.stringType == stUIRichString::UI_STRING_TYPE_FONT)
		{
			createFonts(cc_utf16_from_utf8(content.content.c_str()),content.x,content.y,content.color,content.fontSize);
		}
		else if (content.stringType == stUIRichString::UI_STRING_TYPE_IMG)
		{
			createImage(content.content.c_str(),content.x,content.y,content.width,content.height);
		}
	}
}

void UIRichTextArea::createImage(const char * pngName,float x,float y,float w,float h)
{
	CCSprite * image = CCSprite::create(pngName);
	if (image)
	{
		image->setPosition(ccp(x,y));
		image->setScaleX(w / image->getContentSize().width);
		image->setScaleY(h / image->getContentSize().height);
		if (this->node)
			this->node->addChild(image);
		 if( m_cOpacity != 255 )
        {
            image->setOpacity(m_cOpacity);
        }
	}
}

void UIRichTextArea::createFonts(const unsigned short * m_sString,int nextFontPositionX,int nextFontPositionY,const ccColor3B& m_tColor,int fontSize)
{
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
 //   nextFontPositionY = 0-(m_pConfiguration->m_nCommonHeight - m_pConfiguration->m_nCommonHeight * quantityOfLines);

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

        CCSprite *fontChar = NULL;

       // fontChar = (CCSprite*)(this->getChildByTag(i));
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
        nextFontPositionX +=/* fontDef.xAdvance + kerningAmount +*/ fontSize;
        //prev = c;

        // Apply label properties
        fontChar->setOpacityModifyRGB(m_bIsOpacityModifyRGB);
        // Color MUST be set before opacity, since opacity might change color if OpacityModifyRGB is on
	
		fontChar->setColor(m_tColor);
		if (fontSize)
		{
			fontChar->setScaleX(fontSize / fontChar->getContentSize().width);
			fontChar->setScaleY(fontSize / fontChar->getContentSize().height);
		}
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

 //   tmpSize.width  = (float) longestLine;
 //  tmpSize.height = (float) totalHeight;

 //  this->setContentSize(CC_SIZE_PIXELS_TO_POINTS(tmpSize));
}
/**
 * 从配置文件中加载配置
 * \param node 配置根节点
 */
void UIRichTextArea::takeNode(script::tixmlCodeNode *node)
{
	if (node && node->equal("richtext"))
	{
		node->getAttr("width",width);
		node->getAttr("height",height);
		fntFileName = node->getAttr("fnt");
		if (fntFileName=="")
		{
			fntFileName="sys.fnt";
		}
		stUIRichString content;
		script::tixmlCodeNode fontNode = node->getFirstChildNode("font");
		while (fontNode.isValid())
		{
			content.name = fontNode.getAttr("name");
			content.content = fontNode.getAttr("content");
			fontNode.getAttr("x",content.x);
			fontNode.getAttr("y",content.y);
			fontNode.getAttr("width",content.width);
			fontNode.getAttr("height",content.height);
			int r = 0;
			int b = 0;
			int g = 0;
			fontNode.getAttr("r",r);
			fontNode.getAttr("g",g);
			fontNode.getAttr("b",b);
			content.color = ccc3(r,g,b);
			content.stringType = stUIRichString::UI_STRING_TYPE_FONT;
			fontNode.getAttr("fontsize",content.fontSize);
			fontNode = fontNode.getNextNode("font");
			_contents.push_back(content);
		}
		script::tixmlCodeNode imgNode = node->getFirstChildNode("img");
		while (imgNode.isValid())
		{
			content.content = imgNode.getAttr("content");
			imgNode.getAttr("x",content.x);
			imgNode.getAttr("y",content.y);
			imgNode.getAttr("width",content.width);
			imgNode.getAttr("height",content.height);
			content.stringType = stUIRichString::UI_STRING_TYPE_IMG;
			imgNode = imgNode.getNextNode("img");
			_contents.push_back(content);
		}
	}
}
UIRichArea * UIRichArea::create(const char *fileName)
{
	UIRichArea *pRet = new UIRichArea();
    if(pRet)
    {
		if (pRet->init(fileName))
		{
			pRet->autorelease();
			return pRet;
		}
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
UIRichArea * UIRichArea::createFromString(const char *content)
{
	UIRichArea *pRet = new UIRichArea();
    if(pRet)
    {
		if (pRet->initFromString(content))
		{
			pRet->autorelease();
			return pRet;
		}
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}
bool UIRichArea::init(const char *fileName)
{
	_textarea = UIRichTextArea::createFromFile(this,fileName);
	if (_textarea)
	{
		this->addChild(_textarea);
		return true;
	}
	return false;
}
bool UIRichArea::initFromString(const char *content)
{
	_textarea = UIRichTextArea::createFromString(this,content);
	if (_textarea)
	{
		this->addChild(_textarea);
		return true;
	}
	return false;
}
/**
* 设置位置
*/
void UIRichArea::setPosition(float x,float y)
{
	CCNode::setPosition(ccp(x,y));
}
	
/**
* 设置大小
*/
void UIRichArea::setSize(float w,float h)
{
	if (_textarea)
	{
		_textarea->setScaleX(w / _textarea->getContentSize().width);
		_textarea->setScaleX(w / _textarea->getContentSize().height);
	}
}
/** 
* 检查是否在区域里
*/
bool UIRichArea::touchDown(float x,float y)
{
	return false;
}
/**
* 更新位置
*/
bool UIRichArea::touchMove(float x,float y)
{
	return false;
}
/**
* 停止拖动
*/
bool UIRichArea::touchEnd(float x,float y)
{
	return false;
}
void UIRichArea::setContent(const std::string& content)
{
	if (_textarea)
	{
		_textarea->setString(content.c_str());
	}
}

void UIRichArea::update(const std::string& name,const std::string& content)
{
	if (_textarea)
	{
		_textarea->update(name,content);
	}
}
std::string UIRichArea::getValueByName(const std::string &name)
{
	if (_textarea)
	{
		_textarea->getValueByName(name);
	}
	return "";
}
NS_CC_END
