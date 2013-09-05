//
//  CursorTextField.cpp
//  CursorInputDemo
//
//  Created by Tom on 12-5-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include "CursorTextField.h"

const static float DELTA = 0.5f;

CursorTextField::CursorTextField()
{
    CCTextFieldTTF();
    
    m_pCursorSprite = NULL;
    m_pCursorAction = NULL;
    
    m_pInputText = NULL;
}

CursorTextField::~CursorTextField()
{
    delete m_pInputText;
}

void CursorTextField::onEnter()
{
    CCTextFieldTTF::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -128, false);
    this->setDelegate(this);
	if (m_pCursorSprite)
		m_pCursorSprite->setVisible(false);
}

CursorTextField * CursorTextField::textFieldWithPlaceHolder(const char *placeholder, const char *fontName, float fontSize)
{
    CursorTextField *pRet = new CursorTextField();
    
    if(pRet && pRet->initWithString("", fontName, fontSize))
    {
        pRet->autorelease();
        if (placeholder)
        {
            pRet->setPlaceHolder(placeholder);
        }
        pRet->initCursorSprite(fontSize);
        
        return pRet;
    }
    
    CC_SAFE_DELETE(pRet);
    
    return NULL;
}

void CursorTextField::initCursorSprite(int nHeight)
{
    // 初始化光标
    int column = 4;
 //   int pixels[nHeight][column];
	std::vector<int> pixels;
	pixels.resize(nHeight * column);
    for (int i=0; i<nHeight; ++i) {
        for (int j=0; j<column; ++j) {
             pixels[i * column + j] = 0xffffffff;
        }
    }

    CCTexture2D *texture = new CCTexture2D();
    texture->initWithData(&pixels[0], kCCTexture2DPixelFormat_RGB888, 1, 1, CCSizeMake(column, nHeight));
    
    m_pCursorSprite = CCSprite::spriteWithTexture(texture);
    CCSize winSize = getContentSize();
    m_cursorPos = ccp(0, winSize.height / 2);
    m_pCursorSprite->setPosition(m_cursorPos);
    this->addChild(m_pCursorSprite);
    
    m_pCursorAction = CCRepeatForever::actionWithAction((CCActionInterval *) CCSequence::actions(CCFadeOut::actionWithDuration(0.25f), CCFadeIn::actionWithDuration(0.25f), NULL));
    
    m_pCursorSprite->runAction(m_pCursorAction);
    
    m_pInputText = new std::string();
}

bool CursorTextField::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{    
    m_beginPos = pTouch->locationInView();
    m_beginPos = CCDirector::sharedDirector()->convertToGL(m_beginPos);
    
    return true;
}

CCRect CursorTextField::getRect()
{
    CCSize size = getContentSize();
    
    return  CCRectMake(-size.width / 2, -size.height / 2, size.width, size.height);
}

bool CursorTextField::isInTextField(cocos2d::CCTouch *pTouch)
{
    return CCRect::CCRectContainsPoint(getRect(), convertTouchToNodeSpaceAR(pTouch));
}

void CursorTextField::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
    CCPoint endPos = pTouch->locationInView();
    endPos = CCDirector::sharedDirector()->convertToGL(endPos);
    
    // 判断是否为点击事件
    if (::abs(endPos.x - m_beginPos.x) > DELTA || 
        ::abs(endPos.y - m_beginPos.y)) 
    {
        // 不是点击事件
        m_beginPos.x = m_beginPos.y = -1;
        
        return;
    }
    
    CCLOG("width: %f, height: %f.", getContentSize().width, getContentSize().height);
    
    // 判断是打开输入法还是关闭输入法
    isInTextField(pTouch) ? openIME() : closeIME();
}

bool CursorTextField::onTextFieldAttachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    if (m_pInputText->empty()) {
        return false;
    }
	CCSize winSize = getContentSize();
    m_cursorPos = ccp(getContentSize().width, winSize.height / 2);
    m_pCursorSprite->setPosition(m_cursorPos);
 //   m_pCursorSprite->setPositionX(getContentSize().width);
    
    return false;
}

bool CursorTextField::onTextFieldInsertText(cocos2d::CCTextFieldTTF *pSender, const char *text, int nLen)
{
    CCLOG("Width: %f", pSender->getContentSize().width);
    CCLOG("Text: %s", text);
    CCLOG("Length: %d", nLen);
	if (nLen && *text == '\n')
	{
		return true;
	}
    m_pInputText->append(text);
    setString(m_pInputText->c_str());
    
   // m_pCursorSprite->setPositionX(getContentSize().width);
    CCSize winSize = getContentSize();
    m_cursorPos = ccp(getContentSize().width, winSize.height / 2);
    m_pCursorSprite->setPosition(m_cursorPos);
    return true;
}
void CursorTextField::setContent(const char *content)
{
	*m_pInputText = content;
	setString(content);
	 CCSize winSize = getContentSize();
    m_cursorPos = ccp(getContentSize().width, winSize.height / 2);
    m_pCursorSprite->setPosition(m_cursorPos);
}
bool CursorTextField::onTextFieldDeleteBackward(cocos2d::CCTextFieldTTF *pSender, const char *delText, int nLen)
{
    m_pInputText->resize(m_pInputText->size() - nLen);
    setString(m_pInputText->c_str());
    
    //m_pCursorSprite->setPositionX(getContentSize().width);
    CCSize winSize = getContentSize();
    m_cursorPos = ccp(getContentSize().width, winSize.height / 2);
    m_pCursorSprite->setPosition(m_cursorPos);
    if (m_pInputText->empty()) {
        m_pCursorSprite->setPositionX(0);
    }
    
    return false;
}

bool CursorTextField::onTextFieldDetachWithIME(cocos2d::CCTextFieldTTF *pSender)
{
    return false;
}

void CursorTextField::openIME()
{
    m_pCursorSprite->setVisible(true);
    this->attachWithIME();
}

void CursorTextField::closeIME()
{
    m_pCursorSprite->setVisible(false);
    this->detachWithIME();
}

void CursorTextField::onExit()
{
    CCTextFieldTTF::onExit();
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}