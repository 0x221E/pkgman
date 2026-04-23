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

/* typedef void (*ll_free_fn)(); */
// TODO: Add callback function for freeing ll contents.

#define LL_DEFINE(name, type)           \
    struct ll_##name##_node {           \
        type data;                      \
        struct ll_##name##_node *next;  \
    };                                  \
                                        \
    struct ll_##name {                  \
        struct ll_##name##_node *head;  \
        struct ll_##name##_node *tail;  \
        size_t len;                     \
    }

#define LL_DEFINE_ADD(name, type)                           \
    void ll_##name##_add(struct ll_##name *ll, type val);

//Add malloc guard
//TODO: Cleanup the macro
// O(1) add 
#define LL_IMPL_ADD(name, type)                                     \
    void ll_##name##_add(struct ll_##name *ll, type val) {          \
        struct ll_##name##_node *current = ll->tail;                \
        if(current == NULL) {                                       \
            current = malloc(sizeof(struct ll_##name##_node));      \
            current->data = val;                                    \
            current->next = NULL;                                   \
            ll->tail = current;                                     \
            ll->head = ll->tail;                                    \
            ll->len++;                                              \
            return;                                                 \
        }                                                           \
        current->next = malloc(sizeof(struct ll_##name##_node));    \
        current->next->data = val;                                  \
        current->next->next = NULL;                                 \
        ll->tail = current->next;                                   \
    }

#define LL_FOREACH(name, table)                                 \
    for (struct ll_##name##_node *current = ((table)->head);    \
        current != NULL;                                        \
        current = current->next)                                \

#define LL_DEFINE_FREE(name)                        \
    void ll_##name##_free(struct ll_##name *ll);    

#define LL_IMPL_FREE(name)                          \
    void ll_##name##_free(struct ll_##name *ll)     \
    {                                               \
        LL_FOREACH(name, ll) {                      \
            free(current);                          \
        }                                           \
    } 

#endif
