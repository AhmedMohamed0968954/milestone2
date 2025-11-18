

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"




/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {
    if (list != NULL && *list != NULL) {
        dplist_node_t *current = (*list)->head;
        if (free_element) {
            while (current != NULL) {
                dplist_node_t *next = current->next;
                (*list) -> element_free(&(current->element));
                free(current);
                current = next;
            }
        }
        else {
            while (current != NULL) {
                dplist_node_t *next = current->next;
                free(current);
                current = next;
            }
        }
        free(*list);
        *list = NULL;
    }

}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;
    list_node = malloc(sizeof(dplist_node_t));
    if (insert_copy) {
        void *copy = list -> element_copy(element);
        list_node->element = copy;
    }
    else {
        list_node->element = element;
    }
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {
    if (list == NULL) {
        return NULL;
    }
    if (list->head == NULL) {
        return list;
    }
    dplist_node_t *toRemove = dpl_get_reference_at_index(list, index);
    if (toRemove -> prev) {
        if (toRemove -> next) { // in the middle
            toRemove->prev->next = toRemove->next;
            toRemove->next->prev = toRemove->prev;
        }
        else { // at the end
            toRemove->prev->next = NULL;
        }
    }
    else {
        if (toRemove -> next) { // head
            toRemove->next->prev = NULL;
            list->head = toRemove->next;
        }
        else { //head + at the end
            list->head = NULL;
        }
    }
    if (free_element) {
        list -> element_free(&(toRemove->element));
    }
    free(toRemove);
    return list;

}

int dpl_size(dplist_t *list) {
    if (list->head != NULL) {
        dplist_node_t *temp = list->head;
        int count = 1;
        while (temp->next != NULL) {
            temp = temp->next;
            count++;
        }
        return count;
    }
    return 0;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    if (dpl_get_reference_at_index(list, index) == NULL) {
        return NULL;
    }
    return dpl_get_reference_at_index(list, index)->element;
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    if (list == NULL || list->head == NULL) {
        return -1;
    }
    dplist_node_t *current = list->head;
    int count = 0;
    while (current->element != element) {
        count++;
        current = current->next;
        if (current == NULL) {
            return -1;
        }
    }
    return count;

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }
    int size = dpl_size(list);
    if (index < size && index >= 0) {
        dplist_node_t *current = list->head;
        for (int currentIndex = 0; currentIndex < index; currentIndex++ ) {
            current = current->next;
        }
        return current;
    }
    else if (index >= size) {
        dplist_node_t *current = list->head;
        while (current -> next) {
            current = current -> next;
        }
        return current;
    }
    else if (index < 0) {
        return (list->head);
    }
    return NULL;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    if (list == NULL || reference == NULL || list->head == NULL) {
        return NULL;
    }
    dplist_node_t *current = list->head;
    while (current != NULL) {
        if (current == reference) {
            return current->element;
        }
        current = current->next;
    }
    return NULL;
}