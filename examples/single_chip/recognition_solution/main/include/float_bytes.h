#include "stdlib.h"
#include "stdio.h"
#include "string.h"
/**
 * 浮点数数组转为bytes
 */ 
char *float_array_to_bytes (float *arr, int len) {
  int fb = sizeof(float);
  char *all = (char *)malloc(fb * len);
  for (int i = 0; i < len; i++) {
    memcpy(all + i * fb, (char *) &arr[i], fb);
  }
  return all;
}

/**
 * bytes转为浮点数数组
 */ 
float* bytes_to_float_array(char *all) {
  return (float *) all;
}


int test_float_byte() {
  float arr[] = {1.234, 2.345};
  char *x = float_array_to_bytes(arr, 2);
  printf("%s", x);
  float *arr1 = bytes_to_float_array(x);
  for (int i = 0; i < 2; i++) {
    printf("%f", *(arr1 + i));
  }
  free(x);
  free(arr1);
  return 0;
}
