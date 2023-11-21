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
for (i = 0; i < nbclients; i++) {
       printf("\n####  PID du client %d : %d   ####\n", i + 1, pid_clients[i]);
       kill(pid_clients[i], SIGUSR1);
   }

printf("\n");


/*///////////////////////////creation du mutex de la memoire partagee  //////////////////////*/

 if ((SVmutex = CreationMutex()) == -1)
    {
      printf("Pb creation mutexn");
      exit(0);
    }
 if (Init_Mutex(SVmutex, 1) < 0)	/* Init du semaphore Mutex a 1 */
    {
      printf("Pb init mutex\n");
      exit(0);
    }


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
    for (i = 0; i < nbclients; i++) {
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



