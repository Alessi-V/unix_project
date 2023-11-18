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

void handler_SIGSUR1();
void handler_SIGSUR2();
void handler_SIGTERM();

/*-- structure des tampons --*/
typedef struct {
   int n;       /* indice tableau derniere donnee ecrite */
   int tampon[BUF_SZ];
}BUF;


