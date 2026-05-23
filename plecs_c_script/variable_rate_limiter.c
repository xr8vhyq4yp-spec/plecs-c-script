/*
 Variable Rate Limiter for PLECS C-Script
 
 Usage:
 1. Paste the declaration part into the Declarations section.
 2. Paste the output function part into the Output function section.
 3. Set the number of input ports to 7.
 4. Set the number of output ports to 3.
 5. If a variable-step solver is used, execute this C-Script with a fixed sample time.
 
 Recommended settings:
  - PWM frequency          : 20 kHz
  - C-Script sample time   : 50e-6 s
  - Ts input               : 50e-6 s

 Notes:
 This script limits the rate of change of the q-axis current command
 based on the available voltage margin.
 */

/* Declaration section */

#include <math.h>
#define SINGLE 0
#define DUAL   1

// Privious value
static double id_ref_prev = 0.0;
static double iq_ref_prev = 0.0;

/* Output function section */

/*
 Variable Rate Limiter for PLECS C-Script

 Input(0): id_cmd [A]
 Input(1): iq_cmd [A]
 Input(2): R [ohm]
 Input(3): L [H]
 Input(4): Vmax [V]
 Input(5): Ts [s]
 Input(6): k [-]

 * Default input values:
 - Input(0): id_cmd = 0.0
 - Input(1): iq_cmd = 0.0 to 19.0
 - Input(2): R      = 5.0
 - Input(3): L      = 10e-3
 - Input(4): Vmax   = 100.0
 - Input(5): Ts     = 50e-6
 - Input(6): k      = 0.9
 
 Output(0): id_ref [A]
 Output(1): iq_ref [A]
 Output(2): dmax [A/step]
*/

const int mode = SINGLE;   // SINGLE or DUAL */

double id_cmd;
double iq_cmd;
double R;
double L;
double Vmax;
double Ts;
double k;

double Vbase;
double margin;
double dmax;
double diff;
double id_ref;
double iq_ref;

// Inputs 
id_cmd = Input(0);
iq_cmd = Input(1);
R      = Input(2);
L      = Input(3);
Vmax   = Input(4);
Ts     = Input(5);
k      = Input(6);

// Inverter mode 
switch (mode) {
	case SINGLE:
   	Vbase = Vmax;
   	break;
	case DUAL:
   	Vbase = 2.0 * Vmax;
   	break;
   default:
   	Vbase = Vmax;
}

// Voltage margin 
margin = Vbase - R * fabs(iq_ref_prev);

if (margin < 0.0) {
    margin = 0.0;
}

// Maximum current step 
dmax = k * margin * Ts / L;

// Rate limit 
diff = iq_cmd - iq_ref_prev;

if (diff > dmax) {
    iq_ref = iq_ref_prev + dmax;
} else if (diff < -dmax) {
    iq_ref = iq_ref_prev - dmax;
} else {
    iq_ref = iq_cmd;
}

// pass through 
id_ref = id_cmd;

// Outputs 
Output(0) = id_ref;
Output(1) = iq_ref;
Output(2) = dmax;

// Update 
id_ref_prev = id_ref;
iq_ref_prev = iq_ref;
