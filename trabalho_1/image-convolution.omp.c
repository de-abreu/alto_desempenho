/* Versão do algoritmo de convolução com geração de imagem.
 *
 * Autores:
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

Matrix storeImage(char *src_path, int offset) {
    int height, width, i, j;
    FILE *f = fopen(src_path, "r");

    fscanf(f, "P2\n%d %d\n255\n", &width, &height);
    Matrix image = {malloc((height + 2 * offset) * sizeof(int *)), height,
                    width};

#pragma omp parallel
    {
#pragma omp for nowait
        for (i = 0; i < offset; i++)
            image.value[i] = calloc((width + 2 * offset), sizeof(int));

#pragma omp for collapse(2) nowait
        for (i = 0; i < height; i++)
            for (j = 0; j < width; j++)
                fscanf(f, " %d", image.value[i + offset] + (j + offset));

#pragma omp parallel for
        for (i = 0; i < offset; i++)
            image.value[height + i] = calloc((width + 2 * offset), sizeof(int));
    }
    fclose(f);
    return image;
}

Matrix generateFilter(int size, int seed) {
    int i, j;
    Matrix filter = {malloc(size * sizeof(int *)), size};

    srand(seed);
    for (i = 0; i < size; i++)
        for (j = 0, filter.value[i] = malloc(size * sizeof(int)); j < size; j++)
            filter.value[i][j] = rand() % 10;
    return filter;
}

void freeMatrix(Matrix m, int height) {
    int i;
#pragma omp parallel for
    for (i = 0; i < height; i++) {
        free(m.value[i]);
    }
    free(m.value);
}

// NOTE: Convolution implementation

int convolution(Matrix image, Matrix filter, int i, int j) {
    int k, l;
    float sum = 0;

#pragma omp parallel for simd collapse(2) reduction(+ : sum)
    for (k = 0; k < filter.height; k++) {
        for (l = 0; l < filter.height; l++) {
            sum += (float)image.value[i + k][j + l] * filter.value[k][l] / 10;
        }
    }
    return (sum >= HUES) ? HUES - 1 : sum;
}

Matrix processImage(Matrix src, Matrix filter, int *max, int *min) {
    int i, j, local_max = 0, local_min = HUES - 1;
    Matrix dest = {malloc(src.height * sizeof(int *)), src.height, src.width};

#pragma omp parallel for
    for (i = 0; i < dest.height; i++)
        dest.value[i] = malloc(dest.width * sizeof(int));

#pragma omp parallel for collapse(2) reduction(min : local_min)                \
    reduction(max : local_max)
    for (i = 0; i < src.height; i++) {
        for (j = 0; j < src.width; j++) {
            dest.value[i][j] = convolution(src, filter, i, j);
            if (dest.value[i][j] > local_max)
                local_max = dest.value[i][j];
            if (dest.value[i][j] < local_min)
                local_min = dest.value[i][j];
        }
    }

    *min = local_min;
    *max = local_max;
    return dest;
}

// NOTE: Function to save resulting image

void saveImage(Matrix dest, char *dest_path) {
    int i, j;
    FILE *f = fopen(dest_path, "w");

    printf("P2\n%d %d\n%d\n", dest.width, dest.height, HUES - 1);

    for (i = 0; i < dest.height; i++) {
        fprintf(f, "%d", dest.value[i][0]);
        for (j = 1; j < dest.width; j++)
            fprintf(f, " %d", dest.value[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
}

int main(void) {
    char src_path[256], dest_path[256];
    int height, width, size, seed, max, min;
    Matrix src, dest, filter;

    if (!scanf("%s %s %d %d", src_path, dest_path, &size, &seed))
        return EXIT_FAILURE;

#pragma omp parallel sections
    {
#pragma omp section
        src = storeImage(src_path, size / 2);
#pragma omp section
        filter = generateFilter(size, seed);
    }

    dest = processImage(src, filter, &max, &min);

#pragma omp parallel sections
    {
#pragma omp section
        {
            saveImage(dest, dest_path);
            freeMatrix(dest, dest.height);
        }
#pragma omp section
        printf("Image saved at %s, maximum value: %d, minimum value : "
               "%d\n",
               dest_path, max, min);
#pragma omp section
        freeMatrix(src, src.height + size - 1);
#pragma omp section
        freeMatrix(filter, filter.height);
    }
    return EXIT_SUCCESS;
}
