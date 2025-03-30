/* mem.c
 * Joshua Silva
 * jysilva
 * ECE 2230 Spring 2024
 * MP4
 *
 * Propose: To dynamically allocate memory and free allocated memory of the heap. Provide statistics of the heap as well.
 *
 * Assumptions: User knows the size of a page and amount of bytes in a chunk. User knows what to input functions with and 
 * output of each function. 
 *
 * Bugs: When user requests 1 less chunk that is available in a free block, entire free block is returned due to header block 
 * decided it was safer to give user one more chunk that was requested 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>

#include "mem.h"

/* Chunk definition (do not modify).
 * sizeof(Chunk) must equal SIZEOF_CHUNK_T in mem.h.
 */
typedef struct chunk_tag {
    struct chunk_tag *next;  /* pointer to the next free block */
    unsigned long size;               /* size of memory block in units of
                                sizeof(Chunk) */
} Chunk;


typedef struct heap_stats {

    /* do not include the dummy block when computing the next 4 */
    int numItems;    /* number of chunks in the free list   */
    int min;         /* size of the smallest chunk, in bytes */
    int max;         /* size of the largest chunk, in bytes  */
    int average;     /* average size of the chunks, in bytes */
    int totalBytes;  /* total size of all chunks, in bytes   */

    /* the following two fields are updated in morecore() */
    int numSbrkCalls;  /* number of successful calls to sbrk()  */
    int numPages;      /* number of pages allocated with sbrk() */
} HeapStats;

/* global variables exported via mem.h */
int SearchPolicy = FIRST_FIT;
int SearchLoc = HEAD_FIRST;
int Coalescing = FALSE;

/* global variables restricted to mem.c only */
static Chunk Dummy = {
    &Dummy,  /* next */
    0        /* size */
};
static Chunk * Rover = &Dummy;
static HeapStats stats;  /* initialized by the O/S to all 0s */


/* prototypes for functions private to mem.c */
void mem_validate(void);
Chunk *morecore(int);

/* function to request 1 or more pages from the operating system.
 *
 * new_bytes must be the number of bytes that are being requested from
 *           the OS with the sbrk command.  It must be an integer 
 *           multiple of the PAGESIZE
 *
 * returns a pointer to the new memory location.  If the request for
 * new memory fails this function simply returns NULL, and assumes some
 * calling function will handle the error condition.  Since the error
 * condition is catastrophic, nothing can be done but to terminate 
 * the program.
 */
Chunk *morecore(int new_bytes) 
{
    char *cp;
    Chunk *new_p;

    /* assert preconditions */
    assert(new_bytes % PAGESIZE == 0 && new_bytes > 0);
    assert(PAGESIZE % sizeof(Chunk) == 0);

    /* allocate memory using sbrk() */
    cp = sbrk(new_bytes);
    if (cp == (char *) -1)  /* no space available */
        return NULL;
    new_p = (Chunk *) cp;
    
    /* update statistics */
    stats.numSbrkCalls++;
    stats.numPages += new_bytes/PAGESIZE;

    return new_p;
}

/* Free previously allocated memory
 *  
 * return_ptr: Pointer to allocated memory 
 */
void Mem_free(void *return_ptr)
{
    //Do not run if data does not exist
    assert (return_ptr != NULL); 
    assert (Coalescing == TRUE || Coalescing == FALSE);

    Chunk* return_memory = return_ptr; //set return_ptr to chunk data type for pointer arithmetic
    Chunk* allocate_memory = return_memory - 1; //Grab header chunk of allocated memory
    //Chunk* allocate_rover = NULL; //Not in use for now
    //Chunk* free_rover = &Dummy; //Start roaming at dummy block

    //Free list will not coalesce, shove returning memory at beginning of free list
    if(Coalescing == FALSE) 
    { 
        allocate_memory -> next = Dummy.next;
        Dummy.next = allocate_memory;
    }

    /*
    //Free list will coalesce
    else 
    {
        //If allocated memory no longer points to a free block, find next free block
        while (allocate_rover -> size == 0 && allocate_rover != allocate_memory && allocate_rover != &Dummy) 
        { 
            allocate_rover = allocate_rover -> next; 
        } 
        //printf("Chunks are %d\n", allocate_rover -> size);
        
        //Find free block before our new free block
        while (free_rover -> next != allocate_rover -> next && free_rover == &Dummy) 
        { 
            free_rover = free_rover -> next;
        } 
        //printf("Chunks are %d\n", free_rover -> size);
        
        free_rover -> next -> next = allocate_memory; 
        allocate_memory -> next = allocate_rover; 
    }
    */
}

/* To dynamically allocate memory and return a pointer to this allocated memory to user
 *  
 * nbytes: Amount of bytes a user wants to allocate 
 * 
 * return value: (allocate_memory + 1) - Pointer to 1st chunk of allocated memory
 */
void *Mem_alloc(int nbytes) {
    
    /* assert preconditions */
    assert(nbytes > 0); 
    assert(SearchLoc == HEAD_FIRST || SearchLoc == ROVER_FIRST); 
    assert(SearchPolicy == BEST_FIT || SearchPolicy == FIRST_FIT || SearchPolicy == WORST_FIT); 

    Chunk* start = (SearchLoc == HEAD_FIRST) ? &Dummy : Rover; //Where we start roaming on circular list 
    Chunk* rov = start -> next; //Start roaming at 1st block in free list
    Chunk* memory_found = NULL; //Location of free block that we will allocate to
    
    //Convert requested space to a multiple of chunks, account for needed header block
    int nunits = ((nbytes % SIZEOF_CHUNK_T) == 0) ? (nbytes / SIZEOF_CHUNK_T) : (nbytes / SIZEOF_CHUNK_T) + 1; 

    /* Go through circular free list and find appropriate memory */
    
    //Best Fit allocation (Smallest free block)
    if(SearchPolicy == BEST_FIT) 
    { 
        //Find smallest memory chunk that can fit nunits
        while (rov != start) 
        {
            //Find memory chunk that can fit nunits
            if ((rov -> size) >= nunits) 
            { 
                //Found first chunk that can fit nunits
                if(memory_found == NULL) 
                { 
                    memory_found = rov;
                }

                //Set memory found to smallest chunk found
                else if((rov -> size) < (memory_found -> size))
                {
                    memory_found = rov; 
                }
            } 
            rov = rov -> next; //Go to next free block
        } 
    } 

    //First Fit allocation (First viable free block)
    else if(SearchPolicy == FIRST_FIT) 
    { 
        //Find first memory chunk that can fit nunits
        while (rov != start && memory_found == NULL) 
        {
            //Find memory chunk that can fit nunits
            if ((rov -> size) >= nunits) 
            { 
                memory_found = rov; 
            } 
            rov = rov -> next;
        } 
    } 

    //Worst Fit allocation (Largest Free Block)
    else if(SearchPolicy == WORST_FIT)
    { 
        //Find largest memory chunk that can fit nunits
        while (rov != start) 
        {
            //Find memory chunk that can fit nunits
            if ((rov -> size) >= nunits) 
            { 
                //Found first chunk that can fit nunits
                if(memory_found == NULL) 
                { 
                    memory_found = rov;
                }

                //Set memory found to largest chunk found
                else if((rov -> size) > (memory_found -> size))
                {
                    memory_found = rov; 
                }
            } 
            rov = rov -> next; //Go to next free block
        }   
    } 

    //Unable to find free block to fit requested space, call morecore
    if (memory_found == NULL) 
    { 
        /* Find amount of pages to request, morecore needs bytes that are multiples of PAGESIZE */ 
        int totalbytes = (nunits + 1) * SIZEOF_CHUNK_T;
        int pages_request = ((totalbytes % PAGESIZE) == 0) ? totalbytes / PAGESIZE : (totalbytes / PAGESIZE) + 1; 
        int bytes_request = pages_request * PAGESIZE; //Convert request to bytes
        memory_found = morecore(bytes_request); //Pointer to new memory block
        
        //Unable to fulfill memory allocation! 
        if (memory_found == NULL) 
        { 
            return NULL;
        } 

        /* Successful request for more memory, put new free memory at beginning of free list */
        memory_found -> next = Dummy.next; //New free block points to former first free block
        Dummy.next = memory_found; //Dummy block now points to new free block
        memory_found -> size = (bytes_request / SIZEOF_CHUNK_T) - 1; //Set new memory size to chunks, account for header chunk
    } 

    Chunk *allocate_memory = NULL; 

    /* Set aside chunks of free block, take rightmost chunks */ 
    
    //Memory chunk has more than enough memory
    if ((memory_found -> size) > (nunits + 1)) 
    {
        allocate_memory = memory_found + (memory_found -> size - nunits); //Location of memory block header
        allocate_memory -> size = nunits; //Set size of memory block
        memory_found -> size -= ((allocate_memory -> size) + 1); //Account for header block & memory chunk taken away in block
        Rover = memory_found -> next; //Set free_rover to next free block
    } 
    
    //exact fit requested or user requests exactly fits when accounting for header block
    else if ((memory_found -> size) == nunits || (memory_found -> size) == nunits + 1)
    { 
        allocate_memory = memory_found; //Location of memory block header
        rov = &Dummy; //Start roaming at beginning of circular list again
        
        //free block before found free block
        while (rov -> next != memory_found) 
        { 
            rov = rov -> next; 
        }

        rov -> next = memory_found -> next; //Skip over free block
        Rover = rov -> next; //Set free_rover to next free block
        memory_found -> next = NULL; //Get rid of next pointer in free block that user will receive
    }

    return (allocate_memory + 1);
}

/* prints stats about the current free list
 *
 * -- number of items in the linked list including dummy item
 * -- min, max, and average size of each item (in bytes)
 * -- total memory in list (in bytes)
 * -- number of calls to sbrk and number of pages requested
 *
 * A message is printed if all the memory is in the free list
 */
void Mem_stats(void)
{
    /* Calculate the latest stats and put them in the stats struct */ 

    Chunk* start = &Dummy; //save first block
    Chunk* rov = start -> next; //Start roaming at 1st block
    
    //Save 1st block variables
    stats.numItems = 0; 
    stats.min = rov -> size; 
    stats.max = rov -> size; 
    stats.totalBytes = 0; 

    //Go through circular free list until reaching header/dummy
    while (rov != start) 
    { 
        //Set new min
        if ((rov -> size) < stats.min) 
        { 
            stats.min = rov -> size; 
        } 

        //Set new max
        if ((rov -> size) > stats.max) 
        { 
            stats.max = rov -> size; 
        } 

        stats.totalBytes += ((rov -> size) + 1); //Gather total (including header chunk)
        stats.numItems++; //Account for additional block found
        rov = rov -> next; //Go to next block
    } 

    //Convert all to bytes
    stats.min *= SIZEOF_CHUNK_T; 
    stats.max *= SIZEOF_CHUNK_T; 
    stats.totalBytes *= SIZEOF_CHUNK_T; 
    stats.average = (stats.numItems > 0) ? (stats.totalBytes / stats.numItems) : 0; 
    
    /* ======= DO NOT MODIFY FROM HERE TO END OF Mem_stats() ======= */
    printf("\n\t\tMP4 Heap Memory Statistics\n"
            "----------------------------------------------------\n\n"
            "\tNumber of blocks in free list: %d\n"
            "\tMin: %d\n"
            "\tMax: %d\n"
            "\tAverage: %d\n"
            "\tTotal bytes in free list: %d\n"
            "\tNumber of sbrk calls: %d\n"
            "\tNumber of requested pages: %d\n"
            "\tHeap status:  %s\n",
            stats.numItems, stats.min, stats.max, stats.average, stats.totalBytes,
            stats.numSbrkCalls, stats.numPages,
            stats.totalBytes == stats.numPages*PAGESIZE ? \
            "all memory is in the heap -- no leaks are possible\n"\
            : "heap is in-use -- leaks are possible\n");
}

/* print table of memory in free list 
 *
 * The print should include the dummy item in the list 
 */
void Mem_print(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    Chunk *p = Rover;
    Chunk *start = p; /* save my starting block */
    do {
        printf("p=%p, size=%ld, end=%p, next=%p %s\n", 
                p, p->size, p + p->size, p->next, p->size!=0?"":"<-- dummy");
        p = p->next;
    } while (p != start); /* only 1 time through the list */
    mem_validate();
}

/* This is an experimental function to attempt to validate the free
 * list when coalescing is used.  It is not clear that these tests
 * will be appropriate for all designs.  If your design utilizes a different
 * approach, that is fine.  You do not need to use this function and you
 * are not required to write your own validate function.
 */
void mem_validate(void)
{
    assert(Rover != NULL && Rover->next != NULL);
    assert(Rover->size >= 0);
    int wrapped = FALSE;
    int found_dummy = FALSE;
    int found_rover = FALSE;
    Chunk *p, *largest, *smallest;

    /* for validate begin at Dummy */
    p = &Dummy;
    do {
        if (p->size == 0) {
            assert(found_dummy == FALSE);
            found_dummy = TRUE;
        } else {
            assert(p->size > 0);
        }
        if (p == Rover) {
            assert(found_rover == FALSE);
            found_rover = TRUE;
        }
        p = p->next;
    } while (p != &Dummy);
    assert(found_dummy == TRUE);
    assert(found_rover == TRUE);

    if (Coalescing) {
        do {
            if (p >= p->next) {
                /* this is not good unless at the one wrap */
                if (wrapped == TRUE) {
                    printf("validate: List is out of order, already found wrap\n");
                    printf("first largest %p, smallest %p\n", largest, smallest);
                    printf("second largest %p, smallest %p\n", p, p->next);
                    assert(0);   /* stop and use gdb */
                } else {
                    wrapped = TRUE;
                    largest = p;
                    smallest = p->next;
                }
            } else {
                assert(p + p->size < p->next);
            }
            p = p->next;
        } while (p != &Dummy);
        assert(wrapped == TRUE);
    }
}


