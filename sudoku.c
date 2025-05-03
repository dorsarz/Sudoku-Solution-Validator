#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

int sudoku[SIZE][SIZE];
int results[11] = {0};

void* check_rows(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        int seen[10] = {0};
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || seen[num]) {
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    results[0] = 1;
    pthread_exit(NULL);
}

void* check_cols(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        int seen[10] = {0};
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[j][i];
            if (num < 1 || num > 9 || seen[num]) {
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    results[1] = 1;
    pthread_exit(NULL);
}

typedef struct {
    int startRow;
    int startCol;
    int index;
} parameters;

void* check_subgrid(void* param) {
    parameters* p = (parameters*) param;
    int seen[10] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[p->startRow + i][p->startCol + j];
            if (num < 1 || num > 9 || seen[num]) {
                free(p);
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    results[p->index] = 1;
    free(p);
    pthread_exit(NULL);
}

int main() {
    printf("لطفاً جدول سودوکو را به صورت 9 ردیف وارد کنید (اعداد 1 تا 9):\n");

    for (int i = 0; i < SIZE; i++) {
        printf("ردیف %d: ", i + 1);
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &sudoku[i][j]);
        }
    }

    pthread_t threads[11];

    pthread_create(&threads[0], NULL, check_rows, NULL);
    pthread_create(&threads[1], NULL, check_cols, NULL);

    int threadIndex = 2;
    for (int row = 0; row < SIZE; row += 3) {
        for (int col = 0; col < SIZE; col += 3) {
            parameters* data = (parameters*) malloc(sizeof(parameters));
            data->startRow = row;
            data->startCol = col;
            data->index = threadIndex;
            pthread_create(&threads[threadIndex], NULL, check_subgrid, data);
            threadIndex++;
        }
    }

    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }

    int valid = 1;
    for (int i = 0; i < 11; i++) {
        if (results[i] == 0) {
            valid = 0;
            break;
        }
    }

    if (valid)
        printf("سودوکو معتبر است\n");
    else
        printf("سودوکو معتبر نیست\n");

    return 0;
}
