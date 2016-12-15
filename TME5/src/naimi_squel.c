#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define TAGINIT 0
#define N      3    /* Number of processes */ 
#define L     10    /* Buffer size */
#define NIL (N+1)   /* for an undefined value */
#define MAX_CS 5


struct msg{
	int type;									 /* 0 = req, 1 = ack */
	int clock;
	int rang;									
}msg,m;

int SharedVar = 0;                      /* la variable partagee */ 
int etat;										 /* 0 = not_requesting, 1 = requesting et 2 = SC */
int file[N-1];
int cpt;											 /* compteur de la file */
int horloge;
int date_req;									 /* date de la derniere requete */
int nb_ack;
int provided;

	
void Initialisation ( ) {
   /* initialise les variables locales, sauf reqId, val et typ_mes */
   etat = 1;
   cpt = 0;
   horloge = 0;
   date_req = 0;
	nb_ack = 0;
}



void Release_CS (int r) {
  /* operations effectuees en sortie de SC */
	int i;
	horloge ++;
	for(i=0;i<cpt;i++)
		if(i != r)
			MPI_Send(&file[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
			
	etat = 0;
	cpt=0;
}

void Receive_Request_CS (int r) {
  /* traitement de la reception d'une requete */    	
	MPI_Status status;
	MPI_Recv(&m, 1, MPI_INT, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
	if(m.clock > horloge)
		horloge = m.clock +1;
	else
		horloge ++;
	
	if(msg.type == 0){
		
		if((etat==0 || etat==1) && (date_req<m.clock || (date_req==m.clock && r<m.rang))){
			msg.type=0;
			msg.clock = horloge;
			msg.rang = r;
			MPI_Send(&msg, 1, MPI_INT, m.rang, TAGINIT, MPI_COMM_WORLD);
		}else{
			file[cpt] = m.rang;
			cpt++;
		}
	
	}else{
		nb_ack ++;
		if(nb_ack == N-1)
			etat = 2;
	}
}

void Request_CS (int r) {
	/* operations effectuees lors d'une demande de SC */
	int i;

	horloge++;
	etat = 1;
	date_req = horloge;
	nb_ack = 0;
  
	for(i=0; i<N; i++){
		msg.type=0;
		msg.clock = horloge;
		msg.rang = r;
		MPI_Send(&msg, 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
	}
	
	if(provided>0)
		while(1)
			Receive_Request_CS (r);
}

/******************************************************************************/

int main (int argc, char* argv[]) {
   int nb_proc,rang,i;
   MPI_Init(&argc, &argv);
   MPI_Init_thread(&argc, &argv,MPI_THREAD_FUNNELED,&provided);
   MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	
   if (nb_proc != N) {
      printf("Nombre de processus incorrect !\n");
      MPI_Finalize();
      exit(2);
   }
  
   MPI_Comm_rank(MPI_COMM_WORLD, &rang);
	Initialisation ( );
   for(i=0;i<MAX_CS;i++){
   	Request_CS(rang);
   	Release_CS(rang);
   }
  
   MPI_Finalize();
   return 0;
}
