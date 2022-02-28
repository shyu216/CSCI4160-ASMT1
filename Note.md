
---
## github token

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

## goto hpc15

bash  
export SLURM_CONF=/opt1/slurm/hpc-slurm.conf  
srun -A csci4160 -p csci4160 --qos=csci4160 -c 4 --pty bash  

---
## docker 

wsl  
bash  
docker run -it --rm -v $PWD:/tmp -w /tmp zllai/asgn1a bash

## roofline

advixe-cl -collect roofline -project-dir ./rfanaly -- ./runtest  


---
# the very first O(n^2) version code
```c  
    // idea: loop, use an array to store and update target output
	// example:
	// 0 1 2 3 4 5 6
	// 1 1 1 1 1 1 1
	// 0 1 2 3 4 5 6
	// 1 0 0 1 0 1 1
	// ...

	omp_set_num_threads(thread_number);

	// (1) initialize such array
	int *target = (int *)malloc(sizeof(int) * number);
    #pragma omp simd
	for (int i = 0; i < number; i++)
	{
		target[i] = 1;
	}

	// (2) loop
	int jump_gap, jump_time, uncmp_num = number;

	for (int cmp_gap = 1; cmp_gap < number; cmp_gap *= 2)
	{
		jump_gap = cmp_gap * 2;
		jump_time = uncmp_num / 2;
		uncmp_num = jump_time + uncmp_num % 2;
		// printf("jump_gap:%d, jump_time:%d, cmp_gap:%d, next uncmp_num:%d\n", jump_gap, jump_time, cmp_gap, uncmp_num);

		int cmp = 0, jump = 0;

		for (jump = 0; jump < jump_time; jump++)
		{
			int condition = cmp + cmp_gap < number ? cmp + cmp_gap : number;
            #pragma omp parallel for schedule(dynamic, thread_number)
			for (int former_ptr = cmp; former_ptr < condition; former_ptr++)
			{       
				if (target[former_ptr])
				{
					int write_list = 0;
					int *target_list = (int *)malloc(sizeof(int) * cmp_gap);
					int condition2 = cmp + jump_gap < number ? cmp + jump_gap : number;
					for (int latter_ptr = cmp + cmp_gap; latter_ptr < condition2; latter_ptr++)
					{
						if (target[latter_ptr])
						{
							int fworse = 0;
							int lworse = 0;
							int equal = 0;
							// printf("cmp %d and %d: ", former_ptr, latter_ptr);
							int cmp = 0;
							for (; cmp < dim; cmp++)
							{
								if (fworse && lworse)
								{
									break;
								}
								if (points[former_ptr].values[cmp] < points[latter_ptr].values[cmp])
								{
									fworse++;
									// printf("l ");
								}
								else if (points[former_ptr].values[cmp] > points[latter_ptr].values[cmp])
								{
									lworse++;
									// printf("f ");
								}
								else
								{
									equal++;
									// printf("e ");
								}
							}
							if (cmp < dim)
							{
								continue;
							}
							// printf("\n");
							else if (lworse + equal == dim && lworse > 0)
							{
								target[former_ptr] = 0;
								// printf("%d gone\n", former_ptr);
							}
							else if (fworse + equal == dim && fworse > 0)
							{
								target_list[write_list] = latter_ptr;
								write_list++;
								// printf("%d gone\n", latter_ptr);
							}
						}
					}
					for (int i = 0; i < write_list; i++)
					{
						target[target_list[i]] = 0;
					}
					free(target_list);
				}
			}
			cmp += jump_gap;
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
			permissiblePointNum++;
			// //printPoint(permissiblePoints[permissiblePointNum], dim);
		}
	}
	// printf("%d\n", permissiblePointNum);

	free(target);
```

## Testcode
```c
if (target[former_ptr])
				{
					int write_list = 0;
					int *target_list = (int *)malloc(sizeof(int) * cmp_gap);
					int condition2 = cmp + jump_gap < number ? cmp + jump_gap : number;
					// __m128 former = _mm_load_ps(points[former_ptr].values);
					for (int latter_ptr = cmp + cmp_gap; latter_ptr < condition2; latter_ptr++)
					{
						if (target[latter_ptr])
						{
							__m128 latter = _mm_load_ps(points[latter_ptr].values);
							__m128 ge = _mm_cmpge_ps(former, latter);
							__m128 le = _mm_cmple_ps(former, latter);
							if (isnan(ge[0])&&isnan(ge[1])&&isnan(ge[2])&&isnan(ge[3]))
							{
								target[former_ptr] = 0;
								// printf("%d gone\n", former_ptr);
							}
							else if (isnan(le[0])&&isnan(le[1])&&isnan(le[2])&&isnan(le[3]))
							{
								target_list[write_list] = latter_ptr;
								write_list++;
								// printf("%d gone\n", latter_ptr);
							}
						}
					}
					#pragma omp simd
					for (int i = 0; i < write_list; i++)
					{
						target[target_list[i]] = 0;
					}
					free(target_list);
				}
```