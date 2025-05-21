/*
 * median.c
 *
 *  Created on: 20 Sep 2021
 *      Author: raffael
 */


/*
 *  This Quickselect routine is based on the algorithm described in
 *  "Numerical recipes in C", Second Edition,
 *  Cambridge University Press, 1992, Section 8.5, ISBN 0-521-43108-5
 *  This code by Nicolas Devillard - 1998. Public domain.
 */

#include "../statistics/statistics.h"

#include <string.h>
#include "math.h"


#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }

unsigned int median_index(elem_type* input_arr, int n, elem_type* median_value)
{
    int low, high ;
    int median;
    int middle, ll, hh;

    // create a copy of the input array:
    elem_type arr[n];
    memcpy(arr, input_arr, n*sizeof(elem_type));

    // create an index array of the same length like the input array:
    unsigned int indices[n];
    for(int i = 0; i<n; i++)
    {
    	indices[i] = i;
    }

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
        {
        	*median_value = arr[median];
            return indices[median] ;
        }
        if (high == low + 1) {  /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;

        	*median_value = arr[median];
            return indices[median] ;
        }

    /* Find median of low, middle and high items; swap into position low */
    middle = (low + high) / 2;
    if (arr[middle] > arr[high])    {ELEM_SWAP(arr[middle], arr[high]);	ELEM_SWAP(indices[middle], indices[high]) ;}
    if (arr[low] > arr[high])       {ELEM_SWAP(arr[low], arr[high]); 	ELEM_SWAP(indices[low], indices[high]) ;}
    if (arr[middle] > arr[low])     {ELEM_SWAP(arr[middle], arr[low]);	ELEM_SWAP(indices[middle], indices[low]) ;}

    /* Swap low item (now in position middle) into position (low+1) */
    ELEM_SWAP(arr[middle], arr[low+1]) ;

    /* Nibble from each end towards middle, swapping items when stuck */
    ll = low + 1;
    hh = high;
    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_SWAP(arr[ll], arr[hh]) ;
        ELEM_SWAP(indices[ll], indices[hh]) ;

    }

    /* Swap middle item (in position low) back into correct position */
    ELEM_SWAP(arr[low], arr[hh]) ;
    ELEM_SWAP(indices[low], indices[hh]) ;

    /* Re-set active partition */
    if (hh <= median)
        low = ll;
    if (hh >= median)
        high = hh - 1;
    }
}

float calculateSD(elem_type data[], unsigned int length, float mean)
{
    float SD = 0.0;
    int i;

    for (i = 0; i < length; ++i) {
        SD += pow(data[i] - mean, 2);
    }
    return sqrt(SD / 10);
}

double linear_regression(double x[], double y[], unsigned int n, double* result_slope, double* result_intercept)
{
	double sumX=0, sumX2=0, sumY=0, sumY2=0, sumXY=0, a, b, r2;

	 for(int i=0;i<n;i++)
	  {
		   sumX = sumX + x[i];
		   sumX2 = sumX2 + x[i]*x[i];
		   sumY = sumY + y[i];
		   sumY2 = sumY2 + y[i]*y[i];
		   sumXY = sumXY + x[i]*y[i];
	  }
	  /* Calculating a and b */
	  b = (n*sumXY-sumX*sumY)/(n*sumX2-sumX*sumX);
	  a = (sumY - b*sumX)/n;

	  r2 = fabs((sumXY * n - sumX * sumY) / sqrt((sumX2 * n - sumX * sumX) * (sumY2 * n - sumY * sumY)));

	  *result_slope = b;
	  *result_intercept = a; //todo-ok: verify why a & b inverted!
	  return r2;
}

#undef ELEM_SWAP
