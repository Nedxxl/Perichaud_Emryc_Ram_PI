#include "pid.hpp"

TPid::TPid(double p, double i, double deltaTime, double minOut, double maxOut): kp(p), ki(i), integral(0), dt(deltaTime), minOutput(minOut), maxOutput(maxOut)
{
}

TPid::~TPid()
{
}

double TPid::compute(double setpoint, double processVariable)
{
    double error = setpoint - processVariable;

        // Proportional term
        double proportional = kp * error;

        // Integral term with windup prevention
        integral += error * dt;
        double integralTerm = ki * integral;

        // Compute total output
        double output = proportional + integralTerm;

        // Clamp output within min and max limits
        if (output > maxOutput) {
            output = maxOutput;
        } else if (output < minOutput) {
            output = minOutput;
        }

        // Anti-windup: Prevent the integral term from accumulating beyond output limits
        if (output == maxOutput && error > 0) {
            integral -= error * dt;  // Reverse the last integration step
        } else if (output == minOutput && error < 0) {
            integral -= error * dt;  // Reverse the last integration step
        }

        return output; //value valve
}