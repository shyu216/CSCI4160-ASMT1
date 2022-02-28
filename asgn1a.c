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

	// (1) preparation
	long long compare = 0, ifcondition = 0;
	int head = 0, tail = 0;
	permissiblePoints = (Point *)malloc(sizeof(Point) * number);
	int *flag = (int *)malloc(sizeof(int) * dim);
	int *target = (int *)malloc(sizeof(int) * number);
	int *map = (int *)malloc(sizeof(int) * number);
	int loop = 1;
	for (int i = 0; i < dim; ++i)
	{
		flag[i] = 0;
	}
	for (int i = 0; i < number; ++i)
	{
		target[i] = 0;
	}
	for (int i = 0; i < number; ++i)
	{
		map[i] = 1;
	}

	while (loop)
	{
		// (2) target
		for (int i = 0; i < dim; ++i)
		{
			if (flag[i] == 0)
			{
				for (; flag[i] < number; flag[i]++)
				{
					if (map[flag[i]])
					{
						break;
					}
				}
				if (flag[i] == number)
				{
					loop = 0;
				}
			}

			target[tail] = flag[i];
			flag[i] = 0;
			for (int j = 0; j < number; ++j)
			{
				if (map[j])
				{
					if (points[j].values[i] == points[target[tail]].values[i])
					{
						++tail;
						target[tail] = j;
					}
					else if (points[j].values[i] < points[target[tail]].values[i])
					{
						flag[i] = target[tail];
						tail = head;
						target[tail] = j;
					}
				}
			}
			++tail;
			head = tail;
		}

		// (3) map
		for (int i = 0; i < tail; ++i)
		{
			if (map[target[i]])
			{
				for (int j = 0; j < number; ++j)
				{
					if (map[j])
					{
						int better = 0;
						int worse = 0;
						int ptr;
						for (ptr = 0; ptr < dim; ptr++)
						{
							worse += points[j].values[ptr] > points[target[i]].values[ptr];
						}
						for (ptr = 0; ptr < dim; ptr++)
						{
							better += points[j].values[ptr] < points[target[i]].values[ptr];
						}
						if (worse > 0 && better == 0)
						{
							map[j] = 0;
						}
						 ++ifcondition;
					}
					 ++compare;
				}
			}
		}
		 

		// (3) generate answer
		for (int i = 0; i < tail; i++)
		{
			if (map[target[i]])
			{
				permissiblePoints[permissiblePointNum] = points[target[i]];
				permissiblePointNum++;
				map[target[i]] = 0;
			}
			target[i] = 0;
		}

		head = 0;
		tail = 0;
	}

printf("count of cmp %lld, ount of if %lld\n", compare, ifcondition);
	printf("total added %d \n", permissiblePointNum);
	free(target);
	free(map);
	free(flag);

	// pPermissiblePoints -- your computed answer
	*pPermissiblePoints = permissiblePoints;
	return permissiblePointNum;
}
