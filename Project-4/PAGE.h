#ifndef PAGING_H
#define PAGING_H

#define DEBUG 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define Total_PROCESS 150
#define TotaL_DURATION 60
#define PROCss_DuraTN 7
#define Toal_PAGE 100

extern int Simulation_CLOCK;
extern int *PAGINGOPtns;

typedef struct {
    int pid, PGECNTER, TIMEARR, DRUTION, PGCRR;
} process;

typedef struct page {
    int pid;
    int PGENUMBER;
    struct page* next;

    float FTBOUGHT;
    float LONE;
    int CNTER;
} page;

typedef struct {
    page* HDL;
} LISTOFPAGES;

// pages which are free
int PageWhoAreFree(LISTOFPAGES*,int);
//Existing pages in memory
int MEMinPAGES(LISTOFPAGES*,int,int);

page* PAGEfrreeeg(LISTOFPAGES*);
// freeing memoryss
void MEMRFree(LISTOFPAGES*,int);
//initializing all pages
void Intializing_PAGELIst(LISTOFPAGES*);
/// displaying all pages
void PageDisplaying(LISTOFPAGES*);

//getting all pages number
int PGNUMNXT(int,int);

// comparision between arrival time
int CMP_ARRtime(const void* ,const void*);
// Pagesid who are free
page* IDwhozPGrFree(LISTOFPAGES*,int,int);

void FIFO_FUNCTION(LISTOFPAGES*);
void LRU_FUNCTION(LISTOFPAGES*);
void LFU_FUNCTION(LISTOFPAGES*);
void MFU_FUNCTION(LISTOFPAGES*);
void R_FUNCTION(LISTOFPAGES*);



#endif
