#include "behaviors/CameraBehavior.h"
#include <iostream>
#include <Object.h>

using namespace Neo;

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
	Object* parent = getParent();
	float ratio = width/height;

	Matrix4x4 inverseScale;
	Vector3 scale = parent->getTransform().getScale();
	inverseScale.setScale(Vector3(1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z));
	
	m_viewMatrix = (parent->getTransform() * inverseScale).getInverse();
	setPerspectiveView(m_fov, ratio, m_near, m_far);
}

