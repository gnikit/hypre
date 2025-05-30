/******************************************************************************
 * Copyright (c) 1998 Lawrence Livermore National Security, LLC and other
 * HYPRE Project Developers. See the top-level COPYRIGHT file for details.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 ******************************************************************************/

#ifndef HYPRE_IJ_MV_HEADER
#define HYPRE_IJ_MV_HEADER

#include "HYPRE_config.h"
#include "HYPRE_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

/**
 * @defgroup IJSystemInterface IJ System Interface
 *
 * A linear-algebraic conceptual interface. This interface represents a
 * linear-algebraic conceptual view of a linear system.  The 'I' and 'J' in the
 * name are meant to be mnemonic for the traditional matrix notation A(I,J).
 *
 * @{
 **/

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

/**
 * @name IJ Matrices
 *
 * @{
 **/

struct hypre_IJMatrix_struct;
/**
 * The matrix object.
 **/
typedef struct hypre_IJMatrix_struct *HYPRE_IJMatrix;

/**
 * Create a matrix object.  Each process owns some unique consecutive
 * range of rows, indicated by the global row indices \e ilower and
 * \e iupper.  The row data is required to be such that the value
 * of \e ilower on any process \f$p\f$ be exactly one more than the
 * value of \e iupper on process \f$p-1\f$.  Note that the first row of
 * the global matrix may start with any integer value.  In particular,
 * one may use zero- or one-based indexing.
 *
 * For square matrices, \e jlower and \e jupper typically should
 * match \e ilower and \e iupper, respectively.  For rectangular
 * matrices, \e jlower and \e jupper should define a
 * partitioning of the columns.  This partitioning must be used for
 * any vector \f$v\f$ that will be used in matrix-vector products with the
 * rectangular matrix.  The matrix data structure may use \e jlower
 * and \e jupper to store the diagonal blocks (rectangular in
 * general) of the matrix separately from the rest of the matrix.
 *
 * Collective.
 **/
HYPRE_Int HYPRE_IJMatrixCreate(MPI_Comm        comm,
                               HYPRE_BigInt    ilower,
                               HYPRE_BigInt    iupper,
                               HYPRE_BigInt    jlower,
                               HYPRE_BigInt    jupper,
                               HYPRE_IJMatrix *matrix);

/**
 * Destroy a matrix object.  An object should be explicitly destroyed
 * using this destructor when the user's code no longer needs direct
 * access to it.  Once destroyed, the object must not be referenced
 * again.  Note that the object may not be deallocated at the
 * completion of this call, since there may be internal package
 * references to the object.  The object will then be destroyed when
 * all internal reference counts go to zero.
 **/
HYPRE_Int HYPRE_IJMatrixDestroy(HYPRE_IJMatrix matrix);

/**
 * Prepare a matrix object for setting coefficient values.  This
 * routine will also re-initialize an already assembled matrix,
 * allowing users to modify coefficient values.
 **/
HYPRE_Int HYPRE_IJMatrixInitialize(HYPRE_IJMatrix matrix);

/**
 * Prepare a matrix object for setting coefficient values.  This
 * routine will also re-initialize an already assembled matrix,
 * allowing users to modify coefficient values. This routine
 * also specifies the memory location, i.e. host or device.
 **/
HYPRE_Int HYPRE_IJMatrixInitialize_v2(HYPRE_IJMatrix matrix, HYPRE_MemoryLocation memory_location);

/**
 * Sets values for \e nrows rows or partial rows of the matrix.
 * The arrays \e ncols
 * and \e rows are of dimension \e nrows and contain the number
 * of columns in each row and the row indices, respectively.  The
 * array \e cols contains the column indices for each of the \e
 * rows, and is ordered by rows.  The data in the \e values array
 * corresponds directly to the column entries in \e cols.  Erases
 * any previous values at the specified locations and replaces them
 * with new ones, or, if there was no value there before, inserts a
 * new one if set locally. Note that it is not possible to set values
 * on other processors. If one tries to set a value from proc i on proc j,
 * proc i will erase all previous occurrences of this value in its stack
 * (including values generated with AddToValues), and treat it like
 * a zero value. The actual value needs to be set on proc j.
 *
 * Note that a threaded version (threaded over the number of rows)
 * will be called if
 * HYPRE_IJMatrixSetOMPFlag is set to a value != 0.
 * This requires that rows[i] != rows[j] for i!= j
 * and is only efficient if a large number of rows is set in one call
 * to HYPRE_IJMatrixSetValues.
 *
 * Not collective.
 *
 **/
HYPRE_Int HYPRE_IJMatrixSetValues(HYPRE_IJMatrix       matrix,
                                  HYPRE_Int            nrows,
                                  HYPRE_Int           *ncols,
                                  const HYPRE_BigInt  *rows,
                                  const HYPRE_BigInt  *cols,
                                  const HYPRE_Complex *values);

/**
 * Sets all matrix coefficients of an already assembled matrix to
 * \e value
 **/
HYPRE_Int HYPRE_IJMatrixSetConstantValues(HYPRE_IJMatrix matrix,
                                          HYPRE_Complex value);

/**
 * Adds to values for \e nrows rows or partial rows of the matrix.
 * Usage details are analogous to \ref HYPRE_IJMatrixSetValues.
 * Adds to any previous values at the specified locations, or, if
 * there was no value there before, inserts a new one.
 * AddToValues can be used to add to values on other processors.
 *
 * Note that a threaded version (threaded over the number of rows)
 * will be called if
 * HYPRE_IJMatrixSetOMPFlag is set to a value != 0.
 * This requires that rows[i] != rows[j] for i!= j
 * and is only efficient if a large number of rows is added in one call
 * to HYPRE_IJMatrixAddToValues.
 *
 * Not collective.
 *
 **/
HYPRE_Int HYPRE_IJMatrixAddToValues(HYPRE_IJMatrix       matrix,
                                    HYPRE_Int            nrows,
                                    HYPRE_Int           *ncols,
                                    const HYPRE_BigInt  *rows,
                                    const HYPRE_BigInt  *cols,
                                    const HYPRE_Complex *values);

/**
 * Sets values for \e nrows rows or partial rows of the matrix.
 *
 * Same as IJMatrixSetValues, but with an additional \e row_indexes array
 * that provides indexes into the \e cols and \e values arrays.  Because
 * of this, there can be gaps between the row data in these latter two arrays.
 *
 **/
HYPRE_Int HYPRE_IJMatrixSetValues2(HYPRE_IJMatrix       matrix,
                                   HYPRE_Int            nrows,
                                   HYPRE_Int           *ncols,
                                   const HYPRE_BigInt  *rows,
                                   const HYPRE_Int     *row_indexes,
                                   const HYPRE_BigInt  *cols,
                                   const HYPRE_Complex *values);

/**
 * Adds to values for \e nrows rows or partial rows of the matrix.
 *
 * Same as IJMatrixAddToValues, but with an additional \e row_indexes array
 * that provides indexes into the \e cols and \e values arrays.  Because
 * of this, there can be gaps between the row data in these latter two arrays.
 *
 **/
HYPRE_Int HYPRE_IJMatrixAddToValues2(HYPRE_IJMatrix       matrix,
                                     HYPRE_Int            nrows,
                                     HYPRE_Int           *ncols,
                                     const HYPRE_BigInt  *rows,
                                     const HYPRE_Int     *row_indexes,
                                     const HYPRE_BigInt  *cols,
                                     const HYPRE_Complex *values);

/**
 * Finalize the construction of the matrix before using.
 **/
HYPRE_Int HYPRE_IJMatrixAssemble(HYPRE_IJMatrix matrix);

/**
 * Gets number of nonzeros elements for \e nrows rows specified in \e rows
 * and returns them in \e ncols, which needs to be allocated by the
 * user.
 **/
HYPRE_Int HYPRE_IJMatrixGetRowCounts(HYPRE_IJMatrix  matrix,
                                     HYPRE_Int       nrows,
                                     HYPRE_BigInt   *rows,
                                     HYPRE_Int      *ncols);

/**
 * Gets values for \e nrows rows or partial rows of the matrix.
 * Usage details are mostly
 * analogous to \ref HYPRE_IJMatrixSetValues.
 * Note that if nrows is negative, the routine will return
 * the column_indices and matrix coefficients of the
 * (-nrows) rows contained in rows.
 **/
HYPRE_Int HYPRE_IJMatrixGetValues(HYPRE_IJMatrix  matrix,
                                  HYPRE_Int       nrows,
                                  HYPRE_Int      *ncols,
                                  HYPRE_BigInt   *rows,
                                  HYPRE_BigInt   *cols,
                                  HYPRE_Complex  *values);

/**
 * Gets values for \e nrows rows or partial rows of the matrix.
 *
 * Same as IJMatrixGetValues, but with an additional \e row_indexes array
 * that provides indexes into the \e cols and \e values arrays.  Because
 * of this, there can be gaps between the row data in these latter two arrays.
 *
 **/
HYPRE_Int HYPRE_IJMatrixGetValues2(HYPRE_IJMatrix  matrix,
                                   HYPRE_Int       nrows,
                                   HYPRE_Int      *ncols,
                                   HYPRE_BigInt   *rows,
                                   HYPRE_Int      *row_indexes,
                                   HYPRE_BigInt   *cols,
                                   HYPRE_Complex  *values);

/**
 * Gets values for \e nrows rows or partial rows of the matrix
 * and zeros out those entries in the matrix.
 *
 * Same as IJMatrixGetValues2, but zeros out the entries after getting them.
 *
 **/
HYPRE_Int HYPRE_IJMatrixGetValuesAndZeroOut(HYPRE_IJMatrix  matrix,
                                            HYPRE_Int       nrows,
                                            HYPRE_Int      *ncols,
                                            HYPRE_BigInt   *rows,
                                            HYPRE_Int      *row_indexes,
                                            HYPRE_BigInt   *cols,
                                            HYPRE_Complex  *values);

/**
 * Set the storage type of the matrix object to be constructed.
 * Currently, \e type can only be \c HYPRE_PARCSR.
 *
 * Not collective, but must be the same on all processes.
 *
 * @see HYPRE_IJMatrixGetObject
 **/
HYPRE_Int HYPRE_IJMatrixSetObjectType(HYPRE_IJMatrix matrix,
                                      HYPRE_Int      type);

/**
 * Get the storage type of the constructed matrix object.
 **/
HYPRE_Int HYPRE_IJMatrixGetObjectType(HYPRE_IJMatrix  matrix,
                                      HYPRE_Int      *type);

/**
 * Gets range of rows owned by this processor and range
 * of column partitioning for this processor.
 **/
HYPRE_Int HYPRE_IJMatrixGetLocalRange(HYPRE_IJMatrix  matrix,
                                      HYPRE_BigInt   *ilower,
                                      HYPRE_BigInt   *iupper,
                                      HYPRE_BigInt   *jlower,
                                      HYPRE_BigInt   *jupper);

/**
 * Gets global information about the matrix, including the total number of rows,
 * columns, and nonzero elements across all processes.
 *
 * @param matrix The IJMatrix object to query.
 * @param global_num_rows Pointer to store the total number of rows in the matrix.
 * @param global_num_cols Pointer to store the total number of columns in the matrix.
 * @param global_num_nonzeros Pointer to store the total number of nonzero elements in the matrix.
 *
 * @return HYPRE_Int Error code.
 *
 * Collective (must be called by all processes).
 **/
HYPRE_Int HYPRE_IJMatrixGetGlobalInfo(HYPRE_IJMatrix  matrix,
                                      HYPRE_BigInt   *global_num_rows,
                                      HYPRE_BigInt   *global_num_cols,
                                      HYPRE_BigInt   *global_num_nonzeros);

/**
 * Get a reference to the constructed matrix object.
 *
 * @see HYPRE_IJMatrixSetObjectType
 **/
HYPRE_Int HYPRE_IJMatrixGetObject(HYPRE_IJMatrix   matrix,
                                  void           **object);

/**
 * (Optional) Set the max number of nonzeros to expect in each row.
 * The array \e sizes contains estimated sizes for each row on this
 * process.  This call can significantly improve the efficiency of
 * matrix construction, and should always be utilized if possible.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJMatrixSetRowSizes(HYPRE_IJMatrix   matrix,
                                    const HYPRE_Int *sizes);

/**
 * (Optional) Sets the exact number of nonzeros in each row of
 * the diagonal and off-diagonal blocks.  The diagonal block is the
 * submatrix whose column numbers correspond to rows owned by this
 * process, and the off-diagonal block is everything else.  The arrays
 * \e diag_sizes and \e offdiag_sizes contain estimated sizes
 * for each row of the diagonal and off-diagonal blocks, respectively.
 * This routine can significantly improve the efficiency of matrix
 * construction, and should always be utilized if possible.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJMatrixSetDiagOffdSizes(HYPRE_IJMatrix   matrix,
                                         const HYPRE_Int *diag_sizes,
                                         const HYPRE_Int *offdiag_sizes);

/**
 * (Optional) Sets the maximum number of elements that are expected to be set
 * (or added) on other processors from this processor
 * This routine can significantly improve the efficiency of matrix
 * construction, and should always be utilized if possible.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJMatrixSetMaxOffProcElmts(HYPRE_IJMatrix matrix,
                                           HYPRE_Int      max_off_proc_elmts);

/**
 * (Optional, GPU only) Sets the initial memory allocation for matrix
 * assemble, which factor * local number of rows
 * Not collective.
 **/
HYPRE_Int HYPRE_IJMatrixSetInitAllocation(HYPRE_IJMatrix matrix,
                                          HYPRE_Int      factor);

/**
 * (Optional, GPU only) Sets if matrix assemble routine does reductions
 * during the accumulation of the entries before calling HYPRE_IJMatrixAssemble.
 * This early assemble feature may save the peak memory usage but requires
 * extra work.
 * Not collective.
 **/
HYPRE_Int HYPRE_IJMatrixSetEarlyAssemble(HYPRE_IJMatrix matrix,
                                         HYPRE_Int      early_assemble);

/**
 * (Optional, GPU only) Sets the grow factor of memory in matrix assemble when
 * running out of memory.
 * Not collective.
 **/
HYPRE_Int HYPRE_IJMatrixSetGrowFactor(HYPRE_IJMatrix matrix,
                                      HYPRE_Real     factor);

/**
 * (Optional) Sets the print level, if the user wants to print
 * error messages. The default is 0, i.e. no error messages are printed.
 *
 **/
HYPRE_Int HYPRE_IJMatrixSetPrintLevel(HYPRE_IJMatrix matrix,
                                      HYPRE_Int      print_level);

/**
 * (Optional) if set, will use a threaded version of
 * HYPRE_IJMatrixSetValues and HYPRE_IJMatrixAddToValues.
 * This is only useful if a large number of rows is set or added to
 * at once.
 *
 * NOTE that the values in the rows array of HYPRE_IJMatrixSetValues
 * or HYPRE_IJMatrixAddToValues must be different from each other !!!
 *
 * This option is VERY inefficient if only a small number of rows
 * is set or added at once and/or
 * if reallocation of storage is required and/or
 * if values are added to off processor values.
 *
 **/
HYPRE_Int HYPRE_IJMatrixSetOMPFlag(HYPRE_IJMatrix matrix,
                                   HYPRE_Int      omp_flag);

/**
 * Read the matrix from file.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJMatrixRead(const char     *filename,
                             MPI_Comm        comm,
                             HYPRE_Int       type,
                             HYPRE_IJMatrix *matrix);

/**
 * Read the matrix from MM file.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJMatrixReadMM(const char     *filename,
                               MPI_Comm        comm,
                               HYPRE_Int       type,
                               HYPRE_IJMatrix *matrix);

/**
 * Print the matrix to file. This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJMatrixPrint(HYPRE_IJMatrix  matrix,
                              const char     *filename);

/**
 * Transpose an IJMatrix.
 **/
HYPRE_Int
HYPRE_IJMatrixTranspose( HYPRE_IJMatrix  matrix_A,
                         HYPRE_IJMatrix *matrix_AT );

/**
 * Computes the infinity norm of an IJMatrix
 **/
HYPRE_Int
HYPRE_IJMatrixNorm( HYPRE_IJMatrix  matrix,
                    HYPRE_Real     *norm );

/**
 * Performs C = alpha*A + beta*B
 **/
HYPRE_Int
HYPRE_IJMatrixAdd( HYPRE_Complex    alpha,
                   HYPRE_IJMatrix   matrix_A,
                   HYPRE_Complex    beta,
                   HYPRE_IJMatrix   matrix_B,
                   HYPRE_IJMatrix  *matrix_C );

/**
 * Print the matrix to file in binary format. This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJMatrixPrintBinary(HYPRE_IJMatrix  matrix,
                                    const char     *filename);

/**
 * Read the matrix from file stored in binary format.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJMatrixReadBinary(const char     *filename,
                                   MPI_Comm        comm,
                                   HYPRE_Int       type,
                                   HYPRE_IJMatrix *matrix_ptr);

/**
 * Migrate the matrix to a given memory location.
 **/
HYPRE_Int HYPRE_IJMatrixMigrate(HYPRE_IJMatrix       matrix,
                                HYPRE_MemoryLocation memory_location);

/**@}*/

/*--------------------------------------------------------------------------
 *--------------------------------------------------------------------------*/

/**
 * @name IJ Vectors
 *
 * @{
 **/

struct hypre_IJVector_struct;
/**
 * The vector object.
 **/
typedef struct hypre_IJVector_struct *HYPRE_IJVector;

/**
 * Create a vector object.  Each process owns some unique consecutive
 * range of vector unknowns, indicated by the global indices \e
 * jlower and \e jupper.  The data is required to be such that the
 * value of \e jlower on any process \f$p\f$ be exactly one more than
 * the value of \e jupper on process \f$p-1\f$.  Note that the first
 * index of the global vector may start with any integer value.  In
 * particular, one may use zero- or one-based indexing.
 *
 * Collective.
 **/
HYPRE_Int HYPRE_IJVectorCreate(MPI_Comm        comm,
                               HYPRE_BigInt    jlower,
                               HYPRE_BigInt    jupper,
                               HYPRE_IJVector *vector);

/**
 * Destroy a vector object.  An object should be explicitly destroyed
 * using this destructor when the user's code no longer needs direct
 * access to it.  Once destroyed, the object must not be referenced
 * again.  Note that the object may not be deallocated at the
 * completion of this call, since there may be internal package
 * references to the object.  The object will then be destroyed when
 * all internal reference counts go to zero.
 **/
HYPRE_Int HYPRE_IJVectorDestroy(HYPRE_IJVector vector);

/**
 * This function should be called before `HYPRE_IJVectorSetData`
 * if users intend to reuse an existing data pointer, thereby avoiding
 * unnecessary memory copies. It configures the vector to accept external
 * data without allocating new storage.
 **/
HYPRE_Int HYPRE_IJVectorInitializeShell(HYPRE_IJVector vector);

/**
 * This function sets the internal data pointer of the vector to an external
 * array, allowing direct control over the vector's data storage without
 * transferring ownership. Users are responsible for managing the memory
 * of the `data` array, which must remain valid for the vector's lifetime.
 *
 * Users should call `HYPRE_IJVectorInitializeShell` before this function
 * to prepare the vector for external data. The memory location of the `data`
 * array is expected to be on the host when hypre is configured without GPU
 * support. If hypre is configured with GPU support, it is assumed that `data`
 * resides in device memory.
 **/
HYPRE_Int HYPRE_IJVectorSetData(HYPRE_IJVector  vector,
                                HYPRE_Complex  *data);

/**
 * (Optional) Set an array of tags for the vector.
 *
 * @param vector The vector object.
 * @param owns_tags Whether the vector owns the tags.
 *         If true, vector will allocate and copy tags.
 *         If false, vector will just point to the input tags array.
 * @param num_tags The number of tags.
 * @param tags The tags array. Must reside in the same memory location as the
 *         vector data (e.g., if vector is on GPU, tags must also be on GPU).
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJVectorSetTags(HYPRE_IJVector  vector,
                                HYPRE_Int       owns_tags,
                                HYPRE_Int       num_tags,
                                HYPRE_Int      *tags);

/**
 * Prepare a vector object for setting coefficient values. This
 * routine will also re-initialize an already assembled vector,
 * allowing users to modify coefficient values.
 **/
HYPRE_Int HYPRE_IJVectorInitialize(HYPRE_IJVector vector);

/**
 * Prepare a vector object for setting coefficient values.  This
 * routine will also re-initialize an already assembled vector,
 * allowing users to modify coefficient values. This routine
 * also specifies the memory location, i.e. host or device.
 **/
HYPRE_Int HYPRE_IJVectorInitialize_v2( HYPRE_IJVector vector,
                                       HYPRE_MemoryLocation memory_location );

/**
 * (Optional) Sets the maximum number of elements that are expected to be set
 * (or added) on other processors from this processor
 * This routine can significantly improve the efficiency of matrix
 * construction, and should always be utilized if possible.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJVectorSetMaxOffProcElmts(HYPRE_IJVector vector,
                                           HYPRE_Int      max_off_proc_elmts);

/**
 * (Optional) Sets the number of components (vectors) of a multivector. A vector
 * is assumed to have a single component when this function is not called.
 * This function must be called prior to HYPRE_IJVectorInitialize.
 **/
HYPRE_Int HYPRE_IJVectorSetNumComponents(HYPRE_IJVector  vector,
                                         HYPRE_Int       num_components);

/**
 * (Optional) Sets the component identifier of a vector with multiple components (multivector).
 * This can be used for Set/AddTo/Get purposes.
 **/
HYPRE_Int HYPRE_IJVectorSetComponent(HYPRE_IJVector  vector,
                                     HYPRE_Int       component);

/**
 * Sets values in vector.  The arrays \e values and \e indices
 * are of dimension \e nvalues and contain the vector values to be
 * set and the corresponding global vector indices, respectively.
 * Erases any previous values at the specified locations and replaces
 * them with new ones.  Note that it is not possible to set values
 * on other processors. If one tries to set a value from proc i on proc j,
 * proc i will erase all previous occurrences of this value in its stack
 * (including values generated with AddToValues), and treat it like
 * a zero value. The actual value needs to be set on proc j.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJVectorSetValues(HYPRE_IJVector       vector,
                                  HYPRE_Int            nvalues,
                                  const HYPRE_BigInt  *indices,
                                  const HYPRE_Complex *values);

/**
 * Sets all vector coefficients to \e value
 **/
HYPRE_Int HYPRE_IJVectorSetConstantValues(HYPRE_IJVector vector,
                                          HYPRE_Complex  value);

/**
 * Adds to values in vector.  Usage details are analogous to
 * \ref HYPRE_IJVectorSetValues.
 * Adds to any previous values at the specified locations, or, if
 * there was no value there before, inserts a new one.
 * AddToValues can be used to add to values on other processors.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJVectorAddToValues(HYPRE_IJVector       vector,
                                    HYPRE_Int            nvalues,
                                    const HYPRE_BigInt  *indices,
                                    const HYPRE_Complex *values);

/**
 * Finalize the construction of the vector before using.
 **/
HYPRE_Int HYPRE_IJVectorAssemble(HYPRE_IJVector vector);

/**
 * Update vectors by setting (action 1) or
 * adding to (action 0) values in 'vector'.
 * Note that this function cannot update values owned by other processes
 * and does not allow repeated index values in 'indices'.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJVectorUpdateValues(HYPRE_IJVector       vector,
                                     HYPRE_Int            nvalues,
                                     const HYPRE_BigInt  *indices,
                                     const HYPRE_Complex *values,
                                     HYPRE_Int            action);

/**
 * Gets values in vector.  Usage details are analogous to
 * \ref HYPRE_IJVectorSetValues.
 *
 * Not collective.
 **/
HYPRE_Int HYPRE_IJVectorGetValues(HYPRE_IJVector   vector,
                                  HYPRE_Int        nvalues,
                                  const HYPRE_BigInt *indices,
                                  HYPRE_Complex   *values);

/**
 * Set the storage type of the vector object to be constructed.
 * Currently, \e type can only be \c HYPRE_PARCSR.
 *
 * Not collective, but must be the same on all processes.
 *
 * @see HYPRE_IJVectorGetObject
 **/
HYPRE_Int HYPRE_IJVectorSetObjectType(HYPRE_IJVector vector,
                                      HYPRE_Int      type);

/**
 * Get the storage type of the constructed vector object.
 **/
HYPRE_Int HYPRE_IJVectorGetObjectType(HYPRE_IJVector  vector,
                                      HYPRE_Int      *type);

/**
 * Returns range of the part of the vector owned by this processor.
 **/
HYPRE_Int HYPRE_IJVectorGetLocalRange(HYPRE_IJVector  vector,
                                      HYPRE_BigInt   *jlower,
                                      HYPRE_BigInt   *jupper);

/**
 * Get a reference to the constructed vector object.
 *
 * @see HYPRE_IJVectorSetObjectType
 **/
HYPRE_Int HYPRE_IJVectorGetObject(HYPRE_IJVector   vector,
                                  void           **object);

/**
 * (Optional) Sets the print level, if the user wants to print
 * error messages. The default is 0, i.e. no error messages are printed.
 *
 **/
HYPRE_Int HYPRE_IJVectorSetPrintLevel(HYPRE_IJVector vector,
                                      HYPRE_Int      print_level);

/**
 * Read the vector from file.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJVectorRead(const char     *filename,
                             MPI_Comm        comm,
                             HYPRE_Int       type,
                             HYPRE_IJVector *vector);

/**
 * Read the vector from binary file.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJVectorReadBinary(const char     *filename,
                                   MPI_Comm        comm,
                                   HYPRE_Int       type,
                                   HYPRE_IJVector *vector);

/**
 * Print the vector to file.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJVectorPrint(HYPRE_IJVector  vector,
                              const char     *filename);

/**
 * Print the vector to binary file.  This is mainly for debugging purposes.
 **/
HYPRE_Int HYPRE_IJVectorPrintBinary(HYPRE_IJVector  vector,
                                    const char     *filename);

/**
 * Computes the inner product between two vectors
 **/
HYPRE_Int HYPRE_IJVectorInnerProd(HYPRE_IJVector  x,
                                  HYPRE_IJVector  y,
                                  HYPRE_Real     *prod);

/**
 * Migrate the vector to a given memory location.
 **/
HYPRE_Int HYPRE_IJVectorMigrate(HYPRE_IJVector       vector,
                                HYPRE_MemoryLocation memory_location);

/**@}*/
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
