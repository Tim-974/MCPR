/* UPS/IRIT
   Lecteurs-redacteurs partageant un fichier dans lequel
   les premiers lisent (a plusieurs) et les seconds ecrivent
   (en exclusion mutuelle)
*/
#include <stdio.h>
#include <string.h> /* strerror */
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>

#define NB_MAX_LECTEURS   10 
#define NB_MAX_REDACTEURS 10 
#define NB_LECTURES        5 
#define NB_ECRITURES       6 

/* A compléter pour assurer la synchronisation voulue */

int nbLecteur = 0;
int nbRedacteurAttente = 0;
bool redacPresent = false;

pthread_cond_t redacteur = PTHREAD_COND_INITIALIZER;
pthread_cond_t lecteur = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int fd; // Descriptif partage par tous les threads
// => les lecteurs partagent ce descriptif et donc la position
// de lecture dans le fichier
// Si on veut des lectures indépendantes, ce descriptif doit être local
// au thread qui fera sa propre ouverture et sa propre fermeture du fichier

/*---------------------------------------------------------------------*/
void thdErreur(int codeErr, char *msgErr, void *codeArret) {
/*---------------------------------------------------------------------*/
  fprintf(stderr, "%s: %d soit %s \n", msgErr, codeErr, strerror(codeErr));
  pthread_exit(codeArret);
}

/*---------------------------------------------------------------------*/
void debutLecture(int monNum) {
	pthread_mutex_lock(&mutex);
	
	if (redacPresent == true || nbRedacteurAttente > 0){
		pthread_cond_wait(&lecteur, &mutex);
	}
		
	nbLecteur++;
	
	pthread_cond_signal(&lecteur);
	
	pthread_mutex_unlock(&mutex);
}

/*---------------------------------------------------------------------*/
void finLecture(int monNum) {
	pthread_mutex_lock(&mutex);
	
	nbLecteur--;
	
	if (nbLecteur ==0)
		pthread_cond_signal(&redacteur);
	
	pthread_mutex_unlock(&mutex);
}

/*---------------------------------------------------------------------*/
void debutEcriture(int monNum) {
	pthread_mutex_lock(&mutex);
	
	nbRedacteurAttente++;
	
	if (redacPresent == true || nbLecteur > 0){
		pthread_cond_wait(&redacteur, &mutex);
	}
	
	redacPresent = true;
	
	nbRedacteurAttente--;
	
	pthread_mutex_unlock(&mutex);
}

void finEcriture(int monNum) {
	pthread_mutex_lock(&mutex);
	
	redacPresent = false;
	
	if (nbRedacteurAttente > 0){
		pthread_cond_signal(&redacteur);
	}else{
		pthread_cond_signal(&lecteur);
	}
	
	pthread_mutex_unlock(&mutex);
}

