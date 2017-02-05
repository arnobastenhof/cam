#ifndef NODE_H_
#define NODE_H_

#define Push(me,np)    Node_AddFirst((node_t **)(me),(node_t *)(np))
#define Pop(me)        Node_RemoveFirst((node_t **)(me))
#define Enqueue(me,np) Node_AddLast((node_t **)(me),(node_t *)(np))
#define Append(me,np)  Node_Append((node_t **)(me),(node_t *)(np))
#define Prepend(me,np) Node_Prepend((node_t **)(me),(node_t *)(np))

#define Link(me)    (void *)(((node_t *)(me))->link)
#define IsEmpty(me) ((me) == NULL)
#define Peek(me)    (void *)(IsEmpty(me) ? NULL : ((node_t *)(me))->link)

typedef struct node_s node_t;

struct node_s {
  node_t *  link;
};

extern void     Node_AddFirst(node_t ** const, node_t * const);
extern void     Node_AddLast(node_t ** const, node_t * const);
extern void *   Node_RemoveFirst(node_t ** const);
extern void     Node_Prepend(node_t ** const, node_t * const);
extern void     Node_Append(node_t ** const, node_t * const);

#endif /* NODE_H_ */

