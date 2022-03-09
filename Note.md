
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
