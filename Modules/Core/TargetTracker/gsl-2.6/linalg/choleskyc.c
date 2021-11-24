/* linalg/choleskyc.c
 * 
 * Copyright (C) 2007, 2019 Patrick Alken
 * Copyright (C) 2010 Huan Wu
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>

#include "recurse.h"

/*
 * This module contains routines related to the Cholesky decomposition
 * of a complex Hermitian positive definite matrix.
 */

static void cholesky_complex_conj_vector(gsl_vector_complex *v);
static int complex_cholesky_decomp_L2(gsl_matrix_complex * A);
static int complex_cholesky_decomp_L3(gsl_matrix_complex * A);

/*
gsl_linalg_complex_cholesky_decomp()
  Perform the Cholesky decomposition on a Hermitian positive definite
matrix using Level 3 BLAS.

Inputs: A - (input/output) complex positive definite matrix

Return: success or error

Notes:
1) The lower triangle of A is overwritten with the Cholesky decomposition
*/

int
gsl_linalg_complex_cholesky_decomp(gsl_matrix_complex *A)
{
  const size_t N = A->size1;
  
  if (N != A->size2)
    {
      GSL_ERROR("Cholesky decomposition requires square matrix", GSL_ENOTSQR);
    }
  else
    {
      return complex_cholesky_decomp_L3(A);
    }
}

/*
gsl_linalg_complex_cholesky_solve()
  Solve A x = b where A is in cholesky form
*/

int
gsl_linalg_complex_cholesky_solve (const gsl_matrix_complex * cholesky,
                                   const gsl_vector_complex * b,
                                   gsl_vector_complex * x)
{
  if (cholesky->size1 != cholesky->size2)
    {
      GSL_ERROR ("cholesky matrix must be square", GSL_ENOTSQR);
    }
  else if (cholesky->size1 != b->size)
    {
      GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
    }
  else if (cholesky->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      gsl_vector_complex_memcpy (x, b);
      return gsl_linalg_complex_cholesky_svx(cholesky, x);
    }
}

/*
gsl_linalg_complex_cholesky_svx()
  Solve A x = b in place where A is in cholesky form
*/

int
gsl_linalg_complex_cholesky_svx (const gsl_matrix_complex * cholesky,
                                 gsl_vector_complex * x)
{
  if (cholesky->size1 != cholesky->size2)
    {
      GSL_ERROR ("cholesky matrix must be square", GSL_ENOTSQR);
    }
  else if (cholesky->size2 != x->size)
    {
      GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
    }
  else
    {
      /* solve for y using forward-substitution, L y = b */
      gsl_blas_ztrsv (CblasLower, CblasNoTrans, CblasNonUnit, cholesky, x);

      /* perform back-substitution, L^H x = y */
      gsl_blas_ztrsv (CblasLower, CblasConjTrans, CblasNonUnit, cholesky, x);

      return GSL_SUCCESS;
    }
}


/******************************************************************************

gsl_linalg_complex_cholesky_invert()
  Compute the inverse of an Hermitian positive definite matrix in
  Cholesky form.

Inputs: LLT - matrix in cholesky form on input
              A^{-1} = L^{-H} L^{-1} on output

Return: success or error
******************************************************************************/

int
gsl_linalg_complex_cholesky_invert(gsl_matrix_complex * LLT)
{
  if (LLT->size1 != LLT->size2)
    {
      GSL_ERROR ("cholesky matrix must be square", GSL_ENOTSQR);
    }
  else
    {
      int status;
      size_t N = LLT->size1;
      size_t i, j;

      /* invert the lower triangle of LLT */
      status = gsl_linalg_complex_tri_invert(CblasLower, CblasNonUnit, LLT);
      if (status)
        return status;

      /* compute A^{-1} = L^{-H} L^{-1} */
      status = gsl_linalg_complex_tri_LHL(LLT);
      if (status)
        return status;

      /* copy the Hermitian lower triangle to the upper triangle */
      for (i = 1; i < N; ++i)
        {
          for (j = 0; j < i; ++j)
            {
              gsl_complex z = gsl_matrix_complex_get(LLT, i, j);
              gsl_matrix_complex_set(LLT, j, i, gsl_complex_conjugate(z));
            }
        }

      return GSL_SUCCESS;
    }
}


/********************************************
 *           INTERNAL ROUTINES              *
 ********************************************/

static void
cholesky_complex_conj_vector(gsl_vector_complex *v)
{
  size_t i;

  for (i = 0; i < v->size; ++i)
    {
      gsl_complex * vi = gsl_vector_complex_ptr(v, i);
      GSL_IMAG(*vi) = -GSL_IMAG(*vi);
    }
}

/*
complex_cholesky_decomp_L2()
  Perform the Cholesky decomposition on a Hermitian positive definite
matrix using Level 2 BLAS. See Golub & Van Loan, "Matrix Computations" (3rd ed),
algorithm 4.2.2.

Inputs: A - (input/output) complex postive definite matrix

Return: success or error

Notes:
1) The lower triangle of A is overwritten with the Cholesky decomposition
*/

static int
complex_cholesky_decomp_L2(gsl_matrix_complex * A)
{
  const size_t N = A->size1;
  
  if (N != A->size2)
    {
      GSL_ERROR("Cholesky decomposition requires square matrix", GSL_ENOTSQR);
    }
  else
    {
      size_t j;
      gsl_complex z;
      double ajj;

      for (j = 0; j < N; ++j)
        {
          z = gsl_matrix_complex_get(A, j, j);
          ajj = GSL_REAL(z);

          if (j > 0)
            {
              gsl_vector_complex_const_view aj =
                gsl_matrix_complex_const_subrow(A, j, 0, j);

              gsl_blas_zdotc(&aj.vector, &aj.vector, &z);
              ajj -= GSL_REAL(z);
            }

          if (ajj <= 0.0)
            {
              GSL_ERROR("matrix is not positive definite", GSL_EDOM);
            }

          ajj = sqrt(ajj);
          GSL_SET_COMPLEX(&z, ajj, 0.0);
          gsl_matrix_complex_set(A, j, j, z);

          if (j < N - 1)
            {
              gsl_vector_complex_view av =
                gsl_matrix_complex_subcolumn(A, j, j + 1, N - j - 1);

              if (j > 0)
                {
                  gsl_vector_complex_view aj =
                    gsl_matrix_complex_subrow(A, j, 0, j);
                  gsl_matrix_complex_view am =
                    gsl_matrix_complex_submatrix(A, j + 1, 0, N - j - 1, j);

                  cholesky_complex_conj_vector(&aj.vector);

                  gsl_blas_zgemv(CblasNoTrans,
                                 GSL_COMPLEX_NEGONE,
                                 &am.matrix,
                                 &aj.vector,
                                 GSL_COMPLEX_ONE,
                                 &av.vector);

                  cholesky_complex_conj_vector(&aj.vector);
                }

              gsl_blas_zdscal(1.0 / ajj, &av.vector);
            }
        }

      return GSL_SUCCESS;
    }
}

/*
complex_cholesky_decomp_L3()
  Perform the Cholesky decomposition on a Hermitian positive definite
matrix using Level 3 BLAS.

Inputs: A - (input/output) complex postive definite matrix

Return: success or error

Notes:
1) The lower triangle of A is overwritten with the Cholesky decomposition

2) Based on ReLAPACK recursive variant with Level 3 BLAS
*/

static int
complex_cholesky_decomp_L3(gsl_matrix_complex * A)
{
  const size_t N = A->size1;
  
  if (N != A->size2)
    {
      GSL_ERROR("Cholesky decomposition requires square matrix", GSL_ENOTSQR);
    }
  else if (N <= CROSSOVER_CHOLESKY)
    {
      /* use unblocked Level 2 algorithm */
      return complex_cholesky_decomp_L2(A);
    }
  else
    {
      /*
       * partition matrix:
       *
       * A11 A12
       * A21 A22
       *
       * where A11 is N1-by-N1
       */
      int status;
      const size_t N1 = GSL_LINALG_SPLIT_COMPLEX(N);
      const size_t N2 = N - N1;
      gsl_matrix_complex_view A11 = gsl_matrix_complex_submatrix(A, 0, 0, N1, N1);
      gsl_matrix_complex_view A21 = gsl_matrix_complex_submatrix(A, N1, 0, N2, N1);
      gsl_matrix_complex_view A22 = gsl_matrix_complex_submatrix(A, N1, N1, N2, N2);

      /* recursion on A11 */
      status = complex_cholesky_decomp_L3(&A11.matrix);
      if (status)
        return status;

      /* A21 = A21 * A11^{-1} */
      gsl_blas_ztrsm(CblasRight, CblasLower, CblasConjTrans, CblasNonUnit, GSL_COMPLEX_ONE, &A11.matrix, &A21.matrix);

      /* A22 -= A21 A21^H */
      gsl_blas_zherk(CblasLower, CblasNoTrans, -1.0, &A21.matrix, 1.0, &A22.matrix);

      /* recursion on A22 */
      status = complex_cholesky_decomp_L3(&A22.matrix);
      if (status)
        return status;

      return GSL_SUCCESS;
    }
}

