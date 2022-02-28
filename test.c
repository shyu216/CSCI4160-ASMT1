#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void main(){
    int i = 0;

    printf("%d\n", i++);//0
    int j = 0;
    if(i==0|(++j) ==1){
        j++;
    }
    printf("%d", j);
}