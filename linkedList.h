#ifndef _linkedList_h_
#define _linkedList_h_

struct node_s
{
	struct node_s *next;
	struct node_s *prev;
	void *data;
};

typedef struct node_s node;

struct linkedList_s
{
	node *head;
	node *tail;
	int size;
};

typedef struct linkedList_s linkedList;

node *createNode(void *data);
linkedList *createLinkedList();
void addNode(linkedList *ll, void *data);
void removeNode(linkedList *ll, void *data);
void swapNodes(node *a, node *b);
void addAfter(linkedList *ll, node *after_node, void *data);
void sort(linkedList *ll, int (*cmp)(void *data1, void *data2));

// Queue Implementation

typedef struct linkedList_s queue;

queue *createQueue();
void enqueue(queue *q, void *data);
void *dequeue(queue *q);

#endif