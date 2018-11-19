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
	
	// Used to prevent enable from changing anything -> Used in VR
	bool m_lockPerspective = false;
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
	void enable() { enable(m_width, m_height); }
	
	void lockPerspective() { m_lockPerspective = true; }
	void unlockPerspective() { m_lockPerspective = false; }
	
	Vector3 getProjectedPoint(const Vector3 & point) const;
	Vector3 getUnProjectedPoint(const Vector3 & point) const;
	
	Matrix4x4& getProjectionMatrix() { return m_projectionMatrix; }
	const Matrix4x4& getProjectionMatrix() const { return m_projectionMatrix; }
	
	Matrix4x4& getViewMatrix() { return m_viewMatrix; }
	const Matrix4x4& getViewMatrix() const { return m_viewMatrix; }
	
	void setFov(float fov) { this->fov = fov; }
	float getFov() const { return fov; }
	
	void setNear(float value) { near = value; }
	float getNear() const { return near; }
	
	void setFar(float value) { far = value; }
	float getFar() const { return far; }
	
	unsigned int getHeight() const { return m_height; }
	unsigned int getWidth() const { return m_width; }
	
	void serialize(std::ostream& out) override;
	void deserialize(Level&, std::istream& out) override;
};

}

#endif // NEO_CAMERABEHAVIOR_H
