#ifndef AXIS_ALIGNED_BOUNDING_BOX_HPP
#define AXIS_ALIGNED_BOUNDING_BOX_HPP

#include "mat.h"
#include "vec.h"

struct AABB {
	Point pMin, pMax;
};

bool visible(const Transform&, const AABB&);

bool visibleInProjected(const Transform&, const AABB&);

bool visibleInWorld(const Transform&, const AABB&);

#endif
