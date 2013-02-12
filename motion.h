#ifndef MOTION_H
#define MOTION_H

#include "sensor.h"

typedef struct rotation_component_t
{
	int32_t x, y, z;
} rotation_component;

typedef struct rotationf_component_t
{
	float x, y, z;
} rotationf_component;

typedef struct rotation_t
{
	rotationf_component angle;
	rotation_component rate;
} rotation;

extern rotation orientation;

void MotionInitialise(rotation*, sensor*);
int  MotionIsCalibrated(void);

#endif
