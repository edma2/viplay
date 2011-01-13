/* ds.h - simple data structures
 * author: Eugene Ma (edma2) */
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

typedef struct Node Node;
struct Node {
	void *data;
	struct Node *next; 
};
typedef struct List List;
struct List {
	Node *head;			
	int length;		
};

List *list_new(void); 	
List *list_copy(List *ls);		
Node *list_append(List *ls, void *data); 
Node *list_prepend(List *ls, void *data); 	
Node *list_first(List *ls);	
Node *list_last(List *ls); 
Node *list_search(List *hay, void *needle, int (*s)(void *h, void *n));
void list_free(List *ls);
void list_print(List *ls); 		
void *list_remove_first(List *ls); 
void *list_remove_last(List *ls);
void *list_remove(List *ls, void *data);
void list_traverse(List *ls, void (*func)(void *data));	
int list_size(List *ls);
