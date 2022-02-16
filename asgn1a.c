#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"

// If you have referenced to any source code that is not written by you
// You have to cite them here.
#include <omp.h> // header file for openMP

int asgn1a(Point *points, Point **pPermissiblePoints, int number, int dim, int thread_number)
{
	// points -- input data
	// pPermissiblePoints -- your computed answer
	// number -- number of points in dataset
	// dim -- the dimension of the dataset
	// thread_number -- the number of threads (cores) to use

	int permissiblePointNum = 0;
	Point *permissiblePoints = NULL;

	// the following for-loop iterates the first 20 points that will be inputted by runtest.c
	// for (int i = 0; i < 20; i++)
	// // printPoint(points[i], dim);

	/**********************************************************************************
	 * Work here
	 * *******************************************************************************/
	// idea: stack

	omp_set_num_threads(thread_number);

	// (1) preparation
	int *stack = (int *)malloc(sizeof(int) * number);
	int *target = (int *)malloc(sizeof(int) * number);

	// (2) stack
	int stkptr = 0;
	stack[stkptr] = 0;
	target[stkptr] = 1;
#pragma omp parallel for shared(stkptr, stack, target)
	for (int goahead = 1; goahead < number; goahead++)
	{
		int smaller = 0;
		int larger = 0;
#pragma omp simd reduction(+ \
						   : smaller)
		for (int cmp = 0; cmp < dim; cmp++)
		{
			smaller += points[stack[stkptr]].values[cmp] > points[goahead].values[cmp];
		}
#pragma omp simd reduction(+ \
						   : larger)
		for (int cmp = 0; cmp < dim; cmp++)
		{
			larger += points[stack[stkptr]].values[cmp] < points[goahead].values[cmp];
		}
#pragma omp critical
		{
#pragma omp parallel
			{
#pragma omp single
				{
#pragma omp task
					if (smaller && larger)
					{
						target[goahead] = 1;
						stack[++stkptr] = goahead;
					}
#pragma omp task
					if (smaller && !larger)
					{
						target[stack[stkptr]] = 0;
						target[goahead] = 1;
						stack[stkptr] = goahead;
					}
#pragma omp task
					if (larger && !smaller)
					{
						target[goahead] = 0;
					}
#pragma omp task
					if (!smaller && !larger)
					{
						target[goahead] = 1;
						stack[++stkptr] = goahead;
					}
				}
			}
		}
	}

	while (stkptr)
	{
		if (target[stack[stkptr]])
		{
#pragma omp parallel for shared(stkptr, stack, target)
			for (int goback = stkptr - 1; goback >= 0; goback--)
			{
				if (target[stack[goback]])
				{
					int smaller = 0;
					int larger = 0;
#pragma omp simd reduction(+ \
						   : smaller)
					for (int cmp = 0; cmp < dim; cmp++)
					{
						smaller += points[stack[stkptr]].values[cmp] > points[stack[goback]].values[cmp];
					}
#pragma omp simd reduction(+ \
						   : larger)
					for (int cmp = 0; cmp < dim; cmp++)
					{
						larger += points[stack[stkptr]].values[cmp] < points[stack[goback]].values[cmp];
					}
#pragma omp critical
					{
#pragma omp parallel
						{
#pragma omp single
							{
#pragma omp task
								if (smaller && !larger)
								{
									target[stack[stkptr]] = 0;
								}
#pragma omp task
								if (larger && !smaller)
								{
									target[stack[goback]] = 0;
								}
							}
						}
					}
				}
			}
		}
		stkptr--;
	}

	// (3) generate answer
	permissiblePoints = (Point *)malloc(sizeof(Point) * number);
	int writer = 0;

	for (writer = 0; writer < number; writer++)
	{
		if (target[writer])
		{
			permissiblePoints[permissiblePointNum] = points[writer];
			// printPoint(permissiblePoints[permissiblePointNum], dim);
			permissiblePointNum++;
		}
	}
	// printf("permissiblePointNum: %d\n", permissiblePointNum);

	free(target);
	free(stack);

	// pPermissiblePoints -- your computed answer
	*pPermissiblePoints = permissiblePoints;
	return permissiblePointNum;
}
