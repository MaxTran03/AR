#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define TAGINIT 0
#define NB_SITE 6
#define M 4
#define DATA_SIZE 64

int cpt_site = NB_SITE;
int p;
char** donnees;


struct mes{
	int req; // type de requete, 0 = deconnexion, 1 = reconnexion, 2 = recherche, 3 = MAJ du nouveau successeur, 4 = MAJ du nouveau prédecesseur, 5 = confirmation
	int id; // id chord
	int resp; // la clé responsable
	int succ; // id chord du successeur
	int rangsucc; //rang du successeur
	int rang;
	char donnee[DATA_SIZE];
	int cle; // responsable de la clé de la donnée recherchée
};

struct maj{
	int id;
	int succ;
	int rangsucc;
	int rang;
	int resp;
	int dest; // id du destinataire pour le cas d'arrêt
};
	

void simulateur(void) {
	printf("############ Initialisation ############\n");
	int tab[p];
	int tab2[p];
	int id_chord[p];
	
	srand(time(NULL));
	int i;
	for(i=0; i<p; i++){
		id_chord[i] = 0;
	}
	int id;
	
	// Initialisation
	for(i=1; i<=NB_SITE; i++){
		id = rand()%p;
		while(id_chord[id]){
			id = rand()%p;
		}	
		id_chord[id] = 1;
		MPI_Send(&id, 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
		tab[id] = i;
		tab2[i] = id;
		printf("Rang = %d, id_chord = %d\n",i, id);
	}
	printf("########################################\n");
	
	// Communiquer le successeur 
	int tmp;
	for(i=0; i<p; i++){
		if(id_chord[i]){
			int j;
			for(j=i+1; j<i+1+p; j++){
				int succ = j%p;
				if(id_chord[succ]){
					MPI_Send(&succ, 1, MPI_INT, tab[i], TAGINIT, MPI_COMM_WORLD);
					MPI_Send(&tab[succ], 1, MPI_INT, tab[i], TAGINIT, MPI_COMM_WORLD);
					printf("2X #######"" %d -> %d\n",id, tab[i]);
					//printf("succ = %d, resp = %d\n",succ, resp);
					//printf("i = %d, succ = %d\n",i, succ);
					break;
				}
			}
		}
	}
	
	
	for(i=0; i<p; i++){
		if(id_chord[i]){
			int resp = (i+1)%p;
			int j;
			for(j=i+1; j<i+1+p; j++){
				int succ = j%p;
				if(id_chord[succ]){
					MPI_Send(&resp, 1, MPI_INT, tab[succ], TAGINIT, MPI_COMM_WORLD);
					 printf("1X#######"" %d -> %d\n",id, tab[succ]);
					break;
				}
			}
		}
	}
	
	
	
}

void site(int rang){
	
	MPI_Status status;
	int id, resp, succ, rangsucc;
	MPI_Recv(&id, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&succ, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&rangsucc, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	MPI_Recv(&resp, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
	
	char *data = (char*)malloc(DATA_SIZE*sizeof(char)); 
	
	printf("id_chord %d, resp %d, succ %d, rangsucc %d\n",id, resp, succ, rangsucc);
	
	struct mes reqr;
	struct mes reqs;
	
	//###########################################################
	//####################### Deconnexion #######################
	//###########################################################
	if(rang == 1){
		// Notification de deconnexion
		reqs.req = 0;
		reqs.rang = rang;
		reqs.succ = succ;
		reqs.rangsucc = rangsucc;
		reqs.resp = resp;
		reqs.id = id;
		reqs.cle = 100000;
		MPI_Send(&reqs,sizeof(struct mes), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		printf(" %d -> %d\n",rang,rangsucc);
	}
	
	MPI_Recv(&reqr, sizeof(struct mes), MPI_CHAR, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
		
	if(reqr.req == 0){
		if(rang == reqr.rangsucc){
			resp = reqr.resp;
			MPI_Send(&reqr, sizeof(struct mes), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
			printf(" %d -> %d\n",rang,rangsucc);
		}else{
			if(reqr.rang == rangsucc){
				succ = reqr.succ;
				rangsucc = reqr.rangsucc;
				reqr.req=5;
				MPI_Send(&reqr, sizeof(struct mes), MPI_CHAR, reqr.rang, TAGINIT, MPI_COMM_WORLD); //confirmation deconnexion
				printf(" %d -> %d\n",rang,rangsucc);
			}else{
				MPI_Send(&reqr, sizeof(struct mes), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
				printf(" %d -> %d\n",rang,rangsucc);
			}
		}
	}else{
		if(reqr.req == 5)
			printf("Deconnexion de %d\n",rang);
	}
	
	/*if(rang==1){
		sleep(1);
	}*/
	printf("MAJ: rang %d, id_chord %d, resp %d, succ %d, rangsucc %d\n",rang, id, resp, succ, rangsucc);
	
	
	//#########################################################
	//####################### Recherche #######################
	//#########################################################
	if(rang==2){
		reqs.req = 2;
		reqs.rang = rang;
		reqs.succ = succ;
		reqs.rangsucc = rangsucc;
		reqs.resp = resp;
		reqs.id = id;
		reqs.cle = -1;
		
		strcpy(data,"allo 13");
		strcpy(reqs.donnee,data);
		MPI_Send(&reqs, sizeof(struct mes), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		printf("Qui a le film %s \n",data);
	}
	
	if(rang != 1){
		MPI_Recv(&reqr, sizeof(struct mes), MPI_CHAR, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
		printf("RECEIVEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE emeteur %d, cle %d\n",status.MPI_SOURCE,reqr.cle);
		//si la donnee a déjà été trouvée
		if(reqr.cle !=-1){ 
			//si on a fait le tour
			if(rang==reqr.rang) 
				printf("Mon rang est %d, j'ai fais une recherche sur la donnee %s, et le responsable est %d\n",rang,data,reqr.cle);
			else //sinon on continue de propager la réponse
				MPI_Send(&reqr, sizeof(struct mes), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		}else{
			if(reqr.req == 2){
				int i;
				if(resp > id){
					for(i=resp; i<p; i++){
						if(strcmp(donnees[i],reqr.donnee) == 0){
							printf("jai le film!\n");
							reqr.cle = id;
							break;
						}	
					}
					
					if(reqr.cle == -1){
						for(i=0; i<=id; i++){
							if(strcmp(donnees[i],reqr.donnee) == 0){
							printf("jai le film!\n");
								reqr.cle = id;
								break;
							}	
						}
					}
				}else{
					for(i=resp; i<=id; i++){
						if(strcmp(donnees[i],reqr.donnee) == 0){
						printf("jai le film!\n");
							reqr.cle = id;
							break;
						}
					}
				}
			}
			if(rang == reqr.rang){
				if(reqr.cle != -1)
					printf("La donnée était en fait chez moi %d\n",rang);
				else
					printf("La donnée n'existe pas \n");
			}else
				MPI_Send(&reqr, sizeof(struct mes), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		}
	}
	
	
	//###########################################################
	//####################### Reconnexion #######################
	//###########################################################
	struct maj rq;
	struct maj rqr;
	if(rang == 1){
		rq.rang = rang;
		/*rq.succ = succ;
		rq.rangsucc = rangsucc;
		rq.resp = resp;*/
		rq.id = id;
		rq.dest = 3;
		MPI_Send(&rq,sizeof(struct maj), MPI_CHAR, 3, TAGINIT, MPI_COMM_WORLD);
	}
	
	MPI_Recv(&rqr, sizeof(struct maj), MPI_CHAR, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);

	if(rqr.rang == rang){
		succ = rqr.succ;
		rangsucc = rqr.rangsucc;
		resp = rqr.resp;
	}else if(rqr.dest == rang){
		MPI_Send(&rq,sizeof(struct maj), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		MPI_Recv(&rqr, sizeof(struct maj), MPI_CHAR, MPI_ANY_SOURCE, TAGINIT, MPI_COMM_WORLD, &status);
		MPI_Send(&rq,sizeof(struct maj), MPI_CHAR, 1, TAGINIT, MPI_COMM_WORLD);
	}else if(id < rqr.id && rqr.id < succ){
		rqr.succ = succ;
		rqr.rangsucc = rangsucc;
		MPI_Send(&rq,sizeof(struct maj), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		rangsucc = rqr.rang;
		succ = rqr.id;
	}else if(id > rqr.id && rqr.id >= resp){
		rqr.resp = resp;
		MPI_Send(&rq,sizeof(struct maj), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
		resp = rqr.id + 1;
	}else
		MPI_Send(&rq,sizeof(struct maj), MPI_CHAR, rangsucc, TAGINIT, MPI_COMM_WORLD);
			
	printf("NEW: rang %d, id_chord %d, resp %d, succ %d, rangsucc %d\n",rang, id, resp, succ, rangsucc);
	
	
	
	free(data);
}

	

int main (int argc, char* argv[]) {
   	int nb_proc,rang;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
	
	int i;
	char film[DATA_SIZE];
	char ep[2];
	
	p = 1<<M;
	
	donnees = (char**)malloc(p*sizeof(char*));
	
	for(i=0;i<p;i++){
		donnees[i] = (char*)malloc(DATA_SIZE*sizeof(char));
		strcpy(film,"allo ");
		sprintf(ep, "%d", i);
		strcpy(donnees[i],strcat(film,ep));
	}
	
	if (nb_proc != NB_SITE+1) {
		printf("Nombre de processus incorrect !\n");
		MPI_Finalize();
		exit(2);
	}
  
	MPI_Comm_rank(MPI_COMM_WORLD, &rang);
	if(rang == 0){
		simulateur();
	}else{
		site(rang);
	}
	
	for(i=0;i<p;i++)
		free(donnees[i]);
	
   	free(donnees);
  
	MPI_Finalize();
	return 0;
}
