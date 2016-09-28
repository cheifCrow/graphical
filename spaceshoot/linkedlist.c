#include "linkedlist.h"
#include <stdlib.h>

void recursivefree(llmember* index) {
    if(index->next) {
        recursivefree(index->next);
    }
    free(index->content);
    free(index);
    return;
}

void freell(LList* list) {
    recursivefree(list->head);
    free(list);
}

int llempty(LList* list) {
    if (!(list->head) && !(list->tail)) {
        return 1;
    }
    else {
        return 0;
    }
}

void add_llmember_head(LList* list, llmember* index) {
    if (llempty(list)) {
        list->head = index;
        list->tail = index;
    }
    else {
        list->head->prev = index;
        index->next = list->head;
        list->head = index;
    }
}

void add_llmember(LList* list, llmember* index) {
    if (llempty(list)) {
        list->head = index;
        list->tail = index;
    }
    else {
        list->tail->next = index;
        index->prev = list->tail;
        list->tail = index;
    }
}

void delete_llmember(LList* list, llmember* index) {
    if(index->prev && index->next) {
        index->next->prev = index->prev;
        index->prev->next = index->next;
    }
    else if (index->prev) {
        index->prev->next = NULL;
        list->tail = index->prev;
    }
    else if (index->next) {
        index->next->prev = NULL;
        list->head = index->next;
    }
    free(index->content);
    free(index);

}
