#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Заполнение массива случайными значениями от 1 до 100
void fill_random(int *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 100 + 1;
    }
}

// Операция сложения массивов
void add_arrays(int *a, int *b, int *sum, int size) {
    for (int i = 0; i < size; i++) {
        sum[i] = a[i] + b[i];
    }
}

// Операция вычитания массивов
void subtract_arrays(int *a, int *b, int *diff, int size) {
    for (int i = 0; i < size; i++) {
        diff[i] = a[i] - b[i];
    }
}

// Операция умножения массивов
void multiply_arrays(int *a, int *b, int *prod, int size) {
    for (int i = 0; i < size; i++) {
        prod[i] = a[i] * b[i];
    }
}

// Операция деления массивов
void divide_arrays(int *a, int *b, double *quot, int size) {
    for (int i = 0; i < size; i++) {
        quot[i] = (b[i] != 0) ? ((double)a[i] / b[i]) : 0.0;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Используйте: %s <размер массива> <количество запусков>\n", argv[0]);
        return 1;
    }

    int array_size = atoi(argv[1]);
    int runs = atoi(argv[2]);

    if (array_size <= 0 || runs <= 0) {
        printf("Ошибка: оба параметра должны быть положительными числами.\n");
        return 1;
    }

    int *a = malloc(sizeof(int) * array_size);
    int *b = malloc(sizeof(int) * array_size);
    int *sum = malloc(sizeof(int) * array_size);
    int *diff = malloc(sizeof(int) * array_size);
    int *prod = malloc(sizeof(int) * array_size);
    double *quot = malloc(sizeof(double) * array_size);

    if (!a || !b || !sum || !diff || !prod || !quot) {
        printf("Ошибка выделения памяти.\n");
        return 1;
    }

    // Время для каждой операции
    double time_add = 0.0;
    double time_sub = 0.0;
    double time_mul = 0.0;
    double time_div = 0.0;

    for (int run = 0; run < runs; run++) {
        srand(time(NULL) + run);
        fill_random(a, array_size);
        fill_random(b, array_size);

        clock_t start, end;

        // Сложение
        start = clock();
        add_arrays(a, b, sum, array_size);
        end = clock();
        time_add += (double)(end - start) / CLOCKS_PER_SEC;

        // Вычитание
        start = clock();
        subtract_arrays(a, b, diff, array_size);
        end = clock();
        time_sub += (double)(end - start) / CLOCKS_PER_SEC;

        // Умножение
        start = clock();
        multiply_arrays(a, b, prod, array_size);
        end = clock();
        time_mul += (double)(end - start) / CLOCKS_PER_SEC;

        // Деление
        start = clock();
        divide_arrays(a, b, quot, array_size);
        end = clock();
        time_div += (double)(end - start) / CLOCKS_PER_SEC;
    }

    printf("Среднее время выполнения операций для массива размером %d за %d запусков:\n", array_size, runs);
    printf("Сложение:    %f секунд(ы)\n", time_add / runs);
    printf("Вычитание:   %f секунд(ы)\n", time_sub / runs);
    printf("Умножение:   %f секунд(ы)\n", time_mul / runs);
    printf("Деление:     %f секунд(ы)\n", time_div / runs);

    free(a); free(b); free(sum); free(diff); free(prod); free(quot);
    return 0;
}
