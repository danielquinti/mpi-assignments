#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>
#include <time.h>

int MPI_BinomialBcast(void *buf, int count, MPI_Datatype datatype,int root, MPI_Comm comm){
	
	int numprocs,my_id,i;

	MPI_Comm_size(comm, &numprocs); //2
    	MPI_Comm_rank(comm, &my_id); //0..1

	
	if(my_id == 0){

		for (i = 0; 2^i < numprocs; i++) {

				MPI_Send((int*)buf,1,MPI_INT,2^i,0,comm);

		}

	}else{

		MPI_Recv((int*)buf,1,MPI_INT,MPI_ANY_SOURCE,0,comm,MPI_STATUS_IGNORE);

		for (i = 1; my_id + 2^i < numprocs; i++) {
		
				MPI_Send((int*)buf,1,MPI_INT,my_id + 2^i,0,comm);

		}

	}

	return 1;

}




/*****************************************************************/

int main(int argc, char *argv[])
{
    int i, done = 0, n, count, numprocs, my_id, countprocs;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //2
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id); //0..1
    printf("%i",my_id);

    while (!done)
    {
	count = 0;
	countprocs = 0;
	//srand(time(NULL));

	if (my_id == 0){

        	printf("Enter the number of points: (0 quits) \n");
        	scanf("%d",&n);
    
        	if (n == 0) {
			
		//MPI_Finalize();
		break;
		
		}

		MPI_BinomialBcast(&n,1,MPI_INT,0,MPI_COMM_WORLD); //sending n to every process

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

		MPI_Reduce(&countprocs,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
		//storing in count all the counts of the precesses
	

		pi = ((double) count/(double) n)*4.0;

        	printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));

	}else{

		MPI_BinomialBcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
		//waiting for the bcast to be called

		for (i = 1; i <= n; i += numprocs) {
		    // Get the random numbers between 0 and 1
		    x = ((double) rand()) / ((double) RAND_MAX);
		    y = ((double) rand()) / ((double) RAND_MAX);

		    // Calculate the square root of the squares
		    z = sqrt((x*x)+(y*y));

		    // Check whether z is within the circle
		    if(z <= 1.0)
		        countprocs++;
		}  //endFor  

		MPI_Reduce(&countprocs,&count,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
		//waiting for the reduce call to be made.
	
	
        }//endElse
     
    }//endWhile

}//endMain
