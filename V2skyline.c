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
	//  printPoint(points[i], dim);

	/**********************************************************************************
	 * Work here
	 * *******************************************************************************/
	// idea: map

	omp_set_num_threads(thread_number);

	long long compare = 0;
	permissiblePoints = (Point *)malloc(sizeof(Point) * number);
	int **map = (int **)malloc(sizeof(int *) * 2);
	int *target = (int *)malloc(sizeof(int) * dim);
	int flag0 = 0, flag1 = 1;
	int *mpointer = (int *)malloc(sizeof(int) * 2);
	mpointer[0] = number;
	mpointer[1] = 0;
#pragma omp parallel for
	for (int j = 0; j < 2; ++j)
	{
		map[j] = (int *)malloc(sizeof(int) * number);
#pragma omp simd
		for (int i = 0; i < number; ++i)
		{
			map[j][i] = i;
		}
	}
#pragma omp simd
	for (int i = 0; i < dim; ++i)
	{
		target[i] = 0;
	}

	while (mpointer[flag0])
	{
// printf("pointer %d %d\n", mpointer[flag0], mpointer[flag1]);
// (2) target smallest dim
#pragma omp parallel for
		for (int i = 0; i < dim; ++i)
		{

			target[i] = 0;
			for (int j = 0; j < mpointer[flag0]; ++j)
			{
				// printf("%d ", map[flag0][j]);
				if (points[map[flag0][j]].values[i] < points[map[flag0][target[i]]].values[i])
				{
					target[i] = j;
				}
			}
			// printf("\n");
		}

		// (3) scan map

		// #pragma omp parallel
		for (int i = 0; i < dim; ++i)
		{
			// printf("target%d: ", target[i]);
			int good = 1;
#pragma omp parallel for
			for (int j = 0; j < mpointer[flag0]; ++j)
			{
				int better = 0;
				int worse = 0;
				int ptr;
				if (map[flag0][j] > -1)
				{
#pragma omp simd reduction(+ \
						   : worse)
					for (ptr = 0; ptr < dim; ptr++)
					{
						worse += points[map[flag0][j]].values[ptr] > points[map[flag0][target[i]]].values[ptr];
					}
#pragma omp simd reduction(+ \
						   : better)
					for (ptr = 0; ptr < dim; ptr++)
					{
						better += points[map[flag0][j]].values[ptr] < points[map[flag0][target[i]]].values[ptr];
					}
					if (better == 0 && worse > 0)
					{
						// printf("rm %d", map[flag0][j]);
						map[flag0][j] = -1;
					}
					else if (worse == 0 && better > 0 && target[i] != j)
					{
						good = 0;
						// printf("target %d is worse than %d", target[i], j);
					}
				}
				// printf("c ");
				// ++compare;
			}
			for (int j = 0; j < i; ++j)
			{
				if (target[i] == target[j])
				{
					good = 0;
				}
			}

			// (3) generate answer
			if (good)
			{

				permissiblePoints[permissiblePointNum] = points[map[flag0][target[i]]];
				permissiblePointNum++;
				// printf("ad %d ", map[flag0][target[i]]);
			}
			// printf("\n");
		}

// delete target
#pragma omp prarllel for
		for (int i = 0; i < dim; ++i)
		{
			map[flag0][target[i]] = -1;
			// printf("%d ", target[i]);
		}
		// printf("-> deleted\n");

		// update map
		for (int i = 0; i < mpointer[flag0]; ++i)
		{
			if (map[flag0][i] > -1)
			{
				map[flag1][mpointer[flag1]] = map[flag0][i];
				// printf("%d ", map[flag1][mpointer[flag1]]);
				++mpointer[flag1];
			}
		}
		// printf("\n");

		flag0 = flag0 == 0 ? 1 : 0;
		flag1 = flag1 == 1 ? 0 : 1;

		mpointer[flag1] = 0;
	}

	printf("total opt of cmp %lld\n", compare);
	// printf("total added %d \n", permissiblePointNum);
	free(target);
	free(map);

	// pPermissiblePoints -- your computed answer
	*pPermissiblePoints = permissiblePoints;
	return permissiblePointNum;
}
