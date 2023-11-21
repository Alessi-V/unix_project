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



void handler_SIGUSR1(int sig);
void handler_SIGUSR2(int sig);

#endif /* CLIENT1_H_ */