#include "PlatformRigidbodyPhysicsBehavior.h"
#include <behaviors/MeshBehavior.h>
#include <Level.h>
#include <Log.h>

using namespace Neo;

static btVector3 convert(const Vector3& v)
{
	return btVector3(v.x, v.y, v.z);
}

static btQuaternion convert(const Quaternion& q)
{
	return btQuaternion(q.values[0], 
				q.values[1], 
				q.values[2], 
				q.values[3]);
}

static Vector3 convert(const btVector3& v)
{
	return Vector3(v.x(), v.y(), v.z());
}

static Quaternion convert(const btQuaternion& q)
{
	return Quaternion(q.x(), 
				q.y(), 
				q.z(), 
				q.w());
}

void PlatformRigidbodyPhysicsBehavior::propertyChanged(IProperty*)
{
	setMass(Mass);
	setShape(Shape);
	setLinearFactor(LinearFactor);
	setAngularFactor(AngularFactor);
	setFriction(Friction);
	setLinearDamping(LinearDamping);
	setAngularDamping(AngularDamping);
	setKinematic(Kinematic);
}

void PlatformRigidbodyPhysicsBehavior::begin(Level& level)
{
	Object* parent = getParent();

	// Reserve some memory for collision
	m_contacts.reserve(16);
	
	const Quaternion parentRotation = parent->getRotation();
	const Vector3 parentPosition = parent->getPosition();
	
	btQuaternion rotation(convert(parentRotation));
    
	btVector3 position = convert(parentPosition);
    	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, position));
	
	// mass != 0 to force a dynamic object!
	m_btbody = new btRigidBody(Mass, motionState, nullptr);
	m_btbody->setSleepingThresholds(0.2f, 0.2f);

	// Sets all properties without looking at the argument
	propertyChanged(nullptr);
	
	m_btbody->setUserPointer(this);
	
	m_physics = &level.getPhysicsContext();
	btDiscreteDynamicsWorld* world = m_physics->getWorld();
	world->addRigidBody(m_btbody);
}

void PlatformRigidbodyPhysicsBehavior::end()
{
	delete m_btshape;
	m_btshape = nullptr;
}

void Neo::PlatformRigidbodyPhysicsBehavior::update(Platform& p, float dt)
{
	if(!m_physics->isEnabled())
		return;
	
	Object* parent = getParent();
	if(Kinematic)
	{
		const Vector3 position = parent->getPosition();
		const Quaternion rotation = parent->getRotation();
		
		btTransform& transform = m_btbody->getWorldTransform();
		transform.setOrigin(convert(position));
		transform.setRotation(convert(rotation));
		
		return;
	}
		
	const btTransform transform = m_btbody->getWorldTransform();
	const btVector3 position = transform.getOrigin();
	const btQuaternion rotation = transform.getRotation();
	
	parent->setPosition(convert(position));
	parent->setRotation(convert(rotation));
	parent->updateMatrix();
}

void PlatformRigidbodyPhysicsBehavior::setShape(COLLISION_SHAPE shape)
{
	Shape = shape;
	
	if(!m_btbody)
		return;
	
	if(m_btshape)
	{
		delete m_btshape;
	}
	
	Object* parent = getParent();
	MeshBehavior* mesh = parent->getBehavior<MeshBehavior>();
	
	assert(mesh && "Need a mesh for collision handling!");
	
	switch(shape)
	{
		case SPHERE_SHAPE:
		{
			mesh->updateBoundingBox();
			const AABB aabb = mesh->getBoundingBox();
			m_btshape = new btSphereShape(aabb.diameter*0.5f); 
		}
		break;
			
		default:
		case BOX_SHAPE:
		{
			mesh->updateBoundingBox();
			const AABB aabb = mesh->getBoundingBox();
			const Vector3 halfExtends = (aabb.max - aabb.min) * 0.5f;
					
			m_btshape = new btBoxShape(convert(halfExtends));
		}
		break;
	}
	
	m_btbody->setCollisionShape(m_btshape);
}

void PlatformRigidbodyPhysicsBehavior::setMass(float mass)
{
	Mass = mass;
	
	if(!m_btshape || !m_btbody)
		return;

	btVector3 inertia;
	m_btshape->calculateLocalInertia(mass, inertia);
	m_btbody->setMassProps(mass, inertia);
	
	LOG_INFO(m_btbody->isActive());
	m_btbody->activate(true);
	
	if(mass)
		m_btbody->setCollisionFlags(m_btbody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
}

void PlatformRigidbodyPhysicsBehavior::setKinematic(bool value)
{
	Kinematic = value;
	
	if(m_btbody)
	{
		if(value)
			m_btbody->setCollisionFlags(m_btbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		else
			m_btbody->setCollisionFlags(m_btbody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	}
}

void PlatformRigidbodyPhysicsBehavior::setLinearFactor(const Vector3& factor)
{
	LinearFactor = factor;
	if(m_btbody)
	{
		m_btbody->setLinearFactor(convert(factor));
	}
}

void PlatformRigidbodyPhysicsBehavior::setAngularFactor(const Vector3& factor)
{
	AngularFactor = factor;
	if(m_btbody)
	{
		m_btbody->setAngularFactor(convert(factor));
	}
}

void PlatformRigidbodyPhysicsBehavior::setFriction(float value)
{
	Friction = value;
	if(m_btbody)
	{
		m_btbody->setFriction(value);
	}
}

void PlatformRigidbodyPhysicsBehavior::setLinearDamping(float value)
{
	LinearDamping = value;
	if(m_btbody)
	{
		m_btbody->setDamping(LinearDamping, AngularDamping);
	}
}

void PlatformRigidbodyPhysicsBehavior::setAngularDamping(float value)
{
	AngularDamping = value;
	if(m_btbody)
	{
		m_btbody->setDamping(LinearDamping, AngularDamping);
	}
}

void PlatformRigidbodyPhysicsBehavior::addCentralForce(const Vector3& force)
{
	m_btbody->applyCentralForce(convert(force));
}

void PlatformRigidbodyPhysicsBehavior::addTorque(const Vector3& torque)
{
	m_btbody->applyTorque(convert(torque));
}

void PlatformRigidbodyPhysicsBehavior::clearForces()
{
	m_btbody->clearForces();
}

Vector3 PlatformRigidbodyPhysicsBehavior::getCentralForce() const
{
	return convert(m_btbody->getTotalForce());
}

Vector3 PlatformRigidbodyPhysicsBehavior::getTorque() const
{
	return convert(m_btbody->getTotalTorque());
}
