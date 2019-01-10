#include "PlatformPhysicsContext.h"
#include <Log.h>

#include <Object.h>
#include <PlatformRigidbodyPhysicsBehavior.h>

using namespace Neo;

void PlatformPhysicsContext::begin()
{
	LOG_INFO("Initializing Bullet3.");
	m_broadphase = new btDbvtBroadphase();
	m_collisionConfig = new btDefaultCollisionConfiguration();
	m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfig);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_collisionConfig);

	m_world->setGravity(btVector3(0.0f, 0.0f, -9.81f));
}

void PlatformPhysicsContext::update(float dt)
{
	m_world->stepSimulation(dt, 1, dt);

	for(unsigned int i = 0; i < m_world->getNumCollisionObjects(); i++)
	{
		auto* obj = m_world->getCollisionObjectArray()[i];
		auto* behavior = reinterpret_cast<PlatformRigidbodyPhysicsBehavior*>(obj->getUserPointer());
		
		if(behavior)
			behavior->getContacts().clear();
	}
	
	btDispatcher* dispatcher = m_world->getDispatcher();
	size_t numManifolds = m_world->getDispatcher()->getNumManifolds();
	for (size_t i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);
		const int numContacts = contactManifold->getNumContacts();
		
		if (numContacts > 0)
		{
			const btCollisionObject* objA = contactManifold->getBody0();
			const btCollisionObject* objB = contactManifold->getBody1();
			
			PlatformRigidbodyPhysicsBehavior* behaviorA = (PlatformRigidbodyPhysicsBehavior*) objA->getUserPointer();
			PlatformRigidbodyPhysicsBehavior* behaviorB = (PlatformRigidbodyPhysicsBehavior*) objB->getUserPointer();
			
			if(behaviorA && behaviorB)
			{
				auto& contactsA = behaviorA->getContacts();
				auto& contactsB = behaviorB->getContacts();

				contactsA.push_back(behaviorB->getParent()->getSelf());
				contactsB.push_back(behaviorA->getParent()->getSelf());
			}
		}
	}
}

void PlatformPhysicsContext::end()
{
	LOG_INFO("Cleaning Bullet3 up.");
	delete m_broadphase;
	delete m_collisionConfig;
	delete m_solver;
	delete m_world;

	m_broadphase = nullptr;
	m_collisionConfig = nullptr;
	m_collisionDispatcher = nullptr;
	m_solver = nullptr;
	m_world = nullptr;
}
