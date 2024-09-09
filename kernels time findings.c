
/*#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"


#include "xil_cache.h"
#include <stdio.h>

#define ARRAY_SIZE 10 // Define a constant for array size

void invalidate_cache() {
    Xil_DCacheInvalidate();
    Xil_ICacheInvalidate();
}

void kernel1(int *A, int size, int offset) {
    int i;
    for (i = 0; i < size - offset; i += 4) {
        A[i] += A[i + offset];
        if (i + 1 < size - offset) A[i + 1] += A[i + 1 + offset];
        if (i + 2 < size - offset) A[i + 2] += A[i + 2 + offset];
        if (i + 3 < size - offset) A[i + 3] += A[i + 3 + offset];
    }
}

void kernel2(int *A, int size) {
    int i;
    for (i = 3; i < size; i += 2) {
        A[i] = A[i - 1] + A[i - 2] * A[i - 3];
        if (i + 1 < size) A[i + 1] = A[i] + A[i - 1] * A[i - 2];
    }
}

void kernel3(float *h, float *w, int *idx, int size) {
    for (int i = 0; i < size; ++i) {
        int index = idx[i];
        h[index] = h[index] + w[i];
    }
}

float kernel4(float *A, float *B, int size) {
    float sum = 0;
    for (int i = 0; i < size; i += 4) {
        float diff0 = A[i] - B[i];
        float diff1 = A[i + 1] - B[i + 1];
        float diff2 = A[i + 2] - B[i + 2];
        float diff3 = A[i + 3] - B[i + 3];

        if (diff0 > 0) sum += diff0;
        if (i + 1 < size && diff1 > 0) sum += diff1;
        if (i + 2 < size && diff2 > 0) sum += diff2;
        if (i + 3 < size && diff3 > 0) sum += diff3;
    }
    return sum;
}

int main() {
    invalidate_cache();

    // Initialize arrays
    int A[ARRAY_SIZE];
    float h[ARRAY_SIZE], w[ARRAY_SIZE], B[ARRAY_SIZE];
    int idx[ARRAY_SIZE];
    int size = ARRAY_SIZE;
    int offset = 5;

    // Populate arrays with test data
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        A[i] = i;
        h[i] = i * 1.0f;
        w[i] = i * 1.0f;
        B[i] = i * 1.0f;
        idx[i] = i % ARRAY_SIZE;
    }

    // Call kernels
    kernel1(A, size, offset);
    kernel2(A, size);
    kernel3(h, w, idx, size);
    float result = kernel4(h, B, size);

    // Print results for verification
    printf("Kernel4 result: %f\n", result);

    return 0;
}
*/
#include <xil_printf.h>        // Lightweight version of printf
#include <xparameters.h>       // Defines all the components in the memory map
#include <xio.h>               // Required for reading / writing memory mapped registers
#include <stdlib.h>            // Required for malloc, srand/rand ..
#include <stdio.h>
#include "timer.h"

#define ARRAY_SIZE 64

void kernel1(int *array, int length, int offset);
void kernel2(int *array, int length);
void kernel3(float *height, float *weight, int *indices, int length);
float kernel4(float *arrayA, float *arrayB, int length);

void print_double(const char *label, double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%f", value);
    xil_printf("%s: %s\r\n", label, buffer);
}

void print_float(const char *label, float value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%f", value);
    xil_printf("%s: %s\r\n", label, buffer);
}

int main() {
    srand(42);

    int *intArray = (int *) malloc(ARRAY_SIZE * sizeof(int));
    float *heights = (float *)malloc(ARRAY_SIZE * sizeof(float));
    float *weights = (float *)malloc(ARRAY_SIZE * sizeof(float));
    float *floatArrayA = (float *)malloc(ARRAY_SIZE * sizeof(float));
    float *floatArrayB = (float *)malloc(ARRAY_SIZE * sizeof(float));
    int *indices = (int *)malloc(ARRAY_SIZE * sizeof(int));

    if (intArray == NULL || heights == NULL || weights == NULL || floatArrayA == NULL || floatArrayB == NULL || indices == NULL) {
        xil_printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        intArray[i] = rand() % 100;
        heights[i] = (float)(rand() % 100) / 100.0;
        weights[i] = (float)(rand() % 100) / 100.0;
        floatArrayA[i] = (float)(rand() % 100) / 100.0;
        floatArrayB[i] = (float)(rand() % 100) / 100.0;
        indices[i] = i % ARRAY_SIZE;
    }

    // Kernel 1
    start_timer();
    kernel1(intArray, ARRAY_SIZE, 5);
    double kernel1Time = stop_timer();
    print_double("Kernel 1 time", kernel1Time);

    // Kernel 2
    start_timer();
    kernel2(intArray, ARRAY_SIZE);
    double kernel2Time = stop_timer();
    print_double("Kernel 2 time", kernel2Time);

    // Kernel 3
    start_timer();
    kernel3(heights, weights, indices, ARRAY_SIZE);
    double kernel3Time = stop_timer();
    print_double("Kernel 3 time", kernel3Time);

    // Kernel 4
    start_timer();
    float resultSum = kernel4(floatArrayA, floatArrayB, ARRAY_SIZE);
    double kernel4Time = stop_timer();
    print_double("Kernel 4 time", kernel4Time);
    print_float("Sum", resultSum);

    free(intArray);
    free(heights);
    free(weights);
    free(floatArrayA);
    free(floatArrayB);
    free(indices);

    return 0;
}

void kernel1(int *array, int length, int offset) {
    int i;
    for (i = 0; i < length - offset; i += 4) {
        array[i] += array[i + offset];
        if (i + 1 < length - offset) array[i + 1] += array[i + 1 + offset];
        if (i + 2 < length - offset) array[i + 2] += array[i + 2 + offset];
        if (i + 3 < length - offset) array[i + 3] += array[i + 3 + offset];
    }
}

void kernel2(int *array, int length) {
    int i;
    for (i = 3; i < length; i += 2) {
        array[i] = array[i - 1] + array[i - 2] * array[i - 3];
        if (i + 1 < length) array[i + 1] = array[i] + array[i - 1] * array[i - 2];
    }
}

void kernel3(float *height, float *weight, int *indices, int length) {
    for (int i = 0; i < length; ++i) {
        int index = indices[i];
        height[index] = height[index] + weight[i];
    }
}

float kernel4(float *arrayA, float *arrayB, int length) {
    float sum = 0;
    for (int i = 0; i < length; i += 4) {
        float diff0 = arrayA[i] - arrayB[i];
        float diff1 = arrayA[i + 1] - arrayB[i + 1];
        float diff2 = arrayA[i + 2] - arrayB[i + 2];
        float diff3 = arrayA[i + 3] - arrayB[i + 3];

        if (diff0 > 0) sum += diff0;
        if (i + 1 < length && diff1 > 0) sum += diff1;
        if (i + 2 < length && diff2 > 0) sum += diff2;
        if (i + 3 < length && diff3 > 0) sum += diff3;
    }
    return sum;
}

