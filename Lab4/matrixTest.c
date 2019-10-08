#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>

#include <time.h>

int main(int argc, char* argv[]){
	


	//working named code...
	matrix* namedMatrixOne = createNamedMatrix(argv[1],"mat1");
	matrix* namedMatrixTwo = createNamedMatrix(argv[2],"mat2");
	matrix* resultMatrix = multiplyMatrixNamedMultiprocessing(namedMatrixOne,namedMatrixTwo,"resmat");
	printMatrix(resultMatrix);
	deleteNamedMatrix(namedMatrixOne,"mat1");
	deleteNamedMatrix(namedMatrixTwo,"mat2");
	deleteNamedMatrix(resultMatrix,"resmat");

/*
	//not working anonymous code...
	matrix* anonymousMatrixOne = createAnonymousMatrix(argv[1]);
	matrix* anonymousMatrixTwo = createAnonymousMatrix(argv[2]);
	matrix* resultMatrixAnon = multiplyMatrixNoMultiprocessing(anonymousMatrixOne, anonymousMatrixTwo);
	printMatrix(resultMatrixAnon);
	deleteAnonymousMatrix(anonymousMatrixOne);
	deleteAnonymousMatrix(anonymousMatrixTwo);
	//deleteAnonymousMatrix(resultMatrixAnon);
*/
/*
	matrix* matrixOne = createMatrix(argv[1]);
	matrix* matrixTwo = createMatrix(argv[2]);
	matrix* resultMatrixMult = multiplyMatrix(matrixOne, matrixTwo);
	deleteMatrix(resultMatrixMult);
	deleteMatrix(matrixTwo);
	deleteMatrix(matrixOne);
*/
/*
	clock_t start, end;

	//create all of the input matrixes for each calculation type
	matrix* matrixOne = createMatrix(argv[1]);
	matrix* matrixTwo = createMatrix(argv[2]);
	matrix* anonymousMatrixOne = createAnonymousMatrix(argv[1]);
	matrix* anonymousMatrixTwo = createAnonymousMatrix(argv[2]);
	matrix* namedMatrixOne = createNamedMatrix(argv[1],"mat1");
	matrix* namedMatrixTwo = createNamedMatrix(argv[2],"mat2");
	//print the input matrixes
	printMatrix(matrixOne);
	printMatrix(matrixTwo);
	//calculate times for each multiply method
	start = clock();
	matrix* resultMatrix = multiplyMatrixNoMultiprocessing(matrixOne, matrixTwo);
	end = clock();
	double noMultiprocessing = ((double) (end- start))/ CLOCKS_PER_SEC;

	start = clock();
	matrix* resultMatrixMult = multiplyMatrix(matrixOne, matrixTwo);
	end = clock();
	double pipeMultiprocessing = ((double) (end- start))/ CLOCKS_PER_SEC;

	start = clock();
	matrix* resultMatrixAnon = multiplyMatrixNoMultiprocessing(anonymousMatrixOne, anonymousMatrixTwo);
	end = clock();
	double anonMultiprocessing = ((double) (end- start))/ CLOCKS_PER_SEC;

	start = clock();
	matrix* resultMatrixNamed = multiplyMatrixNamedMultiprocessing(namedMatrixOne,namedMatrixTwo,"resmat");
	end = clock();
	double namedMultiprocessing = ((double) (end- start))/ CLOCKS_PER_SEC;

	//print result (all are the same)
	printMatrix(resultMatrixNamed);
	//free all the matrixes
	deleteMatrix(resultMatrix);
	deleteMatrix(resultMatrixMult);
	deleteMatrix(matrixTwo);
	deleteMatrix(matrixOne);
	deleteAnonymousMatrix(anonymousMatrixOne);
	deleteAnonymousMatrix(anonymousMatrixTwo);
	deleteAnonymousMatrix(resultMatrixAnon);
	deleteNamedMatrix(namedMatrixOne,"mat1");
	deleteNamedMatrix(namedMatrixTwo,"mat2");
	deleteNamedMatrix(resultMatrixNamed,"resmat");
	//print time results
	printf("non multiprocessing: %f sec\n",noMultiprocessing);
	printf("pipe multiprocessing: %f sec\n",pipeMultiprocessing);
	printf("mmap multiprocessing: %f sec\n",anonMultiprocessing);
	printf("named multiprocessing: %f sec\n",namedMultiprocessing);
*/

}
