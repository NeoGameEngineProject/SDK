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
	PhysicsContext* m_physics = nullptr;
	
	btRigidBody* m_btbody = nullptr;
	btCollisionShape* m_btshape = nullptr;
	std::vector<ObjectHandle> m_contacts;
	
	float Mass = 0.0f, Friction = 0.5f, LinearDamping = 0.1f, AngularDamping = 0.1f;
	bool Kinematic = false;
	COLLISION_SHAPE Shape = BOX_SHAPE;
	Vector3 LinearFactor = Vector3(1, 1, 1), AngularFactor = Vector3(1, 1, 1);

protected:
	std::vector<ObjectHandle>& getContacts() { return m_contacts; }

public:
	PlatformRigidbodyPhysicsBehavior()
	{
		REGISTER_PROPERTY(Mass);
		REGISTER_PROPERTY(Friction);
		REGISTER_PROPERTY(LinearDamping);
		REGISTER_PROPERTY(AngularDamping);
		REGISTER_PROPERTY(Kinematic);
		REGISTER_PROPERTY(LinearFactor);
		REGISTER_PROPERTY(AngularFactor);
		REGISTER_PROPERTY(Shape);
	}
	
	void begin(Level& level);
	void begin(Neo::Platform&, Neo::Renderer&, Level& level) override { begin(level); }
	void update(Platform& p, float dt) override;
	void end() override;
	void propertyChanged(IProperty*) override;
	
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
