#include <stdio.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define DEBUG 0

#define N 1000

int main(int argc, char *argv[] ) {

  int i, j, numprocs, my_id, rows;
  //float matrix[N][N];
  float * matrix;
  float vector[N];
  //float result[N];
  float * result;
  struct timeval  tv1, tv2, tv3, tv4;
  float * Mlocal;
  float * Rlocal;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  /* Initialize Matrix and Vector */
 // poner luego como if

  	rows = (N/numprocs);
	if(N%numprocs) rows ++;

  	if(my_id == 0) {
	
		matrix = (float*) malloc(sizeof(float)*N*numprocs*rows); //matriz de N*N y vector inicializada en el root

  		for(i=0;i<N;i++) {
    			vector[i] = i;
    			for(j=0;j<N;j++) {
      				matrix[i*N + j] = i+j;
     			}
   		} //inicialización de la matriz N*N

  	result = (float *)malloc(sizeof(float) * numprocs * rows);	   														
  	} else{

		matrix = (float *)malloc(sizeof(float) * rows * N); //memoria para la submatriz de rows*n
		result = (float *)malloc(sizeof(float) * rows );  //memoria del vector que va a guardar el resultado local de rows
  	}
	

	gettimeofday(&tv3, NULL);
	
	MPI_Bcast(vector, N, MPI_FLOAT, 0, MPI_COMM_WORLD);	//paso el vector a todos los procesos

	MPI_Scatter(matrix, rows*N, MPI_FLOAT, matrix, rows*N, MPI_FLOAT, 0, MPI_COMM_WORLD); //reparto la matriz en numprocs, rows*N  					
	gettimeofday(&tv4, NULL);

	int microsecondscomm = (tv4.tv_usec - tv3.tv_usec)+ 1000000 * (tv4.tv_sec -tv3.tv_sec);						      					
   	gettimeofday(&tv1, NULL);

 	for(i=0;i<rows;i++) {	
	    	result[i]=0;
	    	for(j=0;j<N;j++) {
	      		result[i] += matrix[i*N+j]*vector[j];
	    	}
	}

  	gettimeofday(&tv2, NULL);
    
  	int microseconds = (tv2.tv_usec - tv1.tv_usec)+ 1000000 * (tv2.tv_sec - tv1.tv_sec);

	printf ("Process %d || Computation Time (seconds) = %lf\n", my_id, (double) microseconds/1E6);

	gettimeofday(&tv3, NULL);
	
	MPI_Gather(result, rows, MPI_FLOAT, result, rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

	gettimeofday(&tv4, NULL);

	microsecondscomm += (tv4.tv_usec - tv3.tv_usec)+ 1000000 * (tv4.tv_sec -tv3.tv_sec);

	printf ("Process %d || Communication Time (seconds) = %lf\n", my_id, (double) microsecondscomm/1E6);

	if(my_id == 0){
	  /*Display result */
	  	if (DEBUG){
	    		for(i=0;i<N;i++) {
	      			printf(" %f \t ",result[i]);
	    		}
	  	/*} else {
	    		printf ("Time (seconds) = %lf\n", (double) microseconds/1E6);
	  	*/}  
	}

	MPI_Finalize();  
	free(result);
	free(matrix);

  	return 0;
}


