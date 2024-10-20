#include <stddef.h>
#include <stdint.h>
#include <kernel/spinlock.h>
#ifndef LIST_H
#define LIST_H

#define DEFINE_LIST(nodeType)     \
    struct nodeType;              \
    typedef struct nodeType##list \
    {                             \
        struct nodeType *head;    \
        struct nodeType *tail;    \
        spin_lock_t lock;         \
        uint32_t size;            \
    } nodeType##_list_t;

#define DEFINE_LINK(nodeType)        \
    struct nodeType *next##nodeType; \
    struct nodeType *prev##nodeType; \
    nodeType##_list_t *container;

#define INITIALIZE_LIST(list)          \
    list.head = list.tail = (void *)0; \
    list.size = 0;                     \
    spin_init(&list.lock);

#define IMPLEMENT_LIST(nodeType)                                                  \
    void append_##nodeType##_list(nodeType##_list_t *list, struct nodeType *node) \
    {                                                                             \
        spin_lock(&list->lock);                                                   \
        list->tail->next##nodeType = node;                                        \
        node->prev##nodeType = list->tail;                                        \
        list->tail = node;                                                        \
        node->next##nodeType = NULL;                                              \
        list->size += 1;                                                          \
        if (list->head == NULL)                                                   \
        {                                                                         \
            list->head = node;                                                    \
        }                                                                         \
        node->container = list;                                                   \
        spin_unlock(&list->lock);                                                 \
    }                                                                             \
                                                                                  \
    void push_##nodeType##_list(nodeType##_list_t *list, struct nodeType *node)   \
    {                                                                             \
        spin_lock(&list->lock);                                                   \
        node->next##nodeType = list->head;                                        \
        node->prev##nodeType = NULL;                                              \
        list->head = node;                                                        \
        list->size += 1;                                                          \
        if (list->tail == NULL)                                                   \
        {                                                                         \
            list->tail = node;                                                    \
        }                                                                         \
        node->container = list;                                                   \
        spin_unlock(&list->lock);                                                 \
    }                                                                             \
                                                                                  \
    struct nodeType *peek_##nodeType##_list(nodeType##_list_t *list)              \
    {                                                                             \
        return list->head;                                                        \
    }                                                                             \
                                                                                  \
    struct nodeType *pop_##nodeType##_list(nodeType##_list_t *list)               \
    {                                                                             \
        spin_lock(&list->lock);                                                   \
        struct nodeType *res = list->head;                                        \
        list->head = list->head->next##nodeType;                                  \
        list->head->prev##nodeType = NULL;                                        \
        list->size -= 1;                                                          \
        if (list->head == NULL)                                                   \
        {                                                                         \
            list->tail = NULL;                                                    \
        }                                                                         \
        res->container = NULL;                                                    \
        spin_unlock(&list->lock);                                                 \
        return res;                                                               \
    }                                                                             \
                                                                                  \
    uint32_t size_##nodeType##_list(nodeType##_list_t *list)                      \
    {                                                                             \
        return list->size;                                                        \
    }                                                                             \
                                                                                  \
    struct nodeType *next_##nodeType##_list(struct nodeType *node)                \
    {                                                                             \
        return node->next##nodeType;                                              \
    }                                                                             \
                                                                                  \
    void remove_##nodeType(nodeType##_list_t *list, struct nodeType *node)        \
    {                                                                             \
        spin_lock(&list->lock);                                                   \
        if (node->container == list)                                              \
        {                                                                         \
            if (node->prev##nodeType == NULL)                                     \
            {                                                                     \
                list->head = node->next##nodeType;                                \
            }                                                                     \
            else                                                                  \
            {                                                                     \
                node->prev##nodeType = node->next##nodeType;                      \
            }                                                                     \
            if (node->next##nodeType == NULL)                                     \
            {                                                                     \
                list->tail = node->prev##nodeType;                                \
            }                                                                     \
            else                                                                  \
            {                                                                     \
                node->next##nodeType = node->prev##nodeType;                      \
            }                                                                     \
        }                                                                         \
        node->container = NULL;                                                   \
        spin_unlock(&list->lock);                                                 \
    }

#endif