/*
 * Utility functions. Shamelessly stolen from the Arduino standard library.
 */
#ifndef UTIL_H
#define UTIL_H

#define between(amt,low,high) ((amt)<(low)?(0):((amt)>(high)?(0):(1)))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif
