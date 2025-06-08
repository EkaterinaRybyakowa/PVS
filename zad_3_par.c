#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

// Заполнение массива случайными значениями от 1 до 100
void fill_random(int *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 100 + 1;
    }
}

// Операции над частями массивов
void compute_add(int *a, int *b, int *sum, int size) {
    for (int i = 0; i < size; i++) sum[i] = a[i] + b[i];
}

void compute_sub(int *a, int *b, int *diff, int size) {
    for (int i = 0; i < size; i++) diff[i] = a[i] - b[i];
}

void compute_mul(int *a, int *b, int *prod, int size) {
    for (int i = 0; i < size; i++) prod[i] = a[i] * b[i];
}

void compute_div(int *a, int *b, double *quot, int size) {
    for (int i = 0; i < size; i++) {
        quot[i] = b[i] != 0 ? (double)a[i] / b[i] : 0.0;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        if (MPI_Init(NULL, NULL), MPI_Comm_rank(MPI_COMM_WORLD, &rank), rank == 0) {
            printf("Использование: mpirun -np <процессы> %s <размер массивов> <кол-во запусков>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int array_size = atoi(argv[1]);
    int runs = atoi(argv[2]);

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (array_size <= 0 || runs <= 0) {
        if (rank == 0) {
            printf("Ошибка: оба параметра должны быть положительными числами.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Проверка делимости массива на количество процессов
    if (array_size % size != 0) {
        if (rank == 0) {
            printf("Ошибка: размер массива (%d) должен делиться на количество процессов (%d).\n", 
                   array_size, size);
        }
        MPI_Finalize();
        return 1;
    }

    int local_size = array_size / size;

    // Локальные массивы для каждого процесса
    int *local_a = malloc(local_size * sizeof(int));
    int *local_b = malloc(local_size * sizeof(int));
    int *local_sum = malloc(local_size * sizeof(int));
    int *local_diff = malloc(local_size * sizeof(int));
    int *local_prod = malloc(local_size * sizeof(int));
    double *local_quot = malloc(local_size * sizeof(double));

    // Глобальные массивы только у процесса 0
    int *a = NULL, *b = NULL;
    if (rank == 0) {
        a = malloc(array_size * sizeof(int));
        b = malloc(array_size * sizeof(int));
    }

    // Накопители времени
    double total_time_add = 0.0;
    double total_time_sub = 0.0;
    double total_time_mul = 0.0;
    double total_time_div = 0.0;

    for (int run = 0; run < runs; run++) {
        if (rank == 0) {
            srand(time(NULL) + run);
            fill_random(a, array_size);
            fill_random(b, array_size);
        }

        // Распределение данных между процессами
        MPI_Scatter(a, local_size, MPI_INT, local_a, local_size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(b, local_size, MPI_INT, local_b, local_size, MPI_INT, 0, MPI_COMM_WORLD);

        // Синхронизация перед замером времени
        MPI_Barrier(MPI_COMM_WORLD);
        double start = MPI_Wtime();
        compute_add(local_a, local_b, local_sum, local_size);
        MPI_Barrier(MPI_COMM_WORLD);
        double end = MPI_Wtime();
        total_time_add += end - start;

        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();
        compute_sub(local_a, local_b, local_diff, local_size);
        MPI_Barrier(MPI_COMM_WORLD);
        end = MPI_Wtime();
        total_time_sub += end - start;

        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();
        compute_mul(local_a, local_b, local_prod, local_size);
        MPI_Barrier(MPI_COMM_WORLD);
        end = MPI_Wtime();
        total_time_mul += end - start;

        MPI_Barrier(MPI_COMM_WORLD);
        start = MPI_Wtime();
        compute_div(local_a, local_b, local_quot, local_size);
        MPI_Barrier(MPI_COMM_WORLD);
        end = MPI_Wtime();
        total_time_div += end - start;
    }

    // Вывод результатов только у процесса 0
    if (rank == 0) {
        printf("Среднее время выполнения операций для массива размером %d за %d запусков:\n", 
               array_size, runs);
        printf("Сложение:    %f секунд(ы)\n", total_time_add / runs);
        printf("Вычитание:   %f секунд(ы)\n", total_time_sub / runs);
        printf("Умножение:   %f секунд(ы)\n", total_time_mul / runs);
        printf("Деление:     %f секунд(ы)\n", total_time_div / runs);
    }

    // Очистка памяти
    free(local_a); free(local_b);
    free(local_sum); free(local_diff); free(local_prod); free(local_quot);
    if (rank == 0) {
        free(a); free(b);
    }

    MPI_Finalize();
    return 0;
}
