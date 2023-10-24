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
    if (!head)
        return NULL;

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
    if (!e)
        return false;

    list_add(&e->list, head);

    e->value = malloc(sizeof(char) * (n + 1));
    if (!e->value)
        return false;

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
    if (!e)
        return false;

    list_add_tail(&e->list, head);

    e->value = malloc(sizeof(char) * (n + 1));
    if (!e->value)
        return false;

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

    if (!head)
        return;

    struct list_head *tail_l = head->next;  // including
    struct list_head *tail_r = head->next;  // excluding
    element_t *e1, *e2;
    int i;

    while (tail_r != head) {
        for (i = 0; i < k && tail_r != head; i++)
            tail_r = tail_r->next;

        if (i < k)
            break;

        struct list_head *tail_r2 = tail_r->prev;

        while (tail_l != tail_r2 && tail_l->prev != tail_r2) {
            e1 = list_entry(tail_l, element_t, list);
            e2 = list_entry(tail_r2, element_t, list);

            void *tmp = e1->value;
            e1->value = e2->value;
            e2->value = tmp;

            tail_l = tail_l->next;
            tail_r2 = tail_r2->prev;
        }

        tail_l = tail_r;
    }
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

    if (!head || list_empty(head))
        return 0;

    struct list_head *l1, *l2;
    element_t *e;
    char *min_str = list_entry(head->prev, element_t, list)->value;
    int len = q_size(head);

    for (l1 = head->prev->prev, l2 = l1->prev; l1 != head;
         l1 = l2, l2 = l1->prev) {
        e = list_entry(l1, element_t, list);

        if (strcmp(e->value, min_str) > 0) {
            list_del_init(&e->list);
            q_release_element(e);
            len--;
        } else
            min_str = e->value;
    }

    return len;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    if (!head || list_empty(head))
        return 0;

    struct list_head *l1, *l2;
    element_t *e;
    char *max_str = list_entry(head->prev, element_t, list)->value;
    int len = q_size(head);

    for (l1 = head->prev->prev, l2 = l1->prev; l1 != head;
         l1 = l2, l2 = l1->prev) {
        e = list_entry(l1, element_t, list);

        if (strcmp(e->value, max_str) < 0) {
            list_del_init(&e->list);
            q_release_element(e);
            len--;
        } else
            max_str = e->value;
    }

    return len;
}

static void merge(struct list_head *l1,
                  struct list_head *l2,
                  bool (*cmp)(const char *, const char *))
{
    struct list_head *tail = l1;
    struct list_head *l1_head = l1, *l2_head = l2;

    for (l1 = l1->next, l2 = l2->next;;) {
        element_t *e1 = list_entry(l1, element_t, list);
        element_t *e2 = list_entry(l2, element_t, list);

        if (cmp(e1->value, e2->value)) {
            // pick e1
            tail->next = l1;
            l1->prev = tail;

            tail = tail->next;
            l1 = l1->next;

            if (l1 == l1_head) {
                tail->next = l2;
                l2->prev = tail;

                l2_head->prev->next = l1_head;
                l1_head->prev = l2_head->prev;
                break;
            }
        } else {
            // pick e2
            tail->next = l2;
            l2->prev = tail;

            tail = tail->next;
            l2 = l2->next;

            if (l2 == l2_head) {
                tail->next = l1;
                l1->prev = tail;
                break;
            }
        }
    }

    INIT_LIST_HEAD(l2_head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    bool (*cmp)(const char *, const char *) =
        descend ? &increase_fn : &decrease_fn;

    queue_contex_t *q1, *q2;
    list_for_each_entry_safe (q1, q2, head, chain)
        if (&q2->chain != head)
            merge(q2->q, q1->q, cmp);

    q1 = list_entry(head->next, queue_contex_t, chain);
    q2 = list_entry(head->prev, queue_contex_t, chain);

    void *tmp = q1->q;
    q1->q = q2->q;
    q2->q = tmp;

    return q_size(q1->q);
}
