//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#include <Frustum.h>
#include <behaviors/CameraBehavior.h>
#include <Object.h>

using namespace Neo;

static const int pointsId[4][2] = {
	{1, 0}, {2, 1}, {3, 2}, {0, 3}
};

void Frustum::makeVolume(const CameraBehavior& camera)
{
	auto cameraObject = camera.getParent();
	int wPoints2d[4][2];

	wPoints2d[0][0] = camera.getX();
	wPoints2d[0][1] = camera.getY();
	wPoints2d[1][0] = camera.getX() + camera.getWidth();
	wPoints2d[1][1] = camera.getY();
	wPoints2d[2][0] = camera.getX() + camera.getWidth();
	wPoints2d[2][1] = camera.getY() + camera.getHeight();
	wPoints2d[3][0] = camera.getX();
	wPoints2d[3][1] = camera.getY() + camera.getHeight();

	// front normal
	m_direction = cameraObject->getTransform().getRotatedVector3(Vector3(0, 0, -1)).getNormalized();

	// compute volume points
	Vector3 position = cameraObject->getPosition();

	float zNear = camera.getNear();
	float zFar = camera.getFar();
	m_nearPoint = position + (m_direction * zNear);
	m_farPoint = position + (m_direction * zFar);

	int i;
	Vector3 vec;
	Vector3 p3d(0, 0, 0);
	for (i = 0; i < 4; i++)
	{
		p3d.x = (float) wPoints2d[i][0];
		p3d.y = (float) wPoints2d[i][1];

		vec = camera.getUnProjectedPoint(p3d);
		m_points[i] = vec;
	}

	// compute normals (perspective)
	if(true) //!camera->isOrtho())
	{
		for (i = 0; i < 4; i++)
		{
			m_normals[i] = getTriangleNormal(
				position,
				m_points[pointsId[i][0]],
				m_points[pointsId[i][1]]
			);
		}

		return;
	}
	else
	{
		// compute normals (ortho)
		m_normals[0] = cameraObject->getTransform().getRotatedVector3(Vector3(1, 0, 0)).getNormalized();
		m_normals[2] = -m_normals[0];
		m_normals[1] = cameraObject->getTransform().getRotatedVector3(Vector3(0, 1, 0)).getNormalized();
		m_normals[3] = -m_normals[1];
	}
}

bool Frustum::isVolumePointsVisible(Vector3* points, unsigned int pointsNumber) const
{
	bool out = true;
	unsigned int p;

	// out of far plane
	for (p = 0; p < pointsNumber; p++)
	{
		if ((points[p] - m_farPoint).dotProduct(m_direction) <= 0)
		{
			out = false;
			break;
		}
	}

	if (out) return false;

	// out of near plane
	out = true;
	for (p = 0; p < pointsNumber; p++)
	{
		// near
		if ((points[p] - m_nearPoint).dotProduct(m_direction) > 0)
		{
			out = false;
			break;
		}
	}

	if (out) return false;

	// out of camera planes
	for (int i = 0; i < 4; i++)
	{
		const Vector3& origin = m_points[pointsId[i][1]];

		out = true;
		for (p = 0; p < pointsNumber; p++)
		{
			float dot = (points[p] - origin).dotProduct(m_normals[i]);
			if (dot > 0)
			{
				out = false;
				break;
			}
		}

		if (out) return false;
	}

	return true;
}

Box3D Frustum::getBoundingBox() const
{
	Box3D result;
	// TODO
	
	return result;
}

float Frustum::getRadius() const
{
	return (m_farPoint - m_nearPoint).getLength();
}

Vector4 Frustum::getSphere() const
{
	Vector4 result;
	float halfRadius = getRadius()/2.0f;
	
	result = m_nearPoint + m_direction * halfRadius;
	result.w = halfRadius;
	
	return result;
}
