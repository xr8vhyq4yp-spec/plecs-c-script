/*
 Variable Rate Limiter
 Single or Dual inverter mode
 
 Objective:
 Suppress instability and saturation in the PI controller
 by applying a variable rate limiter to the current command.

 The rate limit is automatically adjusted
 according to the magnitude of the current command.
 
 Inputs:
   id_cmd, iq_cmd
 
 Parameters:
   R : phase resistance [ohm]
   L : phase inductance [H]
   Vmax : available voltage limit [V]
 
 Outputs:
   id_ref, iq_ref
*/

#include <stdio.h>
#include <math.h>

/*
User Setting
*/
#define SINGLE 0
#define DUAL 1

const double R = 5.0;
const double L = 10e-3;
const double Vmax = 100;
const double Ts = 50e-6;
const double end_time = 0.01;
const double k = 0.1;

const double id_cmd_test = 0.0;
const double iq_cmd_test = 19.0; // Variable 0.0 ~ 19.0 [A]

const int mode = SINGLE; // SINGLE or DUAL

/*
function
*/

double get_Vmax(void){

    switch(mode){
        
        case SINGLE:
            return Vmax;
            break;
        case DUAL:
            return Vmax*2;
            break;
        default:
            printf("Invalid inveter mode\n");
            return Vmax;
    
    }
}

double rate_limiter(double cmd,double prev_ref){
    
    double Vmax_base = get_Vmax();
    double margin = Vmax_base - R * fabs(prev_ref);
    
    if(margin <= 0.0){
        margin = 0.0;
    }
    
    double rate = k * margin / L;
    double dmax = rate * Ts;
    double diff = cmd - prev_ref;
    
    //clip diff to range dmax
    if(diff > dmax){
        return prev_ref + dmax;
    }else if(diff < -dmax){
        return prev_ref - dmax;
    }else{
        return cmd;
    }
}


int main(void){
    
    double prev_ref = 0.0;
    double cmd = iq_cmd_test;
    
    int N = (int)(end_time / Ts);
    
    int n;
    
    for (int n = 0; n < N; n++){
        double t = n * Ts;
        prev_ref = rate_limiter(cmd ,prev_ref);
        printf("%f, %f\n", t, prev_ref);
    }
    
    return 0;

}
