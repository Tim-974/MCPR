#include "types.h"

void cacheSnoopBusRequests(arch_t * arch, int core_num, stats_t * stats){
	//verifier que c'est pas notre transaction
	//recuperer la transaction et changer d'était si il faut
	//si mon cache est en etat S et ma prochaine requete est BusRDx* et que je lis sur le bus un BusRDx, je dois changer mon BusRDx* en BusRDx
	int blockNum = arch->bus.current_transaction.block_num;
	char typeT = arch->bus.current_transaction.type;
	//char etatMonCache = arch->caches[core_num].lines[blockNum].state;
	char * modif = &arch->caches[core_num].lines[blockNum].state;
	
	if (arch->bus.served == core_num){
		return;
	} else {
		switch (*modif){
			case 'M':
				if  (typeT == BUS_READ) {
					stats->num_Flush++;
					*modif = 'S';
				} else if (typeT == BUS_READX) {
					stats->num_Flush++;
					stats->num_invalidations++;
					*modif = 'I';
				}
				break;
			case 'E':
				if  (typeT == BUS_READ) {
					*modif = 'S';
				} else if (typeT == BUS_READX) {
					*modif = 'I';
					stats->num_invalidations++;
				}
				break;
			case 'S':
				if  (typeT == BUS_READ) {
					*modif = 'S';
				} else if (typeT == BUS_READX) {
					*modif = 'I';
					stats->num_invalidations++;
				} else if (typeT == BUS_READXX) {
					*modif = 'I';
					stats->num_invalidations++;
				}break;
			 case 'I':
				
				break; 
		}
	}
		
}
