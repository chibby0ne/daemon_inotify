#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdint.h>

struct node {
    uint32_t mask;
    struct node *next;
};

struct linked_list {
    struct node *front;
    struct node *last;
    int size;
};

struct linked_list *create_linked_list();
static struct node *create_node(uint32_t mask);
void insert_node(struct linked_list *ll, uint32_t mask);
static struct node *find_previous_node(struct linked_list *ll, struct node *n);
static void delete_node(struct linked_list *ll, struct node *n);
void delete_linked_list(struct linked_list *ll);
void print_linked_list_fd(struct linked_list *ll, int fd);
void print_linked_list(struct linked_list *ll);

#endif
