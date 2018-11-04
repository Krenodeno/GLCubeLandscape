#include "FreeFlyCamera.hpp"

FlyCamera::FlyCamera()
	: pitch(0.f), yaw(0.f), roll(0.f), eye(10.f, 10.f, 10.f), up(0.f, 1.f, 0.f)
{
}


FlyCamera::~FlyCamera()
{
}

void FlyCamera::move(Vector step)
{
	Vector forward = view().inverse()[2];
	Vector strafe = view().inverse()[0];
	eye = eye + ((forward * step.z) + (strafe * step.x));
}

void FlyCamera::translate(Vector step)
{
	eye = eye + step;
}

void FlyCamera::rotate(Vector rotation)
{
	yaw = yaw + rotation.x;
	pitch = pitch + rotation.y;
	roll = roll + rotation.z;
}

Transform FlyCamera::view() const
{
	return RotationX(pitch) * RotationY(yaw) * Translation(-Vector(eye));
}

Point FlyCamera::position() const
{
	return eye;
}
