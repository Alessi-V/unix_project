/**
 * @file
 *  client1.h
 * 
 * @brief
 *   Client number 1 header file
 *
 *  @author Victor Alessi
 *  @author Betina Zynger
 *      
 */
/******************************************************************************/ 
#ifndef CLIENT1_H_
#define CLIENT1_H_

/*******************************************************************************
 ***************************     INCLUDES       ********************************
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


/*******************************************************************************
 ***************************     DEFINES        ********************************
 ******************************************************************************/

#define FichierCle "SV_def.h"
#define CHAR_SHM 'S'
#define CHAR_MUTEX 'M'
#define KEY_FILE_CLIENT "client1.h"

/*******************************************************************************
 ***************************     TYPE DEFS      ********************************
 ******************************************************************************/

typedef int SEMAPHORE;

typedef struct {
    int n;
} BUF;

/*******************************************************************************
 ***************************     FUNCTIONS     ********************************
 ******************************************************************************/

void handler_SIGUSR1(int sig);
void handler_SIGUSR2(int sig);

/* **************************************** */
/* Creation d'un ensemble de semaphores     */
/* Ici, on utilise le mode IPC_CREAT car il */
/* faut qu'on cree le semaphore s'il n'exist*/
/* pas et qu'il soit ouvert s'il exist deja */
/* **************************************** */
SEMAPHORE Creer_Utiliser_sem(key_t key, int nb_sem, short *val_init);

/* **************************************** */
/* Destruction d'un ensemble de semaphore   */
/*                                          */
/* On utilise donc semctl avec IPC_RMID     */
/* **************************************** */
void Detruire_sem(SEMAPHORE sem);

/* **************************************** */
/* Modification de la valeur des semaphores */
/*                                          */
/* **************************************** */
void Changer_sem(SEMAPHORE sem, short no_sem, int val);

/* **************************************** */
/* **************************************** */
key_t create_key(char *filename, char sem_char);

/*********************	Opération P
 *  ENTREE: semid = identificateur de la famille de semaphores
 *  SORTIE: neant
 *  RETOUR: 0 ou SEMerr
 */
int P(int semid);

/*********************	Opération V
 *  ENTREE: semid = identificateur de la famille de semaphores
 *  SORTIE: neant
 *  RETOUR: 0 ou SEMerr
 */
int V(int semid);


#endif /* CLIENT1_H_ */