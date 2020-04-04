#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    list_ele_t *cur;
    if (!q)
        return;
    cur = q->head;
    while (cur) {
        list_ele_t *tmp = cur;
        cur = cur->next;
        free(tmp->value);
        free(tmp);
    }
    /* Free queue structure */
    free(q);
}

static list_ele_t *create_ele(const char *s)
{
    list_ele_t *newh = NULL;
    char *t = NULL;

    if (!(newh = malloc(sizeof(list_ele_t))))
        return NULL;

    if (!(t = (char *) malloc(sizeof(char) * (strlen(s) + 1))))
        goto out_free_ele;

    snprintf(t, strlen(s) + 1, "%s", s);
    newh->value = t;
    newh->next = NULL;

    return newh;

out_free_ele:
    free(newh);
    return NULL;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;

    if (!q)
        return false;

    if (!(newh = create_ele(s)))
        return false;

    newh->next = q->head;
    q->head = newh;
    if (!q->tail)
        q->tail = newh;
    ++q->size;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newh;

    if (!q)
        return false;

    if (!(newh = create_ele(s)))
        return false;

    if (q->tail)
        q->tail->next = newh;
    else
        q->head = newh;

    q->tail = newh;
    ++q->size;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    list_ele_t *tmp = q->head;

    q->head = q->head->next;
    if (!q->head)
        q->tail = NULL;
    --q->size;

    if (sp && tmp->value) {
        snprintf(sp, bufsize, "%s", tmp->value);
    }
    free(tmp->value);
    free(tmp);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return (q) ? q->size : 0;
}

static list_ele_t *reverse(list_ele_t *head)
{
    list_ele_t *prev = NULL;
    list_ele_t *next = NULL;
    while (head) {
        next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
    // prevent stack overflow, we should not use recursion.
    /*
    list_ele_t *new_head;
    if (!head->next)
        return head;
    new_head = reverse(head->next);
    head->next->next = head;
    head->next = NULL;
    return new_head;
    */
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head)
        return;
    q->tail = q->head;
    q->head = reverse(q->head);
}

static list_ele_t *merge(list_ele_t *left, list_ele_t *right)
{
    list_ele_t *head = NULL;
    list_ele_t **indirect = &head;

    while (left && right) {
        if (!left->value ||
            (right->value && 0 >= strcmp(left->value, right->value))) {
            *indirect = left;
            left = left->next;
        } else {
            *indirect = right;
            right = right->next;
        }
        indirect = &(*indirect)->next;
    }
    *indirect = (left) ? left : right;
    return head;
}

static list_ele_t *merge_sort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;
    while (fast && fast->next) {
        fast = fast->next->next;
        slow = slow->next;
    }
    fast = slow->next;
    slow->next = NULL;

    list_ele_t *left = merge_sort(head);
    list_ele_t *right = merge_sort(fast);
    return merge(left, right);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || !q->head)
        return;
    q->head = merge_sort(q->head);
    list_ele_t *cur = q->head;
    while (cur->next) {
        cur = cur->next;
    }
    q->tail = cur;
}
