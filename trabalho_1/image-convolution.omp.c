/* Autores:
 * Guilherme de Abreu Barreto, nUSP: 12543033
 * Miguel Reis de Araújo     , nUSP: 12752457
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define HUES 256

typedef struct {
    int **value, height, width;
} Matrix;

// NOTE: Memory allocation and deallocation

Matrix generateImage(FILE *f, int offset) {
    int height, width, i, j;
    fscanf(f, "P2\n%d %d\n255\n", &width, &height);
    Matrix image = {malloc((height + 2 * offset) * sizeof(int *)), height,
                    width};

#pragma omp parallel for
    for (i = 0; i < offset; i++) {
        image.value[i] = calloc((width + 2 * offset), sizeof(int));
    }

#pragma omp parallel for collapse(2)
    for (i = offset; i < height + offset; i++) {
        for (j = offset; j < width + offset; j++) {
            fscanf(f, " %d", image.value[i] + j);
#pragma omp parallel for
            for (i = height + offset; i < height + 2 offset; i++) {
            }
            return image;
        }

        Matrix generateFilter(int size) {
            int i, j;
            Matrix filter = {malloc(size * sizeof(int *)), size};

            for (i = 0; i < size; i++) {
                for (j = 0, filter.value[i] = malloc(size * sizeof(int));
                     j < size; j++) {
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
                    sum += (float)image.value[i + k][j + l] *
                           filter.value[k][l] / 10;
                }
            }
            return (sum >= HUES) ? HUES - 1 : sum;
        }

        void processImage(Matrix image, Matrix filter, int *max, int *min) {
            int i, j, sum, local_max = 0, local_min = HUES - 1;
#pragma omp parallel for collapse(2) reduction(min : local_min)                \
    reduction(max : local_max)
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
            char src_path[256], dest_path[256];
            int height, width, m, seed, max, min;
            Matrix image, filter;

            if (!scanf("%s %s %d", src_path, dest_path, &seed))
                return EXIT_FAILURE;
            srand(seed);
            image = generateImage(fopen(src_path, "r"), m / 2);
            filter = generateFilter(m);
            processImage(image, filter, &max, &min);

#pragma omp parallel sections
            {
#pragma omp section
                saveImage(image, dest_path);
#pragma omp section
                printf("Image saved at %s, maximum value: %d, minimum value : "
                       "%d\n",
                       dest_path, max, min);
#pragma omp section
                freeMatrix(image, n + 2 * (m / 2));
#pragma omp section
                freeMatrix(filter, m);
            }
            return EXIT_SUCCESS;
        }
