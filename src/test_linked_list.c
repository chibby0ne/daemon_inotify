#include "linked_list.h"

int main(int argc, char *argv[])
{
    struct linked_list *ll = create_linked_list();
    insert_node(ll, 1);
    print_linked_list(ll);
    insert_node(ll, 2);
    print_linked_list(ll);
    insert_node(ll, 3);
    print_linked_list(ll);
    insert_node(ll, 4);
    print_linked_list(ll);
    delete_linked_list(ll);
    ll = create_linked_list();
    insert_node(ll, 10);
    print_linked_list(ll);
    insert_node(ll, 20);
    print_linked_list(ll);
    insert_node(ll, 30);
    print_linked_list(ll);
    insert_node(ll, 40);
    print_linked_list(ll);
    return 0;
}
