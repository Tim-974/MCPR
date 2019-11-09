/* UPS/IRIT
   Lecteurs-redacteurs partageant un fichier dans lequel
   les premiers lisent (a plusieurs) et les seconds ecrivent
   (en exclusion mutuelle)
*/

#include <string.h> /* strerror */
#include <stdbool.h>

#include "TP3_LRMonitor.h"

#define NB_MAX_LECTEURS   10 
#define NB_MAX_REDACTEURS 10 
#define NB_LECTURES        5 
#define NB_ECRITURES       6 

/* A compléter pour assurer la synchronisation voulue */


TP3_LRMonitor :: TP3_LRMonitor (){
	
	int nbLecteur = 0;
	bool redacPresent = false;

	Fifo = newConditionWithPriority(2);
}


/*---------------------------------------------------------------------*/
void TP3_LRMonitor :: debutLecture(int monNum) {
	enter();
	
	if (redacPresent == true || Fifo->length() >=1){
		Fifo->wait(1);
	}
		
	nbLecteur++;
	
	Fifo->signal();
	
	leave();
}

/*---------------------------------------------------------------------*/
void TP3_LRMonitor :: finLecture(int monNum) {
	enter();
	
	nbLecteur--;
	
	if (nbLecteur == 0)
		Fifo->signal();
	
	leave();
}

/*---------------------------------------------------------------------*/
void TP3_LRMonitor :: debutEcriture(int monNum) {
	enter();
	
	if (redacPresent == true || nbLecteur > 0){
		Fifo->wait(1);
	}
	
	if (nbLecteur > 0){
		Fifo->wait(0);
	}
	
	redacPresent = true;
	
	leave();
}

void TP3_LRMonitor :: finEcriture(int monNum) {
	enter();
	
	redacPresent = false;
	
	Fifo->signal();
	
	leave();
}

