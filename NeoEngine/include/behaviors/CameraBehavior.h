#ifndef NEO_CAMERABEHAVIOR_H
#define NEO_CAMERABEHAVIOR_H

#include <Behavior.h>
#include <Matrix4x4.h>

namespace Neo
{

class CameraBehavior : public Behavior
{
	Matrix4x4 m_projectionMatrix, m_viewMatrix;
	float fov = 60.0f, near = 0.1f, far = 1000.0f, m_width, m_height,
	// The position on the screen when using a custom viewport
	m_screenX = 0, m_screenY = 0;
public:
	const char* getName() const override { return "Camera"; }
	Behavior* getNew() const override { return new CameraBehavior; }

	CameraBehavior()
	{
		REGISTER_PROPERTY(fov);
		REGISTER_PROPERTY(near);
		REGISTER_PROPERTY(far);
	}
	
	CameraBehavior(const CameraBehavior& l):
		CameraBehavior()
	{
		*this = l;
	}
	
	CameraBehavior& operator=(const CameraBehavior& l)
	{
		fov = l.fov;
		near = l.near;
		far = l.far;
		
		m_projectionMatrix = l.m_projectionMatrix;
		m_viewMatrix = l.m_viewMatrix;
		m_width = l.m_width;
		m_height = l.m_height;
		m_screenX = l.m_screenX;
		m_screenY = l.m_screenY;
		
		return *this;
	}
	
	void setPerspectiveView(float fov, float ratio, float zNear, float zFar);
	void setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar);
	void enable(float width, float height);
	
	Vector3 getProjectedPoint(const Vector3 & point) const;
	Vector3 getUnProjectedPoint(const Vector3 & point) const;
	
	const Matrix4x4& getProjectionMatrix() { return m_projectionMatrix; }
	Matrix4x4 getViewMatrix() { return m_viewMatrix; }
	
	void setFov(float fov) { fov = fov; }
	float getFov() const { return fov; }
	
	void setNear(float value) { near = value; }
	float getNear() const { return near; }
	
	void setFar(float value) { far = value; }
	float getFar() const { return far; }
	
	void serialize(std::ostream& out) override;
	void deserialize(Level&, std::istream& out) override;
};

}

#endif // NEO_CAMERABEHAVIOR_H
