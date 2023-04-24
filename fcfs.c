#include "stat.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    char name;
    float arrival_time;
    float expected_run_time;
    int priority;
    float turnaround_time;
    float waiting_time;
    float response_time;
    float start_time;
    float end_time;
} ProcessFCFS;

void sort_by_arrival_time(ProcessFCFS processes[], int num_processes) {
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = i + 1; j < num_processes; j++) {
            if (processes[i].arrival_time > processes[j].arrival_time) {
                ProcessFCFS temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }
}

average_stats firstComeFirstServeNP(linkedList * processes) {
    average_stats result;

    //converting linkedlist to array of struct
    ProcessFCFS* processArr = (ProcessFCFS*)malloc(processes->size * sizeof(ProcessFCFS));
    int processCount = 0;
    int i = 0;
    node* current = processes->head;
    while (current != NULL) {
        process* p = (process*)current->data;
        processArr[i].name = p->pid; // Set name to an arbitrary letter
        processArr[i].arrival_time = p->arrivalTime;
        processArr[i].expected_run_time = p->runTime;
        processArr[i].priority = p->priority;
        current = current->next;
        i++;
    }
    processCount = i;

    //0th Index
    processArr[0].waiting_time = 0;
    processArr[0].turnaround_time = processArr[0].expected_run_time;
    processArr[0].response_time = 0;
    processArr[0].start_time = 0;
    processArr[0].end_time = processArr[0].expected_run_time;
    processArr[0].arrival_time = 0;

    //fcfs requests served based on arrival time.
    sort_by_arrival_time(processArr, processCount);

    //populate values after sorting by arrival time
    for (int i = 1; i < processCount; i++) {
        // Set the start time of the process to the maximum of the end time of the previous process and the arrival time of the current process
        processArr[i].start_time = processArr[i - 1].end_time + 1;
        if(processArr[i].arrival_time > processArr[i].start_time) {
            //edge case incase greater set to arrival time
            processArr[i].start_time = processArr[i].arrival_time;
        }
        if(processArr[i].start_time > 99) {
            //edge case incase greater set to arrival time; can happen these are random values generated
            processArr[i].start_time = processArr[i].arrival_time;
        }
        //end time
        processArr[i].end_time = processArr[i].start_time + processArr[i].expected_run_time;
    }

    // Initialize the wait time, turnaround time, and response time of the rest of the processes
    for (int i = 1; i < processCount; i++)  {
        // Calculate the turnaround time of the process
        processArr[i].turnaround_time = processArr[i].end_time - processArr[i].arrival_time;
        // Calculate the waiting time of the process
        processArr[i].waiting_time = fabs(processArr[i].turnaround_time - processArr[i].expected_run_time);
        // Calculate the response time of the process
        processArr[i].response_time = processArr[i].start_time - processArr[i].arrival_time;
    }

    // Calculate the average wait time, turnaround time, response time, and throughput of all processes
    float avg_wait_time = 0;
    float avg_turnaround_time = 0;
    float avg_response_time = 0;
    float total_time = processArr[processCount - 1].end_time - processArr[0].arrival_time;
    float throughput = processCount / total_time;

    //average summation
    for (int i = 0; i < processCount; i++) {
        avg_wait_time += processArr[i].waiting_time;
        avg_turnaround_time += processArr[i].turnaround_time;
        avg_response_time += processArr[i].response_time;
    }

    // Printing time chart
    printf("**** Time Chart FCFS ****\n");
    printf("|");
    for (int i = 0; i < processCount; i++) {
        for (int j = 0; j < processArr[i].expected_run_time; j++) {
            printf("%c",processArr[i].name);
        }

    }
    printf("|");
    printf("\n");

    // Calculating wait time, turnaround time, and response time of each process
    for (int i = 0; i < processCount; i++) {
        processArr[i].waiting_time = processArr[i].turnaround_time - processArr[i].expected_run_time;
    }

    //average with num of processes
    avg_wait_time /= processCount;
    avg_turnaround_time /= processCount;
    avg_response_time /= processCount;

    // Print the results
    printf("FCFS Scheduling Algorithm\n");
    printf("----------------------------\n");
    printf("Process\tArrival Time\tStart Time\tEnd Time\tPriority\tTurnaround Time\t\tWaiting Time\tResponse Time\n");

    for (int i = 0; i < processCount; i++) {
        printf("%c\t%.2f\t\t%.2f\t\t%.2f\t\t%d\t\t%.2f\t\t\t%.2f\t\t%.2f\n",
               processArr[i].name,
               processArr[i].arrival_time,
               processArr[i].start_time,
               processArr[i].end_time,
               processArr[i].priority,
               fabs(processArr[i].turnaround_time),
               fabs(processArr[i].waiting_time),
               fabs(processArr[i].response_time));
    }

    printf("Average Wait Time: %.2f\n", avg_wait_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Response Time: %.2f\n", avg_response_time);
    printf("Throughput: %.2f processes per quanta unit of time\n", throughput);

    //add to result
    result.avg_wait_time = avg_wait_time;
    result.avg_turnaround = avg_turnaround_time;
    result.avg_response_time = avg_response_time;
    result.avg_throughput = throughput;

    return result;

}