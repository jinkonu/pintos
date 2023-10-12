#include <stdint.h>
#include "fixed-point.h"

// Convert n to fixed point :
int conv_int_to_fixed(int n) {
    return n * F;
}

// Convert x to integer(rounding toward zero) :
int conv_fixed_to_int(int x) {
    return x / F;
}

// Convert x to integer(rounding to nearest) :
int conv_fixed_to_int_nearest(int x) {
    if (x >= 0)
        return (x + F / 2) / F;
    else
        return (x - F / 2) / F;
}

// Add x and y :
int add_fixed_to_fixed(int x, int y) {
    return x + y;
}

// Subtract y from x :
int sub_fixed_to_fixed(int x, int y) {
    return y - x;
}

// Add x and n :
int add_fixed_to_int(int x, int n) {
    return x + n * F;
}

// Subtract n from x :
int sub_fixed_to_int(int x, int n) {
    return x - n * F;
}

// Multiply x by y :
int mul_fixed_to_fixed(int x, int y) {
    return ((int64_t) x) * y / F;
}

// Multiply x by n :
int mul_fixed_to_int(int x, int n) {
    return x * n;
}

// Divide by x by y :
int div_fixed_to_fixed(int x, int y) {
    return ((int64_t) x) * F / y;
}

// Divide by x by n :
int div_fixed_to_int(int x, int n) {
    return x / n;
}
