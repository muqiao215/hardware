#include "PID.h"

void PID_Init(PID_t *pid, float kp, float ki, float kd, float out_min, float out_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->out_min = out_min;
    pid->out_max = out_max;
}

void PID_Reset(PID_t *pid)
{
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
}

static float clampf(float v, float mn, float mx)
{
    if (v < mn)
        return mn;
    if (v > mx)
        return mx;
    return v;
}

float PID_Update(PID_t *pid, float error, float dt)
{
    if (dt <= 0.0f)
        dt = 0.001f;

    pid->integral += error * dt;
    float derivative = (error - pid->prev_error) / dt;
    pid->prev_error = error;

    float out = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;
    return clampf(out, pid->out_min, pid->out_max);
}

