/***********************************************/
/* Solution (client)- Serveur version 2023    */
/* 16 octobre 2023                                      */
/*  Frédéric Rousseau                                 */
/***********************************************/

#include "SV_def.h"
#include "SV_mem.h"
#include "SV_sem.h"


#define TAILLE_CHAINE 100


void handler_SIGTERM();

/* Variables globales du programme client */
int terminaison = 0;
/* static int SVshmid; */

int main(int argc, char *argv[])
{
	int i, j = 0;
	int duree, nbclients;
	BUF	*Tptr;
	int pid_clients[10];
	int SVmutex;
	int SVshmid;

	if (argc < 4)
	{
		printf("ATTENTION, il faut lancer le serveur avec la duree d'execution le nombre de client et les PID des clients !\n");
		printf("./SV  temps_seconde   nombre_clients    PID_client1    PID_client2      ...\n");
		exit(0);
	}
	
	duree = atoi(argv[1]);
	if ((duree > 100) || (duree < 3))
		duree = 15;

	nbclients = atoi(argv[2]);
	if ((nbclients < 1) || (nbclients > 10))
	{
		printf(" Erreur sur le nombre de clients !\n");
		exit (0);
	}

	if ((nbclients + 3) != argc)
	{
		printf(" Erreur sur la liste des PID des clients !\n");
		exit (0);
	}

	for (i = 0; i < nbclients; i++)
		pid_clients[i] = strtol(argv[i + 3], NULL, 0);

	printf("%d clients\n", nbclients);
	for (i = 0; i < nbclients; i++) 
	{
		printf("\n####  PID du client %d : %d   ####\n", i + 1, pid_clients[i]);
		kill(pid_clients[i], SIGUSR1);
	}

	printf("\n");


	/*///////////////////////////creation du mutex de la memoire partagee  //////////////////////*/
    key_t key_1= create_key(FichierCle, CHAR_MUTEX);
	SVshmid = Creer_Utiliser_sem(key_1, 1, 1);

	// if ((SVmutex = CreationMutex()) == -1)
	// {
	// 	printf("Pb creation mutexn");
	// 	exit(0);
	// }
	// if (Init_Mutex(SVmutex, 1) < 0)	/* Init du semaphore Mutex a 1 */
	// {
	// 	printf("Pb init mutex\n");
	// 	exit(0);
	// }


	/*/////////////////////////// creation de la memoire partagee  //////////////////////*/

	SVshmid = AllocTampon(&Tptr);
	printf("ID memoire partagee : %d\n", SVshmid);
	P(SVmutex);	 /* On prend la ressource SHM avec le MUTEX */
	Tptr->n= -1;
	V(SVmutex);    /* On relache la ressource SHM avec le MUTEX */


		
	/*//////////////////////////// Boucle principale  /////////////////////////////*/
	signal(SIGTERM, handler_SIGTERM);
	signal(SIGINT, handler_SIGTERM);
	printf("Emission des données vers %d client(s)\n", nbclients);
	for (i = 0; i < nbclients; i++)
	{
		printf("- PID du client %d : %d   ####\n", i + 1, pid_clients[i]);
	}

	printf("\n");
	sleep(1);

	for (i = 0; i < duree; i++)
	{
		
		P(SVmutex);	 /* On prend la ressource SHM avec le MUTEX */
		GenereData(Tptr);
		V(SVmutex);    /* On relache la ressource SHM avec le MUTEX */

		for (j = 0; j < nbclients; j++)
		{
			kill(pid_clients[j], SIGUSR1);
		}
		sleep(1);
		if (terminaison == 1)
		break;
	}

		
	/*////////////////////////////fin du programme  /////////////////////////////*/
	printf("Terminaison - envoi dun signal de fin aux clients\n\n");
	for (j = 0; j < nbclients; j++)
	{
		kill(pid_clients[j], SIGUSR2);
	}
	sleep(1);

	RelacheMemoires(SVshmid);
	DestructionMutex(SVmutex); 


	return 0;
}	   	   

	   	   	   


/*/////////////////////////////////////////////////////////////////////////*/

void handler_SIGTERM(int i) /* reception SIGTERM ou SIGINT */
{
	terminaison = 1;
	signal(i,handler_SIGTERM);
}


/* **************************************** */
/* Creation d'un ensemble de semaphores     */
/* Ici, on utilise le mode IPC_CREAT car il */
/* faut qu'on cree le semaphore s'il n'exist*/
/* pas et qu'il soit ouvert s'il exist deja */
/* **************************************** */
SEMAPHORE Creer_Utiliser_sem(key_t key, int nb_sem, short *val_init)
{
  SEMAPHORE sem;
  sem = semget(key, nb_sem,IPC_EXCL|IPC_CREAT|0666);
  if (sem > 0) {
    /* si la creation a reussi (le semaphore n'existait pas avant) */
    /* on initialise le semaphore */
    semctl(sem,0,SETALL,val_init);
  }
  else {
    /* on recupère l'identifiant du semaphore */
    sem = semget(key,1,0);
  }
  return sem;
}

/* **************************************** */
/* Destruction d'un ensemble de semaphore   */
/*                                          */
/* On utilise donc semctl avec IPC_RMID     */
/* **************************************** */
void Detruire_sem(SEMAPHORE sem)
{
    if (semctl(sem,0,IPC_RMID,0) != 0)
      {
	perror("Detruire_sem");
	exit(EXIT_FAILURE);
      }
}

/* **************************************** */
/* Modification de la valeur des semaphores */
/*                                          */
/* **************************************** */
void Changer_sem(SEMAPHORE sem, short no_sem, int val)
{
  struct sembuf sb[1];

  sb[0].sem_num = no_sem;
  sb[0].sem_op = val;
  sb[0].sem_flg=0;

  if (semop(sem,sb,1) != 0)
    {
	perror("Changer_sem");
	exit(EXIT_FAILURE);
    }
}

/* **************************************** */
/* **************************************** */
key_t create_key(char *filename, char sem_char)
{
  key_t key;

  if ( (key = ftok(filename,sem_char)) == -1)
    {
      perror("create_key");
      exit(EXIT_FAILURE);
    }
  return key;
}
