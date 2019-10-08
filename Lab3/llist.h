#ifndef LLIST_H
#define LLIST_H
/* llist.h 
 * 
 * External (public) declarations for simple doubly-linked list in C.
 * 
 * This list will know head and tail, and will be capable of forward
 * and reverse iteration.  The tail node should always assign its next
 * pointer to NULL to indicate the end of the list, and likewise, the
 * head node should assign its previous pointer to NULL for the same
 * reason.
 * 
 * Note that the pop operations do not return a reference to the
 * popped node.  This would require storage for the node to be
 * released by the user, which could lead to memory mishandling.
 * The user will need to use head or tail pointers to interact with the
 * node prior to popping it.
 * 
 * This list will only hold strings (char arrays) for simplicity, and
 * will have ownership of the strings.  This means the string will
 * need to be copied into memory under control of the list.
 * 
 */
 
/* Structures */

/* a node - cannot be anonymous because we need a node * inside, but
 * we can make the struct name and the typedef the same */
typedef struct node {
	char *string;
	struct node* next;  /* need struct here because inside typedef */
	struct node* prev;  /* need struct here because inside typedef */
} node;

/* The list itself - struct can be anonymous */
typedef struct {
	node *head;  /* could have been struct node* as well */
	node *tail;
} list;

/* List methods
 * 
 * These methods are used to create and operate on a list as a whole.
 */

/* llInit()
 *     Initialize a list structure.  An empty list will
 *     be characterized by head and tail pointer both being NULL.
 *         Parameters:  myList - a pointer to the structure to be init
 *         Returns: void
 */ 
void llInit(list *myList);

/* llSize()
 *     Reports the current size of the list.  Will need to iterate
 *     the list to get this data size there is no size property, nor
 *     can there really be one given that users can access nodes.
 *         Parameters:  myList - the list
 *         Returns: int, size of list
 */ 
int llSize(list *myList);

/* llPushFront()
 *     Add a new node with provided data and place node at
 *     front of list.  The new node will replace the head node.
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *         Parameters:  myList - the list
 *                      toStore - the char array to store
 *         Returns: int - 0 if no push (toStore was NULL) or non-zero
 *                        if push successful
 */ 
int llPushFront(list *myList,char *toStore);

/* llPopFront()
 *     Removes first item in list.  Note, this does not return
 *     any data from the list.  If the data in the node is needed
 *     it should be accessed prior to the pop (list->head->string).
 *         Parameters:  myList - the list
 *         Returns: int - 0 if no pop (list was empty) or non-zero
 *                        if pop successful
 */ 
int llPopFront(list *myList);

/* llPushBack()
 *     Add a new node with provided data and place node at
 *     end of list.  This new node will be the new tail node.  
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *         Parameters:  myList - the list
 *                      toStore - the char array to store
 *         Returns: int - 0 if no push (toStore was NULL) or non-zero
 *                        if push successful
 */ 
int llPushBack(list *myList, char *toStore);

/* llPopBack()
 *     Removes last item in list.  Note, this does not return
 *     any data from the list.  If the data in the node is needed
 *     it should be accessed prior to the pop (list->tail->string).
 *         Parameters:  myList - the list
 *         Returns: int - 0 if no pop (list was empty) or non-zero
 *                        if pop successful
 */ 
int llPopBack(list *myList);

/* llClear()
 *     Clears all nodes and releases all dynamic memory.  List
 *     structure should be NULLed and can be reused.  
 *         Parameters:  myList - the list
 *         Returns: nothing
 */ 
void llClear(list *myList);


/* Node methods
 * 
 * These methods allow iteration of nodes within the list.  A list
 * reference is still needed if head or tail needs to be modified.
 */

/* llInsertAfter()
 *     Add a new node with provided data and place node after
 *     provided node reference.
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *     If this method is called on the tail node, a change to the
 *     list structure will need to be made.
 *         Parameters:  myList - theList
 *                      insNode - the node after which item is added
 *                      toStore - the char array to store
 *         Returns: int - 0 if no insert (toStore was NULL or insNode
 *                                        is NULL) 
 *                        non-zero if insert successful
 */ 
int llInsertAfter(list* myList, node *insNode, char *toStore);

/* llInsertBefore()
 *     Add a new node with provided data and place node before
 *     provided node reference.
 *     This method should check to make sure the provided char * is
 *     not NULL.  If it is NULL, this method should do nothing and
 *     make no changes to the list.  If it is not NULL, it can be
 *     assumed that it is a valid null-terminated string.
 *     If this method is called on the head node, a change to the
 *     list structure will need to be made.
 *         Parameters:  myList - theList
 *                      insNode - the node before which item is added
 *                      toStore - the char array to store
 *         Returns: int - 0 if no insert (toStore was NULL or insNode
 *                                        is NULL) 
 *                        non-zero if insert successful
 */ 
int llInsertBefore(list* myList, node *insNode, char *toStore);

/* llRemove()
 *     Removes the node referenced.  Releases
 *     all associated dynamic memory.
 *     If this method is called the current head or tail node, changes
 *     to the list structure may need to be made.
 *         Parameters:  myList - the list
 *                      rmvNode - the node prior to the node to be
 *                                removed.
 *         Returns: nothing
 */ 
int llRemove(list* myList, node *rmvNode);
#endif
