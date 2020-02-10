#include "behaviors/CameraBehavior.h"
#include <iostream>
#include <Object.h>

using namespace Neo;

REGISTER_BEHAVIOR(CameraBehavior)

void CameraBehavior::setPerspectiveView(float fov, float ratio, float zNear, float zFar)
{
	float ymax, xmax;
	ymax = zNear * tanf((float)(fov * M_PI / 360.0f));
	xmax = ymax * ratio;

	float left = -xmax;
	float right = xmax;
	float bottom = -ymax;
	float top = ymax;

	float temp, temp2, temp3, temp4;
	temp = 2.0f * zNear;
	temp2 = right - left;
	temp3 = top - bottom;
	temp4 = zFar - zNear;
	m_projectionMatrix.entries[0] = temp / temp2;
	m_projectionMatrix.entries[1] = 0.0f;
	m_projectionMatrix.entries[2] = 0.0f;
	m_projectionMatrix.entries[3] = 0.0f;
	m_projectionMatrix.entries[4] = 0.0f;
	m_projectionMatrix.entries[5] = temp / temp3;
	m_projectionMatrix.entries[6] = 0.0f;
	m_projectionMatrix.entries[7] = 0.0f;
	m_projectionMatrix.entries[8] = (right + left) / temp2;
	m_projectionMatrix.entries[9] = (top + bottom) / temp3;
	m_projectionMatrix.entries[10] = (-zFar - zNear) / temp4;
	m_projectionMatrix.entries[11] = -1.0f;
	m_projectionMatrix.entries[12] = 0.0f;
	m_projectionMatrix.entries[13] = 0.0f;
	m_projectionMatrix.entries[14] = (-temp * zFar) / temp4;
	m_projectionMatrix.entries[15] = 0.0f;
}

void CameraBehavior::setOrthoView(float left, float right, float bottom, float top, float zNear, float zFar)
{
	// TODO Error reporting!
	if(right == left || top == bottom || zFar == zNear)
		return;

	float tx = - (right + left)/(right - left);
	float ty = - (top + bottom)/(top - bottom);
	float tz = - (zFar + zNear)/(zFar - zNear);

	m_projectionMatrix = Matrix4x4(
		2.0f/(right-left), 0.0f, 0.0f, 0.0f,
			  0.0f, 2.0f/(top-bottom), 0.0f, 0.0f,
			  0.0f, 0.0f, -2.0f/(zFar-zNear), 0.0f,
			  tx, ty, tz, 1.0f
	);
}

void CameraBehavior::enable(float width, float height) 
{
	if(m_lockPerspective)
		return;
	
	m_width = width;
	m_height = height;
	
	Object* parent = getParent();
	float ratio = width/height;

	Matrix4x4 inverseScale;
	Vector3 scale = parent->getTransform().getScale();
	inverseScale.setScale(Vector3(1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z));
	
	m_viewMatrix = (parent->getTransform() * inverseScale).getInverse();
	setPerspectiveView(fov, ratio, nearPlane, farPlane);
}

Vector3 CameraBehavior::getProjectedPoint(const Vector3 & point) const
{
	Vector4 v = m_viewMatrix * Vector4(point);
	v = m_projectionMatrix * v;
	v.x = v.x / v.w;
	v.y = v.y / v.w;
	v.z = v.z / v.w;
	
	v.x = m_screenX + (m_width * ((v.x + 1) / 2.0f));
	v.y = m_screenY + (m_height * ((v.y + 1) / 2.0f));
	v.z = (v.z + 1) / 2.0f;

	return Vector3(v.x, v.y, v.z);
}

Vector3 CameraBehavior::getUnProjectedPoint(const Vector3 & point) const
{
	Vector4 nPoint;

	nPoint.x = (2 * ((point.x - m_screenX) / m_width)) - 1;
	nPoint.y = (2 * ((point.y - m_screenY) / m_height)) - 1;
	nPoint.z = (2.0f * point.z) - 1.0f;
	nPoint.w = 1.0f;

	Matrix4x4 matrix = (m_projectionMatrix * m_viewMatrix).getInverse();
	Vector4 v = matrix * nPoint;

	if(v.w == 0.0f)
		return getParent()->getPosition();
	
	float iw = 1.0f / v.w;
	return Vector3(v.x, v.y, v.z)*iw;
}

Frustum CameraBehavior::getFrustum() const
{
	Frustum f;
	f.makeVolume(*this);
	return f;
}

void CameraBehavior::serialize(std::ostream& out)
{
}

void CameraBehavior::deserialize(Level&, std::istream& in)
{
}
