#ifndef __speed_decision_h__
#define __speed_decision_h__

#include "zf_common_headfile.h"

uint8 calculate_minihigh(BoundaryData *left, BoundaryData *right);
float calculate_curvature(BoundaryData *left, BoundaryData *right);

#endif
