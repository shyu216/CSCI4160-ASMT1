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
	long long c = 0;
	int stkptr = 0;
	int goahead = 1, goback = 2;
	int *stack = (int *)malloc(sizeof(int) * number);
	int *target = (int *)malloc(sizeof(int) * number);
#pragma omp simd
	for (int i = 0; i < number; i++)
	{
		target[i] = 1;
	}

	// (2) stack
	stack[stkptr] = 0;
	while (stack[stkptr] + goahead < number)
	{
		c++;
		int smaller = 0;
		int larger = 0;
#pragma omp simd reduction(+ \
						   : smaller)
		for (int cmp = 0; cmp < dim; cmp++)
		{
			smaller += points[stack[stkptr]].values[cmp] > points[stack[stkptr] + goahead].values[cmp];
		}
#pragma omp simd reduction(+ \
						   : larger)
		for (int cmp = 0; cmp < dim; cmp++)
		{
			larger += points[stack[stkptr]].values[cmp] < points[stack[stkptr] + goahead].values[cmp];
		}
		if (smaller && larger)
		{
			// printf("add %d to stack\n", stack[stkptr ] + goahead);
			stkptr++;
			stack[stkptr] = stack[stkptr - 1] + goahead;

			goahead = 1;
		}
		else if (smaller)
		{
			// printf("switch %d to %d\n", stack[stkptr ], stack[stkptr ] + goahead);
			target[stack[stkptr]] = 0;
			stack[stkptr] += goahead;
			goahead = 1;
		}
		else if (larger)
		{
			// printf("%d is gone because %d\n", stack[stkptr - 1] + goahead, stack[stkptr - 1]);
			target[stack[stkptr] + goahead] = 0;
			goahead++;
		}
	}

	while (stkptr)
	{
		c++;
		if (stkptr - goback < 0)
		{
			stkptr--;
			goback = 2;
		}
		else if (target[stack[stkptr - goback]])
		{
			int smaller = 0;
			int larger = 0;
#pragma omp simd reduction(+ \
						   : smaller)
			for (int cmp = 0; cmp < dim; cmp++)
			{
				smaller += points[stack[stkptr - 1]].values[cmp] > points[stack[stkptr - goback]].values[cmp];
			}
#pragma omp simd reduction(+ \
						   : larger)
			for (int cmp = 0; cmp < dim; cmp++)
			{
				larger += points[stack[stkptr - 1]].values[cmp] < points[stack[stkptr - goback]].values[cmp];
			}
			if (smaller && larger)
			{
				// printf("goback\n");
				goback++;
			}
			else if (smaller)
			{
				// printf("%d is gone because %d\n", stack[stkptr - 1], stack[stkptr - goback]);
				target[stack[stkptr - 1]] = 0;
				stkptr--;
				goback = 2;
			}
			else if (larger)
			{
				// printf("%d is gone because %d\n", stack[stkptr - goback], stack[stkptr - 1]);
				target[stack[stkptr - goback]] = 0;
				// stack[stkptr - goback] = stack[stkptr - 1];
				// stkptr--;
				goback++;
			}
		}
		else
		{
			goback++;
		}
	}
	printf("operation %lld\n", c);

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
