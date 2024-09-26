#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define HUES 256

typedef struct {
    char **value;
    int size;
} Matrix;

// NOTE: Memory allocation and deallocation

Matrix generateImage(int size, int offset) {
    int i, j;
    Matrix image = {malloc((size + 2 * offset) * sizeof(char *)), size};

#pragma omp parallel for
    for (i = 0; i < size; i++)
        image.value[i] = calloc((size + 2 * offset), sizeof(char));

    for (i = offset; i < size + offset; i++)
        for (j = offset; j < size + offset; j++)
            image.value[i][j] = rand() % HUES;
    return image;
}

Matrix generateFilter(int size) {
    int i, j;
    Matrix filter = {malloc(size * sizeof(char *)), size};

    for (i = 0; i < size; i++)
        for (j = 0, filter.value[i] = malloc(size * sizeof(char)); j < size;
             j++)
            filter.value[i][j] = rand() % 10;
    return filter;
}

void freeMatrix(Matrix m) {
    int i;
#pragma omp parallel for
    for (i = 0; i < m.size; i++)
        free(m.value[i]);
    free(m.value);
}

// NOTE: Convolution implementation

int convolution(Matrix image, Matrix filter, int i, int j) {
    int k, l, offset = filter.size / 2, sum = 0;
    while (sum < HUES)
        for (k = 0; k < filter.size; k++) {
            for (l = 0; l < filter.size; l++) {
                sum += image.value[i]
            }
        }
    return sum % HUES;
}

void processImage(Matrix image, Matrix filter, char *max, char *min) {
    int i, j, offset = filter.size / 2, sum;
#pragma omp parallel for collapse(2) reduction(min : *min) reduction(max : *max)
    for (i = 0; i < image.size; i++)
        for (j = 0; j < image.size; j++) {
            sum = convolution(image, filter, i + offset, j + offset);
            if (sum > *max)
                *max = sum;
            if (sum < *min)
                *min = sum;
        }
}

int main(void) {
    int n, m, seed;
    char max = CHAR_MIN, min = CHAR_MAX;
    Matrix image, filter;

    scanf("%d %d %d", &n, &m, &seed);
    srand(seed);
    image = generateImage(n, m / 2);
    filter = generateFilter(m);
    processImage(image, filter, &max, &min);

#pragma omp parallel sections
    {
#pragma omp section
        printf("%d %d", max, min);
#pragma omp section
        freeMatrix(image);
#pragma omp section
        freeMatrix(filter);
    }
    return EXIT_SUCCESS;
}
