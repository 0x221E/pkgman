/**
 * @file ll.h
 * @brief Minimal linked list
 * @description This is a v1 linked list with minimal
 * functionality. It does not implement a larger container
 * type, therefore, does not store any metadata. This
 * is enough for the needs of this package manager.
 *
 * @warning This list is SLOW. You have to go to the end 
 * each time to add a new element.
 *
 * @note I will implement a better system if and when I
 * need it.
 */

#ifndef LL_H
#define LL_H

#define LL_DEFINE(name, type)   \
    struct ll_##name {          \
        type data;              \
        struct ll_##name *next; \
    } 

//Add malloc guard
// O(n) add (incredibly inefficient for an ll, however, simple)
#define LL_DEFINE_ADD(name, type)                           \
    void ll_##name##_add(struct ll_##name *ll, type val) {  \
        struct ll_##name *current = ll;                     \
                                                            \
        while (current->next != NULL)                       \
            current = current->next;                        \
                                                            \
        current->next = malloc(sizeof(*ll));                \
        current->next->data = val;                          \
        current->next->next = NULL;                         \
    }

#define LL_FOREACH(name, table)                 \
    for (struct ll_##name *current = (table);   \
        current != NULL;                        \
        current = current->next)                \

#endif
