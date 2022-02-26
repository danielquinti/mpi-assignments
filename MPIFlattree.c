#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>



int MPI_Flattree(void *buff, void *recvbuff, int count,MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){

	int buff1 = 0;
	int numprocs,my_id,i;
	
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //2
    	MPI_Comm_rank(MPI_COMM_WORLD, &my_id); //0..1

	if(my_id == root){

		for (i = 0; i < numprocs; i++){
			if (i!= root){
				MPI_Recv(&buff1,1,datatype,i,root,comm,MPI_STATUS_IGNORE);
				*(int*)recvbuff += buff1;
			}
		}
		
		*(int*)recvbuff += *(int*)buff;

	}else{
		
		 MPI_Send((int*)buff,1,datatype,root,0,comm);  
		
	}
	
	return 1;
}


/*---------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int i, done = 0, n, count, numprocs, my_id, countprocs;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //2
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id); //0..1

    while (!done)
    {
	count = 0;
	countprocs = 0;
	srand(time(NULL));

	if (my_id == 0){

        	printf("Enter the number of points: (0 quits) \n");
        	scanf("%d",&n);
    
        	if (n == 0) break;

		for (i = 1; i <= n; i += numprocs) {
            // Get the random numbers between 0 and 1
	    	x = ((double) rand()) / ((double) RAND_MAX);
	    	y = ((double) rand()) / ((double) RAND_MAX);

	    // Calculate the square root of the squares
	    	z = sqrt((x*x)+(y*y));

	    // Check whether z is within the circle
	    	if(z <= 1.0)
                	countprocs++;
        }
		
		for (i = 1; i <= numprocs; i++)
		MPI_Send(&n,1,MPI_INT,i-1,0,MPI_COMM_WORLD);

		MPI_Flattree(&countprocs,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

	

		pi = ((double) count/(double) n)*4.0;

        	printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));

	}else{

	MPI_Recv(&n,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

        for (i = 1; i <= n; i += numprocs) {
            // Get the random numbers between 0 and 1
	    x = ((double) rand()) / ((double) RAND_MAX);
	    y = ((double) rand()) / ((double) RAND_MAX);

	    // Calculate the square root of the squares
	    z = sqrt((x*x)+(y*y));

	    // Check whether z is within the circle
	    if(z <= 1.0)
                countprocs++;
        }

	    MPI_Flattree(&countprocs,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);   
	
        }

	
        
    }
MPI_Finalize();
	
	
}
