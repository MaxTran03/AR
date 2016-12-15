#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT    0
#define NB_SITE 6

void simulateur(void) {
   int i;

   /* nb_voisins[i] est le nombre de voisins du site i */
   int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
   int min_local[NB_SITE+1] = {-1, 3, 11, 8, 14, 5, 17};

   /* liste des voisins */
   int voisins[NB_SITE+1][3] = {{-1, -1, -1},
         {2, 3, -1}, {1, 4, 5}, 
         {1, 6, -1}, {2, -1, -1},
         {2, -1, -1}, {3, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i],nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
   }
}

/******************************************************************************/

void calcul_min(int rang){
	MPI_Status status;
	int i, tmp, cpt = 0, sent = 0, decision = 0;
	int nb_voisins, min_local, min;
	int voisins[3], recu[NB_SITE];
	int dernier;

	MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	
	/* Initialisation */
	for(i=0; i<nb_voisins; i++)
		recu[voisins[i]] = 0;
	
	
	while(decision == 0){
		
		/* Determiner le nombre de processus qui n'a rien recu */
		cpt = 0;
		for(i=0; i<nb_voisins; i++){
			if(recu[voisins[i]] == 0){
				tmp = voisins[i];
				cpt++;
			}
		}
		
		/* cpt = 1, i.e au moins un qui n'a pas recu */
		if(cpt == 1 && sent == 0){	
			MPI_Send(&min_local, 1, MPI_INT, tmp, 1, MPI_COMM_WORLD);
			sent = 1;
		}
		
		MPI_Recv(&min, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		dernier = status.MPI_SOURCE;
		recu[status.MPI_SOURCE] = 1;
			
		if(min < min_local)
			min_local = min;
		
		decision = 1;
		for(i=0; i<nb_voisins; i++){
			if(recu[voisins[i]] != 1){
				decision = 0;
				break;
			}
		}
	}
	
	printf("Je suis %d, le min est %d\n", rang, min_local);
	
	for(i=0; i<nb_voisins; i++)
		if(voisins[i] != dernier)
			MPI_Send(&min_local, 1, MPI_INT, voisins[i], 1, MPI_COMM_WORLD);
}

/******************************************************************************/

int main (int argc, char* argv[]) {
   int nb_proc,rang;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);

   if (nb_proc != NB_SITE+1) {
      printf("Nombre de processus incorrect !\n");
      MPI_Finalize();
      exit(2);
   }
  
   MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  
   if (rang == 0) {
      simulateur();
   } else {
      calcul_min(rang);
   }
  
   MPI_Finalize();
   return 0;
}
