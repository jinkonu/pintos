#ifndef PROJ3_FIXED_POINT_H
#define PROJ3_FIXED_POINT_H
#define F (1 << 14)

int conv_int_to_fixed(int n);
int conv_fixed_to_int(int x);
int conv_fixed_to_int_nearest(int x);
int add_fixed_to_fixed(int x, int y);
int sub_fixed_to_fixed(int x, int y);
int add_fixed_to_int(int x, int n);
int sub_fixed_to_int(int x, int n);
int mul_fixed_to_fixed(int x, int y);
int mul_fixed_to_int(int x, int n);
int div_fixed_to_fixed(int x, int y);
int div_fixed_to_int(int x, int n);

#endif //PROJ3_FIXED_POINT_H
