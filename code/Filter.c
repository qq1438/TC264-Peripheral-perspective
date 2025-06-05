#include "Filter.h"

Kalman Kmid_error;
Kalman Kspeed;
Kalman KZangle;
Kalman KGtarget;

#define Q_FRACTION_BITS 16
#define Q_SCALE_FACTOR  (1 << Q_FRACTION_BITS)
#define FLOAT_TO_Q(x)   ((int32_t)((x) * Q_SCALE_FACTOR))
#define Q_TO_FLOAT(x)   ((float)(x) / Q_SCALE_FACTOR)

void Kalman_Init(Kalman *kfp, float Q, float R) {
    kfp->Last_P = FLOAT_TO_Q(1.0f);
    kfp->Q = FLOAT_TO_Q(Q);
    kfp->R = FLOAT_TO_Q(R);
    kfp->out = FLOAT_TO_Q(0.0f);
    kfp->Kg = 0;
}

float KalmanFilter(Kalman *kfp, float input) {
    int32_t input_q = FLOAT_TO_Q(input);

    kfp->Now_P = kfp->Last_P + kfp->Q;

    int32_t denominator = kfp->Now_P + kfp->R;
    if (denominator == 0) {
        kfp->Kg = 0;
    } else {
        int64_t numerator = (int64_t)kfp->Now_P << Q_FRACTION_BITS;
        kfp->Kg = (int32_t)(numerator / denominator);
    }

    int32_t error = input_q - kfp->out;
    kfp->out += (int32_t)(((int64_t)kfp->Kg * error) >> Q_FRACTION_BITS);

    int32_t one_minus_Kg = (1 << Q_FRACTION_BITS) - kfp->Kg;
    kfp->Last_P = (int32_t)(((int64_t)one_minus_Kg * kfp->Now_P) >> Q_FRACTION_BITS);

    return Q_TO_FLOAT(kfp->out);
}
