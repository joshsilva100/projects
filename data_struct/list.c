/* list.c
 * Joshua Silva
 * jysilva
 * ECE 2230 Spring 2024
 * MP3
 *
 * Purpose: To be called from another program to modify linked lists. Maintain data privacy by 
 * calling other program's functions, such as data_clean in list.h, to handle data.
 *
 * Assumptions: Writer of another program does not need to know the specifics of how the list is 
 * altered, but knows which functions to call to edit list. Writer of this program does not need to 
 * know the specifics of how the data is altered, but knows which functions to call to edit data.
 *
 * Bugs: None that I know of.
 *
 * The interface definition for the two-way linked list ADT is based on one 
 * defined by OPNET, Inc. http://www.opnet.com/
 */

#include <stdlib.h>
#include <assert.h>

#include "datatypes.h"   /* defines Data */
#include "list.h"        /* defines public functions for list ADT */

/* definitions for private constants used in list.c only */

#define LIST_SORTED_ASCENDING   -1234567
#define LIST_SORTED_DESCENDING  -7654321
#define LIST_UNSORTED           -4444444

/* prototypes for private functions used in list.c only */
void list_debug_validate(List *L);
void list_bubble_sort(List** L, int sort_order);
void list_insertion_sort(List** L, int sort_order);
void list_recursive_selection_sort(List** L, int sort_order);
void list_selection_sort(List** L, int sort_order);
void list_merge_sort(List** L, int sort_order); 
void swap(ListNode *A, ListNode *B); //Swap two nodes 
void Selection_Sort(List* L, ListNode *head, ListNode *tail, int sort_order); //Handle Recursive Selection Sort 
ListNode* FindMax(List* L, ListNode *m, ListNode *n); //Find max task in the list 
ListNode* FindMin(List* L, ListNode *m, ListNode *n); //Find max task in the list 
void Partition(List **L, List *left, List *right); //Partition list into two smaller halves 
void Merge(List **L, List *left, List *right, int sort_order); //Merge two smaller halves into one list

/* sorting helper functions) */

/* Sorts a list used a specific sorting algorithm and direction.
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * sort_type: sorting algorithm used during sorting: 1) Bubble Sort; 
 *            2)Insertion Sort; 3) Recursive Selection Sort; 
 *            4) Iterative Selection Sort; and 5) Merge Sort
 *
 * sort_order: if 1 then the items are to be sorted in descending order.
 *             if 2 then the items are to be sorted in ascending order.
 *
 * return value: None.
 */
void list_sort(List **list_ptr, int sort_type, int sort_order)
{
    //Pointer to list pointer & list pointer is pointing to something
    assert(list_ptr != NULL || (*list_ptr) != NULL); 
    int size = (*list_ptr) -> current_list_size;
    
    //Select which sort to use, do NOT allow sorts for lists with only one task
    if (size > 1) 
    { 
        switch (sort_type) 
        { 
            case 1: 
                list_bubble_sort(list_ptr, sort_order);
                break; 
            case 2: 
                list_insertion_sort(list_ptr, sort_order);
                break; 
            case 3: 
                list_recursive_selection_sort(list_ptr, sort_order);
                break; 
            case 4: 
                list_selection_sort(list_ptr, sort_order);
                break; 
            case 5: 
                list_merge_sort(list_ptr, sort_order);
                break;
        }
    }

    //Set list sorted order
    (*list_ptr)->list_sorted_state = (sort_order == 2) ? LIST_SORTED_ASCENDING: LIST_SORTED_DESCENDING;
    list_debug_validate(*list_ptr);
}

/* Sorting the list via the insertion sort algorithm
 * 
 * L: pointer to list-of-interest.  
 *
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void list_bubble_sort(List** L, int sort_order)
{
    int size = ((*L) -> current_list_size) - 1; 
    ListNode *N = (*L) -> head; 
    int comp; 

    //Set up comp for comp_proc function based on sort order 
    comp = (sort_order == 1) ? 1: -1;

    //Standard bubble sort, made to deal with two way linked list
    for (int i = 0; i < size; i++) 
    { 
        for (int j = 0; j < (size - i); j++) 
        { 
            //Will swap two nodes if they do not follow assigned order
            if((*L) -> comp_proc(N -> data_ptr, N -> next -> data_ptr) == comp) 
            { 
                //swap() will handle swapping the two nodes
                swap(N, N -> next);
            } 
            N = N -> next; //Go to next node 
        } 
        N = (*L) -> head; //Start over
    }

}

/* 
 * Swap data between two given nodes
 *  
 * A: Pointer to a node in list
 * 
 * B: Pointer to the next node in list 
 *  
 * return value: None
 */
void swap(ListNode *A, ListNode *B) 
{ 
    //Switch data between the two nodes, no need to physically redo node connections
    Data *temp = A -> data_ptr; 
    A -> data_ptr = B -> data_ptr; 
    B -> data_ptr = temp;
}

/* Sorting the list via the insertion sort algorithm
 * 
 * L: pointer to list-of-interest.  
 *
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void list_insertion_sort(List** L, int sort_order)
{
    //New List to store sorted tasks
    List *N = list_construct((*L) -> comp_proc, (*L) -> data_clean);  
    int size = (*L) -> current_list_size;

    //Set new list sorted order 
    N -> list_sorted_state = (sort_order == 2) ? LIST_SORTED_ASCENDING: LIST_SORTED_DESCENDING;

    for (int i = 0; i < size; i++) 
    { 
        list_insert_sorted(N, list_remove(*L, LISTPOS_HEAD));
    }
    
    //Delete old list & set list_ptr to new list
    list_destruct(*L); 
    *L = N;
}

/* Sorting the list via the recursive selection sort algorithm
 * 
 * L: pointer to list-of-interest.
 *
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void list_recursive_selection_sort(List** L, int sort_order)
{
    ListNode *m = (*L) -> head;
    ListNode *n = (*L) -> tail;

    Selection_Sort(*L, m, n, sort_order);
}

/* Handles the recursive calls for recursive selection sort
 * 
 * L: pointer to list-of-interest. 
 *  
 * head: pointer to head node
 *  
 * tail: pointer to tail node
 *
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void Selection_Sort(List* L, ListNode *head, ListNode *tail, int sort_order) 
{ 
    ListNode *max_min; 

    if (head != tail) //Base Case, will stop when end of list reached
    {
        //Descending List, higher ID goes first
        if (sort_order == 1) 
        {
            max_min = FindMax(L, head, tail); 
        } 
        //Ascending List, lower ID goes first
        else if (sort_order == 2) 
        { 
            max_min = FindMin(L, head, tail);
        } 

        swap(head, max_min); //Use Swap function to swap A & B
        Selection_Sort(L, head -> next, tail, sort_order); //Recursive Call 
    }
} 

/* 
 * Find max node by its ID
 * Similar to Find_Max from Standish 
 *  
 * L: Pointer to the list_ptr 
 *  
 * m: Pointer to the list head 
 * 
 * n: Pointer to the list tail 
 *  
 * return value: Returns pointer to max node
 */
ListNode* FindMax(List* L, ListNode *m, ListNode *n) 
{ 
    ListNode *i = m -> next; //Visit all tasks from m to n
    ListNode *j = m; //Save position of the largest 

    while (i != NULL) //Stop when end of list reached
    { 
        if (L -> comp_proc(j -> data_ptr, i -> data_ptr) == 1) //i is larger than j
        { 
            j = i; //save current largest position
        } 

        i = i -> next; //Go to next node
    } 

    return j; //Largest Position returned
}

/* 
 * Find min node by its ID
 * Similar to Find_Max from Standish 
 *  
 * L: Pointer to the list_ptr 
 *  
 * m: Pointer to the list head 
 * 
 * n: Pointer to the list tail 
 *  
 * return value: Returns pointer to min node
 */
ListNode* FindMin(List* L, ListNode *m, ListNode *n) 
{ 
    ListNode *i = m -> next; //Visit all tasks from m to n
    ListNode *j = m; //Save position of the smallest 

    while (i != NULL) //Stop when end of list reached
    { 
        if (L -> comp_proc(j -> data_ptr, i -> data_ptr) == -1) //i is smaller than j
        { 
            j = i; //save current smallest position
        } 

        i = i -> next; //Go to next node
    } 

    return j; //Smallest Position returned
}

/* Sorting the list via the selection sort algorithm
 * 
 * L: pointer to list-of-interest.  
 *
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void list_selection_sort(List** L, int sort_order)
{
    ListNode *i; 
    ListNode *j; 
    ListNode *head = (*L) -> head; 

    while (head != NULL)
    {
        i = head -> next; 
        j = head;  
        while (i != NULL) 
        {
            //Descending List, higher ID goes first
            if (sort_order == 1) 
            {
                if ((*L) -> comp_proc(j -> data_ptr, i -> data_ptr) == 1) //i is larger than j
                    { 
                        j = i; //save current largest position
                    }  
            } 
            //Ascending List, lower ID goes first
            else if (sort_order == 2) 
            { 
                if ((*L) -> comp_proc(j -> data_ptr, i -> data_ptr) == -1) //i is smaller than j
                { 
                    j = i; //save current smallest position
                } 
            } 
            i = i -> next;
        }

        swap(head, j); //Use Swap function to swap A & B 
        head = head -> next; //Go to next node
    }

}

/* Sorting the list via the merge sort algorithm
 * 
 * L: pointer to list-of-interest.  
 *
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void list_merge_sort(List** L, int sort_order)
{
    int size = (*L) -> current_list_size; 
    
    //Keep going until list has only one item
    if (size > 1) 
    { 
        //Keep making two lists to hold both halves of list 
        List *left = list_construct((*L) -> comp_proc, (*L) -> data_clean);
        List *right = list_construct((*L) -> comp_proc, (*L) -> data_clean); 

        //Partition the lists into two half-lists
        Partition(L, left, right); 

        //Continue to sort the two halves into smaller halves
        list_merge_sort(&left, sort_order); 
        list_merge_sort(&right, sort_order); 

        //Finally merge the two halves
        Merge(L, left, right, sort_order);
    }

} 

/* Partition list into two smaller halves
 * 
 * L: pointer to list-of-interest.  
 *
 * left: pointer to left half of list
 * 
 * right: pointer to right half of list
 */
void Partition(List **L, List *left, List *right) 
{ 
    int size = (*L) -> current_list_size;
    int mid_size = size / 2; 
    ListNode* mid_N = (*L) -> head; 
    
    //Find the middle of the list
    for (int i = 1; i < (mid_size + (size % 2)); i++) 
    { 
        mid_N = mid_N -> next;
    } 

    //Set up left half (will contain greater half if L has odd nodes)
    left -> head = (*L) -> head; 
    left -> tail = mid_N; 
    left -> current_list_size = mid_size + (size % 2); //Larger half if odd, otherwise equal halves

    //Set up right half (will contain lesser half if L has odd nodes)
    right -> head = mid_N -> next; 
    right -> tail = (*L) -> tail; 
    right -> current_list_size = mid_size; 

    //Cut off connection between two halves, done here to prevent SEGFAULT 
    left -> tail -> next = NULL; 
    right -> head -> prev = NULL; 

    //Free old list_ptr
    free(*L); 
} 

/* Merge two sorted half lists into one list 
 * 
 * L: pointer to list-of-interest.  
 *
 * left: pointer to left half of list
 * 
 * right: pointer to right half of list
 * 
 * sort_order: 1 sort list in descending order 2 sort in ascending order 
 */
void Merge(List **L, List *left, List *right, int sort_order) 
{ 
    int size = (left -> current_list_size) + (right-> current_list_size); 
    int comp; 

    //New list for merged halves
    *L = list_construct(left -> comp_proc, left -> data_clean);

    //Set up comp for comp_proc function based on sort order
    comp = (sort_order == 1) ? -1: 1;

    for (int i = 0; i < size; i++) 
    {
        //Left Half is empty, take from right if its not empty
        if ((left -> current_list_size) == 0 && (right -> current_list_size) > 0)
        { 
            list_insert(*L, list_remove(right, LISTPOS_HEAD), LISTPOS_TAIL);
        } 

        //Right Half is empty, take from left if its not empty
        else if ((right -> current_list_size) == 0 && (left -> current_list_size) > 0)
        { 
            list_insert(*L, list_remove(left, LISTPOS_HEAD), LISTPOS_TAIL);
        } 

        //Both halves are not empty, take from left or right & insert at new list tail
        else 
        { 
            //Compare both list heads and insert at the tail of new list
            if((*L) -> comp_proc(left -> head -> data_ptr, right -> head -> data_ptr) == comp) 
            { 
                //Left was smaller/bigger, insert at end of new list
                list_insert(*L, list_remove(left, LISTPOS_HEAD), LISTPOS_TAIL);
            } 
            else 
            { 
                //Right was smaller/bigger, insert at tend of new list
                list_insert(*L, list_remove(right, LISTPOS_HEAD), LISTPOS_TAIL);
            }
        } 
    } 

    list_destruct(left); 
    list_destruct(right);
}




/* ----- below are the functions  ----- */

/* Obtains a pointer to an element stored in the specified list, at the
 * specified list position
 * 
 * list_ptr: pointer to list-of-interest.  A pointer to an empty list is
 *           obtained from list_construct.
 *
 * pos_index: position of the element to be accessed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LISTPOS_HEAD and LISTPOS_TAIL
 *
 * return value: pointer to element accessed within the specified list.  A
 * value NULL is returned if the pos_index does not correspond to an element in
 * the list.  For example, if the list is empty, NULL is returned.
 */
Data * list_access(List *list_ptr, int pos_index)
{
    //Pointer to Node
    ListNode *L = NULL;
 
    assert(list_ptr != NULL);

    /* debugging function to verify that the structure of the list is valid */
    //list_debug_validate(list_ptr);

    // List is Empty, cannot return location
    if (list_ptr -> head == NULL) 
    { 
        return NULL;
    } 

    //Asking for the head, return 1st node
    else if (pos_index == LISTPOS_HEAD || pos_index == 0) 
    { 
        L = list_ptr -> head;
    } 

    //Asking for the tail, return last node
    else if (pos_index == LISTPOS_TAIL || pos_index == ((list_ptr ->current_list_size) - 1)) 
    { 
        L = list_ptr -> tail;
    }

    //Asking for invalid position, cannot return location
    else if (pos_index < 0 || pos_index >= (list_ptr -> current_list_size)) 
    { 
        return NULL;
    }

    /* we now know pos_index is for an internal element, because L is not assigned & index given is valid */
    /* Loop through the list until find correct position index */ 
    else
    {
        L = list_ptr -> head;
        //Go through linked list until node at pos_index is found
        for (int i = 0; i < pos_index; i++) 
        { 
            //Go to next node 
            L = L -> next;
        }

    }
    
    assert(L != NULL);
    return L->data_ptr;
}

/* Allocates a new, empty list 
 *
 * By convention, the list is initially assumed to be sorted.  The field sorted
 * can only take values LIST_SORTED_ASCENDING LIST_SORTED_DESCENDING or 
 * LIST_UNSORTED
 *
 * Use list_free to remove and deallocate all elements on a list (retaining the
 * list itself).
 *
 * comp_proc = pointer to comparison function
 *
 * Use the standard function free() to deallocate a list which is no longer
 * useful (after freeing its elements).
 */
List * list_construct(int (*fcomp)(const Data *, const Data *),
        void (*dataclean)(Data *))
{
    List *L;
    L = (List *) malloc(sizeof(List));
    L->head = NULL;
    L->tail = NULL;
    L->current_list_size = 0;
    L->list_sorted_state = LIST_SORTED_ASCENDING;
    L->comp_proc = fcomp;
    L->data_clean = dataclean;

    /* the last line of this function must call validate */
    list_debug_validate(L);
    return L;
}

/* Finds an element in a list and returns a pointer to it.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: element against which other elements in the list are compared.
 *           Note: use the comp_proc function pointer found in the List 
 *           header block. 
 *
 * The function returns a pointer to the matching element with lowest index if
 * a match if found.  If a match is not found the return value is NULL.
 *
 * The function also returns the integer position of matching element with the
 *           lowest index.  If a matching element is not found, the position
 *           index that is returned should be -1. 
 *
 * pos_index: used as a return value for the position index of matching element
 *
 *
 */
Data * list_elem_find(List *list_ptr, Data *elem_ptr, int *pos_index)
{
    list_debug_validate(list_ptr);

    //List is empty, can't find anything
    if (list_ptr -> head == NULL) 
    { 
        *pos_index = -1; 
        return NULL;
    } 
    
    ListNode *N = list_ptr -> head; 

    //Go through list to find element
    for (int i = 0; i < (list_ptr -> current_list_size); i++) 
    { 
        //Check if data in current node matches data in elem_ptr 
        //comp_proc returns zero if data is equal
        if ((list_ptr -> comp_proc(N -> data_ptr, elem_ptr)) == 0)
        {
            //Return position and data found
            *pos_index = i; 
            return N -> data_ptr;
        }
        N = N -> next; 
    } 

    //Unable to find matching data
    *pos_index = -1; 
    return NULL;

}

/* Deallocates the contents of the specified list, releasing associated memory
 * resources for other purposes.
 *
 * Free all elements in the list, and the header block.  Use the data_clean
 * function point to free the Data items in the list.
 */
void list_destruct(List *list_ptr)
{
    /* the first line must validate the list */
    list_debug_validate(list_ptr);

    ListNode *N = list_ptr -> head; 
    ListNode *N_Next; //Make sure to not lose next node
    int size = list_ptr -> current_list_size; 

    //Go through list and free all pointers
    for (int i = 0; i < size; i++) 
    { 
        //If there is a next node, save its location
        if (i < size - 1) 
        {
            N_Next = N -> next; 
        } 
        else N_Next = NULL; 
        list_ptr -> data_clean(N->data_ptr); //Use data clean for Data structure
        free(N); 
        N = N_Next;  
    }
    free(list_ptr); 
}

/* Inserts the specified data element into the specified list at the specified
 * position.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * pos_index: numeric position index of the element to be inserted into the 
 *            list.  Index starts at 0 at head of the list, and incremented by 
 *            one until the tail is reached.  The index can also be equal
 *            to LISTPOS_HEAD or LISTPOS_TAIL (these are special negative 
 *            values use to provide a short cut for adding to the head
 *            or tail of the list).
 *
 * If pos_index is greater than the number of elements currently in the list, 
 * the element is simply appended to the end of the list (no additional elements
 * are inserted).
 *
 * Note that use of this function results in the list to be marked as unsorted,
 * even if the element has been inserted in the correct position.  That is, on
 * completion of this subroutine the list_ptr->list_sorted_state must be equal 
 * to LIST_UNSORTED.
 */
void list_insert(List *list_ptr, Data *elem_ptr, int pos_index)
{
       assert(list_ptr != NULL); 

    //Create new list node for given data
    ListNode *new_node = (ListNode *)malloc(sizeof(ListNode)); 
    new_node->data_ptr = elem_ptr; //New node points to data to be inserted

    //List is empty
    if (list_ptr -> head == NULL) 
    { 
        //Set up new node, no other nodes to point to.
        new_node -> prev = NULL; 
        new_node -> next = NULL;
        //Set up head & tail. Only one node, both head and tail will point to new node
        list_ptr -> head = new_node; 
        list_ptr -> tail = new_node; 
    } 
    //Inserting at the head
    else if (pos_index == LISTPOS_HEAD || pos_index == 0) 
    { 
        //Set up new node. 
        new_node -> prev = NULL;
        new_node -> next = list_ptr -> head; //Point to previous head
        //Set up head
        list_ptr -> head -> prev = new_node; //Previous head points to new head
        list_ptr -> head = new_node; 
    } 

    //Inserting at the tail
    else if (pos_index == LISTPOS_TAIL || pos_index >= (list_ptr -> current_list_size)) 
    { 
        //Set up new node. 
        new_node -> prev = list_ptr -> tail; //Point to previous tail
        new_node -> next = NULL;
        //Set up tail
        list_ptr -> tail -> next = new_node; //Previous tail points to new tail
        list_ptr -> tail = new_node;
    } 

    //Position is not valid
    else if (pos_index < 0)  
    { 
        return;
    }

    //Inserting somewhere else
    else
    { 
        //Go through linked list until pos_index is found
        ListNode *N = list_ptr -> head; 
        //Find the node
        for (int i = 0; i <= pos_index; i++) 
        { 
            if (i == pos_index) 
            {
                //Set up new node
                new_node -> prev = N-> prev;
                new_node -> next = N; 
                //Update surrounding nodes
                N -> prev -> next = new_node; //Update node before
                N -> prev = new_node; //Update node after 
            } 
            N = N -> next; 
        } 
        
    }

    //Valid insert, list has been increased
    (list_ptr->current_list_size)++; 

    /* the last two lines of this function must be the following */
    if (list_ptr->list_sorted_state != LIST_UNSORTED)
        list_ptr->list_sorted_state = LIST_UNSORTED;
    //list_debug_validate(list_ptr);
}

/* Inserts the element into the specified sorted list at the proper position,
 * as defined by the comp_proc function pointer found in the header block.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * elem_ptr: pointer to the element to be inserted into list.
 *
 * If you use list_insert_sorted, the list preserves its sorted nature.
 *
 * If you use list_insert, the list will be considered to be unsorted, even
 * if the element has been inserted in the correct position.
 *
 * If the list is not sorted and you call list_insert_sorted, this subroutine
 * should generate a system error and the program should immediately stop.
 *
 * The comparison procedure must accept two arguments (A and B) which are both
 * pointers to elements of type Data.  The comparison procedure returns an
 * integer code which indicates the precedence relationship between the two
 * elements.  The integer code takes on the following values:
 *    1: A should be closer to the list head than B
 *   -1: B should be closer to the list head than A
 *    0: A and B are equal in rank
 * This definition results in the list being in ascending order.  To insert
 * in decending order, change the sign of the value that is returned.
 *
 * Note: if the element to be inserted is equal in rank to an element already
 * in the list, the newly inserted element will be placed after all the
 * elements of equal rank that are already in the list.
 */
void list_insert_sorted(List *list_ptr, Data *elem_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->list_sorted_state != LIST_UNSORTED); //Function cannot be used with unsorted list

    ListNode *N = list_ptr -> head; 
    int sorted_state = list_ptr -> list_sorted_state; 
    int size = list_ptr -> current_list_size;
    int i = 0;

    //List is sorted from smallest to largest
    if (list_order(list_ptr) == 1) 
    { 
        //Will run until next node has larger element, or list has ended 
        while (i < size && (list_ptr -> comp_proc(N -> data_ptr, elem_ptr)) != -1)
        { 
            i++;
            N = N -> next;
        }
    } 

    //List is sorted from largest to smallest
    else if (list_order(list_ptr) == -1) 
    { 
      //Will run until next node has smaller element, or list has ended 
        while (i < size && (list_ptr -> comp_proc(N -> data_ptr, elem_ptr)) != 1)
        { 
            i++;
            N = N -> next;
        }  
    } 

    //Let list_insert handle inserting the new data, but set the sorted state again
    list_insert(list_ptr, elem_ptr, i); 
    list_ptr -> list_sorted_state = sorted_state;

    /* the last line of this function must be the following */
    //list_debug_validate(list_ptr);
}

/* Removes an element from the specified list, at the specified list position,
 * and returns a pointer to the element.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * pos_index: position of the element to be removed.  Index starts at 0 at
 *            head of the list, and incremented by one until the tail is
 *            reached.  Can also specify LISTPOS_HEAD and LISTPOS_TAIL
 *
 * Attempting to remove an element at a position index that is not contained in
 * the list will result in no element being removed, and a NULL pointer will be
 * returned.
 */
Data * list_remove(List *list_ptr, int pos_index)
{
    assert(list_ptr != NULL);
    Data *D = list_access(list_ptr, pos_index); 
    ListNode *N_Prev;  
    ListNode *N_Next; 

    //List is empty, can't remove
    if(list_ptr -> head == NULL) 
    { 
        return NULL;
    } 

    //Removing at the head 
    else if (pos_index == LISTPOS_HEAD || pos_index == 0) 
    { 
        //More nodes in list, store next node
        if (list_ptr -> head -> next != NULL)
        {
            N_Next = list_ptr -> head -> next; 
            free(list_ptr -> head); 
            list_ptr -> head = N_Next; 
            list_ptr -> head -> prev = NULL; //Ensure new head doesn't point to anything
        } 
        //Removing only node, ensure tail is NULL as well
        else 
        { 
            N_Next = NULL; 
            free(list_ptr -> head); 
            list_ptr -> head = N_Next;
            list_ptr -> tail = N_Next;
        } 
    } 

    //Removing at the tail 
    else if (pos_index == LISTPOS_TAIL || pos_index == ((list_ptr -> current_list_size) - 1)) 
    { 
        //More nodes in the list, grab previous node
        if (list_ptr -> tail -> prev != NULL)
        {
            N_Prev = list_ptr -> tail -> prev;  
        } 
        //Removing only node, ensure head is NULL 
        else 
        { 
            N_Prev = NULL; 
            list_ptr -> head = NULL;
        }
        free(list_ptr -> tail); 
        list_ptr -> tail = N_Prev; 
        list_ptr -> tail -> next = NULL; //Ensure new tail is not pointing to anything
    } 

    //Position is not valid, unable to remove
    else if (pos_index < 0 || pos_index >= (list_ptr -> current_list_size))  
    { 
        return NULL;
    }

    //Removing somewhere in middle
    else
    { 
        ListNode *N = list_ptr -> head;
        //Go through linked list until pos_index is found
        for (int i = 0; i <= pos_index; i++) 
        { 
            //Check for index match, assign node pointers if hit.
            if (i == pos_index) 
            { 
                //Update surrounding nodes 
                N -> prev -> next = N -> next; 
                N -> next -> prev = N -> prev;
                free (N); 
                N = NULL;
            } 
            //Go to next node 
            if (N != NULL) 
            { 
                N = N -> next;
            } 
        }
    }

    //Indicate node has been removed
    (list_ptr->current_list_size)--;

    //list_debug_validate(list_ptr);
    return D;
}

/* Reverse the order of the elements in the list.  Also change the 
 * list_sorted_state flag.  This function can only be called on a list
 * that is sorted.
 *
 * list_ptr: pointer to list-of-interest.  
 */
void list_reverse(List *list_ptr)
{
    //Only runs for sorted lists
    assert(list_order(list_ptr) != 0); 
    
    int size = list_ptr -> current_list_size;

    //If list is bigger than 1, will go through each node to reverse
    if (size > 1) 
    {
        ListNode* N = list_ptr -> tail; 
        ListNode* N_temp;
        //Flip head and tail
        list_ptr -> tail = list_ptr -> head; 
        list_ptr -> head = N;
        //Start at end of list, and update pointers
        for (int i = 0; i < size; i++) 
        { 
            N_temp = N -> next;
            N -> next = N -> prev; 
            N -> prev = N_temp;
            N = N -> next; //Go to previous node
        }  
    }

    //Flip sorted state
    if (list_ptr -> list_sorted_state == LIST_SORTED_ASCENDING) 
    { 
        list_ptr -> list_sorted_state = LIST_SORTED_DESCENDING;
    } 
    else 
    { 
        list_ptr -> list_sorted_state = LIST_SORTED_ASCENDING;
    }
    
    // after the list is reversed verify it is valid.
    list_debug_validate(list_ptr);
}

/* Obtains the length of the specified list, that is, the number of elements
 * that the list contains.
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns an integer equal to the number of elements stored in the list.  An
 * empty list has a size of zero.
 */
int list_size(List *list_ptr)
{
    assert(list_ptr != NULL);
    assert(list_ptr->current_list_size >= 0);
    return list_ptr->current_list_size;
}

/* Obtains the sort status and order of the specified list. 
 *
 * list_ptr: pointer to list-of-interest.  
 *
 * Returns 
 *    1: the list is sorted in ascending order
 *   -1: descending order
 *    0: the list is not sorted but a queue
 */
int list_order(List *list_ptr)
{
    assert(list_ptr != NULL);
    if (list_ptr->list_sorted_state == LIST_SORTED_ASCENDING)
        return 1;
    else if (list_ptr->list_sorted_state == LIST_SORTED_DESCENDING)
        return -1;
    else if (list_ptr->list_sorted_state == LIST_UNSORTED)
        return 0;
    else 
        exit(5);  // this should not happen
}


/* This function verifies that the pointers for the two-way linked list are
 * valid, and that the list size matches the number of items in the list.
 *
 * If the linked list is sorted it also checks that the elements in the list
 * appear in the proper order.
 *
 * The function produces no output if the two-way linked list is correct.  It
 * causes the program to terminate and print a line beginning with "Assertion
 * failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void list_debug_validate(List *L)
{
#ifdef VALIDATE
    ListNode *N;
    int count = 0;
    assert(L != NULL);
    if (L->head == NULL)
        assert(L->tail == NULL && L->current_list_size == 0);
    if (L->tail == NULL)
        assert(L->head == NULL && L->current_list_size == 0);
    if (L->current_list_size == 0)
        assert(L->head == NULL && L->tail == NULL);
    assert(L->list_sorted_state == LIST_SORTED_ASCENDING 
            || L->list_sorted_state == LIST_SORTED_DESCENDING
            || L->list_sorted_state == LIST_UNSORTED);

    if (L->current_list_size == 1) {
        assert(L->head == L->tail && L->head != NULL);
        assert(L->head->next == NULL && L->head->prev == NULL);
        assert(L->head->data_ptr != NULL);
    }
    if (L->head == L->tail && L->head != NULL)
        assert(L->current_list_size == 1);
    if (L->current_list_size > 1) {
        assert(L->head != L->tail && L->head != NULL && L->tail != NULL);
        N = L->head;
        assert(N->prev == NULL);
        while (N != NULL) {
            assert(N->data_ptr != NULL);
            if (N->next != NULL)
                assert(N->next->prev == N);
            else
                assert(N == L->tail);
            count++;
            N = N->next;
        }
        assert(count == L->current_list_size);
    }
    if (L->list_sorted_state != LIST_UNSORTED && L->head != NULL) {
        N = L->head;
        int comp_val = -1 * list_order(L);
        while (N->next != NULL) {
            assert((L->comp_proc)(N->data_ptr, N->next->data_ptr) != comp_val);
            N = N->next;
        }
    }
#endif
}

