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
	
	if(my_rank != MASTER){
		sprintf(message, "Hello Master from %d", my_rank);
		dest = MASTER;
		MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}else{
		for(source=0; source<nb_proc; source++){
			if(source != my_rank){
				MPI_Recv(message, SIZE, MPI_CHAR, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
				printf("%s\n", message); 
			}
		}
	}
	
	MPI_Finalize();
	return 0;
}

/* 

2) On fait l'affichage que pour les processus "fils" c'est à dire que le rang 0 est celui du "père"

> mpirun -np 10 bin/hello_master
Hello Master from 1
Hello Master from 2
Hello Master from 3
Hello Master from 4
Hello Master from 5
Hello Master from 6
Hello Master from 7
Hello Master from 8
Hello Master from 9

3) Avec la variage MPI_ANY_SOURCE, l'affichage sera fait de façon désordonné puisque la lecture se fait dans le buffer sans connaitre la source: non déterministe.

> mpirun -np 10 bin/hello_master
Hello Master from 3
Hello Master from 4
Hello Master from 6
Hello Master from 7
Hello Master from 8
Hello Master from 9
Hello Master from 1
Hello Master from 2
Hello Master from 5

 */
