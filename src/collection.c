#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "collection.h"
// we also need a linked list for rolling input

LinkedList newList(int size) {
	LinkedList list = { .length = 0, .nodeSize = size, .head = NULL, .tail =
	NULL };
	return list;
}

void appendList(LinkedList *list, void *data) {
	// TODO check for bad data size?

	Node *node = NULL;
	node = malloc(sizeof(Node));
	node->data = data;
	node->next = NULL;

	if (list->length == 0) {
		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}
	list->length++;
}

//void deleteNode(LinkedList *list, Node node){
//	node.
//}
//
//int deleteAtIndex(LinkedList *list, int index) {
//	if (index > list->length) {
//		return -1;
//	}
//
//	Node *node = list->head;
//	int i = 0;
//	while (node != NULL && i <  index-1) {
//
//		node = node->next;
//		i++;
//	}
//
//	return 1;
//}

void **toArray(LinkedList *list) {
	void **arr = malloc(sizeof(void *) * list->length);
	int i = 0;
	Node *node = list->head;
	while (node != NULL) {
		arr[i] = node->data;
		node = node->next;
		i++;
	}
	return arr;
}

void freeList(LinkedList *list) {
	while (list->head != NULL) {
		Node *current = list->head;
		list->head = current->next;
		free(current);
	}
}

void forEachList(LinkedList *list, Iterator iterator) {
	Node *node = list->head;
	bool result = 1;
	while (node != NULL && result) {
		result = iterator(node->data);
		node = node->next;
	}
}

void forEachListNode(LinkedList *list, NodeIterator iterator) {
	Node *node = list->head;
	bool result = 1;
	while (node != NULL && result) {
		result = iterator(node);
		node = node->next;
	}
}
