/**
 * @file
 *  client1.c
 * 
 * @brief
 *   Client number 1 main application code
 *
 *  @author Victor Alessi
 *  @author Betina Zynger
 *      
 */
/******************************************************************************/ 

/*******************************************************************************
 ***************************     INCLUDES       ********************************
 ******************************************************************************/
#include "client1.h"

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

/*******************************************************************************
 ***************************  GLOBAL VARIABLES   *******************************
 ******************************************************************************/
uint8_t read_signal_received = 0;
uint8_t death_signal_received = 0;
/*******************************************************************************
 ***************************  LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

int main(void)
{

  int i, j = 0;
	BUF	*Tptr;

  key_t key_1;
  key_t key_2;

  int pid_client1;
	int pid_child;
  int server_mutex;
	int client_mutex;
	int client_shmid;

  /* Client program kickoff, print client PID */
  pid_client1 = getpid();
  printf("#### PID du client 1: %d ####\n", getpid());

  /* Definition of the handlers for the signals*/
  signal(SIGUSR1, handler_SIGUSR1);
  signal(SIGUSR2, handler_SIGUSR1);

  /* Create and initialises the Semaphore for the interaction with the server */
  key_1= create_key(FichierCle, CHAR_MUTEX);
  server_mutex = Creer_Utiliser_sem(key_1, 1, 1);

  /* Wait for the release of the server semaphore (line 87 SV.c) */
  P(server_mutex);

  /* Create or open and attatch the segment of the shared memmory by using the function
  defined in the server code */
  /* attachement de la memoire partagee par le pere */
  if ((Tptr = (BUF) shmat(shmid,NULL,0)) == NULL) {
      perror("Erreur a l attachement de la memoire");
  }
  client_shmid = AllocTampon(&Tptr);
  printf("ID memoire partagee : %d\n", client_shmid);

  /* Create and initialises the Client Semaphore */
  key_2 = create_key(KEY_FILE_CLIENT, CHAR_MUTEX);
  client_mutex = Creer_Utiliser_sem(key_2, 1, 1);

  /* Release the server semaphore (line 85 SV.c) */
  V(server_mutex);	 



  if ((pid_child = fork()))
  {
    while(1)
    {
      /* Wait for the release of the server semaphore */
      P(server_mutex);
      
      /* This is the father process - MONITOR */
      if(read_signal_received)
        /* Release the client semaphore for the child process*/
        V(client_mutex);
    }
    
  }
  else
  {
    /*This is the 1st child process - READER */
    printf("Creation du fil lecteur : %d\n", getpid());

    while(1)
    {
      /* Wait for the read signal to be received by parent */
      P(client_mutex);

      printf("Donne dans la memoire partagee : %d\n",Tptr->n);
    }

  }

  RelacheMemoires(client_shmid);
	DestructionMutex(server_mutex); 
  DestructionMutex(client_mutex); 




}

void handler_SIGUSR1()
{
  read_signal_received = 1;
}
void handler_SIGUSR2()
{
  death_signal_received = 1;
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
