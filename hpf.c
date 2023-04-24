#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "stat.h"
#include "linkedList.h"

typedef struct {
    char name;
    float arrival_time;
    float expected_run_time;
    int priority;
} process_ak;

typedef struct {
    process_ak* processes;
    int size;
} process_list;

typedef struct {
    process_ak p;
    int start_time;
    int end_time;
    float time_left;
} stats_ak;

typedef struct {
    char* time_chart;
    int idle_slices;
    float average_turnaround_time;
    float average_waiting_time;
    float average_response_time;
    float throughput;
} output;

// Highest priority first (HPF) [non-preemptive]
output hpf_nonpreemptive(process_list plist) {
    process_ak* priority_queue[4] = {NULL, NULL, NULL, NULL};
    int queue_sizes[4] = {0, 0, 0, 0};
    for (int i = 0; i < plist.size; i++) {
        process_ak p = plist.processes[i];
        int priority = p.priority - 1;
        priority_queue[priority] = realloc(priority_queue[priority], sizeof(process_ak) * (queue_sizes[priority] + 1));

        priority_queue[priority][queue_sizes[priority]] = p;
        queue_sizes[priority]++;
    }

    char* time_chart = malloc(sizeof(char) * 110 );
    int idle_time = 0;
    float total_turnaround_time = 0;
    float total_waiting_time = 0;
    float total_response_time = 0;
    int num_processes_run = 0;

    // printf("HPF (non-preemptive) Algorithm: \n");
    int i;
    for (i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            if (queue_sizes[j] > 0 && priority_queue[j][0].arrival_time <= i) {
                process_ak p = priority_queue[j][0];

                float response_time = i - p.arrival_time;
                float turnaround_time = response_time + p.expected_run_time;
                float waiting_time = turnaround_time - p.expected_run_time;

                for (int k = 0; k < p.expected_run_time; k++, i++) {
                    // printf("%c", p.name);
                    time_chart[i] = p.name;
                }
                i--;
                
                total_turnaround_time += turnaround_time;
                total_waiting_time += waiting_time;
                total_response_time += response_time;

                num_processes_run++;

                for (int k = 0; k < queue_sizes[j] - 1; k++) {
                    priority_queue[j][k] = priority_queue[j][k + 1];
                }
                queue_sizes[j]--;
                break;
            }

            if (j == 3) {
                // printf("-");
                time_chart[i] = '-';
                idle_time++;
            }
        }
    }

    // printf("\n");

    output o;
    o.idle_slices = idle_time;
    o.time_chart = time_chart;
    o.average_turnaround_time = total_turnaround_time / num_processes_run;
    o.average_waiting_time = total_waiting_time / num_processes_run;
    o.average_response_time = total_response_time / num_processes_run;
    
    // printf("Number of processes run: %d\n", num_processes_run);
    // printf("Total time spent: %f\n", i * 1.0);

    o.throughput = num_processes_run / (i * 1.0);

    for (int i = 0; i < 4; i++) {
        free(priority_queue[i]);
    }

    return o;
}

// Highest priority first (HPF) [preemptive && rr]
output hpf_preemptive(process_list plist) {
    stats_ak* priority_queue[4] = {NULL, NULL, NULL, NULL};
    int queue_sizes[4] = {0, 0, 0, 0};
    for (int i = 0; i < plist.size; i++) {
        process_ak p = plist.processes[i];
        int priority = p.priority - 1;
        priority_queue[priority] = realloc(priority_queue[priority], sizeof(stats_ak) * (queue_sizes[priority] + 1));
        
        stats_ak s;
        s.p = p;
        s.start_time = -1;
        s.end_time = -1;
        s.time_left = p.expected_run_time;

        priority_queue[priority][queue_sizes[priority]] = s;
        queue_sizes[priority]++;
    }

    int next_run[] = {0, 0, 0, 0};

    char* time_chart = malloc(sizeof(char) * 140 );
    int idle_time = 0;
    float total_turnaround_time = 0;
    float total_waiting_time = 0;
    float total_response_time = 0;
    int num_processes_run = 0;

    int i;
    // printf("HPF (preemptive) Algorithm: \n");
    for (i = 0; i < 100; i++) {
        for (int j = 0; j < 4; j++) {
            if (queue_sizes[j] > 0 && priority_queue[j][next_run[j]].p.arrival_time <= i) {
                if (priority_queue[j][next_run[j]].start_time == -1) {
                    priority_queue[j][next_run[j]].start_time = i;
                }

                // printf("%c", priority_queue[j][next_run[j]].p.name);
                time_chart[i] = priority_queue[j][next_run[j]].p.name;

                if (priority_queue[j][next_run[j]].time_left > 1) {
                    priority_queue[j][next_run[j]].time_left--;

                    // If next process in queue has arrived, move to next process else reset to first process
                    if (next_run[j] + 1 < queue_sizes[j] && priority_queue[j][next_run[j] + 1].p.arrival_time <= i) {
                        next_run[j]++;
                    } else {
                        next_run[j] = 0;
                    }
                } else {
                    priority_queue[j][next_run[j]].end_time = i + priority_queue[j][next_run[j]].time_left;

                    float response_time = priority_queue[j][next_run[j]].start_time - priority_queue[j][next_run[j]].p.arrival_time;
                    float turnaround_time = priority_queue[j][next_run[j]].end_time - priority_queue[j][next_run[j]].p.arrival_time;
                    float waiting_time = turnaround_time - priority_queue[j][next_run[j]].p.expected_run_time;

                    total_turnaround_time += turnaround_time;
                    total_waiting_time += waiting_time;
                    total_response_time += response_time;

                    num_processes_run++;

                    for (int k = next_run[j]; k < queue_sizes[j] - 1; k++) {
                        priority_queue[j][k] = priority_queue[j][k + 1];
                    }
                    queue_sizes[j]--;

                    // If current process has arrived do nothing, else reset to first process
                    if (next_run[j] < queue_sizes[j] && priority_queue[j][next_run[j]].p.arrival_time <= i) {
                        next_run[j] = next_run[j];
                    } else {
                        next_run[j] = 0;
                    }
                }
                
                break;
            }

            if (j == 3) {
                printf("-");
                // time_chart[i] = '-';
                idle_time++;
            }
        }
    }

    for (int j = 0; j < 4; j++) {
        while (1) {
            // If queue is empty or first process in queue has not started, break
            if (queue_sizes[j] == 0 || priority_queue[j][0].start_time == -1) {
                break;
            }

            // printf("%c", priority_queue[j][next_run[j]].p.name);
            time_chart[i] = priority_queue[j][next_run[j]].p.name;

            if (priority_queue[j][next_run[j]].time_left > 1) {
                priority_queue[j][next_run[j]].time_left--;

                // If next process in queue has been started, move to next process else reset to first process
                if (next_run[j] + 1 < queue_sizes[j] && priority_queue[j][next_run[j] + 1].start_time != -1) {
                    next_run[j]++;
                } else {
                    next_run[j] = 0;
                }
            } else {
                priority_queue[j][next_run[j]].end_time = i + priority_queue[j][next_run[j]].time_left;

                float response_time = priority_queue[j][next_run[j]].start_time - priority_queue[j][next_run[j]].p.arrival_time;
                float turnaround_time = priority_queue[j][next_run[j]].end_time - priority_queue[j][next_run[j]].p.arrival_time;
                float waiting_time = turnaround_time - priority_queue[j][next_run[j]].p.expected_run_time;

                total_turnaround_time += turnaround_time;
                total_waiting_time += waiting_time;
                total_response_time += response_time;

                num_processes_run++;

                for (int k = next_run[j]; k < queue_sizes[j] - 1; k++) {
                    priority_queue[j][k] = priority_queue[j][k + 1];
                }
                queue_sizes[j]--;

                // If current process has been started do nothing, else reset to first process
                if (next_run[j] < queue_sizes[j] && priority_queue[j][next_run[j]].start_time != -1) {
                    next_run[j] = next_run[j];
                } else {
                    next_run[j] = 0;
                }
            }
            
            i++;
        }
    }

    // printf("\n");

    output o;
    o.average_turnaround_time = total_turnaround_time / num_processes_run;
    o.average_waiting_time = total_waiting_time / num_processes_run;
    o.average_response_time = total_response_time / num_processes_run;
    o.idle_slices = idle_time;
    o.time_chart = time_chart;

    // printf("Number of processes run: %d\n", num_processes_run);
    // printf("Total time spent: %f\n", i * 1.0);

    o.throughput = num_processes_run / (i * 1.0);

    for (int j = 0; j < 4; j++) {
        free(priority_queue[j]);
    }
    
    return o;
}

// sort processes by arrival time
process_list sort_processes(process_list plist) {
    // Sort by arrival time
    for (int i = 0; i < plist.size; i++) {
        for (int j = i + 1; j < plist.size; j++) {
            if (plist.processes[i].arrival_time > plist.processes[j].arrival_time) {
                process_ak temp = plist.processes[i];
                plist.processes[i] = plist.processes[j];
                plist.processes[j] = temp;
            }
        }
    }

    // // Reassign names to processes after sorting by arrival time (A, B, C, D, ...)
    // for (int i = 0; i < plist.size; i++) {
    //     if (i < 26) {
    //         plist.processes[i].name = 'A' + i;
    //     } else {
    //         plist.processes[i].name = 'a' + i - 26;
    //     }
    // }

    return plist;
}

average_stats run_hpf_algorithm(int preemptive, linkedList* ll) {
    // Copy linked list into process* array
    int size = 0;
    process_ak* processes = (process_ak *)malloc(sizeof(process_ak) * ll->size);

    node* ptr = ll->head;
    for (int i = 0; i < ll->size; i++) {
        if (ptr == NULL) {
            break;
        }

        process* p = (process *)ptr->data;

        process_ak pak;
        pak.name = (char)p->pid;
        pak.arrival_time = p->arrivalTime;
        pak.expected_run_time = p->runTime;
        pak.priority = (int)p->priority;

        processes[size] = pak;
        size++;

        ptr = ptr->next;
    }
    
    process_list plist;
    plist.size = size;
    plist.processes = processes;

    plist = sort_processes(plist);

    output o;

    if (preemptive) {
        printf("\nHPF (preemptive) Algorithm:\n");
        o = hpf_preemptive(plist);
    } else {
        printf("\nHPF (non-preemptive) Algorithm:\n");
        o = hpf_nonpreemptive(plist);
    }

    printf("Time chart: %s\n", o.time_chart);
    printf("Average turnaround time: %f\n", o.average_turnaround_time);
    printf("Average waiting time: %f\n", o.average_waiting_time);
    printf("Average response time: %f\n", o.average_response_time);
    printf("Throughput: %f\n", o.throughput);

    average_stats as;
    as.avg_turnaround = o.average_turnaround_time;
    as.avg_wait_time = o.average_waiting_time;
    as.avg_response_time = o.average_response_time;
    as.avg_throughput = o.throughput;

    free(plist.processes);
    free(o.time_chart);

    return as;    
}