#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *e1, *e2;
    list_for_each_entry_safe (e1, e2, head, list)
        q_release_element(e1);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    size_t n = strlen(s);
    element_t *e = malloc(sizeof(element_t));

    list_add(&e->list, head);

    e->value = malloc(sizeof(char) * (n + 1));
    strncpy(e->value, s, n);
    e->value[n] = '\0';
    // cppcheck-suppress memleak
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    size_t n = strlen(s);
    element_t *e = malloc(sizeof(element_t));

    list_add_tail(&e->list, head);

    e->value = malloc(sizeof(char) * (n + 1));
    strncpy(e->value, s, n);
    e->value[n] = '\0';
    // cppcheck-suppress memleak
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->next;
    list_del(node);

    element_t *e = container_of(node, element_t, list);
    strncpy(sp, e->value, bufsize - 1);
    sp[bufsize - 1] = '\0';

    return e;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    struct list_head *node = head->prev;
    list_del(node);

    element_t *e = container_of(node, element_t, list);
    strncpy(sp, e->value, bufsize - 1);
    sp[bufsize - 1] = '\0';

    return e;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    struct list_head *node;
    int n = 0;

    list_for_each (node, head)
        n++;

    return n;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (!head || list_empty(head))
        return false;

    struct list_head *fw = head->next, *bw = head->prev;
    while (fw != bw && fw->next != bw) {
        fw = fw->next;
        bw = bw->prev;
    }

    list_del_init(fw);
    element_t *e = container_of(fw, element_t, list);
    q_release_element(e);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (!head)
        return false;

    element_t *e1, *e2;
    bool is_this_dup = false, is_next_dup;

    list_for_each_entry_safe (e1, e2, head, list) {
        if (&e2->list != head && !strcmp(e1->value, e2->value))
            is_this_dup = is_next_dup = true;
        else
            is_next_dup = false;

        if (is_this_dup) {
            list_del_init(&e1->list);
            q_release_element(e1);
        }

        is_this_dup = is_next_dup;
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/

    if (!head)
        return;

    element_t *e1, *e2;
    int cnt = 0;
    list_for_each_entry_safe (e1, e2, head, list) {
        if (!(cnt % 2) && &e2->list != head) {
            void *tmp = e1->value;
            e1->value = e2->value;
            e2->value = tmp;
        }
        cnt++;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *fw = head->next, *bw = head->prev;
    while (fw != bw) {
        char **fw_c = &(container_of(fw, element_t, list)->value);
        char **bw_c = &(container_of(bw, element_t, list)->value);

        void *tmp = *fw_c;
        *fw_c = *bw_c;
        *bw_c = tmp;

        fw = fw->next;
        bw = bw->prev;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

static bool increase_fn(const char *s1, const char *s2)
{
    return strcmp(s1, s2) > 0;
}

static bool decrease_fn(const char *s1, const char *s2)
{
    return strcmp(s1, s2) < 0;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head)
        return;

    bool (*cmp)(const char *, const char *) =
        descend ? &decrease_fn : &increase_fn;

    int len = q_size(head);
    element_t *e1, *e2;

    while (len) {
        list_for_each_entry_safe (e1, e2, head, list) {
            if (&e2->list != head && cmp(e1->value, e2->value)) {
                void *tmp = e1->value;
                e1->value = e2->value;
                e2->value = tmp;
            }
        }
        len--;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
