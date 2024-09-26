#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define HUES 256

typedef struct {
    int **value;
    int size;
} Matrix;

// NOTE: Memory allocation and deallocation

Matrix generateImage(int size, int offset) {
    int i, j;
    Matrix image = {malloc((size + 2 * offset) * sizeof(int *)), size};

#pragma omp parallel for
    for (i = 0; i < size; i++)
        image.value[i] = calloc((size + 2 * offset), sizeof(int));

    for (i = offset; i < size + offset; i++)
        for (j = offset; j < size + offset; j++)
            image.value[i][j] = rand() % HUES;
    return image;
}

Matrix generateFilter(int size) {
    int i, j;
    Matrix filter = {malloc(size * sizeof(int *)), size};

    for (i = 0; i < size; i++)
        for (j = 0, filter.value[i] = malloc(size * sizeof(int)); j < size; j++)
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
    int k, l, sum = 0;
    for (k = 0; k < filter.size; k++, i++)
        for (l = 0; l < filter.size; l++, j++)
            sum = (sum + image.value[i][j] * filter.value[k][l]) % HUES;
    return sum;
}

void processImage(Matrix image, Matrix filter, int *max, int *min) {
    int i, j, sum, local_max = *max, local_min = *min;
#pragma omp parallel for collapse(2) reduction(min : local_min)                \
    reduction(max : local_max)
    for (i = 0; i < image.size; i++) {
        for (j = 0; j < image.size; j++) {
            sum = convolution(image, filter, i, j);
            if (sum > *max)
                *max = sum;
            if (sum < *min)
                *min = sum;
        }
    }
    *min = local_min;
    *max = local_max;
}

int main(void) {
    int n, m, seed;
    int max = 0, min = HUES - 1;
    Matrix image, filter;

    scanf("%d %d %d", &n, &m, &seed);
    srand(seed);
    image = generateImage(n, m / 2);
    filter = generateFilter(m);
    processImage(image, filter, &max, &min);

#pragma omp parallel sections
    {
#pragma omp section
        printf("%d %d\n", max, min);
#pragma omp section
        freeMatrix(image);
#pragma omp section
        freeMatrix(filter);
    }
    return EXIT_SUCCESS;
}
