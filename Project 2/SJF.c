#include <stdio.h>
#include <stdlib.h>
#include "stat.h"
#include "linkedList.h"

process_stat *createProcessStat(process *proc);

int sortByRunTime(void *p1, void *p2)
{
	process_stat *ps1 = (process_stat *)p1;
	process_stat *ps2 = (process_stat *)p2;
	if (((process *)ps1->proc)->runTime < ((process *)ps2->proc)->runTime)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}

average_stats shortestJobFirstNP(linkedList *processes)
{
	int time = 0;

	// Create process queue
	queue *process_queue = (queue *)createQueue();
	node *process_ptr = processes->head;
	if (processes->head == NULL)
	{
		fprintf(stderr, "No Process to schedule\n");
	}
	// check if process queue is not empty or time quanta is less than 100
	process_stat *scheduled_process = NULL;

	linkedList *ll = createLinkedList();
	printf("\nShortest Job First:\n");
	while (time < 100 || scheduled_process != NULL)
	{
		if (process_ptr != NULL)
		{
			process *new_process = (process *)(process_ptr->data);
			while (process_ptr != NULL && new_process->arrivalTime <= time)
			{
				enqueue(process_queue, createProcessStat(new_process));
				sort(process_queue, sortByRunTime);
				process_ptr = process_ptr->next;
				if (process_ptr != NULL)
					new_process = (process *)(process_ptr->data);
			}
		}
		// Schedule the process
		if (scheduled_process == NULL && process_queue->size > 0)
		{
			scheduled_process = (process_stat *)dequeue(process_queue);
		}

		if (scheduled_process != NULL)
		{
			process *proc = scheduled_process->proc;
			printf("%c", proc->pid);
			if (scheduled_process->startTime == -1)
			{
				scheduled_process->startTime = time;
			}
			scheduled_process->runTime++;
			if (scheduled_process->runTime >= proc->runTime)
			{
				scheduled_process->endTime = time;
				addNode(ll, scheduled_process);
				scheduled_process = NULL;
			}
		}
		else
		{
			printf("_");
		}
		time++;
	}
	printf("\n");
	return printPolicyStat(ll);
}
