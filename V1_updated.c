#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "util.h"

// If you have referenced to any source code that is not written by you
// You have to cite them here.
#include <omp.h>       // header file for openMP
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
    long long compare = 0;
    int jump_gap, jump_time, uncmp_num = number;
    int *map = (int *)malloc(sizeof(int) * number);
    for (int i = 0; i < number; i++)
    {
        map[i] = i;
    }
    int *len = (int *)malloc(sizeof(int) * number);
    for (int i = 0; i < number; i++)
    {
        len[i] = 1;
    }

    // (2) loop
    for (int cmp_gap = 1; cmp_gap < number; cmp_gap *= 2)
    {
        // printf("gap %d, ", cmp_gap);
        jump_gap = cmp_gap * 2;
        jump_time = uncmp_num / 2;
        uncmp_num = jump_time + uncmp_num % 2;

        for (int jump = 0; jump < jump_time; jump++)
        {
            int fms = jump * jump_gap;
            int lts = fms + cmp_gap;
            int flen = fms + len[fms];
            int llen = lts + len[lts];
            for (int fp = fms; fp < flen; fp++)
            {
                // printf("%d here out of %d\n", omp_get_thread_num(), omp_get_num_threads());

                for (int lp = lts; lp < llen; lp++)
                {
                    if (map[fp] > -1 && map[lp] > -1)
                    {
                        int fbtr = 0;
                        int lbtr = 0;
                        int ptr;
                        for (ptr = 0; ptr < dim; ptr++)
                        {
                            lbtr += points[map[fp]].values[ptr] >= points[map[lp]].values[ptr];
                        }
                        for (ptr = 0; ptr < dim; ptr++)
                        {
                            fbtr += points[map[fp]].values[ptr] <= points[map[lp]].values[ptr];
                        }
                        if (lbtr == dim && fbtr < dim)
                        {
                            map[fp] = -1;
                            // printf("%d rm by %d\n", fp, lp);
                        }
                        else if (fbtr == dim && lbtr < dim)
                        {
                            map[lp] = -1;
                            // printf("%d rm by %d\n", lp, fp);
                        }
                        ++compare;
                    }
                }
            }

            int *tem = (int *)malloc(sizeof(int) * jump_gap);
            int *tp = tem;
            for (int i = fms; i < flen; ++i)
            {
                if (map[i] > -1)
                {
                    *tp = map[i];
                    ++tp;
                }
            }
            for (int i = lts; i < llen; ++i)
            {
                if (map[i] > -1)
                {
                    *tp = map[i];
                    ++tp;
                }
            }
            int tlen = tp - tem;
            len[fms] = tlen;
            // printf("%d(%d): ", fms, tlen);
            int *mp = map + fms;
            tp = tem;
            for (int i = 0; i < tlen; ++i)
            {
                *mp = *tp;
                // printf("%d ", *tp);
                ++mp;
                ++tp;
            }

            free(tp);
            free(tem);
        }
        // printf("\n");
    }

    // printf("total cmp %lld\n", compare);

    // (3) generate answer
    permissiblePoints = (Point *)malloc(sizeof(Point) * number);
    int tlen = *len;
    int *mp = map;
    for (int i = 0; i < tlen; i++)
    {
        permissiblePoints[permissiblePointNum] = points[*mp];
        // printPoint(permissiblePoints[permissiblePointNum], dim);
        permissiblePointNum++;
        ++mp;
    }
    // printf("%d added\n", permissiblePointNum);
    free(mp);
    free(map);
    free(len);

    // pPermissiblePoints -- your computed answer
    *pPermissiblePoints = permissiblePoints;
    return permissiblePointNum;
}
