/* task_support.c        
 * Joshua Silva        
 * jysilva
 * ECE 2230 Spring 2024
 * MP3
 *
 * Purpose: To be called from another program to modify data/tasks. Maintain list privacy by calling 
 * other programs, like list.c, to manage list
 *
 * Assumptions: Writer of another program does not need to know the specifics of how the data is 
 * altered, but knows which functions to call to edit data. Writer of this program does not need to 
 * know the specifics of how the list is altered, but knows which functions to call to edit lists. 
 *
 * Bugs: None that I know of. 
 *
 * You can modify the interface for any of the task_ functions if you like
 * But you must clearly document your changes.
 *
 * (You CANNOT modify any of the details of the list.h interface, or use any 
 *  of the private variables outside of list.c.)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h> 
#include <time.h>

#include "datatypes.h"
#include "list.h"
#include "task_support.h"

#define FALSE 0
#define TRUE  1

// private functions for use in task_support.c only
void fill_task_record(Task *new);          // collect input
int* getDetermineArgs(int* num_args);        // get input for "DETERMINE" cmd
char* state2str(enum state state);           // Converts enum to ASCII
void print_task_rec_short(Task *rec);      // print one record
void print_task_rec_long(Task *rec) ;      // print one record 
Data* task_lookup_id_assist (List* list_ptr, int* index, int id); //Finds data & location based on task_id 
Task* task_update_assist (List* list_ptr, int state, int id); //Update task based on given task_id & new state 
Task* task_schedule_assist(List* list_ptr, int priority, int id); //Schedule task based on given id & priority 
Task* task_lookup_first_priority(List* list_ptr, int priority); //Find task by given priority 
int task_determine_runnable(List* list_ptr, int nargs, int*args); //Find first runnable task 
List* task_remove_finished_assist(List* list_ptr, List* rm); //Remove finished tasks from list


/* 
 * Adds a new task to the end of MP3 list
 * Similar to task_list_add_tail, except user will be giving a task id & nothing else
 * 
 * list_ptr: Pointer to the list 
 *  
 * id: Task ID that user will be assigning to new task 
 *  
 * return value: None
 */
void task_list_add_tail_mp3(List * list_ptr, int id)
{ 
    //Allocate memory, initialize all values to zero
    Data *rec_ptr = (Data*) calloc(1, sizeof(Data)); 
    
    //Assign data, function only cares about task_id
    rec_ptr -> task_id = id; 
    
    //Add to tail, list is unsorted so directly use list_insert
    list_insert(list_ptr, rec_ptr, LISTPOS_TAIL);
}

/* Function to sort a list using a given method and direction 
 * 
 * list_ptr: pointer to list-of-interest
 * 
 * sort_type: Which sort type to use
 * 
 * sort_order: Order to sort list in
 */
void task_list_sort(List ** list_ptr, int sort_type, int sort_order)
{
    clock_t start, end;
    double elapse_time;  // time in milliseconds
    int initial_size = list_size(*list_ptr); //Later use in testing list integrity
    start = clock(); //Start stopwatch for sort
    
    list_sort(list_ptr, sort_type, sort_order); //Use list.c to sort
    
    end = clock(); //Sort over, end stopwatch
    elapse_time =  1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC; //conversion into ms
    assert(list_size(*list_ptr) == initial_size); //Check list integrity
    printf("Sorting: n = %d time = %f type = %d order = %d\n", initial_size, elapse_time, sort_type, sort_order);

}


/* task_compare is required by the list ADT for sorted lists. 
 *
 * This function returns 
 *     1 if rec_a should be closer to the head than rec_b,
 *    -1 if rec_b is to be considered closer to the head, and 
 *     0 if the records are equal.
 *
 * For the task data we want to sort from lowest task ID up, so
 * closer to the front means a smaller task ID.
 *
 * The function expects pointers to two record structures, and it is an error
 * if either is NULL
 *
 * THIS FUNCTION SHOULD NOT BE CHANGED
 */
int task_compare(const Task *record_a, const Task *record_b)
{
    assert(record_a != NULL && record_b !=NULL);
    if (record_a->task_id < record_b->task_id)
        return 1;
    else if (record_a->task_id > record_b->task_id)
        return -1;
    else
        return 0;

}

/* Function to pass into List ADT during construction to free the memory 
 * associate with a task record.  This hides the details of how the record
 * is defined from the List ADT.
 */
void task_rec_cleanup(Task *rec)
{
    if (rec != NULL) {
        free(rec->args);
        free(rec);
    }
}



/* This creates a list and it can be either a sorted or unsorted list
 *
 * This function is provided to you as an example of how to use 
 * function pointers.  
 */
List * task_list_create(void)
{
    return list_construct(task_compare, task_rec_cleanup);
}

/* this function frees the memory for either a sorted or unsorted list.
 */
void task_list_cleanup(List * list_ptr)
{
    list_destruct(list_ptr);
}


/* print one of the task record lists
 *
 * inputs: 
 *    list_ptr: a List * pointer to either sorted or unsorted list
 *
 *    type_of_list: a charter string that must be either "List" or "Queue"
 *
 * output: prints the list in the format required for grading.
 *         Do not change any of these lines (i.e. printfs && print_task_rec_short()
 */
void task_list_print(List * list_ptr, char *Listype)
{
    assert(strcmp(Listype, "List")==0 || strcmp(Listype, "Queue")==0);

    int i, num_in_list;

    num_in_list = list_size(list_ptr); //Set size of list
    Task *rec_ptr;
    
    
    if (num_in_list == 0) {
        printf("%s empty\n", Listype);
    } else {
        printf("%s has %d record%s\n", Listype, num_in_list,
                num_in_list==1 ? "." : "s");

        for (i = 0; i < num_in_list; i++) {
            printf("%4d: ", i);
            rec_ptr = list_access(list_ptr, i); //Grab record at pos i
            print_task_rec_short(rec_ptr);
        }
    }
    rec_ptr = NULL;
}


/* the function takes a pointer to each list and prints the
 * number of items in each list
 */
void task_list_stats(List * sorted, List * unsorted, List* mp3)
{
    // Get the number in list and size of the list
    int num_in_sorted_list = list_size(sorted); //0;
    int num_in_unsorted_list = list_size(unsorted); //0;
    int order_of_list = list_order(sorted); 
    int num_in_mp3_sorted_list = list_size(mp3);
    char* sort_order_mp3; 

    switch (list_order(mp3)) {
        case 1:
            sort_order_mp3 = "Ascending";
            break;
        case -1:
            sort_order_mp3 = "Descending";
            break;
        default:
            sort_order_mp3 = "Unsorted";
    }
    printf("[MP3] Number records: %d, Order: %s\n", num_in_mp3_sorted_list, sort_order_mp3);
    
    printf("Number records: %d, Order: %s\n", num_in_sorted_list, 
            order_of_list == 1 ? "Ascending" : "Descending");
    printf("Number records in queue: %d\n", num_in_unsorted_list);
}








/* This function adds an task to the list.  
 *
 * Otherwise, if the list is full the task is rejected.
 */
void task_list_add(List * list_ptr, int max_list_size)
{
    Task *rec_ptr = (Task*) malloc(sizeof(Task)); 
    fill_task_record(rec_ptr);

    int added_return = -2;
    int size = list_size(list_ptr);

    //List is full, cannot add
    if (size >= max_list_size) 
    { 
        added_return = -1;
    }
    //List is not full, can add
    else 
    { 
        //List is sorted, run list_insert_sorted to add
        if (list_order(list_ptr) != 0) 
        {
            list_insert_sorted(list_ptr, rec_ptr); 
        } 
        //List is unsorted, directly run list_insert to add
        else 
        {
            list_insert(list_ptr, rec_ptr, max_list_size); 
        } 
        added_return = 1; 
    }

    if (added_return == 1) {
        printf("Inserted: %d\n", rec_ptr->task_id);
    } else if (added_return == -1) {
        printf("Rejected: %d\n", rec_ptr->task_id);
    } else {
        printf("Error with return value! Fix your code.\n");
    }

    //clean up
    rec_ptr = NULL;
}



/* This function prints the first task with the matching id in the
 * list.  
*/
void task_list_lookup_id(List * list_ptr, int id)
{
    int index = -1; 

    //Call function to find task id & its index
    Task *rec_ptr = task_lookup_id_assist(list_ptr, &index, id);
    
    if (rec_ptr == NULL) {
        printf("Did not find: %d\n", id);
    } else {
        printf("Found: %d at index %d\n", id, index);
        print_task_rec_long(rec_ptr);
    }
}



/* This function removes the first task from the list with the matching
 * task id
 */
void task_list_remove_id(List * list_ptr, int id)
{
    Task *rec_ptr = NULL; 
    int index = -1;
    //Grab data to be removed 
    rec_ptr = task_lookup_id_assist(list_ptr, &index, id);
    rec_ptr = list_remove(list_ptr, index); 

    if (rec_ptr == NULL) { 
        printf("Did not remove: %d\n", id);
    } else {
        //Print Data to be removed
        printf("Removed: %d\n", id);
        print_task_rec_long(rec_ptr);
        //Bye bye data 
        task_rec_cleanup(rec_ptr);
    }
    rec_ptr = NULL;
}


/* This function updates the state of the task identified by id if possible */
void task_list_update(List * list_ptr, int id, int state)
{
    //Call function to find task by id, then update if valid new state
    Task* rec_ptr = task_update_assist(list_ptr, state, id); 

    if (rec_ptr != NULL){
        
        // displays the current state for the task even if not changed 
        printf("Task %d has state of %s\n", id, state2str(rec_ptr->state));
    
    }
    rec_ptr = NULL;

} 


/* This function schedules based on id and priority */
void task_list_schedule(List * list_ptr, int id, int priority)
{
    Task* rec_ptr = NULL;

    //Calls task_schedule_assist to schedule task by its id or priority
    rec_ptr = task_schedule_assist(list_ptr, priority, id);
    
    if (rec_ptr != NULL)
    {
        printf("Now running task:\n");
        print_task_rec_short(rec_ptr);
        
    }
    else {
        printf("Did not find task to schedule...\n");
    }

}

/* This function determins if a task is runnable */
void task_list_determine(List * list_ptr)
{
    int num_args;
    int idx = -1;
    int* args = getDetermineArgs(&num_args);
    
    Task* task = NULL;

    //Find index of 1st runnable task
    idx = task_determine_runnable(list_ptr, num_args, args);

    if (idx != -1) {
        task = list_access(list_ptr, idx);
        printf("Task %d is runable.\n", task->task_id);
    }
    else {
        printf("No runable tasks.\n");
    }

    free(args);
}

/* Removes finished tasks */
void task_list_remove_finished(List * list_ptr)
{
    Task* task = NULL;
    List* rm = task_list_create(); //Construct list 
    int i;

    //Remove tasks
    rm = task_remove_finished_assist(list_ptr, rm); 

    int num_in_list = list_size(rm);
    printf("Removing %d finshed tasks\n", num_in_list);
    for (i = 0; i<num_in_list; i++) { 
        task = list_access(rm, i);
        print_task_rec_short(task);
    }
    
    
    //Cleanup 
    task = NULL;
    task_list_cleanup(rm);

}

/* This function reverses the order of the items in the list */
void task_list_reverse(List * list_ptr)
{
    list_reverse(list_ptr);
    int order_of_list = list_order(list_ptr);
    printf("List reversed, new order: %s\n",
            order_of_list == 1 ? "Ascending" : "Descending");
}





/****************************************************************************/
/****************************************************************************/

/*       Queue Functions                                                    */

/****************************************************************************/
/****************************************************************************/


/* This functions adds an task record to the tail of a list.  The list is
 * unsorted, and it does not have any limits on the maximum size of the list.
 */
void task_list_add_tail(List * list_ptr)
{
    Data *rec_ptr = (Data*) malloc(sizeof(Data)); //Make space for new data
    fill_task_record(rec_ptr); //Collect input
    
    //Add to tail, list is unsorted so directly use list_insert
    list_insert(list_ptr, rec_ptr, LISTPOS_TAIL);

    printf("Appended %d onto queue\n", rec_ptr->task_id);
}

/* This function removes the task record at the head of the list.  The
 * list is unsorted, and there is no limit on the maximum size of the list.
 *
 * The task should be recycled so there are no memory leaks.
 */
void task_list_remove_head(List * list_ptr)
{
    //Remove head using list_remove 
    Task *task_ptr = list_remove(list_ptr, LISTPOS_HEAD);

    if (task_ptr != NULL) 
        printf("Deleted head with task id and priority: %d %d\n", 
                task_ptr->task_id, task_ptr->priority);  
    else
        printf("Queue empty, did not remove\n"); 
    
    /* Bye bye task :( */ 
    task_rec_cleanup(task_ptr);
}

/* This function prints the task at the head of the queue.
 * Print all fields of the task record. 
 */
void task_list_print_head(List * list_ptr)
{
    Task *rec_ptr = list_access(list_ptr, LISTPOS_HEAD); //Grab element at the head

    if (rec_ptr == NULL) {
        printf("Queue is empty\n");
    } else {
        /* print items in structure */
        printf("Found at head of queue:\n");
        print_task_rec_long(rec_ptr);
    } 

    rec_ptr = NULL;
}




/****************************************************************************/
/****************************************************************************/

/*       Private functions for the task list you declare                    */

/****************************************************************************/
/****************************************************************************/


/* 
 * Finds task_id & index
 *
 * list_ptr: Pointer to the list 
 * 
 * index: pointer to an index variable in task_lookup_id
 *  
 * id: task_id to be found in list
 * 
 * return value: Returns "element," a pointer to the data at given task_id. Does not return 
 * index where found, but given pointer to index is altered in list_elem_find
 */
Data* task_lookup_id_assist (List* list_ptr, int *index, int id) 
{ 
    Data* element; 
    int size = list_size(list_ptr);
    
    //Go through whole list looking for matching task id
    for (int i = 0; i < size; i++) 
    { 
        //Find matching ID in list, then find its location
        element = list_access(list_ptr, i); 
        if (element->task_id == id) 
        { 
            //ID was found, now get its location
            element = list_elem_find(list_ptr, element, index); 
            return element;
        }
    } 
    return NULL;
}

/* 
 * Finds task by task_id, then updates state if valid new state 
 * Similar to task_list_set_state from MP1, mainly the Switch statement 
 *  
 * list_ptr: Pointer to the list 
 *  
 * id: task_id to be found in list 
 *  
 * state: State that user wants to update to 
 *  
 * return value: Returns back task that matches task_id, with an altered state if valid transition
 */
Task* task_update_assist (List* list_ptr, int state, int id) 
{ 
    int i = -1; //Necessary for lookup_assist, won't need location but will have it
    Task* task = task_lookup_id_assist(list_ptr, &i, id); //Find relevant task  

    //If task was found, proceed to updating
    if (task != NULL) 
    { 
        /*
         *Compares current state to new state, will set new state if transition is valid 
         *Switch Statement is from MP1 task_list_set_state 
         */
        int curr_state = task -> state; 
        switch(curr_state) 
        { 
            //Queued will transition if next state is "RUNNING"
            case QUEUED: 
                if (state == RUNNING) 
                { 
                    (task -> state) = state;
                }
                break; 

            //Running will transition if next state is "BLOCKED" or "FINISHED" 
            case RUNNING:
                if (state == BLOCKED || state == FINISHED) 
                { 
                    (task -> state) = state;
                }
                break; 

            //Blocked will transition if next state is "RUNNING"
            case BLOCKED: 
                if (state == RUNNING) 
                { 
                    (task -> state) = state;
                }
                break; 

            //Finished cannot transition to a new state
            case FINISHED:
                break; 
        }
    }

    return task;
} 

/* 
 * Finds task by task_id or priority, then schedules task if valid
 * VERY similar to task_list_schedule from MP1 
 *  
 * list_ptr: Pointer to the list  
 * 
 * priority: priority to be found in list, necessary if id won't match
 *  
 * id: task_id to be found in list
 * 
 * return value: Returns back task found by id or priority. Will "schedule" or set found task to 
 * run if valid transition.
 */
Task* task_schedule_assist(List* list_ptr, int priority, int id)
{
    Task* task = NULL; 
    int index = -1; 
    int curr_state;
    
    //Call lookup assist function to find task that matches given ID
    task = task_lookup_id_assist(list_ptr, &index, id);
    
    //ID Found, schedule task if valid
    if(task != NULL) 
    { 
        curr_state = task -> state; 
        //Set state to RUNNING if state is valid
        switch (curr_state) 
        { 
            case RUNNING:
                return NULL; 
            
            case FINISHED: 
                return NULL;

            default: 
                task -> state = RUNNING; 
                return task;
        }
    } 
    
    //Task ID not found, try finding priority and schedule
    else 
    { 
        //Call "lookup_first_priority" to find task that matches priority
        task = task_lookup_first_priority(list_ptr, priority);
        //Priority found
        if(task != NULL) 
        { 
            curr_state = task -> state;
            //Set state to RUNNING if state is valid
            switch (curr_state) 
            { 
                case RUNNING:
                    return NULL; 
            
                case FINISHED: 
                    return NULL;

                default: 
                    task -> state = RUNNING; 
                    return task;
            } 
        }
    } 

    return task;
} 

/* 
 * Finds task by priority
 * VERY similar to task_list_lookup_first_priority from MP1, returns task instead 
 *  
 * list_ptr: Pointer to the list 
 *  
 * priority: priority to be found in list
 * 
 * return value: Returns 1st task that matches priority
 */
Task* task_lookup_first_priority(List* list_ptr, int priority)
{
    assert(list_ptr != NULL); 
    int size = list_size(list_ptr); 

    //Go through all nodes and try to find a match for priority
    for(int i = 0; i < size; i++)
    { 
        Task* task = list_access(list_ptr, i);
        if((task -> priority) == priority) 
        { 
            return task; //return task
        } 
    } 

    return NULL; //failed to find priority
} 

/* 
 * Finds index of 1st runnable task
 * VERY similar to task_list_determine_runnable from MP1 
 *  
 * list_ptr: Pointer to the list 
 * 
 * nargs: Number of arguments in a task
 *  
 * args: Array of arguments in a task
 * 
 * return value: Returns the index of the first runnable task
 */
int task_determine_runnable(List* list_ptr, int nargs, int*args)
{
    int size = list_size(list_ptr); 

    //Find first task, if any, that is runnable
    for (int i = 0; i < size; i++) 
    { 
        Task* task = list_access(list_ptr, i);
        int curr_state = task -> state; 
        int curr_nargs = task -> nargs;
        
        //Checks if state is valid
        if (curr_state == QUEUED || curr_state == BLOCKED) 
        { 
            //Check if nargs is valid
            if (curr_nargs == nargs) 
            { 
                //Check if args is valid 
                for (int j = 0; j < nargs; j++) 
                { 
                    //Will pass if args is invalid
                    if ((task -> args[j]) != args[j]) 
                    { 
                        return -1;
                    }
                }

                //Passed all tests, return index of task
                return i;
            }
        }
    }
    
    //First Runnable Task is not runnable
    return -1;
} 

/* 
 * Remove all finished tasks
 * Similar to task_list_remove_finished from MP1 
 *  
 * list_ptr: Pointer to the list 
 *  
 * rm: Pointer to the list storing finished tasks 
 *  
 * return value: Returns rm after it has grabbed all finished tasks from list_ptr 
 */
List* task_remove_finished_assist(List* list_ptr, List* rm)
{
    //Make sure function is handed existing lists
    assert(list_ptr != NULL); 
    assert(rm != NULL);
    
    //Go through original list, grab finished tasks and insert them into rm, then remove from original 
    //Use list_size directly to prevent segfaults, will change size of original list after removal
    for(int i = 0; i < list_size(list_ptr); i++)
    { 
        Task* task = list_access(list_ptr, i);
        if((task -> state) == FINISHED) 
        { 
            //Data grabbed & removed from list_ptr then inserted into rm
            list_insert(rm, list_remove(list_ptr, i), i); 
            i--; //Prevent skipping after removal, nodes will move left 1
        } 
    } 

   return rm; 
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/*       Functions from here to end of file should not be changed           */

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/* If a string ends with an end-of-line \n, remove it. */
void chomp(char *str) 
{
    int lastchar = strlen(str) - 1;
    if (lastchar >= 0 && str[lastchar] == '\n') {
        str[lastchar] = '\0';
    }
}

/* Prompts user for task record input.
 *
 * The input to the function assumes that the structure has already been
 * created.  The contents of the structure are filled in.
 *
 * There is no output.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void fill_task_record(Task *new)
{
    static int task_id = 0;
    char line[MAXLINE];
    assert(new != NULL);

    new->task_id = task_id;
    task_id++;
    
    printf("Priority:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%u", &new->priority);

    //printf("State:");
    //fgets(line, MAXLINE, stdin);
    //sscanf(line, "%u", &new->state);
    new->state = QUEUED;
    
    printf("Wallclocktime:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%le", &new->wallclocktime);
    

    printf("Number of Args:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%u", &new->nargs);

    assert(new->nargs >= 0 && "# args negative\n");
    new->args = (int*) malloc(sizeof(int) * new->nargs);
    int i=0;
    for (i=0; i<new->nargs; i++) {
        printf("Arg %d:", i);
        fgets(line, MAXLINE, stdin);
        sscanf(line, "%d", &new->args[i]);
    }

    printf("\n");
}

/* Gets values (from user) to use as arguments to determine if a task is runnable
 *
 * Input/Output argument is the number of args the user provides
 * Returns pointer to array of args
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
int* getDetermineArgs(int* num_args)
{
    char line[MAXLINE];
    int i;

    printf("Number of Args:");
    fgets(line, MAXLINE, stdin);
    sscanf(line, "%u", num_args);

    assert(*num_args >= 0 && "# args negative\n");
    int* args = (int*) malloc(sizeof(int) * (*num_args));
    for (i=0; i<*num_args; i++) {
        printf("Arg %d:", i);
        fgets(line, MAXLINE, stdin);
        sscanf(line, "%d", &args[i]);
    }

    return args;
}

/* Converts state enum value to a printable string
 *
 * Input state enum value
 *
 * DO NOT CHANGE THIS FUNCTION
 */
char* state2str(enum state state)
{
    if (state == QUEUED)
        return "QUEUED";
    else if (state == RUNNING)
        return "RUNNING";
    else if (state == BLOCKED)
        return "BLOCKED";
    else if (state == FINISHED)
        return "FINISHED";
    else
        return "UNKNOWN";
}




/* print the information for a particular task record in short form
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void print_task_rec_short(Task *rec)
{
    assert(rec != NULL);
    printf("Task ID: %d priority = %d state = %s\n",
            rec->task_id, rec->priority, state2str(rec->state));
}

/* Long form to print a particular task record 
 *
 * Input is a pointer to a record, and no entries are changed.
 *
 * DO NOT CHANGE THIS FUNCTION!
 */
void print_task_rec_long(Task *rec)
{
    assert(rec != NULL);
    int i;
    
    printf("Task ID: %d\n", rec->task_id);
    printf("     priority = %d\n", rec->priority);
    printf("     state     = %s\n", state2str(rec->state));
    printf("     time = %e\n", rec->wallclocktime);
    printf("     nargs = %d\n", rec->nargs);
    printf("     args = {");
    for (i=0; i<rec->nargs; i++)
        printf(" %d, ", rec->args[i]);
    printf("}\n");
}


/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
