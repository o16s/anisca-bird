/*
 * median.h
 *
 *  Created on: 20 Sep 2021
 *      Author: raffael
 */

#ifndef STATISTICS_STATISTICS_H_
#define STATISTICS_STATISTICS_H_

#include <stdint.h>

#define elem_type	int32_t

unsigned int median_index(elem_type* input_arr, int n, elem_type* median_value);
float calculateSD(elem_type data[], unsigned int length, float mean);
double linear_regression(double x[], double y[], unsigned int n, double* result_slope, double* result_intercept);
#endif /* STATISTICS_STATISTICS_H_ */
