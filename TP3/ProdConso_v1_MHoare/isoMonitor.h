#include "MONITEURS_HOARE/HoareMonitor.h"
#include "MONITEURS_HOARE/HoareCondition.h"

#define NBI_MAX 5 

class isoMonitor : public HoareMonitor {
	
	public:
		isoMonitor(unsigned nbIsoloirs);
		
		void accederIsoloir(unsigned prio);
		void sortirIsoloir();
	
	protected:
		HoareConditionWithPriority *Queue;
		
		int buffer[NBI_MAX];
		unsigned nbi;
		unsigned nbIsoloirOQP;
	
};
