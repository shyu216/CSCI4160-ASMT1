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

	// the following for-loop iterates the first 20 points that will be inputted by runtest.compare
	// for (int i = 0; i < 20; i++)
	// // printPoint(points[i], dim);

	/**********************************************************************************
	 * Work here
	 * *******************************************************************************/
	// idea: map

	omp_set_num_threads(thread_number);

	// (1) preparation
	long long compare = 0, condi = 0;
	int head = 0, tail = 0;
	int mptr = 0;
	int *target = (int *)malloc(sizeof(int) * number);
	int *map = (int *)malloc(sizeof(int) * number);
#pragma omp simd
	for (int i = 0; i < number; ++i)
	{
		target[i] = 0;
	}
#pragma omp simd
	for (int i = 0; i < number; ++i)
	{
		map[i] = 1;
	}
	permissiblePoints = (Point *)malloc(sizeof(Point) * number);

	do
	{
		// (2) target
		for (int i = 0; i < dim; ++i)
		{
#pragma omp parallel for
			for (int j = 0; j < number; ++j)
			{
				if (map[j])
				{
					if (points[j].values[i] == points[target[tail]].values[i])
					{
						++tail;
						target[tail] = j;
						// printPoint(points[j], dim);
					}
					else if (points[j].values[i] < points[target[tail]].values[i])
					{
						tail = head;
						target[tail] = j;
						// printPoint(points[j], dim);
					}
				}
			}
			++tail;
			head = tail;
		}
		printf("tail is %d ", tail);

		// (3) map
		for (int i = 0; i < tail; ++i)
		{
#pragma omp parallel for
			for (int j = 0; j < number; ++j)
			{
				if (map[j])
				{
					int better = 0;
					int worse = 0;
					int ptr;
#pragma omp simd reduction(+ \
						   : worse)
					for (ptr = 0; ptr < dim; ptr++)
					{
						worse += points[j].values[ptr] >= points[target[i]].values[ptr];
					}
#pragma omp simd reduction(+ \
						   : better)
					for (ptr = 0; ptr < dim; ptr++)
					{
						better += points[j].values[ptr] <= points[target[i]].values[ptr];
					}
					if (worse == dim && better < dim)
					{
						map[j] = 0;
						++mptr;
						// printf("byebye ");
						// printPoint(points[j], dim);
					}
					++condi;
				}
				++compare;
			}
			// printf("tail %d, target %d\n", i, target[i]);
		}
		printf("compare is %lld, condi is %lld, mptr is %d\n", compare, condi, mptr);

		// (3) generate answer
		for (int i = 0; i < tail; i++)
		{
			if (map[target[i]])
			{
				permissiblePoints[permissiblePointNum] = points[target[i]];
				// printPoint(permissiblePoints[permissiblePointNum], dim);
				permissiblePointNum++;
				map[target[i]] = 0;
				++mptr;
				// printf("byebye ");
				// printPoint(points[target[i]], dim);
			}
			target[i] = 0;
		}

		head = 0;
		tail = 0;
		// printf("permissiblePointNum: %d\n", permissiblePointNum);
	} while (mptr < number);

	free(target);
	free(map);
	printf("compare is %lld\n", compare);

	// pPermissiblePoints -- your computed answer
	*pPermissiblePoints = permissiblePoints;
	return permissiblePointNum;
}
