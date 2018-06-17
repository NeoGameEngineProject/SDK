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

#include "Maths.h"
using namespace Neo;

Vector3::Vector3(const Vector4 & vec):
x(vec.x), 
y(vec.y), 
z(vec.z)
{}

Vector3::Vector3(const Vector2 & vec):
		x(vec.x), 
		y(vec.y), 
		z(0) {}

void Vector3::normalize()
{
	float length = getLength();

	if(length == 1)
		return;

	else if(length == 0)
	{
		set(0, 0, 0);
		return;
	}

	float factor = 1.0f / length;
	x *= factor;
	y *= factor;
	z *= factor;
}

Vector3 Vector3::getNormalized() const
{
	Vector3 result(*this);
	result.normalize();

	return result;
}

Vector3 Vector3::getRotatedX(double angle) const
{
	if(angle == 0.0){
		return (*this);
	}

	float sinAngle = (float)sin(angle * DEG_TO_RAD);
	float cosAngle = (float)cos(angle * DEG_TO_RAD);

	return Vector3(
		x,
		y * cosAngle - z * sinAngle,
		y * sinAngle + z * cosAngle
	);
}

void Vector3::rotateX(double angle)
{
	(*this) = getRotatedX(angle);
}

Vector3 Vector3::getRotatedY(double angle) const
{
	if(angle == 0.0){
		return (*this);
	}

	float sinAngle = (float)sin(angle * DEG_TO_RAD);
	float cosAngle = (float)cos(angle * DEG_TO_RAD);

	return Vector3(
		x * cosAngle + z * sinAngle,
		y,
		-x * sinAngle + z * cosAngle
	);
}

void Vector3::rotateY(double angle)
{
	(*this) = getRotatedY(angle);
}

Vector3 Vector3::getRotatedZ(double angle) const
{
	if(angle == 0.0){
		return (*this);
	}

	float sinAngle = (float)sin(angle * DEG_TO_RAD);
	float cosAngle = (float)cos(angle * DEG_TO_RAD);
	
	return Vector3(
		x * cosAngle - y * sinAngle,
		x * sinAngle + y * cosAngle,
		z
	);
}

void Vector3::rotateZ(double angle)
{
	(*this) = getRotatedZ(angle);
}

Vector3 Vector3::getRotatedAxis(double angle, const Vector3 & axis) const
{
	if(angle == 0.0){
		return (*this);
	}

	Vector3 u = axis.getNormalized();
	Vector3 rotMatrixRow0, rotMatrixRow1, rotMatrixRow2;

	float sinAngle = (float)sin(angle * DEG_TO_RAD);
	float cosAngle = (float)cos(angle * DEG_TO_RAD);
	float oneMinusCosAngle = 1.0f - cosAngle;

	rotMatrixRow0.x = (u.x)*(u.x) + cosAngle*(1-(u.x)*(u.x));
	rotMatrixRow0.y = (u.x)*(u.y)*(oneMinusCosAngle) - sinAngle*u.z;
	rotMatrixRow0.z = (u.x)*(u.z)*(oneMinusCosAngle) + sinAngle*u.y;

	rotMatrixRow1.x = (u.x)*(u.y)*(oneMinusCosAngle) + sinAngle*u.z;
	rotMatrixRow1.y = (u.y)*(u.y) + cosAngle*(1-(u.y)*(u.y));
	rotMatrixRow1.z = (u.y)*(u.z)*(oneMinusCosAngle) - sinAngle*u.x;
	
	rotMatrixRow2.x = (u.x)*(u.z)*(oneMinusCosAngle) - sinAngle*u.y;
	rotMatrixRow2.y = (u.y)*(u.z)*(oneMinusCosAngle) + sinAngle*u.x;
	rotMatrixRow2.z = (u.z)*(u.z) + cosAngle*(1-(u.z)*(u.z));

	return Vector3(
		dotProduct(rotMatrixRow0),
		dotProduct(rotMatrixRow1),
		dotProduct(rotMatrixRow2)
	);
}

void Vector3::rotateAxis(double angle, const Vector3 & axis)
{
	(*this) = getRotatedAxis(angle, axis);
}
