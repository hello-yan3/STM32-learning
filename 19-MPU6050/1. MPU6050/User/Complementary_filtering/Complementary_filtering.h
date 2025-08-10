#ifndef __FILTER_H
#define __FILTER_H

#include "stm32f1xx.h"

float Complementary_Filter_x(float angle_m, float gyro_m);
float Complementary_Filter_y(float angle_m, float gyro_m);

#endif /* __FILTERING_H */
