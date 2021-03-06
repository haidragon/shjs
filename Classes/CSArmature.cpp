﻿/*
 * Copyright (c) 2012 Chukong Technologies, Inc.
 *
 * http://www.cocostudio.com
 * http://tools.cocoachina.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CSArmature.h"
#include "CSArmatureDataManager.h"
#include "CSDatas.h"
#include "CSArmatureDefine.h"
#include "CSDataReaderHelper.h"
#include "CSSkin.h"

using namespace cocos2d;

#if CS_DEBUG_FOR_EDIT
#include "CSEditorArmature.h"
#endif

namespace cs {

    
std::map<int, Armature*> Armature::m_sArmatureIndexDic;

    
Armature *Armature::create()
{
#if CS_DEBUG_FOR_EDIT
	Armature *armature = new EditorArmature();
#else
	Armature *armature = new Armature();
#endif
    if (armature && armature->init())
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return NULL;
}

    
Armature *Armature::create(const char* _name)
{
#if CS_DEBUG_FOR_EDIT
	Armature *armature = new EditorArmature();
#else
	Armature *armature = new Armature();
#endif
    if (armature && armature->init(_name))
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return NULL;
}

Armature *Armature::create(const char* name, Bone *parentBone)
{
	Armature *armature = new Armature();

	if (armature && armature->init(name, parentBone))
	{
		armature->autorelease();
		return armature;
	}
	CC_SAFE_DELETE(armature);
	return NULL;
}

Armature::Armature()
    :m_pAnimation(NULL)
    ,m_pBoneDic(NULL)
	,m_pAtlas(NULL)
	,m_pBatchNode(NULL)
	,m_pParentBone(NULL)
	,m_pTopBoneList(NULL)
{
}


Armature::~Armature(void)
{
    if(NULL != m_pBoneDic)
    {
        m_pBoneDic->removeAllObjects();
        CC_SAFE_DELETE(m_pBoneDic);
    }
	if (NULL != m_pTopBoneList)
	{
		m_pTopBoneList->removeAllObjects();
		CC_SAFE_DELETE(m_pTopBoneList);
	}
    
    CC_SAFE_DELETE(m_pAnimation);
}

    
bool Armature::init()
{
    return init(NULL);
}


bool Armature::init(const char *name)
{
    bool bRet = false;
    do
    {
        //cocos2d::CCLog("Armature (%s)  create.", name);

		CC_SAFE_DELETE(m_pAnimation);
        m_pAnimation = new Animation();
		m_pAnimation->init(this);

		CC_SAFE_DELETE(m_pBoneDic);
        m_pBoneDic	= new CCDictionary();

		CC_SAFE_DELETE(m_pTopBoneList);
		m_pTopBoneList = new CCArray();
		m_pTopBoneList->init();


		m_sBlendFunc.src = CC_BLEND_SRC;
		m_sBlendFunc.dst = CC_BLEND_DST;
        

		m_strName = name == NULL ? "" : name;

        ArmatureDataManager *armatureDataManager = ArmatureDataManager::sharedArmatureDataManager();
        
        if(m_strName.compare("") != 0)
        {
            m_strName = name;
            
            AnimationData* animationData = armatureDataManager->getAnimationData(name);
            CCAssert(animationData, "AnimationData not exist! ");
            
            m_pAnimation->setAnimationData(animationData);
            
            
            ArmatureData *armatureData = armatureDataManager->getArmatureData(name);
            CCAssert(armatureData, "");
            
            m_pArmatureData = armatureData;
            
            
            CCDictElement *_element = NULL;
			CCDictionary *boneDataDic = &armatureData->boneDataDic;
            CCDICT_FOREACH(boneDataDic, _element)
            {
                Bone *bone = createBone(_element->getStrKey());
                
                //! init bone's  Tween to 1st movement's 1st frame
                do {
                    
                    MovementData *movData = animationData->getMovement(animationData->movementNames.at(0).c_str());
                    CC_BREAK_IF(!movData);
                    
                    MovementBoneData *movBoneData = movData->getMovementBoneData(bone->getName().c_str());
                    CC_BREAK_IF(!movBoneData || movBoneData->frameList.count() <= 0);
                    
                    FrameData *frameData = movBoneData->getFrameData(0);
                    CC_BREAK_IF(!frameData);
                    
                    bone->getTweenData()->copy(frameData);
					bone->changeDisplayByIndex(frameData->displayIndex, false);
                } while (0);
			}
			
			update(0);
			updateOffsetPoint();
        }
        else
        {
            m_strName = "new_armature";
            m_pArmatureData = ArmatureData::create();
            m_pArmatureData->name = m_strName;
            
            AnimationData *animationData = AnimationData::create();
			animationData->name = m_strName;
            
            armatureDataManager->addArmatureData(m_strName.c_str(), m_pArmatureData);
            armatureDataManager->addAnimationData(m_strName.c_str(), animationData);
            
            m_pAnimation->setAnimationData(animationData);
           
        }

		setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));

        unscheduleUpdate();
		scheduleUpdate();

		setCascadeOpacityEnabled(true);
		setCascadeColorEnabled(true);
        
        bRet = true;
    }
    while (0);

    return bRet;
}

bool Armature::init(const char *name, Bone *parentBone)
{
	m_pParentBone = parentBone;
	return init(name);
}


Bone *Armature::createBone(const char *boneName)
{
	Bone *existedBone = getBone(boneName);
	if(existedBone != NULL)
		return existedBone;
    
    BoneData *boneData = (BoneData*)m_pArmatureData->getBoneData(boneName);
	std::string parentName = boneData->parentName;

    Bone *bone = NULL;
    
    if( parentName.compare("") !=0 )
    {
        createBone(parentName.c_str());
        bone = Bone::create(boneName);
        addBone(bone, parentName.c_str());
    }else{
        bone = Bone::create(boneName);
        addBone(bone, "");
    }
    
    bone->setBoneData(boneData);
	bone->getDisplayManager()->changeDisplayByIndex(-1, false);
    
    return bone;
}


void Armature::addBone(Bone *bone, const char *parentName)
{
    CCAssert( bone != NULL, "Argument must be non-nil");
    CCAssert(m_pBoneDic->objectForKey(bone->getName()) == NULL, "bone already added. It can't be added again");

    if (NULL != parentName)
    {
		Bone *boneParent = (Bone*)m_pBoneDic->objectForKey(parentName);
        if (boneParent)
		{
			boneParent->addChildBone(bone);
		}
		else
		{
			if (m_pParentBone) 
				m_pParentBone->addChildBone(bone);
			else
				m_pTopBoneList->addObject(bone);
		}
    }
	else
	{
		if (m_pParentBone) 
			m_pParentBone->addChildBone(bone);
		else
			m_pTopBoneList->addObject(bone);
	}
    
    bone->setArmature(this);
    
    m_pBoneDic->setObject(bone, bone->getName());
    addChild(bone);
}


void Armature::removeBone(Bone *bone, bool recursion)
{
    CCAssert(bone != NULL, "bone must be added to the bone dictionary!");
    
    bone->setArmature(NULL);
    bone->removeFromParent(recursion);
    
	if (m_pTopBoneList->containsObject(bone))
	{
		m_pTopBoneList->removeObject(bone);
	}
    m_pBoneDic->removeObjectForKey(bone->getName());
    m_pChildren->removeObject(bone);
}


Bone *Armature::getBone(const char* _name)
{
	return (Bone*)m_pBoneDic->objectForKey(_name);
}


void Armature::changeBoneParent(Bone *bone, const char *parentName)
{
	CCAssert(bone != NULL, "bone must be added to the bone dictionary!");

	bone->getParentBone()->getChildren()->removeObject(bone);
	bone->setParentBone(NULL);

	if (parentName != NULL)
	{
		Bone *boneParent = (Bone*)m_pBoneDic->objectForKey(parentName);

		if (boneParent)
		{
			boneParent->addChildBone(bone);
		}
	}
}

CCDictionary *Armature::getBoneDic()
{
    return m_pBoneDic;
}

CCAffineTransform Armature::nodeToParentTransform()
{
	if (m_bTransformDirty) 
	{

		// Translate values
		float x = m_obPosition.x;
		float y = m_obPosition.y;

		if (m_bIgnoreAnchorPointForPosition) 
		{
			x += m_obAnchorPointInPoints.x;
			y += m_obAnchorPointInPoints.y;
		}

		// Rotation values
		// Change rotation code to handle X and Y
		// If we skew with the exact same value for both x and y then we're simply just rotating
		float cx = 1, sx = 0, cy = 1, sy = 0;
		if (m_fRotationX || m_fRotationY)
		{
			float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationX);
			float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationY);
			cx = cosf(radiansX);
			sx = sinf(radiansX);
			cy = cosf(radiansY);
			sy = sinf(radiansY);
		}

		// Add offset point
		x += cy * m_pOffsetPoint.x * m_fScaleX + -sx * m_pOffsetPoint.y * m_fScaleY;
		y += sy * m_pOffsetPoint.x * m_fScaleX + cx * m_pOffsetPoint.y * m_fScaleY;

		bool needsSkewMatrix = ( m_fSkewX || m_fSkewY );
		
		// optimization:
		// inline anchor point calculation if skew is not needed
		// Adjusted transform calculation for rotational skew
		if (! needsSkewMatrix && !m_obAnchorPointInPoints.equals(CCPointZero))
		{
			x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
			y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;
		}


		// Build Transform Matrix
		// Adjusted transform calculation for rotational skew
		m_sTransform = CCAffineTransformMake( cy * m_fScaleX,  sy * m_fScaleX,
			-sx * m_fScaleY, cx * m_fScaleY,
			x, y );

		// XXX: Try to inline skew
		// If skew is needed, apply skew and then anchor point
		if (needsSkewMatrix) 
		{
			CCAffineTransform skewMatrix = CCAffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(m_fSkewY)),
				tanf(CC_DEGREES_TO_RADIANS(m_fSkewX)), 1.0f,
				0.0f, 0.0f );
			m_sTransform = CCAffineTransformConcat(skewMatrix, m_sTransform);

			// adjust anchor point
			if (!m_obAnchorPointInPoints.equals(CCPointZero))
			{
				m_sTransform = CCAffineTransformTranslate(m_sTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
			}
		}

		if (m_bAdditionalTransformDirty)
		{
			m_sTransform = CCAffineTransformConcat(m_sTransform, m_sAdditionalTransform);
			m_bAdditionalTransformDirty = false;
		}

		m_bTransformDirty = false;
	}

	return m_sTransform;
}
    
void Armature::updateOffsetPoint()
{
	// Set contentsize and Calculate anchor point. 
	CCRect rect = boundingBox();
	setContentSize(rect.size);
	m_pOffsetPoint = ccp(-rect.origin.x,  -rect.origin.y);
	setAnchorPoint(ccp(m_pOffsetPoint.x/rect.size.width, m_pOffsetPoint.y/rect.size.height));
}


void Armature::update(float dt)
{
    m_pAnimation->update(dt);

	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pTopBoneList, object)
	{
		((Bone*)object)->update(dt);
	}
}

void Armature::draw()
{
	if (m_pParentBone == NULL)
	{
		CC_NODE_DRAW_SETUP();
		ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);
	}
	

	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pChildren, object)
	{
		Bone *bone = (Bone*)object;

		DisplayManager *displayManager = bone->getDisplayManager();
		CCNode *node = displayManager->getDisplayRenderNode();
		
		if (NULL == node)
			continue;

		if(Skin *skin = dynamic_cast<Skin*>(node))
		{
			CCTextureAtlas *textureAtlas = skin->getTextureAtlas();
			if(m_pAtlas != textureAtlas)
			{
				if (m_pAtlas) {
					m_pAtlas->drawQuads();
					m_pAtlas->removeAllQuads();
				}
			}

			m_pAtlas = textureAtlas;
			if (m_pAtlas->getCapacity() == m_pAtlas->getTotalQuads() && !m_pAtlas->resizeCapacity(m_pAtlas->getCapacity() * 2)) 
				return;	

			skin->draw();
		}
		else if(Armature *armature = dynamic_cast<Armature*>(node))
		{
			CCTextureAtlas *textureAtlas = armature->getTextureAtlas();

			if(m_pAtlas != textureAtlas)
			{
				if (m_pAtlas) {
					m_pAtlas->drawQuads();
					m_pAtlas->removeAllQuads();
				}
			}
			armature->draw();
		}
		else
		{
			if (m_pAtlas) {
			 	m_pAtlas->drawQuads();
			 	m_pAtlas->removeAllQuads();
			}
			node->visit();
			 
			CC_NODE_DRAW_SETUP();
			ccGLBlendFunc(m_sBlendFunc.src, m_sBlendFunc.dst);
		}
	}

	if(m_pAtlas && !m_pBatchNode && m_pParentBone == NULL)
	{
		m_pAtlas->drawQuads();
		m_pAtlas->removeAllQuads();
	}
}

void Armature::visit()
{
	// quick return if not visible. children won't be drawn.
	if (!m_bVisible)
	{
		return;
	}
	kmGLPushMatrix();

	if (m_pGrid && m_pGrid->isActive())
	{
		m_pGrid->beforeDraw();
	}

	transform();
	sortAllChildren();
	draw();

	// reset for next frame
	m_uOrderOfArrival = 0;

	if (m_pGrid && m_pGrid->isActive())
	{
		m_pGrid->afterDraw(this);
	}

	kmGLPopMatrix();
}

CCRect Armature::boundingBox()
{
	float minx, miny, maxx, maxy = 0;

	bool first = true;

	CCRect boundingBox = CCRectMake(0, 0, 0, 0);

	CCObject *object = NULL;
	CCARRAY_FOREACH(m_pChildren, object)
	{
		Bone *bone = (Bone*)object;
		CCRect r = bone->getDisplayManager()->getBoundingBox();
		
		if(first)
		{
			minx = r.getMinX();
			miny = r.getMinY();
			maxx = r.getMaxX();
			maxy = r.getMaxY();

			first = false;
		}
		else
		{
			minx = r.getMinX() < boundingBox.getMinX() ? r.getMinX() : boundingBox.getMinX();
			miny = r.getMinY() < boundingBox.getMinY() ? r.getMinY() : boundingBox.getMinY();
			maxx = r.getMaxX() > boundingBox.getMaxX() ? r.getMaxX() : boundingBox.getMaxX();
			maxy = r.getMaxY() > boundingBox.getMaxY() ? r.getMaxY() : boundingBox.getMaxY();
		}
		
		boundingBox.setRect(minx, miny, maxx - minx, maxy - miny);
	}

	return boundingBox;
}

Bone *Armature::getBoneAtPoint(float _x, float _y)
{
    int _length = m_pChildren->data->num;
    Bone **_bs = (Bone**)m_pChildren->data->arr;
    
    for(int i = _length-1; i>=0; i--)
    {
        if(_bs[i]->getDisplayManager()->containPoint(_x, _y))
        {
            return _bs[i];
        }
    }    
    return NULL;
}


}