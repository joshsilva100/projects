/* lab4.c
 * Joshua Silva
 * jysilva
 * ECE 2230 Spring 2024
 * MP4
 *
 * Purpose: Run test drivers for mem.c
 * --------
 *
 * This file contains drivers to test the mem.c package for dynamic memory
 * allocation.
 * 
 * The following parameters can be set on the command line.  If not set,
 * default values are used.  
 *
 * If different options are implemented for the memory package, this provides a
 * simple mechanism to change the options.  
 *
 * -f best|first|worst  search policy to find memory block (first by default)
 * -c                   turn on coalescing (off by default)
 *
 * General options for all test drivers
 * -s 19283  random number generator seed 
 *
 * The Unit test drivers.  
 * 
 * -u 0      Tests one allocation for 16 bytes
 * -u 1      Tests 4 allocations including a new page
 * -u 2      Requests the number of bytes that matches a whole page, and a size that is one unit smaller than a page
 * -u 3      Request more bytes than in one page
 * -u 4      Do frees and allocates together instead of separately allocating first then freeing. Prove our heap can handle both 
 *           despite the order and we can return everything back to the free list
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h> //Included for Performance Evaluation

#include "mem.h"

/* prototypes for functions in this file only */
void getCommandLine(int, char**, int*);//, int*);

int main(int argc, char **argv)
{
    int unit_driver;  /* the unit test number to run */
    long seed = time(NULL);         /* random number seed */
    
    getCommandLine(argc, argv, &unit_driver);//, &seed);
    //printf("Seed: %d\n", seed);
    srand48(seed);
    double random; 
    random = drand48(); 
    if (random < 0.1) 
    { 
        random *= 10; //In case random is too small
    }
    random *= 10; 

    // The major choices: search policy and coalescing option 
    if (SearchPolicy == BEST_FIT) printf("Best-fit search policy");
    else if (SearchPolicy == FIRST_FIT) printf("First-fit search policy");
    else if (SearchPolicy == WORST_FIT) printf("Worst-fit search policy");
    else {
        fprintf(stderr, "Error with undefined search policy\n");
        exit(1);
    }
    if (SearchLoc == HEAD_FIRST) printf(" starting at head");
    else if (SearchLoc == ROVER_FIRST) printf( "starting at rover");
    else {
        fprintf(stderr, "Error with undefined search starting location\n");
        exit(1);

    }
    if (Coalescing == TRUE) printf(" using coalescing\n");
    else if (Coalescing == FALSE) printf(" without coalescing\n");
    else {
        fprintf(stderr, "Error specify coalescing policy\n");
        exit(1);
    }

    if (unit_driver == 0)
    {
        // unit driver 0: basic test with one allocation and free
        printf("\n----- Begin unit driver 0 -----\n");
        char *string;
        const char msg[] = "hello world 15c";
        int len = strlen(msg);
        // add one for null character at end of string 
        string = (char *) Mem_alloc((len+1) * sizeof(char));
        strcpy(string, msg);
        printf("string length=%d\n:%s:\n", len, string);
        printf("\nFree list after first allocation\n");
        Mem_stats();
        Mem_print();
        Mem_free(string);
        printf("\nFree list after first free\n");
        printf("unit driver 0 has returned all memory to free list\n");
        Mem_stats();
        Mem_print();
        string = NULL;
        printf("\n----- End unit driver 0 -----\n");
    }
    else if (unit_driver == 1)
    {
        printf("\n----- Begin unit driver 1 -----\n");
        /* 
         This test makes four allocations from the free list with the goal
         of making the third allocation the correct size so that the free
         list is left empty. The fourth allocation should allocate a new
         page. 
         */
        int unit_size = SIZEOF_CHUNK_T;
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and the size of Chunk is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4;
        int num_bytes_1, num_bytes_2, num_bytes_3;
        int num_bytes_4;

        // allocate 1st pointer to 1/8 of a page
        num_bytes_1 = (units_in_first_page/8 - 1)*unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
         
        // allocate for 2nd pointer to 1/2 of a page
        num_bytes_2 = (units_in_first_page/2 - 1)*unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("second: %d bytes (%d units) p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print(); 

        // allocate remaining memory in free list
        num_bytes_3 = units_in_first_page - num_bytes_1/unit_size 
            - num_bytes_2/unit_size - 3;
        num_bytes_3 *= unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("third: %d bytes (%d units) p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print(); 
        printf("unit driver 1: above Mem_print shows empty free list\n"); 

        // allocate for 4th pointer to 1/4 a page when free list is empty
        num_bytes_4 = (units_in_first_page/4 - 1)*unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("fourth: %d bytes (%d units) p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print(); 

        // next put the memory back into the free list:

        printf("first free of 1/8 a page p=%p \n", p1);
        Mem_free(p1);
        Mem_print();

        printf("second free of 3/8 a page p=%p \n", p3);
        Mem_free(p3);
        Mem_print();

        printf("third free of 1/2 a page p=%p \n", p2);
        Mem_free(p2);
        Mem_print();
        printf("fourth free of 1/4 a page p=%p\n", p4);
        Mem_free(p4);
        printf("unit driver 1 has returned all memory to free list\n");
        Mem_print();
        Mem_stats();
        printf("\n----- End unit test driver 1 -----\n");
    }

    //Requests the number of bytes that matches a whole page, and a size that is one unit smaller than a page
    else if (unit_driver == 2) 
    { 
        printf("\n----- Begin unit driver 2 -----\n"); 

        int unit_size = SIZEOF_CHUNK_T;
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and the size of Chunk is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2;
        int num_bytes_1, num_bytes_2;

        // allocate 1st pointer to 1 page
        num_bytes_1 = (units_in_first_page - 1) * unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
         printf("unit driver 2: above Mem_print shows empty free list due to requesting full page (exact fit)\n"); 

        // allocate 2nd pointer to one chunk less than 1 page
        num_bytes_2 = (units_in_first_page - 2) * unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("second: %d bytes (%d units) p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print(); 
        printf("unit driver 2: above Mem_print shows empty free list due to memory requiring a header chunk (exact fit)\n"); 

        //Free memory requested
        Mem_free(p1); 
        Mem_free(p2); 

        Mem_stats();
        Mem_print();
        printf("\n----- End unit test driver 2 -----\n");
    } 

    //Request more bytes than in one page
    else if (unit_driver == 3) 
    { 
        printf("\n----- Begin unit driver 3 -----\n");
        
        int unit_size = SIZEOF_CHUNK_T;
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and the size of Chunk is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4;
        int num_bytes_1, num_bytes_2, num_bytes_3, num_bytes_4;
        
        // allocate 1st pointer to 1 page
        num_bytes_1 = (units_in_first_page - 1) * unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
        printf("unit driver 3: above Mem_print shows empty free list due to requesting full page (exact fit)\n"); 

        // allocate 2nd pointer to 2 pages
        num_bytes_2 = (2 * units_in_first_page - 1) * unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("second: %d bytes (%d units) p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print(); 
        printf("unit driver 3: above Mem_print shows empty free list due to requesting two full pages (exact fit)\n"); 

        // allocate 3rd pointer to 5 pages
        num_bytes_3 = (5 * units_in_first_page - 1) * unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("third: %d bytes (%d units) p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print(); 
        printf("unit driver 3: above Mem_print shows empty free list due to requesting five full pages (exact fit)\n");

        // allocate 4th pointer to 10 pages
        num_bytes_4 = (10 * units_in_first_page - 1) * unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("fourth: %d bytes (%d units) p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print(); 
        printf("unit driver 3: above Mem_print shows empty free list due to requesting ten full pages (exact fit)\n");

        //Free memory requested
        Mem_free(p1); 
        Mem_free(p2); 
        Mem_free(p3); 
        Mem_free(p4);

        Mem_stats(); 
        printf("unit driver 3: above Mem_stats shows 18 requested pages with 4 calls for more memory.\n"); 
        Mem_print();

        printf("\n----- End unit test driver 3 -----\n");
    } 

    //Do frees and allocates together instead of allocating first then freeing. Prove our heap can handle both 
    //despite the order and we can return everything back to the free list
    else if (unit_driver == 4) 
    { 
        clock_t start, end; 
        double elapse_time; //time in ms 
        start = clock(); //Start stopwatch 

        printf("\n----- Begin unit driver 4 -----\n");
        int unit_size = SIZEOF_CHUNK_T;
        int units_in_first_page = PAGESIZE/unit_size;
        assert(units_in_first_page * unit_size == PAGESIZE);
        printf("There are %d units per page, and the size of Chunk is %d bytes\n", 
                units_in_first_page, unit_size); 

        int *p1, *p2, *p3, *p4;
        int num_bytes_1, num_bytes_2, num_bytes_3, num_bytes_4; 

        // allocate 1st pointer to random
        num_bytes_1 = (2 * random - 1) * unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("first: %d bytes (%d units) p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
        printf("unit driver 4: above Mem_print shows free list due to requesting 2 random\n");

        // allocate 2nd pointer to random
        num_bytes_2 = (4 * random - 1) * unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("second: %d bytes (%d units) p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print(); 
        printf("unit driver 4: above Mem_print shows free list with 4 random\n");

        Mem_free(p2);
        printf("1st free of 4 random p=%p \n", p2);
        Mem_print();

        // allocate 3rd pointer to random
        num_bytes_3 = (50*(random/4) - 1) * unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("third: %d bytes (%d units) p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print(); 
        printf("unit driver 4: above Mem_print shows 50/4 random\n");

        Mem_free(p3);
        printf("2nd free of 50/4 random p=%p \n", p3);
        Mem_print(); 

        // allocate 4th pointer to random
        num_bytes_4 = (random - 1) * unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("fourth: %d bytes (%d units) p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print(); 
        printf("unit driver 4: above Mem_print shows free list due to requesting 1 random\n");

        Mem_free(p4); 
        printf("3rd free of 1 random p=%p \n", p4);
        Mem_print();
        Mem_free(p1); 
        printf("4th free of 2 random p=%p \n", p1);
        Mem_print();

        // allocate 1st pointer to random
        num_bytes_1 = (5 * random - 1) * unit_size;
        p1 = (int *) Mem_alloc(num_bytes_1);
        printf("fifth: %d bytes (%d units) p=%p \n", 
                num_bytes_1, num_bytes_1/unit_size, p1);
        Mem_print();
        printf("unit driver 4: above Mem_print shows free list due to requesting 5 random\n");

        // allocate 2nd pointer to random
        num_bytes_2 = (3 * random - 1) * unit_size;
        p2 = (int *) Mem_alloc(num_bytes_2);
        printf("sixth: %d bytes (%d units) p=%p \n", 
                num_bytes_2, num_bytes_2/unit_size, p2);
        Mem_print(); 
        printf("unit driver 4: above Mem_print shows free list with 3 random\n");

        Mem_free(p2);
        printf("5th free of 3 random p=%p \n", p2);
        Mem_print();

        // allocate 3rd pointer to random
        num_bytes_3 = (23*(random/4) - 1) * unit_size;
        p3 = (int *) Mem_alloc(num_bytes_3);
        printf("7th: %d bytes (%d units) p=%p \n", 
                num_bytes_3, num_bytes_3/unit_size, p3);
        Mem_print(); 
        printf("unit driver 4: above Mem_print shows 23/4 random\n");

        Mem_free(p3);
        printf("6th free of 23/4 random p=%p \n", p3);
        Mem_print(); 

        // allocate 4th pointer to random
        num_bytes_4 = (random - 1) * unit_size;
        p4 = (int *) Mem_alloc(num_bytes_4);
        printf("8th: %d bytes (%d units) p=%p \n", 
                num_bytes_4, num_bytes_4/unit_size, p4);
        Mem_print(); 
        printf("unit driver 4: above Mem_print shows free list due to requesting 1 random\n");

        Mem_free(p4); 
        printf("7th free of 1 random p=%p \n", p4);
        Mem_print();
        Mem_free(p1); 
        printf("8th free of 5 random p=%p \n", p1);
        Mem_print();

        Mem_stats();
        Mem_print();  

        end = clock();
        elapse_time = 1000.0 * ((double) (end - start)) / CLOCKS_PER_SEC; //convert to ms 
        printf("unit driver 4: It takes %f ms to allocate and free this combination\n", elapse_time);
        printf("\n----- End unit test driver 4 -----\n");
    } 

    return 0;
}

/* read in command line arguments.  Note that Coalescing and SearchPolicy 
 * are stored in global variables for easy access by other 
 * functions.
 *
 * -u 0  is for the unit drivers, starting with driver 0
 *
 *  argc:           number of command line arguments
 *  argv:           array of the command line arguments
 *  unit_driver:    is set to the number for the unit test
 *  seed:           is set to the seed for the random number generator
 */
void getCommandLine(int argc, char **argv, int *unit_driver)//, int *seed)
{
    /* The geopt function creates three global variables:
     *    optopt--if an unknown option character is found
     *    optind--index of next element in argv 
     *    optarg--argument for option that requires argument 
     *
     * The third argument to getopt() specifies the possible argument flags
     *   If an argument flag is followed by a colon, then an argument is 
     *   expected. E.g., "x:y" means -x must have an argument but not -y
     */
    int c;
    int index;

    while ((c = getopt(argc, argv, "s:f:u:h:c")) != -1)
        switch(c) {
            case 'u': *unit_driver = atoi(optarg);     break;
            //case 's': *seed = atoi(optarg);            break;
            case 'c': Coalescing = TRUE;               break;
            case 'f':
                  if (strcmp(optarg, "best") == 0)
                      SearchPolicy = BEST_FIT;
                  else if (strcmp(optarg, "first") == 0)
                      SearchPolicy = FIRST_FIT;
                  else if (strcmp(optarg, "worst") == 0)
                      SearchPolicy = WORST_FIT;
                  else {
                      fprintf(stderr, "invalid search policy: %s\n", optarg);
                      exit(1);
                  }
                  break;
            case 'h':
                  if (strcmp(optarg, "rover") == 0)
                      SearchLoc = ROVER_FIRST;
                  else if (strcmp(optarg, "head") == 0)
                      SearchLoc = HEAD_FIRST;
                  else {
                      fprintf(stderr, "invalid search starting location: %s\n", optarg);
                      exit(1);
                  }
                  break;
            case '?':
                  if (isprint(optopt))
                      fprintf(stderr, "Unknown option %c.\n", optopt);
                  else
                      fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                  /* fall through */
            default:
                  printf("Lab4 command line options\n");
                  printf("General options ---------\n");
                  printf("  -c        turn on coalescing (default off)\n");
                  printf("  -f best|first|worst\n");
                  printf("            search policy to find memory block (first by default)\n");
                  printf("  -h rover|head\n");
                  printf("            starting location for search\n");
                  printf("  -u 0      run unit test driver number 0\n");
                  exit(1);
        }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}

