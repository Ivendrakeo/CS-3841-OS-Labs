#ifndef MATRIX_H
#define MATRIX_H

/*
 * A matrix struct contains data to construct a matrix
 *
 * data : array of values for each cell in the matrix. cell data is indexed left to right.
 * length : number of rows the matrix has (non-zero indexed)
 * width : number of collumns the matrix has (non-zero indexed)
 */
typedef struct matrix {
	int* data;
	int length;
	int width;
} matrix;

/*
 * Result type supplied from a child process. A child supplies the data value for
 * the cell along with it's indexed position.
 *
 * cell : indexed cell position. indexed left to right
 * data : calculated value of the cell
 */
typedef struct childResult {
	int cell;
	int data;
} childResult;

/*
 * Creates a matrix type from a local file.
 * A matrix file contains first the number of rows followed by number of collumns.
 * All following cell entries are in logical order indexed left to right 
 *
 * filename : string of the file path
 *
 * returns : pointer to a new matrix struct with data populated by the file contents. 
 * 	     Returns NULL if the file strcture is inconsistant
 */
matrix* createMatrix(const char* filename);

/*
 * Deletes and free's the memory associated with a matrix
 *
 * matrix : pointer to the matrix to be deleted
 */
void deleteMatrix(matrix* matrix);

/*
 * Takes two matrixes and multiplies them together using multiprocessing
 *
 * first : logical first matrix to be multiplied
 * second : logical second matrix to be multiplied
 *
 * return : pointer to a new result matrix assuming p==q otherwise NULL 
 *          if matrixes cannot be multiplied
 */
matrix* multiplyMatrixNoMultiprocessing(matrix* first, matrix* second);

/*
 * Takes two matrixes and multiplies them together without using multiprocessing
 *
 * first : logical first matrix to be multiplied
 * second : logical second matrix to be multiplied
 *
 * return : pointer to a new result matrix assuming p==q otherwise NULL 
 *          if matrixes cannot be multiplied
 */
matrix* multiplyMatrix(matrix* first, matrix* second);

/*
 * Prints the dimensions of the supplied matrix followed by the contents of the matrix.
 *
 * matrix : pointer to the matrix which to print the contents of.
 */
void printMatrix(matrix* matrix);

/*
 * Creates a matrix type from a local file.
 * A matrix file contains first the number of rows followed by number of collumns.
 * All following cell entries are in logical order indexed left to right.
 * An Anonymous Matrix exists within shared memory using mmap but without an associated name
 *
 * filename : string of the file path
 *
 * returns : pointer to a new matrix struct with data populated by the file contents. 
 * 	     Returns NULL if the file strcture is inconsistant
 */
matrix* createAnonymousMatrix(const char* filename);

/*
 * Deletes and free's the memory associated with an anonymous matrix
 *
 * matrix : pointer to the anonymous matrix to be deleted
 */
void deleteAnonymousMatrix(matrix* matrix);

/*
 * Takes two anonymous matrixes and multiplies them together using a multiprocessing
 * strategy. The result matrix will also be anonymous and persist in shared memory
 *
 * first : logical first anonymous matrix to be multiplied
 * second : logical second anonymous matrix to be multiplied
 *
 * return : pointer to a new result anonymous matrix assuming p==q otherwise NULL 
 *          if matrixes cannot be multiplied
 */
matrix* multiplyMatrixAnonymousMultiprocessing(matrix* first, matrix* second);

/*
 * Creates a named matrix type from a local file.
 * A matrix file contains first the number of rows followed by number of collumns.
 * All following cell entries are in logical order indexed left to right.
 * A named Matrix exists within shared memory using mmap and shm_link to associate a name
 *
 * filename : string of the file path
 * marixName : string name associated with the 'named' memory segment.
 *
 * returns : pointer to a new matrix struct with data populated by the file contents. 
 * 	     Returns NULL if the file strcture is inconsistant
 */
matrix* createNamedMatrix(const char* filename, const char* matrixName);

/*
 * Deletes and free's the memory associated with a named matrix
 *
 * matrix : pointer to the anonymous matrix to be deleted
 * marixName : string name associated with the 'named' memory segment.
 */
void deleteNamedMatrix(matrix* matrix, const char* matrixName);

/*
 * Takes two named matrixes and multiplies them together using a multiprocessing
 * strategy. The result matrix will also be a named matrix and persist in shared memory
 *
 * first : logical first named matrix to be multiplied
 * second : logical second named matrix to be multiplied
 * resultName : string name associated with the result matrixes 'named' memory segment
 *
 * return : pointer to a new result named matrix assuming p==q otherwise NULL 
 *          if matrixes cannot be multiplied
 */
matrix* multiplyMatrixNamedMultiprocessing(matrix* first, matrix* second, const char* resultName);

#endif
