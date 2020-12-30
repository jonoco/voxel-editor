// Easing equations source https://easings.net/
#pragma once

#include <complex>

# define PI 3.14159265358979323846

enum EASING
{
    IN_SINE,
    IN_OUT_BACK,
    IN_OUT_ELASTIC,
    IN_OUT_EXPO,
    OUT_BACK,
    OUT_CUBIC
};


inline float easeInOutElastic ( float t )
{
    const float c5 = (2. * PI) / 4.5;

    return t == 0 ? 0
        : t == 1. ? 1.
        : t < 0.5
        ? -(pow(2., 20. * t - 10.) * sin((20. * t - 11.125) * c5)) / 2.
        : (pow(2., -20. * t + 10.) * sin((20. * t - 11.125) * c5)) / 2. + 1.;
}


inline float easeInOutBack ( float t )
{
    const float c1 = 1.70158;
    const float c2 = c1 * 1.525;

    return t < 0.5
      ? (pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
      : (pow(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2;
}


inline float easeInOutExpo (float t )
{
    return t == 0
      ? 0
      : t == 1
      ? 1
      : t < 0.5 ? pow(2, 20 * t - 10) / 2
      : (2 - pow(2, -20 * t + 10)) / 2;
}


inline float easeOutBack ( float t )
{
    const float c1 = 1.70158;
    const float c3 = c1 + 1;

    return 1 + c3 * pow(t - 1, 3) + c1 * pow(t - 1, 2);
}


inline float easeOutCubic( float t)
{
    return 1 - pow(1 - t, 3);
}


inline float  easeInSine( float t)
{
  return 1 - cos((t * PI) / 2);
}


inline float ease(float t, EASING easing)
{
    switch (easing)
    {
    case IN_SINE:           return easeInSine(t);
    case IN_OUT_BACK:       return easeInOutBack(t);
    case IN_OUT_ELASTIC:    return easeInOutElastic(t);
    case IN_OUT_EXPO:       return easeInOutExpo(t);
    case OUT_BACK:          return easeOutBack(t);
    case OUT_CUBIC:         return easeOutCubic(t);

    default:
        return t;
    }
}