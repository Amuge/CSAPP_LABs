/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int x1, x2, x3, x4, x5, x6, x7, x8;
    if (M == 32)
    {
        for (int i = 0; i < M; i += 8)
        {
            for (int j = 0; j < N; j += 8)
            {
                // copy the rows of A to B
                for (int x = 0; x < 8; ++x)
                {
                    x1 = A[i + x][j];
                    x2 = A[i + x][j + 1];
                    x3 = A[i + x][j + 2];
                    x4 = A[i + x][j + 3];
                    x5 = A[i + x][j + 4];
                    x6 = A[i + x][j + 5];
                    x7 = A[i + x][j + 6];
                    x8 = A[i + x][j + 7];
                    B[j + x][i] = x1;
                    B[j + x][i + 1] = x2;
                    B[j + x][i + 2] = x3;
                    B[j + x][i + 3] = x4;
                    B[j + x][i + 4] = x5;
                    B[j + x][i + 5] = x6;
                    B[j + x][i + 6] = x7;
                    B[j + x][i + 7] = x8;
                }
                // transform B
                for (int k = 0; k < 8; ++k)
                {
                    for (int m = 0; m < k; ++m)
                    {
                        x1 = B[j + k][i + m];
                        B[j + k][i + m] = B[j + m][i + k];
                        B[j + m][i + k] = x1;
                    }
                }
            }
        }
    }
    if (M == 64)
    {
        for (int i = 0; i < M; i += 8)
        {
            for (int j = 0; j < N; j += 8)
            {
                // move the first 4 rows of A to B
                // 8 * 8 -> 4 *(4*4) block ,represent as A[0][0],A[0][1],A[1][0],A[1][1]
                /*
                    <<<<<<<>>>>>>>
                    < 00  <> 01  >
                    <<<<<<<>>>>>>>
                    < 10  <> 11  >
                    <<<<<<<>>>>>>>
                */
                for (int k = 0; k < 4; ++k)
                {
                    x1 = A[i + k][j];
                    x2 = A[i + k][j + 1];
                    x3 = A[i + k][j + 2];
                    x4 = A[i + k][j + 3];
                    x5 = A[i + k][j + 4];
                    x6 = A[i + k][j + 5];
                    x7 = A[i + k][j + 6];
                    x8 = A[i + k][j + 7];

                    // B[0][0] = Transform of A[0][0]
                    B[j][i + k] = x1;
                    B[j + 1][i + k] = x2;
                    B[j + 2][i + k] = x3;
                    B[j + 3][i + k] = x4;

                    // B[0][1] = Transform of A[0][1]
                    B[j][i + k + 4] = x5;
                    B[j + 1][i + k + 4] = x6;
                    B[j + 2][i + k + 4] = x7;
                    B[j + 3][i + k + 4] = x8;
                }

                // move the second 4 row of A to B,Set B[0][1] and B[1][0]
                for (int k = 0; k < 4; ++k)
                {
                    // get the col val of  A[1][0]
                    x5 = A[i + 4][j + k];
                    x6 = A[i + 5][j + k];
                    x7 = A[i + 6][j + k];
                    x8 = A[i + 7][j + k];
                    // get the row val of B[0][1]
                    x1 = B[j + k][i + 4];
                    x2 = B[j + k][i + 5];
                    x3 = B[j + k][i + 6];
                    x4 = B[j + k][i + 7];
                    // move col of A[1][0] to corresponding row of B[0][1]
                    B[j + k][i + 4] = x5;
                    B[j + k][i + 5] = x6;
                    B[j + k][i + 6] = x7;
                    B[j + k][i + 7] = x8;
                    // copy the row of B[0][1] to B[1][0]
                    B[j + 4 + k][i] = x1;
                    B[j + 4 + k][i + 1] = x2;
                    B[j + 4 + k][i + 2] = x3;
                    B[j + 4 + k][i + 3] = x4;
                }
                // Set B[1][1]
                for (int k = 4; k < 8; ++k)
                {
                    x1 = A[i + k][j + 4];
                    x2 = A[i + k][j + 5];
                    x3 = A[i + k][j + 6];
                    x4 = A[i + k][j + 7];
                    B[j + k][i + 4] = x1;
                    B[j + k][i + 5] = x2;
                    B[j + k][i + 6] = x3;
                    B[j + k][i + 7] = x4;
                }

                for (int k = 4; k < 8; ++k)
                {
                    for (int m = 4; m < k; ++m)
                    {
                        x1 = B[j + k][i + m];
                        B[j + k][i + m] = B[j + m][i + k];
                        B[j + m][i + k] = x1;
                    }
                }
            }
        }
    }

    if (M == 61)
    {
        int i, j, ii, jj;
        for (i = 0; i + 16 < N; i += 16)
        {
            for (j = 0; j + 16 < M; j += 16)
            {
                for (int x = 0; x < 16; ++x)
                {
                    x1 = A[i + x][j];
                    x2 = A[i + x][j + 1];
                    x3 = A[i + x][j + 2];
                    x4 = A[i + x][j + 3];
                    x5 = A[i + x][j + 4];
                    x6 = A[i + x][j + 5];
                    x7 = A[i + x][j + 6];
                    x8 = A[i + x][j + 7];
                    B[j][i + x] = x1;
                    B[j + 1][i + x] = x2;
                    B[j + 2][i + x] = x3;
                    B[j + 3][i + x] = x4;
                    B[j + 4][i + x] = x5;
                    B[j + 5][i + x] = x6;
                    B[j + 6][i + x] = x7;
                    B[j + 7][i + x] = x8;

                    x1 = A[i + x][j + 8];
                    x2 = A[i + x][j + 9];
                    x3 = A[i + x][j + 10];
                    x4 = A[i + x][j + 11];
                    x5 = A[i + x][j + 12];
                    x6 = A[i + x][j + 13];
                    x7 = A[i + x][j + 14];
                    x8 = A[i + x][j + 15];
                    B[j + 8][i + x] = x1;
                    B[j + 9][i + x] = x2;
                    B[j + 10][i + x] = x3;
                    B[j + 11][i + x] = x4;
                    B[j + 12][i + x] = x5;
                    B[j + 13][i + x] = x6;
                    B[j + 14][i + x] = x7;
                    B[j + 15][i + x] = x8;
                }
            }
        }

        for (ii = i; ii < N; ++ii)
        {
            for (jj = 0; jj < M; ++jj)
                B[jj][ii] = A[ii][jj];
        }
        for (ii = 0; ii < i; ++ii)
        {
            for (jj = j; jj < M; ++jj)
                B[jj][ii] = A[ii][jj];
        }
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
