#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"

// If you have referenced to any source code that is not written by you
// You have to cite them here.
#include <omp.h>	   // header file for openMP
#include <immintrin.h> // header file for latest SIMD

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
	// //printPoint(points[i], dim);

	/**********************************************************************************
	 * Work here
	 * *******************************************************************************/

	// (1) preparation
	long long compare = 0, condi = 0;
	omp_set_num_threads(thread_number);
	int jump_gap, jump_time, uncmp_num = number;
	int *target = (int *)malloc(sizeof(int) * number);
#pragma omp simd
	for (int i = 0; i < number; i++)
	{
		target[i] = 1;
	}

	// (2) loop
	for (int cmp_gap = 1; cmp_gap < number; cmp_gap *= 2)
	{
		printf("gap %d, ", cmp_gap);
		jump_gap = cmp_gap * 2;
		jump_time = uncmp_num / 2;
		uncmp_num = jump_time + uncmp_num % 2;

#pragma omp parallel
		for (int jump = 0; jump < jump_time; jump++)
		{
			int condition = jump * jump_gap + cmp_gap < number ? jump * jump_gap + cmp_gap : number;

#pragma omp for
			for (int former_ptr = jump * jump_gap; former_ptr < condition; former_ptr++)
			{
				// printf("%d here out of %d\n", omp_get_thread_num(), omp_get_num_threads());
				if (target[former_ptr])
				{
					int write_list = 0;
					int *target_list = (int *)malloc(sizeof(int) * cmp_gap);
					int condition2 = condition + cmp_gap < number ? condition + cmp_gap : number;
					
					for (int latter_ptr = condition; latter_ptr < condition2; latter_ptr++)
					{
						if (target[latter_ptr])
						{
							int fworse = 0;
							int lworse = 0;
							int ptr;
#pragma omp simd reduction(+ \
						   : lworse)
							for (ptr = 0; ptr < dim; ptr++)
							{
								lworse += points[former_ptr].values[ptr] >= points[latter_ptr].values[ptr];
							}
#pragma omp simd reduction(+ \
						   : fworse)
							for (ptr = 0; ptr < dim; ptr++)
							{
								fworse += points[former_ptr].values[ptr] <= points[latter_ptr].values[ptr];
							}
							if (lworse == dim && fworse < dim)
							{
								target[former_ptr] = 0;
								// printf("%d is gone because of %d\n", former_ptr, latter_ptr);
							}
							else if (fworse == dim && lworse < dim)
							{
								target_list[write_list] = latter_ptr;
								// printf("%d is gone because of %d\n", latter_ptr, former_ptr);
								write_list++;
							}
							++compare;
						}
						++condi;
					}
					for (int i = 0; i < write_list; i++)
					{
						target[target_list[i]] = 0;
					}
					free(target_list);
				}
			}
		}
		printf("condi %lld, cmp %lld\n", condi, compare);
	}

	// (3) generate answer
	permissiblePoints = (Point *)malloc(sizeof(Point) * number);
#pragma omp parallel for
	for (int writer = 0; writer < number; writer++)
	{
		if (target[writer])
		{
#pragma omp critical
			{
				permissiblePoints[permissiblePointNum] = points[writer];
				// printPoint(permissiblePoints[permissiblePointNum], dim);
				permissiblePointNum++;
			}
		}
	}
	printf("%d added\n", permissiblePointNum);
	free(target);

	// pPermissiblePoints -- your computed answer
	*pPermissiblePoints = permissiblePoints;
	return permissiblePointNum;
}
