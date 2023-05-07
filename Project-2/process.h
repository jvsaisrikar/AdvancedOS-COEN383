#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "linkedList.h"

struct process_s
{
	unsigned char pid;
	float arrivalTime;
	float runTime;
	unsigned char priority;
};

typedef struct process_s process;

process *createProcess(char pid, float arrivalTime, float runTime, unsigned char priority);

process *getCopyofProcess(process *proc);

linkedList *generateProcesses(int n);

#endif