#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define NB 5
#define NB_MEALS 5
#define REQ_BAGUETTE 0
#define ACK_BAGUETTE 1
#define FIN 2

#define HUNGRY
#define THINKING
#define DONE


int main(int argc, char **argv){
	int my_rank, nb_proc;
	int dest;
	int tag = 0, i = 0;
	int local_clock;
	bool fin = false;
	int etat, etat_gauche, etat_droit;
	int left, right;
	
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	
	left = (my_rank+NB-1)%NB;
	right = (my_rank+1)%NB;
	
	while(!fin){
		if((i < NB_MEALS) && (etat == THINKING){
			etat = HUNGRY;
			MPI_Send(&local_clock, 1, MPI_INT, right, REQ_BAGUETTE, MPI_COMM_WORLD);
			MPI_Send(&local_clock, 1, MPI_INT, left, REQ_BAGUETTE, MPI_COMM_WORLD);
		}
		
		MPI_Recv(&local_clock, 1, MPI_INT, (my_rank+1)%NB, REQ_BAGUETTE, MPI_COMM_WORLD, &status);
		//MPI_Recv(&local_clock, 1, MPI_INT, (my_rank+NB-1)%NB, REQ_BAGUETTE, MPI_COMM_WORLD, &status);
		
		switch(typemsg):
			case(ACK_BAGUETTE):
				if(left)
					bag_left = true;
				else
					bag_right = true;
				if(bag_left && bag_right){
					printf("mange");
					etat = THINKING;
					if(etat_gauche == HUNGRY){
						etat_gauche = THINKING;
						bag_left = false;
						MPI_Send(ACK_BAGUETTE);
					}
					if(etat_droit == HUNGRY){
						bag_right = false;
						MPI_Send(ACK_BAGUETTE);
						etat_droit = THINKING;
					}
				}
				break;
			case(REQ_BAGUETTE):
				if(je suis prio){
					if(demande voisin droite)
						etat_droit = HUNGRY;
					else
						etat_gauche = HUNGRY;
					}
				}else{
					voisin prio
					if(demande left){
						bag_left = false;
						MPI_Send(ACK_BAGUETTE);
						if(etat == HUNGRY)
							MPI_Send(REQ_BAGUETTE);
						else
					}
				}		
	}
	
	MPI_Finalize();
	return 0;
}
