#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"

// If you have referenced to any source code that is not written by you
// You have to cite them here.
#include <omp.h>       // header file for openMP
#include <immintrin.h> // header file for latest SIMD

void mergesort(int start, int len, int end, int *id, float *value)
{
    int former_len = start + len < end ? start + len : end;
    int later_len = former_len + len < end ? former_len + len : end;
    int former_ptr = start;
    int later_ptr = former_len;
    float *tem_v = (float *)malloc(sizeof(float) * len * 2);
    int *tem_id = (int *)malloc(sizeof(int) * len * 2);
    int i = 0;
    while (former_ptr < former_len || later_ptr < later_len)
    {
        if (former_ptr == former_len)
        {
            tem_v[i] = value[later_ptr];
            tem_id[i] = id[later_ptr];
            later_ptr++;
            i++;
        }
        else if (later_ptr == later_len)
        {
            tem_v[i] = value[former_ptr];
            tem_id[i] = id[former_ptr];
            former_ptr++;
            i++;
        }
        else if (value[later_ptr] > value[former_ptr])
        {
            tem_v[i] = value[former_ptr];
            tem_id[i] = id[former_ptr];
            former_ptr++;
            i++;
        }
        else
        {
            tem_v[i] = value[later_ptr];
            tem_id[i] = id[later_ptr];
            later_ptr++;
            i++;
        }
    }
    for (int j = 0; j < i; ++j)
    {
        id[start + j] = tem_id[j];
        value[start + j] = tem_v[j];
    }
}

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

    // sort each dim, find center
    int **sorted_id = (int **)malloc(sizeof(int *) * dim);
    float **sorted_value = (float **)malloc(sizeof(float *) * number);
    for (int i = 0; i < dim; ++i)
    {
        sorted_id[i] = (int *)malloc(sizeof(int) * number);
        sorted_value[i] = (float *)malloc(sizeof(float) * number);
    }

    // restore points
    for (int i = 0; i < number; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            sorted_value[j][i] = points[i].values[j];
            sorted_id[j][i] = i;
        }
    }

    // sort
    for (int i = 0; i < dim; ++i)
    {
        int jump_gap, jump_time, unsort_num = number;
        for (int sort_gap = 1; sort_gap < number; sort_gap *= 2)
        {
            jump_gap = sort_gap * 2;
            jump_time = unsort_num / 2;
            unsort_num = jump_time + unsort_num % 2;
            for (int jump = 0; jump < jump_time; jump++)
            {
                mergesort(jump * jump_gap, sort_gap, number, sorted_id[i], sorted_value[i]);
            }
        }
    }

    // first round
    int *count = (int *)malloc(sizeof(int) * number);
    int *map = (int *)malloc(sizeof(int) * number * dim);
    for (int i = 0; i < number; ++i)
    {
        count[i] = 0;
    }
    int flag = 0;
    int i = 0;
    int j = 0;
    int center;
    for (; i < number; ++i)
    {
        for (j = 0; j < dim; ++j)
        {
            int tem = sorted_id[j][i];
            ++count[tem];
            map[tem * dim + j] = i;
            if (count[tem] == dim)
            {
                center = tem;
                flag = 1;
                break;
            }
        }
        if (flag == 1)
        {
            break;
        }
    }
    printf("center: %d: ", center);
    for (int m = 0; m < dim; ++m)
    {
        printf("%d ", map[center * dim + m]);
    }
    printf("\n");

    int *first = (int *)malloc(sizeof(int) * number);
    for (int m = 0; m < number; ++m)
    {
        first[m] = 0;
    }
    for (int m = 0; m < dim; ++m)
    {
        for (int n = 0; n < map[center * dim + m]; ++n)
        {
            first[sorted_id[m][n]] = 1;
        }
    }
    int *acc = (int *)malloc(sizeof(int) * number);
    int acced = 0;
    int firacc = 0;
    acc[acced] = center;
    acced++;

    long long cmp = 0, cdi = 0;

    // second round
    flag = 0;
    for (; i < number; ++i)
    {
        for (; j < dim; ++j)
        {
            int tem = sorted_id[j][i];
            if (first[tem] > 0)
            {
                ++count[tem];
                if (count[tem] == dim)
                {

                    int c = 0;
                    for (int k = 0; k < acced; ++k)
                    {
                        int fworse = 0;
                        int lworse = 0;
                        int ptr;
                        for (ptr = 0; ptr < dim; ptr++)
                        {
                            lworse += points[tem].values[ptr] >= points[acc[k]].values[ptr];
                        }
                        for (ptr = 0; ptr < dim; ptr++)
                        {
                            fworse += points[tem].values[ptr] <= points[acc[k]].values[ptr];
                        }
                        if (lworse == dim && fworse < dim)
                        {
                            c++;
                            break;
                        }
                        ++cmp;
                    }
                    if (c == 0)
                    {
                        acc[acced] = tem;
                        acced++;
                    }
                    else
                    {
                        firacc--;
                    }
                    if (firacc == acced)
                    {
                        flag = 1;
                        break;
                    }
                }
            }
            ++cdi;
        }
        j = 0;
        if (flag)
        {
            break;
        }
    }
    printf("total cdi: %lld cmp: %lld\n", cdi, cmp);

    // add
    permissiblePoints = (Point *)malloc(sizeof(Point) * number);
    for (int i = 0; i < acced; ++i)
    {
        permissiblePoints[permissiblePointNum] = points[acc[i]];
        permissiblePointNum++;
    }

    // pPermissiblePoints -- your computed answer
    *pPermissiblePoints = permissiblePoints;
    return permissiblePointNum;
}
