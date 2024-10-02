/* Autores:
 * Guilherme de Abreu Barreto, nUSP: 12543033
 * Miguel Reis de Araújo     , nUSP: 12752457
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define HUES 256

typedef struct {
    int **value, size;
} Matrix;

// NOTE: Memory allocation and deallocation

Matrix generateImage(int size, int offset) {
    int i, j;
    Matrix image = {malloc((size + 2 * offset) * sizeof(int *)), size};

#pragma omp parallel for
    for (i = 0; i < size + 2 * offset; i++) {
        image.value[i] = calloc((size + 2 * offset), sizeof(int));
    }

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            image.value[i + offset][j + offset] = rand() % HUES;
        }
    }
    return image;
}

Matrix generateFilter(int size) {
    int i, j;
    Matrix filter = {malloc(size * sizeof(int *)), size};

    for (i = 0; i < size; i++) {
        for (j = 0, filter.value[i] = malloc(size * sizeof(int)); j < size;
             j++) {
            filter.value[i][j] = rand() % 10;
        }
    }
    return filter;
}

void freeMatrix(Matrix m, int size) {
    int i;
#pragma omp parallel for
    for (i = 0; i < size; i++) {
        free(m.value[i]);
    }
    free(m.value);
}

// NOTE: Convolution implementation

int convolution(Matrix image, Matrix filter, int i, int j) {
    int k, l;
    float sum = 0;

#pragma omp parallel for simd collapse(2) reduction(+ : sum)
    for (k = 0; k < filter.size; k++) {
        for (l = 0; l < filter.size; l++) {
            sum += (float)image.value[i + k][j + l] * filter.value[k][l] / 10;
        }
    }
    return (sum >= HUES) ? HUES - 1 : sum;
}

void processImage(Matrix image, Matrix filter, int *max, int *min) {
    int i, j, sum, local_max = 0, local_min = HUES - 1;
#pragma omp parallel for collapse(2) reduction(min : local_min)                \
    reduction(max : local_max) private(sum)
    for (i = 0; i < image.size; i++) {
        for (j = 0; j < image.size; j++) {
            sum = convolution(image, filter, i, j);
            if (sum > local_max) {
                local_max = sum;
            }
            if (sum < local_min) {
                local_min = sum;
            }
        }
    }
    *min = local_min;
    *max = local_max;
}

int main(void) {
    int n, m, seed, max, min;
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
        freeMatrix(image, n + 2 * (m / 2));
#pragma omp section
        freeMatrix(filter, m);
    }
    return EXIT_SUCCESS;
}
