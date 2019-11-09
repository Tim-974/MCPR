#ifndef PC_H
#define PC_H

#include "MONITEURS_HOARE/HoareMonitor.h"
#include "MONITEURS_HOARE/HoareCondition.h"

class TP3_LRMonitor : public HoareMonitor {
	
	public:
	
		TP3_LRMonitor();
		
		void debutLecture(int monNum);
		void debutEcriture(int monNum);
		void finLecture(int monNum);
		void finEcriture(int monNum);
		
	protected:
	
		HoareConditionWithPriority *Fifo;
		int nbLecteur = 0;
		bool redacPresent = false;
};

#endif
