#include "linklist.h"
#include "memory.h"

bool LINKLIST_Clean(LinkList *_list)
{
  /* Non empty? */
  if(_list->count != 0)
  {
    LinkList_Node *node;

    /* Gets first node */
    node = _list->first;

    /* Clean all nodes */
    while(node != NULL)
    {
      LinkList_Node *current;

      /* Backups current node */
      current = node;

      /* Go to next node */
      node = node->next;

      /* Cleans current node */
      memset(current, 0, sizeof(LinkList_Node));
    }
  }

  /* Cleans list */
  memset(_list, 0, sizeof(LinkList));

  /* Done! */
  return TRUE;
}

bool LINKLIST_AddStart(LinkList *_list, LinkList_Node *_node)
{
  bool result = TRUE;

  /* Not already used in a list? */
  if(_node->list == NULL)
  {
    /* Adds it at the start of the list */
    _node->next     = _list->first;
    _node->previous = NULL;
    _node->list     = _list;

    /* Updates old node if needed */
    if(_list->first != NULL)
    {
      _list->first->previous = _node;
    }
    else
    {
      /* Updates last node */
      _list->last   = _node;
    }

    /* Stores node at the start of the list */
    _list->first    = _node;

    /* Updates count */
    _list->count++;
  }
  else
  {
    /* Not linked */
    result = FALSE;
  }

  /* Done! */
  return result;
}

bool LINKLIST_AddEnd(LinkList *_list, LinkList_Node *_node)
{
  bool result = TRUE;

  /* Not already used in a list? */
  if(_node->list == NULL)
  {
    /* Adds it at the end of the list */
    _node->previous = _list->last;
    _node->next     = NULL;
    _node->list     = _list;

    /* Updates old node if needed */
    if(_list->last != NULL)
    {
      _list->last->next = _node;
    }
    else
    {
      /* Updates first node */
      _list->first  = _node;
    }

    /* Stores node at the end of the list */
    _list->last     = _node;

    /* Updates count */
    _list->count++;
  }
  else
  {
    /* Not linked */
    result = FALSE;
  }

  /* Done! */
  return result;
}

bool LINKLIST_AddBefore(LinkList_Node *_refNode, LinkList_Node *_node)
{
  bool result = TRUE;
  LinkList *list;

  /* Isn't already linked? */
  if(_node->list == NULL)
  {
    /* Gets list */
    list = _refNode->list;

    /* Valid? */
    if(list != NULL)
    {
      /* Adds it in the list */
      _node->next         = _refNode;
      _node->previous     = _refNode->previous;
      _node->list         = list;

      /* Updates previous? */
      if(_refNode->previous != NULL)
      {
        /* Updates it */
        _refNode->previous->next = _node;
      }
      else
      {
        /* Updates new first node */
        list->first = _node;
      }

      /* Updates ref node */
      _refNode->previous  = _node;

      /* Updates count */
      list->count++;
    }
    else
    {
      /* No list found */
      result = FALSE;
    }
  }
  else
  {
    /* Already linked */
    result = FALSE;
  }

  /* Done! */
  return result;
}

bool LINKLIST_AddAfter(LinkList_Node *_refNode, LinkList_Node *_node)
{
  bool result = TRUE;
  LinkList *list;

  /* Isn't already linked? */
  if(_node->list == NULL)
  {
    /* Gets list */
    list = _refNode->list;

    /* Valid? */
    if(list != NULL)
    {
      /* Adds it in the list */
      _node->next         = _refNode->next;
      _node->previous     = _refNode;
      _node->list         = list;

      /* Updates next? */
      if(_refNode->next != NULL)
      {
        /* Updates it */
        _refNode->next->previous = _node;
      }
      else
      {
        /* Updates new last node */
        list->last        = _node;
      }

      /* Updates ref node */
      _refNode->next      = _node;

      /* Updates count */
      list->count++;
    }
    else
    {
      /* No list found */
      result = FALSE;
    }
  }
  else
  {
    /* Already linked */
    result = FALSE;
  }

  /* Done! */
  return result;
}

bool LINKLIST_Remove(LinkList_Node *_node)
{
  LinkList *list;
  LinkList_Node *previous, *next;
  bool result = TRUE;

  /* Gets list */
  list = _node->list;

  /* Valid? */
  if(list != NULL)
  {
    /* Gets neighbours pointers */
    previous = _node->previous;
    next     = _node->next;

    /* Not at the start of the list? */
    if(previous != NULL)
    {
      /* Updates previous node */
      previous->next  = next;
    }
    else
    {
      /* Updates list first pointer */
      list->first     = next;
    }

    /* Not at the end of the list? */
    if(next != NULL)
    {
      /* Updates previous node */
      next->previous  = previous;
    }
    else
    {
      /* Updates list last pointer */
      list->last      = previous;
    }

    /* Cleans node */
    memset(_node, 0, sizeof(LinkList_Node));

    /* Udpates count */
    list->count--;
  }
  else
  {
    /* Failed */
    result = FALSE;
  }

  /* Done! */
  return result;
}
