#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT    0
#define NB_SITE 6

void simulateur(void) {
   int i;

   /* nb_voisins[i] est le nombre de voisins du site i */
   int nb_voisins[NB_SITE+1] = {-1, 3, 3, 2, 3, 5, 2};
   int min_local[NB_SITE+1] = {-1, 12, 11, 8, 14, 5, 17};

   /* liste des voisins */
   int voisins[NB_SITE+1][5] = {{-1, -1, -1, -1, -1},
            {2, 5, 3, -1, -1}, {4, 1, 5, -1, -1}, 
            {1, 5, -1, -1, -1}, {6, 2, 5, -1, -1},
            {1, 2, 6, 4, 3}, {4, 5, -1, -1, -1}};
                               
   for(i=1; i<=NB_SITE; i++){
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
   }
}

/******************************************************************************/

void calcul_min(int rang){
	MPI_Status status;
	int i, pere, cond = 0, cpt = 0, decision = 0;
	int nb_voisins, min_local, min;
	int voisins[3], recu[NB_SITE];

	MPI_Recv(&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	
	/* Initialisation */
	for(i=0; i<nb_voisins; i++)
		recu[voisins[i]] = 0;
	
	/* Supposons que le processus 1 est l'initiateur */
	if(rang == 1){
		while(decision == 0){
			
			for(i=0; i<nb_voisins; i++){
				MPI_Send(&min_local, 1, MPI_INT, voisins[i], 1, MPI_COMM_WORLD);
			}
			
			MPI_Recv(&min, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if(min < min_local)
				min_local = min;
			recu[status.MPI_SOURCE] = 1;
			
			decision = 1;
			for(i=0; i<nb_voisins; i++){
				if(recu[voisins[i]] != 1){
					decision = 0;
					break;
				}
			}
		}
	}else{
		while(cond == 0){
			MPI_Recv(&min, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if(min < min_local)
				min_local = min;
			recu[status.MPI_SOURCE] = 1;
		
			if(pere == 0){
				pere = status.MPI_SOURCE;
			
				for(i=0; i<nb_voisins; i++){
					if(voisins[i] != pere)
						MPI_Send(&min_local, 1, MPI_INT, voisins[i], 1, MPI_COMM_WORLD);
				}
			}
		
			cpt = 0;
			cond = 1;
			for(i=0; i<nb_voisins; i++){
				if(recu[voisins[i]] == 1){
					cpt++;
				}else{
					cond = 0;
					break;
				}
			}
		
			if(cpt == nb_voisins)
				MPI_Send(&min_local, 1, MPI_INT, pere, 1, MPI_COMM_WORLD);
		}
	}
	
	printf("Je suis %d, le min est %d\n", rang, min_local);
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
