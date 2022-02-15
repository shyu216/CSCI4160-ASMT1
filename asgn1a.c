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
	// //printPoint(points[i], dim);

	/**********************************************************************************
	 * Work here
	 * *******************************************************************************/
	// idea:
	// (1) loop find most permissible in each dim
	// (2) loop find not privail
	// (3) loop generate output

	omp_set_num_threads(thread_number);

	// (1) initialize target array
	int target_min = 0;
	int *target = (int *)malloc(sizeof(int) * number);
#pragma omp simd
	for (int i = 0; i < number; i++)
	{
		target[i] = 1;
	}

	// (2) loop find min
	for (int find_min = 1; find_min < number; find_min++)
	{
		printf("cmp min to %d: ", find_min);
		int smaller = 0;
		int larger = 0;
		for (int cmp = 0; cmp < dim; cmp++)
		{
			if (points[target_min].values[cmp] > points[find_min].values[cmp])
			{
				smaller++;
				printf("s ");
			}
			if (points[target_min].values[cmp] < points[find_min].values[cmp])
			{
				larger++;
				printf("l ");
			}
		}
		printf("\n");
		if (smaller && larger)
		{
			continue;
		}
		else if (smaller)
		{
			target[target_min] = 0;
			target_min = find_min;
		}
		else if (larger)
		{
			target[find_min] = 0;
		}
	}

	// (3) loop find target
	for (int find_target = 0; find_target < number; find_target++)
	{
		if (target[find_target])
		{
			printf("cmp min to %d: ", find_target);
			int smaller = 0;
			int larger = 0;
			for (int cmp = 0; cmp < dim; cmp++)
			{
				if (points[target_min].values[cmp] > points[find_target].values[cmp])
				{
					smaller++;
					printf("s ");
				}
				if (points[target_min].values[cmp] < points[find_target].values[cmp])
				{
					larger++;
					printf("l ");
				}
			}
			printf("\n");
			if (smaller && larger)
			{
				continue;
			}
			else if (smaller)
			{
				if (target_min != find_target)
					printf("\n\n\n****************error!!*****************\n\n\n");
					else
						printf("\n\n\n*************no error!!!************\n\n\n");
			}
			else if (larger)
			{
				target[find_target] = 0;
			}
		}
	}

	// (3) generate answer
	permissiblePoints = (Point *)malloc(sizeof(Point) * number);
	int writer = 0;

	for (writer = 0; writer < number; writer++)
	{
		if (target[writer])
		{
			permissiblePoints[permissiblePointNum] = points[writer];
			printPoint(permissiblePoints[permissiblePointNum], dim);
			permissiblePointNum++;
		}
	}
	printf("permissiblePointNum: %d\n", permissiblePointNum);

	free(target);

	// pPermissiblePoints -- your computed answer
	*pPermissiblePoints = permissiblePoints;
	return permissiblePointNum;
}
