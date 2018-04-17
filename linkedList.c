#include "20161577.h"
#include "linkedList.h"

void addToList(LIST* listHead, void* data) {
    NODE* cur = *listHead;
    NODE* newNode = malloc(sizeof(NODE));
    newNode->data = data;
    newNode->next = NULL;
    if(!cur) {
        *listHead = newNode;
        return;
    }
    while(cur->next)
        cur = cur->next;
    cur->next = newNode;
}

void freeList(LIST* listHead) {
    NODE* cur = *listHead;
    NODE* next = NULL;
    while(cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
    *listHead = NULL;
}

void printList(LIST listHead, void (fptr)(void*)) {
    NODE* cur = listHead;
    while(cur) {
        (fptr)(cur->data);
        cur = cur->next;
    }
}

void printHistory(void* data) {
    static int index = 1;
    printf("%-3d  ", index++);
    puts(((HIST_NODE*)data)->str);
}
