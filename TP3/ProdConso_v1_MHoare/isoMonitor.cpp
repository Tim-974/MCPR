#include "isoMonitor.h"
#include <string.h>

isoMonitor :: isoMonitor(unsigned nbIsoloirs){
	nbi = nbIsoloirs;
	nbIsoloirOQP = 0;
	Queue = newConditionWithPriority(2);
}

void isoMonitor :: accederIsoloir(unsigned prio){
	enter();
	
	if (nbIsoloirOQP == nbi){
		if (prio == 'H'){
			Queue->wait(0);
		} else {
			Queue->wait(1);
		}
	}
	
	nbIsoloirOQP++;
	
	leave();
}

void isoMonitor :: sortirIsoloir(){
	enter();
	
	nbIsoloirOQP--;
	
	Queue->signal();
	
	leave();
}
