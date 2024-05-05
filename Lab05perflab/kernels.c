/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*
 * Please fill in the following team struct
 */
team_t team = {
    "Breath", /* Team name */

    "John Wick",          /* First member full name */
    "john@hightable.edu", /* First member email address */

    "Link", /* Second member full name (leave blank if none) */
    "Zelda" /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/*
 * naive_rotate - The naive baseline version of rotate
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst)
{
    int block_size = 16;
    for (int i = 0; i < dim; i += block_size)
    {
        for (int j = 0; j < dim; j += block_size)
        {
            for (int k = i; k < i + block_size; ++k)
            {
                for (int s = j; s < j + block_size; ++s)
                {
                    dst[RIDX(dim - 1 - s, k, dim)] = src[RIDX(k, s, dim)];
                }
            }
        }
    }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_rotate_functions()
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    /* ... Register additional test functions here */
}

/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct
{
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
static void initialize_pixel_sum(pixel_sum *sum)
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
static void accumulate_sum(pixel_sum *sum, pixel p)
{
    sum->red += (int)p.red;
    sum->green += (int)p.green;
    sum->blue += (int)p.blue;
    sum->num++;
    return;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short)(sum.red / sum.num);
    current_pixel->green = (unsigned short)(sum.green / sum.num);
    current_pixel->blue = (unsigned short)(sum.blue / sum.num);
    return;
}

/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
        for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
            accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
            dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth.
 * IMPORTANT: This is the version you will be graded on
 */
static void set_corner(int idx, pixel *src, pixel *des, int p1, int p2, int p3)
{
    des[idx].blue = (src[idx].blue + src[p1].blue + src[p2].blue + src[p3].blue) >> 2;
    des[idx].green = (src[idx].green + src[p1].green + src[p2].green + src[p3].green) >> 2;
    des[idx].red = (src[idx].red + src[p1].red + src[p2].red + src[p3].red) >> 2;
}

static void set_top(pixel *src, pixel *des, int col, int dim)
{
    des[col].blue = (src[col - 1].blue + src[col].blue + src[col + 1].blue +
                     src[dim + col - 1].blue + src[dim + col].blue + src[dim + col + 1].blue) /
                    6;
    des[col].green = (src[col - 1].green + src[col].green + src[col + 1].green +
                      src[dim + col - 1].green + src[dim + col].green + src[dim + col + 1].green) /
                     6;
    des[col].red = (src[col - 1].red + src[col].red + src[col + 1].red +
                    src[dim + col - 1].red + src[dim + col].red + src[dim + col + 1].red) /
                   6;
}
static void set_left(pixel *src, pixel *des, int row, int dim)
{
    des[RIDX(row, 0, dim)].blue = (src[RIDX(row - 1, 0, dim)].blue + src[RIDX(row, 0, dim)].blue + src[RIDX(row + 1, 0, dim)].blue +
                                   src[RIDX(row - 1, 1, dim)].blue + src[RIDX(row, 1, dim)].blue + src[RIDX(row + 1, 1, dim)].blue) /
                                  6;
    des[RIDX(row, 0, dim)].green = (src[RIDX(row - 1, 0, dim)].green + src[RIDX(row, 0, dim)].green + src[RIDX(row + 1, 0, dim)].green +
                                    src[RIDX(row - 1, 1, dim)].green + src[RIDX(row, 1, dim)].green + src[RIDX(row + 1, 1, dim)].green) /
                                   6;
    des[RIDX(row, 0, dim)].red = (src[RIDX(row - 1, 0, dim)].red + src[RIDX(row, 0, dim)].red + src[RIDX(row + 1, 0, dim)].red +
                                  src[RIDX(row - 1, 1, dim)].red + src[RIDX(row, 1, dim)].red + src[RIDX(row + 1, 1, dim)].red) /
                                 6;
}
static void set_right(pixel *src, pixel *des, int row, int dim)
{
    des[RIDX(row, dim - 1, dim)].blue = (src[RIDX(row - 1, dim - 1, dim)].blue + src[RIDX(row, dim - 1, dim)].blue + src[RIDX(row + 1, dim - 1, dim)].blue +
                                         src[RIDX(row - 1, dim - 2, dim)].blue + src[RIDX(row, dim - 2, dim)].blue + src[RIDX(row + 1, dim - 2, dim)].blue) /
                                        6;
    des[RIDX(row, dim - 1, dim)].green = (src[RIDX(row - 1, dim - 1, dim)].green + src[RIDX(row, dim - 1, dim)].green + src[RIDX(row + 1, dim - 1, dim)].green +
                                          src[RIDX(row - 1, dim - 2, dim)].green + src[RIDX(row, dim - 2, dim)].green + src[RIDX(row + 1, dim - 2, dim)].green) /
                                         6;
    des[RIDX(row, dim - 1, dim)].red = (src[RIDX(row - 1, dim - 1, dim)].red + src[RIDX(row, dim - 1, dim)].red + src[RIDX(row + 1, dim - 1, dim)].red +
                                        src[RIDX(row - 1, dim - 2, dim)].red + src[RIDX(row, dim - 2, dim)].red + src[RIDX(row + 1, dim - 2, dim)].red) /
                                       6;
}
static void set_bottom(pixel *src, pixel *des, int col, int dim)
{
    des[RIDX(dim - 1, col, dim)].blue = (src[RIDX(dim - 1, col - 1, dim)].blue + src[RIDX(dim - 1, col, dim)].blue + src[RIDX(dim - 1, col + 1, dim)].blue +
                                         src[RIDX(dim - 2, col - 1, dim)].blue + src[RIDX(dim - 2, col, dim)].blue + src[RIDX(dim - 2, col + 1, dim)].blue) /
                                        6;
    des[RIDX(dim - 1, col, dim)].green = (src[RIDX(dim - 1, col - 1, dim)].green + src[RIDX(dim - 1, col, dim)].green + src[RIDX(dim - 1, col + 1, dim)].green +
                                          src[RIDX(dim - 2, col - 1, dim)].green + src[RIDX(dim - 2, col, dim)].green + src[RIDX(dim - 2, col + 1, dim)].green) /
                                         6;
    des[RIDX(dim - 1, col, dim)].red = (src[RIDX(dim - 1, col - 1, dim)].red + src[RIDX(dim - 1, col, dim)].red + src[RIDX(dim - 1, col + 1, dim)].red +
                                        src[RIDX(dim - 2, col - 1, dim)].red + src[RIDX(dim - 2, col, dim)].red + src[RIDX(dim - 2, col + 1, dim)].red) /
                                       6;
}
static void set_in(pixel *src, pixel *des, int idx, int dim)
{
    des[idx].blue = (src[idx - dim - 1].blue + src[idx - dim].blue + src[idx - dim + 1].blue +
                     src[idx - 1].blue + src[idx].blue + src[idx + 1].blue +
                     src[idx + dim - 1].blue + src[idx + dim].blue + src[idx + dim + 1].blue) /
                    9;
    des[idx].green = (src[idx - dim - 1].green + src[idx - dim].green + src[idx - dim + 1].green +
                      src[idx - 1].green + src[idx].green + src[idx + 1].green +
                      src[idx + dim - 1].green + src[idx + dim].green + src[idx + dim + 1].green) /
                     9;
    des[idx].red = (src[idx - dim - 1].red + src[idx - dim].red + src[idx - dim + 1].red +
                    src[idx - 1].red + src[idx].red + src[idx + 1].red +
                    src[idx + dim - 1].red + src[idx + dim].red + src[idx + dim + 1].red) /
                   9;
}

char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{
    // set corner
    set_corner(RIDX(0, 0, dim), src, dst, RIDX(0, 1, dim), RIDX(1, 0, dim), RIDX(1, 1, dim));
    set_corner(RIDX(0, dim - 1, dim), src, dst, RIDX(0, dim - 2, dim), RIDX(1, dim - 2, dim), RIDX(1, dim - 1, dim));
    set_corner(RIDX(dim - 1, 0, dim), src, dst, RIDX(dim - 2, 0, dim), RIDX(dim - 2, 1, dim), RIDX(dim - 1, 1, dim));
    set_corner(RIDX(dim - 1, dim - 1, dim), src, dst, RIDX(dim - 1, dim - 2, dim), RIDX(dim - 2, dim - 2, dim), RIDX(dim - 2, dim - 1, dim));

    // set left, right, top and bottom
    for (int i = 1; i <= dim - 2; ++i)
    {
        set_top(src, dst, i, dim);
        set_left(src, dst, i, dim);
        set_right(src, dst, i, dim);
        set_bottom(src, dst, i, dim);
    }

    // set inside
    for (int i = 1; i <= dim - 2; ++i)
    {
        for (int j = 1; j <= dim - 2; ++j)
        {
            set_in(src, dst, RIDX(i, j, dim), dim);
        }
    }
}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions()
{
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}
