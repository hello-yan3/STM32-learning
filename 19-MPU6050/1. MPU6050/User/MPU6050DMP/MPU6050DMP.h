#ifndef __MPU6050DMP_H
#define __MPU6050DMP_H

#include "stm32f1xx.h"

int8_t MPU6050_DMP_Init(void);
int8_t MPU6050_DMP_Get_Data(void);

#endif /* __MPU6050DMP_H */
