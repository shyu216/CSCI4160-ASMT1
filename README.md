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

# pp result
```powershell
Suite: SUITE
  Test: testcase12 ...
Generating 5-dimension point dataset, number of points in dataset = 512000
c is 9798329496
Time cost: 97.4947s
passed

Run Summary:    Type  Total    Ran Passed Failed Inactive
              suites      1      0    n/a      0        0
               tests     13      1      1      0        0
             asserts      1      1      1      0      n/a

Elapsed time =  382.709 seconds
root@30a5a3b18a6b:/tmp#
```