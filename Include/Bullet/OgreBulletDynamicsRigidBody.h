#ifndef _OGREBULLETDYNAMICS_RigidObject_H
#define _OGREBULLETDYNAMICS_RigidObject_H

#include "OgreBulletDynamicsPreRequisites.h"
#include "OgreBulletCollisionsObject.h"
#include "OgreBulletCollisionsWorld.h"
#include "Utils/OgreBulletConverter.h"
#include "OgreBulletDynamicsWorld.h"
#include "Constraints/OgreBulletDynamicsRaycastVehicle.h"
#include "BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h"

namespace OgreBulletDynamics
{
    // basic rigid body class
    class CORE_EXPORT RigidBody : public OgreBulletCollisions::Object
    {
    public:
	    RigidBody(const Ogre::String &name, 
				  DynamicsWorld *world,
				  const short collisionGroup = 0,
				  const short collisionMask = 0);

	    virtual ~RigidBody();

        void setShape(Ogre::SceneNode *node, 
            OgreBulletCollisions::CollisionShape *shape,
            const float      bodyRestitution,
            const float      bodyFriction,
            const float      bodyMass,
            const Ogre::Vector3 &pos = Ogre::Vector3::ZERO, 
            const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);

        void setStaticShape(Ogre::SceneNode *node,
            OgreBulletCollisions::CollisionShape *shape,
            const float      bodyRestitution,
            const float      bodyFriction,
            const Ogre::Vector3 &pos = Ogre::Vector3::ZERO, 
            const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);


		   void setStaticShape(btScaledBvhTriangleMeshShape *shape,
		           const float      bodyRestitution,
		           const float      bodyFriction,
		           const Ogre::Vector3 &pos = Ogre::Vector3::ZERO,
		           const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);


        void setStaticShape(OgreBulletCollisions::CollisionShape *shape,
            const float      bodyRestitution,
            const float      bodyFriction,
            const Ogre::Vector3 &pos = Ogre::Vector3::ZERO, 
            const Ogre::Quaternion &quat = Ogre::Quaternion::IDENTITY);


        void setLinearVelocity(const Ogre::Vector3 &vel);
        void setLinearVelocity(const Ogre::Real x, const Ogre::Real y, const Ogre::Real z);
		Ogre::Vector3 getLinearVelocity();
        void applyImpulse(const Ogre::Vector3 &impulse, const Ogre::Vector3 &position);
        void applyForce(const Ogre::Vector3 &impulse, const Ogre::Vector3 &position);


        inline btRigidBody*         getBulletRigidBody() const;
        inline btDynamicsWorld*     getBulletDynamicsWorld() const;
        inline DynamicsWorld*       getDynamicsWorld();

        Ogre::SceneNode*  getSceneNode() const {return mRootNode;};

        inline bool isStaticObject() const;
        inline bool isKinematicObject() const;

		void setKinematicObject(bool isKinematic);

        inline void disableDeactivation();
        inline void enableActiveState ();
        inline void forceActivationState();
        void setDeactivationTime(const float ftime);

        inline const btTransform &getCenterOfMassTransform() const;
        inline Ogre::Quaternion    getCenterOfMassOrientation () const;
        inline Ogre::Vector3 getCenterOfMassPosition() const;

        Ogre::Vector3       getCenterOfMassPivot (const Ogre::Vector3 &pivotPosition) const;
        
        void setDamping( const Ogre::Real linearDamping, const Ogre::Real angularDamping );
	protected:
		short mCollisionGroup;
		short mCollisionMask;
    };
    
    // basic rigid body class
    class CORE_EXPORT WheeledRigidBody : public RigidBody
    {
    public:
		WheeledRigidBody(const Ogre::String &name, DynamicsWorld *world) : RigidBody(name, world), mVehicle(0) { };
		virtual ~WheeledRigidBody() { };

        void setVehicle(RaycastVehicle *v){ mVehicle = v; };
		virtual void setPosition(const btVector3 &pos);
		virtual void setOrientation(const btQuaternion &quat);
		virtual void setTransform(const btVector3 &pos, const btQuaternion &quat);
		virtual void setTransform(const btTransform& worldTrans);

    protected:
		RaycastVehicle *mVehicle;
    };

    // -------------------------------------------------------------------------
    // basic rigid body class inline methods
    // -------------------------------------------------------------------------
    inline btRigidBody* RigidBody::getBulletRigidBody() const 
    {
        return static_cast <btRigidBody* > (mObject);
    };
    // -------------------------------------------------------------------------
    inline btDynamicsWorld* RigidBody::getBulletDynamicsWorld() const 
    { 
        return static_cast <btDynamicsWorld * > (mWorld->getBulletCollisionWorld ());
    };
    // -------------------------------------------------------------------------
    inline DynamicsWorld* RigidBody::getDynamicsWorld() 
    { 
        return static_cast <DynamicsWorld* > (mWorld);
    };
    // -------------------------------------------------------------------------
    inline bool RigidBody::isStaticObject() const 
    {
        return getBulletRigidBody()->isStaticObject();
    }
    // -------------------------------------------------------------------------
    inline bool RigidBody::isKinematicObject() const 
    {
        return getBulletRigidBody()->isKinematicObject();
    };
    // -------------------------------------------------------------------------
    inline void RigidBody::disableDeactivation()
    {
        getBulletRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    }
    // -------------------------------------------------------------------------
    inline void RigidBody::enableActiveState()
    {
        getBulletRigidBody()->setActivationState(ACTIVE_TAG);
    }
    // -------------------------------------------------------------------------
    inline void RigidBody::forceActivationState()
    {
        getBulletRigidBody()->forceActivationState(ACTIVE_TAG);
    }
    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    inline const btTransform &RigidBody::getCenterOfMassTransform() const
    {
        return getBulletRigidBody()->getCenterOfMassTransform();
    }
    // -------------------------------------------------------------------------
    inline Ogre::Quaternion RigidBody::getCenterOfMassOrientation() const
    {
        return OgreBulletCollisions::BtOgreConverter::to(getCenterOfMassTransform().getRotation ());
    }
    // -------------------------------------------------------------------------
    inline Ogre::Vector3 RigidBody::getCenterOfMassPosition() const
    {
        return OgreBulletCollisions::BtOgreConverter::to(getBulletRigidBody()->getCenterOfMassPosition());
    }
    // -------------------------------------------------------------------------   
    
}
#endif //_OGREBULLETDYNAMICS_RigidObject_H

