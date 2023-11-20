/* Fichier SV_mem.c */

#include "SV_def.h"
#include "SV_mem.h"



/*************   ALLOCATION D'UN TAMPON DE DONNEES
*  ENTREE: Ptr = pointeur du pointeur du tampon
*  SORTIE: le pointeur est renseigne
*  RETOUR: code erreur <0 ou identificateur de la memoire
*/
int AllocTampon(BUF **Ptr)
{
BUF *MemTamponPtr;
int CLTshmid;
key_t key;

if (( key = ftok(FichierCle, CHAR_SHM)) < 0 )
   return CLEerr;
if ((CLTshmid = shmget(key, sizeof(BUF), 0666|IPC_CREAT|IPC_EXCL)) <0)
   return SHMerr;
if ((MemTamponPtr = (BUF *)shmat(CLTshmid,NULL,0)) == 0 )
   return SHMerr;
*Ptr = MemTamponPtr ;
return CLTshmid;
}

/*********************	DESALLOCATION DES TAMPONS
*  ENTREE: Tshmid,Cshmid = ID des memoires partagee
*  SORTIE: neant
*  RETOUR: code erreur <0 ou  0 ;
*/
int RelacheMemoires(int shmid)
{
int ret = 0;

ret = shmctl(shmid, IPC_RMID, NULL);

return ret;
}


/************************* GENERATEUR DE DONNEES
* ENTREE: Tptr = pointeur du tampon des donnees
*  SORTIE: une donnee est cree sur la voie 1 ou sur la voie 2
*  RETOUR: numero de la voie renseignee
*/
int GenereData(BUF *Tptr)
{
  int n;
  n = Tptr->n;
  n = (n+1)%BUF_SZ;
  Tptr->tampon[n] = BUF_SZ - n;
  Tptr->n = n;
  return n;
}

