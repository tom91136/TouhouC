/*
 * common.h
 *
 *  Created on: Sep 29, 2015
 *      Author: Tom
 */

#ifndef SRC_COLLECTION_H_
#define SRC_COLLECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
	void *data;
	struct Node *next;
} Node;

typedef struct LinkedList {
	int length;
	int nodeSize;
	Node *head;
	Node *tail;
} LinkedList;

typedef bool (*Iterator)(void *);
typedef bool (*NodeIterator)(void *);

LinkedList newList(int size);

void appendList(LinkedList *list, void *data);

void **toArray(LinkedList *list);

void freeList(LinkedList *list);

void forEachList(LinkedList *list, Iterator iterator);

#endif /* SRC_COLLECTION_H_ */
