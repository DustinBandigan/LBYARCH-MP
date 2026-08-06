#define _POSIX_C_SOURCE 199309L

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RUNS 30

extern void imgCvtGrayIntToDouble(
    const int *input,
    double *output,
    size_t count
);

static double getTimeSeconds(void)
{
    struct timespec timeValue;

    clock_gettime(CLOCK_MONOTONIC, &timeValue);

    return (double)timeValue.tv_sec +
           (double)timeValue.tv_nsec / 1000000000.0;
}

static int checkCorrectness(
    const int *input,
    const double *output,
    size_t count
)
{
    const double tolerance = 1e-12;

    for (size_t i = 0; i < count; i++) {
        double expected = (double)input[i] / 255.0;

        if (fabs(output[i] - expected) > tolerance) {
            printf("\nIncorrect output at pixel %zu\n", i + 1);
            printf("Input: %d\n", input[i]);
            printf("Expected: %.15f\n", expected);
            printf("Output: %.15f\n", output[i]);

            return 0;
        }
    }

    return 1;
}

static void printIntegerImage(
    const int *input,
    int height,
    int width
)
{
    printf("\nInteger grayscale image:\n");

    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            int index = row * width + column;
            printf("%4d ", input[index]);
        }

        printf("\n");
    }
}

static void printDoubleImage(
    const double *output,
    int height,
    int width
)
{
    printf("\nConverted double grayscale image:\n");

    for (int row = 0; row < height; row++) {
        for (int column = 0; column < width; column++) {
            int index = row * width + column;
            printf("%.2f ", output[index]);
        }

        printf("\n");
    }
}

static void runPerformanceTest(int height, int width)
{
    size_t count = (size_t)height * (size_t)width;

    int *input = malloc(count * sizeof(int));
    double *output = malloc(count * sizeof(double));

    if (input == NULL || output == NULL) {
        printf("Memory allocation failed for %d x %d test.\n",
               height, width);

        free(input);
        free(output);
        return;
    }

    for (size_t i = 0; i < count; i++) {
        input[i] = rand() % 256;
    }

    /*
     * Warm-up run before timing.
     */
    imgCvtGrayIntToDouble(input, output, count);

    double totalTime = 0.0;
    double fastestTime = 0.0;
    double slowestTime = 0.0;

    for (int run = 0; run < RUNS; run++) {
        double startTime = getTimeSeconds();

        imgCvtGrayIntToDouble(input, output, count);

        double endTime = getTimeSeconds();
        double elapsedTime = endTime - startTime;

        totalTime += elapsedTime;

        if (run == 0 || elapsedTime < fastestTime) {
            fastestTime = elapsedTime;
        }

        if (run == 0 || elapsedTime > slowestTime) {
            slowestTime = elapsedTime;
        }
    }

    double averageTime = totalTime / RUNS;

    printf("\n========================================\n");
    printf("Image size: %d x %d\n", height, width);
    printf("Number of pixels: %zu\n", count);
    printf("Number of runs: %d\n", RUNS);
    printf("Correctness: %s\n",
           checkCorrectness(input, output, count)
               ? "PASSED"
               : "FAILED");

    printf("Average execution time: %.9f seconds\n", averageTime);
    printf("Average execution time: %.3f microseconds\n",
           averageTime * 1000000.0);
    printf("Fastest execution time: %.3f microseconds\n",
           fastestTime * 1000000.0);
    printf("Slowest execution time: %.3f microseconds\n",
           slowestTime * 1000000.0);
    printf("========================================\n");

    free(input);
    free(output);
}

int main(void)
{
    int height;
    int width;

    srand((unsigned int)time(NULL));

    printf("Manual correctness test\n");
    printf("=======================\n");

    printf("Enter image height: ");

    if (scanf("%d", &height) != 1) {
        printf("Invalid height.\n");
        return 1;
    }

    printf("Enter image width: ");

    if (scanf("%d", &width) != 1) {
        printf("Invalid width.\n");
        return 1;
    }

    if (height <= 0 || width <= 0) {
        printf("Height and width must be positive.\n");
        return 1;
    }

    size_t count = (size_t)height * (size_t)width;

    int *input = malloc(count * sizeof(int));
    double *output = malloc(count * sizeof(double));

    if (input == NULL || output == NULL) {
        printf("Memory allocation failed.\n");

        free(input);
        free(output);

        return 1;
    }

    printf("\nEnter the grayscale pixel values row by row.\n");
    printf("Each pixel must be from 0 to 255.\n\n");

    for (int row = 0; row < height; row++) {
        printf("Row %d (%d values): ", row + 1, width);

        for (int column = 0; column < width; column++) {
            int index = row * width + column;

            if (scanf("%d", &input[index]) != 1) {
                printf("Invalid input.\n");

                free(input);
                free(output);

                return 1;
            }

            if (input[index] < 0 || input[index] > 255) {
                printf("Pixel values must be from 0 to 255.\n");

                free(input);
                free(output);

                return 1;
            }
        }
    }

    imgCvtGrayIntToDouble(input, output, count);

    printIntegerImage(input, height, width);
    printDoubleImage(output, height, width);

    printf("\nCorrectness check: %s\n",
           checkCorrectness(input, output, count)
               ? "PASSED"
               : "FAILED");

    free(input);
    free(output);

    printf("\n\nAutomatic performance tests\n");
    printf("===========================\n");

    runPerformanceTest(10, 10);
    runPerformanceTest(100, 100);
    runPerformanceTest(1000, 1000);

    return 0;
}
