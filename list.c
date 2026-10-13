#include "list.h"

/* Allocates and initializes an empty linked list */
list_t *list_alloc() {
    list_t *l = (list_t *)malloc(sizeof(list_t));
    if (l != NULL) {
        l->head = NULL;
    }
    return l;
}

/* Frees all nodes in the list, then frees the list struct itself */
void list_free(list_t *l) {
    if (l == NULL) return;
    node_t *current = l->head;
    while (current != NULL) {
        node_t *next = current->next;
        free(current);
        current = next;
    }
    free(l);
}

/* Returns list contents formatted as string, e.g. "10->20->30->NULL" */
char *listToString(list_t *l) {
    static char buf[1024];
    memset(buf, 0, sizeof(buf));

    if (l == NULL || l->head == NULL) {
        snprintf(buf, sizeof(buf), "NULL");
        return buf;
    }

    node_t *curr = l->head;
    char temp[32];
    while (curr != NULL) {
        snprintf(temp, sizeof(temp), "%d->", curr->value);
        strncat(buf, temp, sizeof(buf) - strlen(buf) - 1);
        curr = curr->next;
    }
    strncat(buf, "NULL", sizeof(buf) - strlen(buf) - 1);
    return buf;
}

/* Returns the total number of elements */
int list_length(list_t *l) {
    if (l == NULL) return 0;
    int count = 0;
    node_t *curr = l->head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    return count;
}

/* Adds an element to the front */
void list_add_to_front(list_t *l, elem value) {
    if (l == NULL) return;
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = l->head;
    l->head = new_node;
}

/* Adds an element to the end */
void list_add_to_back(list_t *l, elem value) {
    if (l == NULL) return;
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = NULL;

    if (l->head == NULL) {
        l->head = new_node;
        return;
    }

    node_t *curr = l->head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
}

/* Adds an element at a 1-based or 0-based index (handles bounds gracefully) */
void list_add_at_index(list_t *l, elem value, int index) {
    if (l == NULL) return;
    if (index <= 1 || l->head == NULL) {
        list_add_to_front(l, value);
        return;
    }

    node_t *curr = l->head;
    int count = 1;
    while (curr->next != NULL && count < index - 1) {
        curr = curr->next;
        count++;
    }

    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = curr->next;
    curr->next = new_node;
}

/* Removes and returns the first element */
elem list_remove_from_front(list_t *l) {
    if (l == NULL || l->head == NULL) return -1;
    node_t *temp = l->head;
    elem val = temp->value;
    l->head = l->head->next;
    free(temp);
    return val;
}

/* Removes and returns the last element */
elem list_remove_from_back(list_t *l) {
    if (l == NULL || l->head == NULL) return -1;
    
    if (l->head->next == NULL) {
        elem val = l->head->value;
        free(l->head);
        l->head = NULL;
        return val;
    }

    node_t *curr = l->head;
    while (curr->next->next != NULL) {
        curr = curr->next;
    }

    elem val = curr->next->value;
    free(curr->next);
    curr->next = NULL;
    return val;
}

/* Removes and returns element at specified index */
elem list_remove_at_index(list_t *l, int index) {
    if (l == NULL || l->head == NULL) return -1;

    if (index <= 1) {
        return list_remove_from_front(l);
    }

    node_t *curr = l->head;
    int count = 1;
    while (curr->next != NULL && count < index - 1) {
        curr = curr->next;
        count++;
    }

    if (curr->next == NULL) {
        return -1;
    }

    node_t *target = curr->next;
    elem val = target->value;
    curr->next = target->next;
    free(target);
    return val;
}

/* Checks if value exists in the list */
bool list_is_in(list_t *l, elem value) {
    if (l == NULL) return false;
    node_t *curr = l->head;
    while (curr != NULL) {
        if (curr->value == value) return true;
        curr = curr->next;
    }
    return false;
}

/* Retrieves element at specified index without removing it */
elem list_get_elem_at(list_t *l, int index) {
    if (l == NULL || l->head == NULL) return -1;
    node_t *curr = l->head;
    int count = 1;
    while (curr != NULL) {
        if (count == index) return curr->value;
        count++;
        curr = curr->next;
    }
    return -1;
}

/* Returns the index of a given value */
int list_get_index_of(list_t *l, elem value) {
    if (l == NULL) return -1;
    node_t *curr = l->head;
    int idx = 1;
    while (curr != NULL) {
        if (curr->value == value) return idx;
        curr = curr->next;
        idx++;
    }
    return -1;
}