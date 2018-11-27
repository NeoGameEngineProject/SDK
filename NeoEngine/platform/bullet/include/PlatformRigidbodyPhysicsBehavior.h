#ifndef NEO_PLATFORMRIGIDBODYPHYSICSBEHAVIOR_H
#define NEO_PLATFORMRIGIDBODYPHYSICSBEHAVIOR_H

#include <Object.h>
#include <Behavior.h>
#include "PhysicsContext.h"

namespace Neo 
{

enum COLLISION_SHAPE
{
	BOX_SHAPE = 0,
	SPHERE_SHAPE,
	MESH_SHAPE,
	HULL_SHAPE
};

class PlatformRigidbodyPhysicsBehavior : public Neo::Behavior
{
	friend PlatformPhysicsContext;
	
	btRigidBody* m_btbody = nullptr;
	btCollisionShape* m_btshape = nullptr;
	std::vector<ObjectHandle> m_contacts;
	
	float m_mass = 1.0f, m_friction = 0.5f, m_linearDamping = 0.1f, m_angularDamping = 0.1f;
	bool m_kinematic = false;
	COLLISION_SHAPE m_shape = BOX_SHAPE;
	Vector3 m_linearFactor = Vector3(1, 1, 1), m_angularFactor = Vector3(1, 1, 1);

protected:
	std::vector<ObjectHandle>& getContacts() { return m_contacts; }

public:
	PlatformRigidbodyPhysicsBehavior()
	{
		REGISTER_PROPERTY(m_mass);
		REGISTER_PROPERTY(m_friction);
		REGISTER_PROPERTY(m_linearDamping);
		REGISTER_PROPERTY(m_angularDamping);
		REGISTER_PROPERTY(m_kinematic);
		REGISTER_PROPERTY(m_linearFactor);
		REGISTER_PROPERTY(m_angularFactor);
		REGISTER_PROPERTY(m_shape);
	}
	
	void begin(Level& level);
	void begin(Neo::Platform&, Neo::Renderer&, Level& level) override { begin(level); }
	void update(Platform& p, float dt) override;
	void end() override;
	
	void setShape(COLLISION_SHAPE shape);
	void setMass(float mass);
	void setKinematic(bool value);
	void setLinearFactor(const Vector3& factor);
	void setAngularFactor(const Vector3& factor);
	void setFriction(float value);
	void setLinearDamping(float value);
	void setAngularDamping(float value);
	
	void addCentralForce(const Vector3& force);
	void addTorque(const Vector3& torque);
	void clearForces();
	
	Vector3 getCentralForce() const;
	Vector3 getTorque() const;
	
	const std::vector<ObjectHandle>& getContacts() const { return m_contacts; }
};

}

#endif // NEO_PLATFORMRIGIDBODYPHYSICSBEHAVIOR_H
