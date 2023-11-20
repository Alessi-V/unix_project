/***********************************************/
/* Solution (client)- Serveur version 2023    */
/* 16 octobre 2023                                      */
/*  Frédéric Rousseau                                 */
/***********************************************/

#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <stdint.h>

#define FichierCle "SV_def.h"
#define CHAR_SHM 'S'
#define CHAR_MUTEX 'M'

#define BUF_SZ  100	/* Taille des buffers d'entree */

/*---- Codes d'erreur --------*/
#define NOERR_err       0
#define CLEerr          -100
#define SHMerr          -101
#define MSGerr          -102
#define SEMerr         -103

/*-- structure des tampons --*/
typedef struct {
   int n;       /* indice tableau derniere donnee ecrite */
   int tampon[BUF_SZ];
}BUF;

typedef int SEMAPHORE;

void handler_SIGUSR1();
void handler_SIGUSR2();
void handler_SIGTERM(int i);

SEMAPHORE Creer_Utiliser_sem(key_t key, int nb_sem, short *val_init);
void Detruire_sem(SEMAPHORE sem);
void Changer_sem(SEMAPHORE sem, short no_sem, int val);
key_t create_key(char *filename, char sem_char);
