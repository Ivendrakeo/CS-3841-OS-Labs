#include <stdio.h>
#include "matrix.h"
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * This method is what runs the child process called from fork.
 * The child process will calculate the results of a multiplication matrix for a designated cell.
 *
 * fdes : file descriptor pipe that links to the parent process
 * first : the logical first matrix of the opperand.
 * second : the logical second matrix of the opperand.
 * cell : which reseult cell the process is supposed to calculate. The cell is indexed left to right.
 *
 * return : void, but the pipe is filled with a resultMatrix containing the calculated data value 
 * 	    along with its associated cell.
 */
static void childprocess(int* fdes, matrix* first, matrix* second, int cell){
	close(fdes[0]); //close read end
	childResult* result = malloc (sizeof(childResult)); //malloc space for the return result
	int width = second->width;
	int cell_x_pos = cell % width;
	int cell_y_pos = ((cell - cell_x_pos) / width);
	int sum = 0;
	for(int i = 0; i < first->width; i++){
		//calculate the cell multiplication
		sum = sum + (*(first->data + first->width*(cell_y_pos) + i)) * (*(second->data + second->width*i + (cell_x_pos)));
	}
	result->cell = cell;
	result->data = sum;
	//fill the pipe
	write(fdes[1],result,sizeof(childResult));
	close(fdes[1]);
	free(result);
}

matrix* createMatrix(const char* filename){
	//malloc a new matrix
	matrix* newMatrix = malloc (sizeof(matrix));
	int rows, columns;
	FILE* input = fopen(filename, "r");
	//read the file and malloc enough space for the matrix data
	if(input != NULL){
		fscanf(input, "%d", &rows);
		fscanf(input, "%d", &columns);
		newMatrix->data = malloc (sizeof(int)*rows*columns);
		newMatrix->length = rows;
		newMatrix->width = columns;
		//populate the matrix with data from the supplied file
		for(int i = 0; i < rows; i++){
			for(int j = 0; j < columns; j++){
				int value;
				fscanf(input, "%d", &value);
				*(newMatrix->data + ((i*columns)+j)) = value;
			}
		}
		//close the file and return the resulting matrix
		fclose(input);
		return newMatrix;
	}
	return NULL;
}

void deleteMatrix(matrix* matrix){
	//free the heap space
	free(matrix->data);
	free(matrix);
}

matrix* multiplyMatrixNoMultiprocessing(matrix* first, matrix* second){
	//ensure that for m x n and q x p, that n==q
	if(first->width == second->length){
		//malloc and setup resulting matrix
		matrix* resultMatrix = malloc (sizeof(matrix));
		resultMatrix->length = first->length;
		resultMatrix->width = second->width;
		int size = resultMatrix->width * resultMatrix->length;
		resultMatrix->data = malloc (sizeof(int)*size);
		//malloc pids and start iterating through each cell
		for(int i = 0; i < size; i++){
			int width = second->width;
			int cell_x_pos = i % width;
			int cell_y_pos = ((i - cell_x_pos) / width);
			int sum = 0;
			for(int i = 0; i < first->width; i++){
				//calculate the cell multiplication
				sum = sum + (*(first->data + first->width*(cell_y_pos) + i)) * (*(second->data + second->width*i + (cell_x_pos)));
			}
			resultMatrix->data[i] = sum;

		}
		return resultMatrix;
	} else {
		return NULL;
	}
}

matrix* multiplyMatrix(matrix* first, matrix* second){
	//ensure that for m x n and q x p, that n==q
	if(first->width == second->length){
		//setup pipe
		int fdes[2];
		if(pipe(fdes) == -1){
			exit(EXIT_FAILURE);
		}
		//malloc and setup resulting matrix
		matrix* resultMatrix = malloc (sizeof(matrix));
		resultMatrix->length = first->length;
		resultMatrix->width = second->width;
		int size = resultMatrix->width * resultMatrix->length;
		resultMatrix->data = malloc (sizeof(int)*size);
		//malloc pids and start iterating through each cell
		pid_t* pids = malloc (sizeof(pid_t)*size);
		for(int i = 0; i < size; i++){
			//fork and run child processes
			*(pids+i) = fork();
			if( (*(pids+i)) < 0 ){
				printf("Fork failed, terminating\n");
				exit(EXIT_FAILURE);
			} else if( (*(pids+i)) == 0){
				childprocess(fdes, first, second, i);
				free(pids);
				free(resultMatrix->data);
				free(resultMatrix);
				free(first->data);
				free(first);
				free(second->data);
				free(second);
				exit(0);
			}
		}
		//back to parent process
		childResult result;
		close(fdes[1]);
		int charread;
		//read and store cell result data
		while((charread = read(fdes[0],&result,sizeof(childResult)))){
			*(resultMatrix->data + result.cell) = result.data;
		}
		for(int i = 0; i < size; i++){
			wait(0);
		}
		free(pids);
		return resultMatrix;
	} else {
		return NULL;
	}
}

void printMatrix(matrix* matrix){
	printf("len:%d wid:%d\n", matrix->length, matrix->width); //print dimensions first
	//loop and print data for each cell
	for(int i = 0; i < matrix->length; i++){
		for(int j = 0; j < matrix->width; j++){
			printf("%d ", *(matrix->data + ((i*matrix->width)+j)));
		}
		printf("\n");
	}
}
