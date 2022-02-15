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
