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
 ***************************  GLOBAL VARIABLES   *******************************
 ******************************************************************************/
uint8_t read_signal_received = 0;
uint8_t death_signal_received = 0;

int main(void) 
{
  BUF *Tptr;
  
  int shmid;
  int pid_client, pid_1, pid_2;
  int client_mutex, server_mutex;
  key_t key_1, key_2, key_3;

  char pipe_1[BUF_SZ];
  int pfd[2];

  int read_data = 0;

  time_t rawtime;
  struct tm * timeinfo;

  /* Client program kickoff, print client PID */
  pid_client = getpid();
  printf("#### PID du client 1: %d ####\n", pid_client);
  
    /* Definition of the handlers for the signals*/
  signal(SIGUSR1, handler_SIGUSR1);

  pause();
  
  signal(SIGUSR2, handler_SIGUSR2);
  
  /* Create and initialises the Server Semaphore */
  key_1 = create_key(FichierCle, CHAR_MUTEX);
  server_mutex = semget(key_1,1,0666);
  
  if(server_mutex < 0)
	  return 0;

  /* Create and initialises the Client Semaphore */
  key_2 = create_key(KEY_FILE_CLIENT, CHAR_MUTEX);
  client_mutex = Creer_Utiliser_sem(key_2, 2, (short)0);
  
  if(client_mutex < 0)
	  return 0;

  if (pipe(pfd) == -1)
  	{
      perror("Erreur pipe");
      exit(EXIT_FAILURE);
	}

  /* Open and attatch the segment of the shared memmory created by server */
  key_3 = create_key(FichierCle, CHAR_SHM);

  if ((shmid = shmget(key_3, sizeof(BUF), 0666)) == -1) {
      perror("shmget");
      exit(EXIT_FAILURE);
  }

  if ((Tptr = (BUF *)shmat(shmid, NULL, 0)) == (BUF *)-1) {
      perror("shmat");
      exit(EXIT_FAILURE);
  }

  // struct sigaction sa;
  // sa.sa_handler = handler_SIGUSR1;
  // sigemptyset(&sa.sa_mask);
  // sa.sa_flags = 0;
  // sigaction(SIGUSR1, &sa, NULL);

  printf("Client waiting for SIGUSR1...\n");

  
  if ((pid_1 = fork()))
  {
    /*This is the parent process - MONITOR */
    printf("Pere monitor : %d\n", getpid());

     if ((pid_2 = fork()))
    {
      /*This is still the parent process - MONITOR */

      while(1)
      {
        /* Wait for a signal, since the USR2 handler terminates
        * the process, this will most likely be USR1 */
        pause(); 

        if(death_signal_received)
        {
          /* Wait for the death of the child*/
          wait(0);

          shmdt(Tptr);
          shmctl(key_3, IPC_RMID, NULL);
          Detruire_sem(client_mutex);

          return 0;
        }

        V(client_mutex,0);
      }
    }
    else
    {
      /*This is the 1st child process - READER */
      printf("Fil lecteur : %d\n", getpid());

      while(1)
      {
        /* Death of the reader child */
        if(death_signal_received)
          //DOUBLE CHECK THE EXIT
          exit(EXIT_SUCCESS);

        /* Wait for the read signal to be received by parent */
        P(client_mutex, 0);

        // Access shared memory
        read_data = Tptr->tampon[Tptr->n];

        // Send data through pipe
        close(pfd[0]);
        sprintf(pipe_1,"%d", read_data);
        write(pfd[1], &pipe_1, sizeof(pipe_1));

        V(client_mutex,1);

      }

    }
    
  }
  else
  {
    /*This is the 2st child process - WRITER */
    printf("Fil redacteur : %d\n", getpid());

    while(1)
      {
        /* Death of the writer child */
        if(death_signal_received)
          //DOUBLE CHECK THE EXIT
          exit(EXIT_SUCCESS);

        /* Wait for the pipe completion by child 2 */
        P(client_mutex, 1);
        
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );

        /* Wait for the read signal to be received and print data */
        close(pfd[1]);
        read(pfd[0], &pipe_1, sizeof(pipe_1));

        printf("Data dans la memoire partagee: %s\n", pipe_1);
        printf ("Date et temps de reception: %s", asctime (timeinfo));

        read_signal_received = 0;
  
      }   
  }

  kill(pid_1, SIGKILL);
  kill(pid_2, SIGKILL);

  shmdt(Tptr);
  shmctl(key_3, IPC_RMID, NULL);
  Detruire_sem(client_mutex);

  return 0;
}

void handler_SIGUSR1(int sig) 
{
	//printf("I GOT THE USER SIGNAL 1\n");
	signal(SIGUSR1, handler_SIGUSR1);
}

void handler_SIGUSR2(int sig) 
{
	//printf("I GOT THE USER SIGNAL 2\n");
	signal(SIGUSR2, handler_SIGUSR2);
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
  sem = semget(key, nb_sem,IPC_CREAT|IPC_EXCL|0666);
  if (sem > 0) {
    /* si la creation a reussi (le semaphore n'existait pas avant) */
    /* on initialise le semaphore */
    semctl(sem,0,SETALL,val_init);
  }
  else {
    /* on recupère l'identifiant du semaphore */
    sem = semget(key,1,0666);
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

/*********************	Op�ration P
 *  ENTREE: semid = identificateur de la famille de semaphores
 *  SORTIE: neant
 *  RETOUR: 0 ou SEMerr
 */
int P(int semid, int nb_sm)
{
  struct sembuf semoper;
  semoper.sem_num = nb_sm;
  semoper.sem_op = -1;
  semoper.sem_flg = 0;
  
  if (semop(semid, &semoper, 1) < 0)
    {
      //perror("Erreur P sur le Mutex");
      return -1;
    }
  
  return 0;
}

/*********************	Op�ration V
 *  ENTREE: semid = identificateur de la famille de semaphores
 *  SORTIE: neant
 *  RETOUR: 0 ou SEMerr
 */
int V(int semid, int nb_sm)
{
  struct sembuf semoper;
  semoper.sem_num = nb_sm;
  semoper.sem_op = 1;
  semoper.sem_flg = 0;
  
  if (semop(semid, &semoper, 1) < 0)
    {
      //perror("Erreur V sur le Mutex");
      return -1;
    }
  
  return 0;
}

