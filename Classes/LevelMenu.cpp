#include "LevelMenu.h"
#include "ccb.h"
#include "baseRes.h"
#include "MainMenu.h"
#include "ChoseMenu.h"
#include "AppDelegate.h"
#include "Common.h"
#include "PayService.h"
#include "ShopMenu.h"

using namespace cocos2d;
using namespace cocos2d::extension;

#define NeedTime 100

ccbLevelMenu* ccbLevelMenu::s_pccbLevelMenu = NULL;
ccbLevelMenu::ccbLevelMenu()
	: m_AnimationManager(NULL)
	, m_iLevel(0)
{
	m_iChose	= -1;
}

ccbLevelMenu::~ccbLevelMenu()
{
	s_pccbLevelMenu = NULL;
	CC_SAFE_RELEASE_NULL(m_AnimationManager);
}

void ccbLevelMenu::onNodeLoaded(cocos2d::CCNode * pNode,  cocos2d::extension::CCNodeLoader * pNodeLoader) 
{
	s_pccbLevelMenu = this;
	for ( int i = 0; i < 6; i ++ )
	{
		m_pNode[i] = pNode->getChildByTag(1+i);
	}
}

SEL_MenuHandler ccbLevelMenu::onResolveCCBCCMenuItemSelector(CCObject * pTarget, const char * pSelectorName) 
{
	return NULL;    
}

SEL_CCControlHandler ccbLevelMenu::onResolveCCBCCControlSelector(CCObject * pTarget, const char * pSelectorName) 
{
	return NULL;
}


bool ccbLevelMenu::onAssignCCBMemberVariable(CCObject * pTarget, const char * pMemberVariableName, CCNode * pNode) 
{
	return false;
}

bool ccbLevelMenu::onAssignCCBCustomProperty(CCObject* pTarget, const char* pMemberVariableName, cocos2d::extension::CCBValue* pCCBValue)
{
	bool bRet = false;
	return bRet;
}

void ccbLevelMenu::setAnimationManager(CCBAnimationManager *pAnimationManager)
{
	CC_SAFE_RELEASE_NULL(m_AnimationManager);
	m_AnimationManager = pAnimationManager;
	CC_SAFE_RETAIN(m_AnimationManager);
}

void ccbLevelMenu::ccTouchesBegan( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent )
{
	if ( -1 == m_iChose && 2 == CMainMenu::GetStatus() && !CMainMenu::m_bShop && !CMainMenu::m_bOnSell )
	{
		CCSetIterator iter = pTouches->begin();
		CCPoint locationT = ((CCTouch*)(*iter))->getLocation();
		for ( int i = 0; i < 6; i++ )
		{
			CCPoint locationP = m_pNode[i]->getPosition();
			if ( abs(locationT.x - locationP.x*Scale_X) < d_fLevelPicW
				&& abs(locationT.y - locationP.y*Scale_Y) < d_fLevelPicH )
			{
				m_iChose = i;
				break;
			}
		}
	}
}

void ccbLevelMenu::ccTouchesMoved( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent )
{
}

void ccbLevelMenu::ccTouchesEnded( cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent )
{
	if ( -1 != m_iChose && 2 == CMainMenu::GetStatus() )
	{
		CCSetIterator iter = pTouches->begin();
		CCPoint locationT = ((CCTouch*)(*iter))->getLocation();
		for ( int i = 0; i < 6; i++ )
		{
			CCPoint locationP = m_pNode[i]->getPosition();
			if ( abs(locationT.x - locationP.x*Scale_X) < d_fLevelPicW
				&& abs(locationT.y - locationP.y*Scale_Y) < d_fLevelPicH )
			{
				if ( m_iChose == i )
				{
					if ( !AppDelegate::s_LevelOpen[i+m_iLevel*6] || ((ccbLayer*)CMainMenu::GetMenuMain())->m_bShare )
					{
						m_iChose = -1;
						return;
					}
					if ( AppDelegate::s_FirstLogin == 4 )
					{
						if ( m_iChose != 0 )
						{
							m_iChose = -1;
							return;
						}
						AppDelegate::s_FirstLogin = 5;
						AppDelegate::SaveGuide();
					}
#ifdef Plat_MM
					else if ( m_iChose+ m_iLevel*6 > 4 && AppDelegate::m_Sell1 == 0 )
#else
					else if ( m_iChose+ m_iLevel*6 > 0 && AppDelegate::m_Sell1 == 0 )
#endif
					{
#ifndef GameTypeC
						Sell1();
						return;
#endif
					}
					if ( m_iChose + m_iLevel*6 >= LevelLimit*6 )
					{
						if ( getChildByTag(410) )
						{
							getChildByTag(410)->stopAllActions();
							removeChildByTag(410);
						}
						CCSprite* pSprite = CCSprite::create("tu14/qidai.png");
						pSprite->setPosition(ccp(400, 240));
						CCFadeOut* pAction = CCFadeOut::create(1.0f);
						pSprite->runAction(CCSequence::create(pAction,
							CCCallFunc::create(this, callfunc_selector(ccbLevelMenu::tipdisappear)), NULL));
						addChild(pSprite, 25, 410);
						return;
					}
					int level = m_iChose;
					CMainMenu::SetStatus(0);
					s_pccbLevelMenu = NULL;
					ccbShopMenu::s_pccbShopMenu = NULL;
					AppDelegate::ChangeScene( CMainMenu::StartGame(level + m_iLevel*6, CMainMenu::GetDifficult()) );
				}
				break;
			}
		}
		m_iChose = -1;
	}
}

void ccbLevelMenu::tipdisappear()
{
	getChildByTag(410)->stopAllActions();
	removeChildByTag(410);
}

void ccbLevelMenu::Appear()
{
	//AppDelegate::AudioPlayBgm("MS/Music/mainmenu/LevelChose.mp3");
	m_iLevel = ((ccbLayer*)CMainMenu::GetMenuMain())->GetLevel();
	((ccbLayer*)CMainMenu::GetMenuMain())->SetSpriteDisable(false);
	for ( int i = 0; i < 6; i ++ )
	{
		if ( m_pNode[i]->getChildByTag(10) )
		{
			for ( int j = 7; j < 14; j++ )
				if ( m_pNode[i]->getChildByTag(j) )
					m_pNode[i]->getChildByTag(j)->removeFromParent();
		}
		CCSprite* pSprite	= NULL;
		CCSprite* pNum		= NULL;
		if ( AppDelegate::s_LevelOpen[i+m_iLevel*6] )
		{
			if ( i == 5 )
			{
				pSprite = CCSprite::create("tu2/di2.png");
				char boss[50];
#ifdef GameTypeC
				sprintf(boss, "tu2/suzi/boss1.png");
#else
				sprintf(boss, "tu2/suzi/boss%d.png", m_iLevel+1);
#endif
				CCSprite* pBoss = CCSprite::create(boss);
				pBoss->setPosition(ccp(100, 53));
				pSprite->addChild(pBoss);
			}
			//else if ( i+m_iLevel*6 == 7 )
			//{//坚守
			//	pSprite = CCSprite::create("tu2/xiaoguankai.png");
			//	CCSprite* pBoss = CCSprite::create("tu2/suzi/mode1.png");
			//	pBoss->setPosition(ccp(100, 58));
			//	pSprite->addChild(pBoss);
			//}
			//else if ( i+m_iLevel*6 == 2 )
			//{//保护
			//	pSprite = CCSprite::create("tu2/xiaoguankai.png");
			//	CCSprite* pBoss = CCSprite::create("tu2/suzi/mode2.png");
			//	pBoss->setPosition(ccp(100, 58));
			//	pSprite->addChild(pBoss);
			//}
			//else if ( i+m_iLevel*6 == 9 )
			//{//突围
			//	pSprite = CCSprite::create("tu2/xiaoguankai.png");
			//	CCSprite* pBoss = CCSprite::create("tu2/suzi/mode3.png");
			//	pBoss->setPosition(ccp(100, 58));
			//	pSprite->addChild(pBoss);
			//}
			else
			{
				pSprite = CCSprite::create("tu2/xiaoguankai.png");
				CCSprite* pBoss = CCSprite::create("tu2/suzi/tou.png");
				pBoss->setPosition(ccp(110, 53));
				pSprite->addChild(pBoss);
			}
		}
		else
		{
			pSprite = CCSprite::create("tu2/xiaoguan.png");
		}
		pSprite->setPosition(ccp(0,0));
		m_pNode[i]->removeChildByTag(10);
		m_pNode[i]->addChild(pSprite,0,10);

		CCSprite* pSpriteNum = CCSprite::create("tu2/suzi/shang.png");
		pSpriteNum->setPosition(ccp(0,0));
		m_pNode[i]->addChild(pSpriteNum, 1, 9);

		char buffer[50] ;
		sprintf(buffer,"tu2/suzi/%d.png",i+1);
		pNum= CCSprite::create(buffer);
		pNum->setPosition(CCPoint(50, 79));
		pSpriteNum->addChild(pNum);

		if ( AppDelegate::s_LevelOpen[i+m_iLevel*6] && i < 5 )
		{
			if ( AppDelegate::s_LevelPerfect[i+m_iLevel*6] )
			{
				CCSprite* pMark = CCSprite::create("tu2/1.png");
				pMark->setPosition(ccp(-70,-6));
				m_pNode[i]->addChild(pMark, 2, 8);
			}
			if ( AppDelegate::s_LevelUnfire[i+m_iLevel*6] )
			{
				CCSprite* pMark = CCSprite::create("tu2/2.png");
				pMark->setPosition(ccp(-37,-26));
				m_pNode[i]->addChild(pMark, 2, 7);
			}
			for ( int j = 0; j < AppDelegate::s_LevelStar[i+m_iLevel*6]; j++ )
			{
				m_pNode[i]->removeChildByTag(11+j);
				CCSprite* pSprite = CCSprite::create("tu2/xing.png");
				pSprite->setPosition(ccp(48+(1-abs(1-j))*20, d_fLevelStarY+(2-j)*d_fLevelStarDist));
				m_pNode[i]->addChild(pSprite,2,11+j);
			}
		}
	}

	m_iChose = -1;
	//((ccbLayer*)CMainMenu::GetMenuMain())->ShowDifficultName();
	m_AnimationManager->runAnimationsForSequenceNamedTweenDuration("Default Timeline", 0.0f);
	scheduleOnce(schedule_selector(ccbLevelMenu::SetTouch), 0.7f);
	//AppDelegate::AudioPlayEffect("MS/Sound/EfLoadingOpen.mp3");
}

void ccbLevelMenu::Return()
{
	setTouchEnabled(false);
	m_AnimationManager->runAnimationsForSequenceNamedTweenDuration("Default Timeline copy", 0.0f);
	((ccbLayer*)CMainMenu::GetMenuMain())->BottomBack();
	scheduleOnce(schedule_selector(ccbLevelMenu::ReturnCallBack), 0.5f);
}

void ccbLevelMenu::ShopReturn()
{
	m_AnimationManager->runAnimationsForSequenceNamedTweenDuration("Default Timeline copy", 0.0f);
}

void ccbLevelMenu::ReturnCallBack( float _t )
{
	CMainMenu::SetStatus(0);
	((ccbLayer*)CMainMenu::GetMenuMain())->m_iStartTime = NeedTime-1;
	((ccbLayer*)CMainMenu::GetMenuMain())->m_bReturn = false;
	((ccbLayer*)CMainMenu::GetMenuMain())->Back();
}

void ccbLevelMenu::SetTouch( float _t )
{
	setTouchEnabled(true);
}

void ccbLevelMenu::Sell1()
{
#ifdef Plat_Telecom_Demo
	Sell1Resume();
	return;
#endif
	CMainMenu::m_bOnSell = true;

	CCLayerColor* pCCLayerColor = CCLayerColor::create(ccc4(20,20,20,230), 800, 480);
	//pCCLayerColor->setPosition(ccp(0,0));
	//SetScale(pCCLayerColor);
	addChild(pCCLayerColor, 29, 194);
	CCMenuItem* pItem1 = CCMenuItemImage::create("sell/huode2-1.png", "sell/huode2-2.png", this, menu_selector(ccbLevelMenu::Sell1Resume));
	CCMenuItem* pItem2 = CCMenuItemImage::create("sell/guan.png", "sell/guan.png", this, menu_selector(ccbLevelMenu::Sell1Back));
	pItem1->setPosition(ccp(725,195));
	pItem2->setPosition(ccp(728,437));
	CCMenu* pMenu = CCMenu::create(pItem1, pItem2, NULL);
	pMenu->setPosition(ccp(0,0));
	addChild(pMenu, 30, 197);

	CCSprite* pSprite1 = CCSprite::create("sell/di1.png");
	pSprite1->setPosition(ccp(400, 240));
	addChild(pSprite1, 31, 199);
	CCSprite* pSprite4 = CCSprite::create("sell/vip/tu.png");
	pSprite4->setPosition(ccp(400, 240));
	addChild(pSprite4, 31, 198);
	CCSprite* pSprite2 = common::CreateAnimation("sell/vip/meizi1.png", ccp(265, 190), ccp(0.5f, 0.5f),
		"sell/vip/meizi", 2, CCRect(0,0,282,284), true);
	addChild(pSprite2, 31, 196);
	CCSprite* pSprite3 = CCSprite::create("sell/vip/dang.png");
	pSprite3->setPosition(ccp(265, 50));
	addChild(pSprite3, 31, 195);
}

void ccbLevelMenu::Sell1Resume( cocos2d::CCObject *pSender /*= NULL*/ )
{
	if ( pSender )
	{
		PayService::pay(4);
	}
	else
	{
#ifdef Plat_Telecom_Demo
		AppDelegate::m_Sell1		= 1;
		AppDelegate::s_WeaponOwn[6] = 1;
		AppDelegate::s_WeaponOwn[9] = 1;
		if ( AppDelegate::s_HeroType == 0 )
		{
			AppDelegate::s_WeaponUse[0] = 6;
			AppDelegate::s_WeaponUse[1] = 9;
		}
		AppDelegate::s_BulletNum[9] += 100;
		if ( AppDelegate::s_BulletNum[9] > 999 )
			AppDelegate::s_BulletNum[9] = 999;
		AppDelegate::s_Healbox = 3;
		AppDelegate::s_GrenadeNum = 10;
		AppDelegate::s_Money += 9999;
		AppDelegate::SaveStatus();
#endif
		scheduleOnce(schedule_selector(ccbLevelMenu::Sell1Enter), 0.1f);
	}
}

void ccbLevelMenu::Sell1Enter(float _t)
{
	CMainMenu::m_bOnSell = false;
	AppDelegate::ChangeScene( CMainMenu::StartGame(m_iChose + m_iLevel*6, CMainMenu::GetDifficult()) );
}

void ccbLevelMenu::Sell1Back( cocos2d::CCObject *pSender /*= NULL*/ )
{
	CMainMenu::m_bOnSell = false;
	m_iChose = -1;
	if ( getChildByTag(194) )
		getChildByTag(194)->removeFromParentAndCleanup(true);
	if ( getChildByTag(196) )
		getChildByTag(196)->removeFromParentAndCleanup(true);
	if ( getChildByTag(195) )
		getChildByTag(195)->removeFromParentAndCleanup(true);
	if ( getChildByTag(197) )
		getChildByTag(197)->removeFromParentAndCleanup(true);
	if ( getChildByTag(198) )
		getChildByTag(198)->removeFromParentAndCleanup(true);
	if ( getChildByTag(199) )
		getChildByTag(199)->removeFromParentAndCleanup(true);
}