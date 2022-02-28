# CSCI4160-ASMT1A-OPENMP

# Token 

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

# Connect hpc15

bash  
export SLURM_CONF=/opt1/slurm/hpc-slurm.conf  
srun -A csci4160 -p csci4160 --qos=csci4160 -c 4 --pty bash

# End and job in Slurm
```
squeue
```
then
```
scancel (your jobid)
```

## roofline

export PATH=$PATH:/opt/intel/advisor/bin64/
advixe-cl -collect roofline -project-dir ./result -- ./runtest  

# pp result
```powershell
    Test: testcase12 ...
cmp gap is 262144 condi is 38093261676, cmp is 11550475000
Time cost: 97.2421s
passed

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      1      0    n/a      0        0
               tests     13      1      1      0        0
             asserts      1      1      1      0      n/a

Elapsed time =  377.650 seconds
```
```
tail is 5 compare is 39514624000, condi is 12105431878, mptr is 511690
```
```
tail is 5 compare is 39514624000, condi is 12105448130, mptr is 511697
```