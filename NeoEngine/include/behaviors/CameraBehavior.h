#ifndef NEO_CAMERABEHAVIOR_H
#define NEO_CAMERABEHAVIOR_H

#include <Behavior.h>
#include <Matrix4x4.h>

namespace Neo
{

class CameraBehavior : public Behavior
{
	Matrix4x4 m_projectionMatrix, m_viewMatrix;
	float m_fov = 60.0f, m_near = 0.1f, m_far = 1000.0f, m_width, m_height,	
	// The position on the screen when using a custom viewport
	m_screenX = 0, m_screenY = 0;
public:
	const char* getName() const override { return "Camera"; }
	Behavior* getNew() const override { return new CameraBehavior; }

	void setPerspectiveView(float fov, float ratio, float zNear, float zFar);
	void setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar);
	void enable(float width, float height);
	
	Vector3 getProjectedPoint(const Vector3 & point) const;
	Vector3 getUnProjectedPoint(const Vector3 & point) const;
	
	const Matrix4x4& getProjectionMatrix() { return m_projectionMatrix; }
	Matrix4x4 getViewMatrix() { return m_viewMatrix; }
	
	void setFov(float fov) { m_fov = fov; }
	float getFov() const { return m_fov; }
	
	void setNear(float value) { m_near = value; }
	float getNear() const { return m_near; }
	
	void setFar(float value) { m_far = value; }
	float getFar() const { return m_far; }
	
	void serialize(std::ostream& out) override;
	void deserialize(Level&, std::istream& out) override;
};

}

#endif // NEO_CAMERABEHAVIOR_H
