#ifndef NEO_CAMERABEHAVIOR_H
#define NEO_CAMERABEHAVIOR_H

#include <Behavior.h>
#include <Matrix4x4.h>

namespace Neo
{

class CameraBehavior : public Behavior
{
	Matrix4x4 m_projectionMatrix, m_viewMatrix;
	float m_fov = 60.0f, m_near = 0.1f, m_far = 1000.0f; 
public:
	const char* getName() const override { return "Camera"; }
	Behavior* getNew() const override { return new CameraBehavior; }

	void setPerspectiveView(float fov, float ratio, float zNear, float zFar);
	void setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar);
	void enable(float width, float height);
	
	const Matrix4x4& getProjectionMatrix() { return m_projectionMatrix; }
	Matrix4x4 getViewMatrix() { return m_viewMatrix; }
	
	void setFov(float fov) { m_fov = fov; }
	float getFov() const { return m_fov; }
	
	void setNear(float near) { m_near = near; }
	float getNear() const { return m_near; }
	
	void setFar(float far) { m_far = far; }
	float getFar() const { return m_far; }
};

}

#endif // NEO_CAMERABEHAVIOR_H
