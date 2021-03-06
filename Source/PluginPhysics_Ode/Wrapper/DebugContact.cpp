/**
* File: OgreOdeDebugContact.cpp
*
* description: This create create a billboarding object that display a text.
*  From MovableText
* @author  2003 by cTh see gavocanov@rambler.ru
* @update  2006 by barraq see nospam@barraquand.com
*/
#include "Ogre.h"
#include "OgreFontManager.h"

#include "DebugContact.h"
#include "Collision.h"
#include "Geometry.h"
#include "Body.h"
#include "World.h"

using namespace Ogre;

#define POS_TEX_BINDING    0
#define COLOUR_BINDING     1

namespace Ode
{
    //------------------------------------------------------------------------------------------------
    DebugContact::DebugContact(const String &name, World *world) : 
        _name(name), 
        _world(world),
        _enabled(false)
    {
        // scene node 
        _node = _world->getSceneManager()->getRootSceneNode ()->createChildSceneNode ();

        // sphere attach to contact point
        _point = _world->getSceneManager()->createEntity(name + String("_debug_point"), "sphere.mesh");
		_point->setQueryFlags(0);
        _point_node = _node->createChildSceneNode ();
        _point_node->attachObject (_point);
        _point_node->setScale(0.001f, 0.001f, 0.001f);

        // normal direction debug
         _normal = new DebugNormal();
         _node->attachObject (_normal);

         // text info on contact
        _text = new DebugContactText(name + String("_debug_text"), _node->createChildSceneNode ());
        _text->setPosition (Vector3(0,5,0));
        _text->setCaption ("-");
        _text->setVisible (false);

        _node->setVisible (false);
    }
     //------------------------------------------------------------------------------------------------
    DebugContact::~DebugContact()
    {
        delete _text;

        _point_node->detachObject (_point->getName ());
        _node->detachObject (_normal->getName ());

        _world->getSceneManager()->destroyEntity (_point->getName ());

        _node->detachAllObjects ();
        _node->removeAndDestroyAllChildren ();

        delete _normal;
    }
    //------------------------------------------------------------------------------------------------
    void DebugContact::update(Contact * const contact)
    {
        _node->setPosition (contact->getPosition ());

        _point_node->setPosition (contact->getPenetrationDepth () * contact->getNormal());

        //String contactLabel("p: ");
        //contactLabel = contactLabel + StringConverter::toString(contact->getPosition());
        //contactLabel = contactLabel + ", n: ";
        //contactLabel = contactLabel + StringConverter::toString(contact->getNormal());
        //_text->setCaption (contactLabel);
        //_text->setVisible (false);

        //_text->setPosition (contact->getPosition () + Vector3(0,5,0));
        
        _normal->update (contact);
    }
    //------------------------------------------------------------------------------------------------
    bool DebugContact::isEnabled () const 
    {
        return _enabled;
    }
    //------------------------------------------------------------------------------------------------
    void DebugContact::setEnabled (bool enable)
    {
        _enabled = enable;
        _node->setVisible (_enabled);
    };
    //------------------------------------------------------------------------------------------------
    void DebugNormal::update(Contact * const contact)
    {
       DebugLines::clear ();
       // set normal following contact normal
       //contact->_normal 
       //contact->_position
       DebugLines::addLine (Vector3::ZERO, 
                            5*contact->getNormal ());
        DebugLines::draw ();
    }
    //------------------------------------------------------------------------------------------------
    DebugContactText::DebugContactText(const String &name, 
                                SceneNode *node,
                                const String &caption, 
                                const String &fontName, 
                                int charHeight, 
                                const ColourValue &color)
        : mpCam(NULL)
        , mpWin(NULL)
        , mpFont(NULL)
        , mName(name)
        , mCaption(caption)
        , mFontName(fontName)
        , mCharHeight(charHeight)
        , mColor(color)
        , mType("DebugContact")
        , mTimeUntilNextToggle(0)
        , mSpaceWidth(0)
        , mUpdateColors(true)
        , mOnTop(false)
        , mHorizontalAlignment(H_LEFT)
        , mVerticalAlignment(V_BELOW)
        , mAdditionalHeight(0.0)
        , mNode(node)

    {
        if (name.empty())
            Exception(Exception::ERR_INVALIDPARAMS, "Trying to create DebugContact without name", "DebugContact::DebugContact");
// 
//         if (caption.empty())
//            Exception(Exception::ERR_INVALIDPARAMS, "Trying to create DebugContact without caption", "DebugContact::DebugContact");

        mRenderOp.vertexData = NULL;
        this->setFontName(mFontName);
        //this->_setupGeometry();
        mNode->attachObject(this);
    }
    //------------------------------------------------------------------------------------------------
    DebugContactText::~DebugContactText()
    {
        mNode->detachObject(this->getName());
        if (mRenderOp.vertexData)
            delete mRenderOp.vertexData;
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setPosition(const Vector3 &pos)
    {
        mNode->setPosition (pos);
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setFontName(const String &fontName)
    {
        if((Ogre::MaterialManager::getSingletonPtr()->resourceExists(mName + "Material"))) 
        { 
            Ogre::MaterialManager::getSingleton().remove(mName + "Material"); 
        }

        if (mFontName != fontName || mpMaterial.isNull() || !mpFont)
        {
            mFontName = fontName;
            mpFont = (Font *)FontManager::getSingleton().getByName(mFontName).getPointer();
            if (!mpFont)
                Exception(Exception::ERR_ITEM_NOT_FOUND, "Could not find font " + fontName, "DebugContact::setFontName");

            mpFont->load();
            if (!mpMaterial.isNull())
            {
                MaterialManager::getSingletonPtr()->remove(mpMaterial->getName());
                mpMaterial.setNull();
            }

            mpMaterial = mpFont->getMaterial()->clone(mName + "Material");
            if (!mpMaterial->isLoaded())
                mpMaterial->load();

            mpMaterial->setDepthCheckEnabled(!mOnTop);
            mpMaterial->setDepthBias(0,!mOnTop);
            mpMaterial->setDepthWriteEnabled(mOnTop);
            mpMaterial->setLightingEnabled(false);
            mNeedUpdate = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setCaption(const String &caption)
    {
        if (caption != mCaption)
        {
            mCaption = caption;
            mNeedUpdate = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setColor(const ColourValue &color)
    {
        if (color != mColor)
        {
            mColor = color;
            mUpdateColors = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setCharacterHeight(unsigned int height)
    {
        if (height != mCharHeight)
        {
            mCharHeight = height;
            mNeedUpdate = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setSpaceWidth(unsigned int width)
    {
        if (width != mSpaceWidth)
        {
            mSpaceWidth = width;
            mNeedUpdate = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setTextAlignment(const HorizontalAlignment& horizontalAlignment, const VerticalAlignment& verticalAlignment)
    {
        if(mHorizontalAlignment != horizontalAlignment)
        {
            mHorizontalAlignment = horizontalAlignment;
            mNeedUpdate = true;
        }
        if(mVerticalAlignment != verticalAlignment)
        {
            mVerticalAlignment = verticalAlignment;
            mNeedUpdate = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::setAdditionalHeight( Real height )
    {
        if( mAdditionalHeight != height )
        {
            mAdditionalHeight = height;
            mNeedUpdate = true;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::showOnTop(bool show)
    {
        if( mOnTop != show && !mpMaterial.isNull() )
        {
            mOnTop = show;
            mpMaterial->setDepthBias(0,!mOnTop);
            mpMaterial->setDepthCheckEnabled(!mOnTop);
            mpMaterial->setDepthWriteEnabled(mOnTop);
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::_setupGeometry()
    {
        assert(mpFont);
        assert(!mpMaterial.isNull());

        unsigned int vertexCount = static_cast<unsigned int>(mCaption.size() * 6);

        if (mRenderOp.vertexData)
        {
            // Removed this test as it causes problems when replacing a caption
            // of the same size: replacing "Hello" with "hello"
            // as well as when changing the text alignment
            //if (mRenderOp.vertexData->vertexCount != vertexCount)
            {
                delete mRenderOp.vertexData;
                mRenderOp.vertexData = NULL;
                mUpdateColors = true;
            }
        }

        if (!mRenderOp.vertexData)
            mRenderOp.vertexData = new VertexData();

        mRenderOp.indexData = 0;
        mRenderOp.vertexData->vertexStart = 0;
        mRenderOp.vertexData->vertexCount = vertexCount;
        mRenderOp.operationType = RenderOperation::OT_TRIANGLE_LIST; 
        mRenderOp.useIndexes = false; 

        VertexDeclaration  *decl = mRenderOp.vertexData->vertexDeclaration;
        VertexBufferBinding   *bind = mRenderOp.vertexData->vertexBufferBinding;
        size_t offset = 0;

        // create/bind positions/tex.ccord. buffer
        if (!decl->findElementBySemantic(VES_POSITION))
            decl->addElement(POS_TEX_BINDING, offset, VET_FLOAT3, VES_POSITION);

        offset += VertexElement::getTypeSize(VET_FLOAT3);

        if (!decl->findElementBySemantic(VES_TEXTURE_COORDINATES))
            decl->addElement(POS_TEX_BINDING, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, 0);

        HardwareVertexBufferSharedPtr ptbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(POS_TEX_BINDING),
            mRenderOp.vertexData->vertexCount,
            HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
        bind->setBinding(POS_TEX_BINDING, ptbuf);

        // Colours - store these in a separate buffer because they change less often
        if (!decl->findElementBySemantic(VES_DIFFUSE))
            decl->addElement(COLOUR_BINDING, 0, VET_COLOUR, VES_DIFFUSE);

        HardwareVertexBufferSharedPtr cbuf = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(COLOUR_BINDING),
            mRenderOp.vertexData->vertexCount,
            HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);
        bind->setBinding(COLOUR_BINDING, cbuf);

        size_t charlen = mCaption.size();
        Real *pPCBuff = static_cast<Real*>(ptbuf->lock(HardwareBuffer::HBL_DISCARD));

        float largestWidth = 0.0f;
        float left = 0.0f * 2.0f - 1.0f;
        float top = -((0.0f * 2.0f) - 1.0f);

        // Derive space width from a capital A
        if (mSpaceWidth == 0)
            mSpaceWidth = (u32)(mpFont->getGlyphAspectRatio('A') * mCharHeight * 2.0f);

        // for calculation of AABB
        Vector3 min, max, currPos;
        f32 maxSquaredRadius;
        bool first = true;

        // Use iterator
        String::iterator i, iend;
        iend = mCaption.end();
        bool newLine = true;
        Real len = 0.0f;

        if(mVerticalAlignment == DebugContactText::V_ABOVE)
        {
            // Raise the first line of the caption
            top += mCharHeight;
            for (i = mCaption.begin(); i != iend; ++i)
            {
                if (*i == '\n')
                    top += mCharHeight * 2.0f;
            }
        }

        for (i = mCaption.begin(); i != iend; ++i)
        {
            if (newLine)
            {
                len = 0.0f;
                for (String::iterator j = i; j != iend && *j != '\n'; j++)
                {
                    if (*j == ' ')
                        len += mSpaceWidth;
                    else 
                        len += mpFont->getGlyphAspectRatio(*j) * mCharHeight * 2.0f;
                }
                newLine = false;
            }

            if (*i == '\n')
            {
                left = 0.0f * 2.0f - 1.0f;
                top -= mCharHeight * 2.0f;
                newLine = true;
                continue;
            }

            if (*i == ' ')
            {
                // Just leave a gap, no tris
                left += mSpaceWidth;
                // Also reduce tri count
                mRenderOp.vertexData->vertexCount -= 6;
                continue;
            }

            Real horiz_height = mpFont->getGlyphAspectRatio(*i);
            const Font::UVRect &uvRect = mpFont->getGlyphTexCoords(*i);
            const Real u1 = uvRect.left; 
            const Real u2 = uvRect.right; 
            const Real v1 = uvRect.top; 
            const Real v2 = uvRect.bottom; 

            // each vert is (x, y, z, u, v)
            //-------------------------------------------------------------------------------------
            // First tri
            //
            // Upper left
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                *pPCBuff++ = left;
            else
                *pPCBuff++ = left - (len / 2);
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u1;
            *pPCBuff++ = v1;

            // Deal with bounds
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                currPos = Vector3(left, top, -1.0);
            else
                currPos = Vector3(left - (len / 2), top, -1.0);
            if (first)
            {
                min = max = currPos;
                maxSquaredRadius = currPos.squaredLength();
                first = false;
            }
            else
            {
                min.makeFloor(currPos);
                max.makeCeil(currPos);
                maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());
            }

            top -= mCharHeight * 2.0f;

            // Bottom left
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                *pPCBuff++ = left;
            else
                *pPCBuff++ = left - (len / 2);
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0f;
            *pPCBuff++ = u1;
            *pPCBuff++ = v2;

            // Deal with bounds
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                currPos = Vector3(left, top, -1.0);
            else
                currPos = Vector3(left - (len / 2), top, -1.0);
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

            top += mCharHeight * 2.0f;
            left += horiz_height * mCharHeight * 2.0f;

            // Top right
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                *pPCBuff++ = left;
            else
                *pPCBuff++ = left - (len / 2);
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u2;
            *pPCBuff++ = v1;
            //-------------------------------------------------------------------------------------

            // Deal with bounds
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                currPos = Vector3(left, top, -1.0);
            else
                currPos = Vector3(left - (len / 2), top, -1.0);
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

            //-------------------------------------------------------------------------------------
            // Second tri
            //
            // Top right (again)
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                *pPCBuff++ = left;
            else
                *pPCBuff++ = left - (len / 2);
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u2;
            *pPCBuff++ = v1;

            currPos = Vector3(left, top, -1.0f);
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

            top -= mCharHeight * 2.0f;
            left -= horiz_height  * mCharHeight * 2.0f;

            // Bottom left (again)
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                *pPCBuff++ = left;
            else
                *pPCBuff++ = left - (len / 2);
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u1;
            *pPCBuff++ = v2;

            currPos = Vector3(left, top, -1.0);
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

            left += horiz_height  * mCharHeight * 2.0f;

            // Bottom right
            if(mHorizontalAlignment == DebugContactText::H_LEFT)
                *pPCBuff++ = left;
            else
                *pPCBuff++ = left - (len / 2);
            *pPCBuff++ = top;
            *pPCBuff++ = -1.0;
            *pPCBuff++ = u2;
            *pPCBuff++ = v2;
            //-------------------------------------------------------------------------------------

            currPos = Vector3(left, top, -1.0);
            min.makeFloor(currPos);
            max.makeCeil(currPos);
            maxSquaredRadius = std::max(maxSquaredRadius, currPos.squaredLength());

            // Go back up with top
            top += mCharHeight * 2.0f;

            float currentWidth = (left + 1)/2 - 0;
            if (currentWidth > largestWidth)
                largestWidth = currentWidth;
        }

        // Unlock vertex buffer
        ptbuf->unlock();

        // update AABB/Sphere radius
        mAABB = Ogre::AxisAlignedBox(min, max);
        mRadius = Ogre::Math::Sqrt(maxSquaredRadius);

        if (mUpdateColors)
            this->_updateColors();

        mNeedUpdate = false;
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::_updateColors(void)
    {
        assert(mpFont);
        assert(!mpMaterial.isNull());

        // Convert to system-specific
        RGBA color;
        Root::getSingleton().convertColourValue(mColor, &color);
        HardwareVertexBufferSharedPtr vbuf = mRenderOp.vertexData->vertexBufferBinding->getBuffer(COLOUR_BINDING);
        RGBA *pDest = static_cast<RGBA*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
        for (unsigned int i = 0; i < mRenderOp.vertexData->vertexCount; ++i)
            *pDest++ = color;
        vbuf->unlock();
        mUpdateColors = false;
    }
    //------------------------------------------------------------------------------------------------
    const Quaternion& DebugContactText::_getDerivedOrientation(void) const
    {
        assert(mpCam);
        return const_cast<Quaternion&>(mpCam->getDerivedOrientation());
    }
    //------------------------------------------------------------------------------------------------
    const Vector3& DebugContactText::_getDerivedPosition(void) const
    {
        assert(mParentNode);
        return mParentNode->_getDerivedPosition();
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::getWorldTransforms(Matrix4 *xform) const 
    {	
        if (this->isVisible() && mpCam)
        {
            Matrix3 rot3x3, scale3x3 = Matrix3::IDENTITY;

            // store rotation in a matrix
            mpCam->getDerivedOrientation().ToRotationMatrix(rot3x3);

            // parent node position
            Vector3 ppos = mParentNode->_getDerivedPosition() + Vector3::UNIT_Y*mAdditionalHeight;

            // apply scale
            scale3x3[0][0] = mParentNode->_getDerivedScale().x / 2;
            scale3x3[1][1] = mParentNode->_getDerivedScale().y / 2;
            scale3x3[2][2] = mParentNode->_getDerivedScale().z / 2;

            // apply all transforms to xform       
            *xform = (rot3x3 * scale3x3);
            xform->setTrans(ppos);
        }
   }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::getRenderOperation(RenderOperation &op)
    {
        if (this->isVisible())
        {
            if (mNeedUpdate)
                this->_setupGeometry();
            if (mUpdateColors)
                this->_updateColors();
            op = mRenderOp;
        }
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::_notifyCurrentCamera(Camera *cam)
    {
        mpCam = cam;
    }
    //------------------------------------------------------------------------------------------------
    void DebugContactText::_updateRenderQueue(RenderQueue* queue)
    {
        if (this->isVisible())
        {
            if (mNeedUpdate)
                this->_setupGeometry();
            if (mUpdateColors)
                this->_updateColors();

            queue->addRenderable(this, mRenderQueueID, OGRE_RENDERABLE_DEFAULT_PRIORITY);
            //      queue->addRenderable(this, mRenderQueueID, RENDER_QUEUE_SKIES_LATE);
        }
    }
}
