#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float power = 0;
    int i;
    for(i=0;i<N;i++){
        power += (x[i]*x[i]);
    }
    return log10(power/N)*10;
}

float compute_am(const float *x, unsigned int N) {
    float amp = 0;
    int i;
    for(i=0;i<N;i++){
        amp += fabs(x[i]);
    }
    return amp/N;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    float zcr = 0;
    int i;
    for(i=1;i<N;i++){
        if(x[i]*x[i-1]>0){
            zcr ++;
        }
    }
    return (fm/2)*(1/(N-1))*zcr;
}