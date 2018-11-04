#ifndef FREE_FLY_CAMERA_H
#define FREE_FLY_CAMERA_H

#include "vec.h"
#include "mat.h"

/* A free fly camera is flying around at will */
class FlyCamera
{
public:
	FlyCamera();
	~FlyCamera();

	/* step is in screen coordinates, x axis is right/left, y is up/down, z is forward/backward*/
	void move(Vector step);
	/* step is in world coordinates */
	void translate(Vector step);
	/* rotating the camera on itself, rotation is in screen coordinates */
	void rotate(Vector rotation);

	Transform view() const;

	Point position() const;

private:
	float pitch;	// how much you are looking up or down
	float yaw;		// how much you are looking left or right
	float roll;		// how much you are rolling your view

	Point eye;		// eye's position (in world coordinates)
	Vector up;		// direction to up (in world coordinates)
};

#endif // FREE_FLY_CAMERA_H
