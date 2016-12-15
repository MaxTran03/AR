#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mpi.h>

#define MASTER 0
#define SIZE 128

int main(int argc, char **argv){
	int my_rank;
	int nb_proc;
	int source;
	int dest;
	int tag = 0;
	char message[SIZE];
	
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	
	dest = (my_rank + 1) % nb_proc;
	sprintf(message, "Hello Master from %d", my_rank);
	
	if(my_rank != MASTER){
		/*MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);*/
		MPI_Ssend(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
		MPI_Recv(message, SIZE, MPI_CHAR, (my_rank - 1) % nb_proc, tag, MPI_COMM_WORLD, &status);
	}else{
		MPI_Recv(message, SIZE, MPI_CHAR, (my_rank - 1) % nb_proc, tag, MPI_COMM_WORLD, &status);
		MPI_Ssend(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}
	
	printf("My rank %d : %s\n", my_rank, message);
	
	MPI_Finalize();
	return 0;
}
