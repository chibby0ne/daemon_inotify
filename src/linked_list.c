#include "linked_list.h"

struct linked_list *create_linked_list()
{
    struct linked_list *p = (struct linked_list *) malloc(sizeof(struct linked_list));
    if (p == NULL) {
        fprintf(stderr, "something went horribly wrong. Couldn't allocate"\
                "linked list\n");
        exit(EXIT_FAILURE);
    }
    p->front = p->last = NULL;
    p->size = 0;
    return p;
}

int size(struct linked_list *ll)
{
    return ll->size;
}

static struct node *create_node(uint32_t mask)
{
    struct node *n = (struct node *) malloc(sizeof(struct node));
    if (n == NULL) {
        fprintf(stderr, "something went horribly wrong. Couldn't allocate node\n");
        exit(EXIT_FAILURE);
    }
    n->mask = mask;
    n->next = NULL;
    return n;
}

void insert_node(struct linked_list *ll, uint32_t mask)
{
    struct node *n = create_node(mask);
    // If list is empty, put node in the front, else put it at as next of the
    // last node inserted
    if (ll->front == NULL) {
        ll->front = n;
        ll->last = n;
    } else {
        ll->last->next = n;
        ll->last = n;
    }
    ll->size = ll->size + 1;
}

static struct node *find_previous_node(struct linked_list *ll, struct node *n)
{
    struct node *p;
    for (p = ll->front; p->next != n; p = p->next) {
    }
    return p;
}

static void delete_node(struct linked_list *ll, struct node *n)
{
    if (ll->front == n) {
        ll->front = n->next;
    } else {
        struct node *p = find_previous_node(ll, n);
        p->next = n->next;
    }
    free(n);
    ll->size = ll->size - 1;
}

void delete_linked_list(struct linked_list *ll)
{
    for (struct node *p = ll->front; p != NULL; p = p->next) {
        delete_node(ll, p);
    }
    free(ll);
}

void print_linked_list_fd(struct linked_list *ll, int fd)
{
    dprintf(fd, "Linked list:\n");
    if (ll->front != NULL) {
        for (struct node *p = ll->front; p != NULL; p = p->next) {
            dprintf(fd, "\tNode:\n");
            dprintf(fd, "\t\tMask: %d\n\n", p->mask);
        }
    } else {
        dprintf(fd, "Empty\n");
    }
}

void print_linked_list(struct linked_list *ll)
{
    print_linked_list_fd(ll, STDOUT_FILENO);
}
