#ifndef _LINKLIST_H_
#define _LINKLIST_H_

#include "types.h"

typedef struct __LinkList_Node_t
{
  struct __LinkList_Node_t *next;
  struct __LinkList_Node_t *previous;
  struct __LinkList_t      *list;

} LinkList_Node;

typedef struct __LinkList_t
{
  LinkList_Node *first;
  LinkList_Node *last;
  u32            count;

} LinkList;

bool                               LINKLIST_Clean(LinkList *_list);
bool                               LINKLIST_AddStart(LinkList *_list, LinkList_Node *_node);
bool                               LINKLIST_AddEnd(LinkList *_list, LinkList_Node *_node);
bool                               LINKLIST_AddBefore(LinkList_Node *_refNode, LinkList_Node *_node);
bool                               LINKLIST_AddAfter(LinkList_Node *_refNode, LinkList_Node *_node);
bool                               LINKLIST_Remove(LinkList_Node *_node);

static inline LinkList *                  LINKLIST_GetList(const LinkList_Node *_node)
{
  return(_node->list);
}

static inline LinkList_Node *             LINKLIST_GetPrevious(const LinkList_Node *_node)
{
  return((_node->list != NULL) ? _node->previous : (LinkList_Node *)NULL);
}

static inline LinkList_Node *             LINKLIST_GetNext(const LinkList_Node *_node)
{
  return((_node->list != NULL) ? _node->next : (LinkList_Node *)NULL);
}

static inline LinkList_Node *             LINKLIST_GetFirst(const LinkList *_list)
{
  return(_list->first);
}

static inline LinkList_Node *             LINKLIST_GetLast(const LinkList *_list)
{
  return(_list->last);
}

static inline u32                         LINKLIST_GetCount(const LinkList *_list)
{
  return(_list->count);
}

#endif
