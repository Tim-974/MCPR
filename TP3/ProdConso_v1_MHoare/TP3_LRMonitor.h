#ifndef PC_H
#define PC_H

#include <stdio.h>
#include <string.h> /* strerror */
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>

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
		HoareCondition *Fifo;
};

#endif
