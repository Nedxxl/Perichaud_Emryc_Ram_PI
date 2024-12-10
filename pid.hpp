#ifndef PID_H_
#define PID_H_

#include <stdlib.h>
#include <math.h>
#include <iostream>

class TPid
{
private:
    double kp, ki;           // PI coefficients
    double integral;         // Integral of the error
    double dt;               // Fixed time interval (sampling time)
    double minOutput;        // Minimum output limit
    double maxOutput;
public:
    TPid(double p, double i, double deltaTime, double minOut, double maxOut);
    ~TPid();
    double compute(double setpoint, double processVariable);
};

#endif // PID_H_
