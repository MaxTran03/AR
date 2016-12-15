#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "../include/mpi_server.h"

static server the_server;

int my_rank;
int nb_proc;
int source;
int provided;
int tag = 0;

void start_server(void (*callback)(int tag, int source)){
	MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED, &provided);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
}

void destroy_server(){
	
}

pthread_mutex_t* getMutex(){
	return the_server.mutex;
}


/* 

void message_pending(int tag, int source){
	char message[SIZE];
	MPI_Status status;
	
	switch(tag){
		case TAG_BONJOUR;
			MPI_Recv(message, SIZE, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
			printf("");
			break;
		default;
			printf("Message inconnu");
			exit(1);
	}
}

void *procedure_listener(void * arg){
	serveur arg_serveur = (server *)arg;
	int flag;,
	while(1){
		pthread_mutex_lock(&(arg_server->mutex));
		MPI_IProbe(..., &flag, ..., &status);
		pthreadèmutex_unlock(&(arg_server->mutex));
		if(flag)
			arg_server->callback(status.MPI_TAG, status.MPI_SOURCE),
	}
}

 */
