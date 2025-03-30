/* huffTree.h
 * Huffman Tree Library
 *
 * Purpose: To assist with Huffman Algorithm, include structures, alphabet length, and function declarations here to save space.
 */

#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 256 // Length of our alphabet (1 Byte)

/* DEFINE STRUCTURES */

// Create structure to hold data into Huffman Nodes
typedef struct huff_node_tag {
  int symbol;                  // Store symbols. Not all nodes will have a symbol
  long int freq;               // Store frequency of symbols
  struct huff_node_tag *left;  // Go to left child
  struct huff_node_tag *right; // Go to right child
} huffNode;

// Create structure to hold Huffman Tree. Mainly to start at root
typedef struct {
  huffNode *root;
  int syms; // Number of symbols in the tree
} huffTree;

// Create structure to hold queue for Huffman nodes. Only use right pointer for huff_nodes
typedef struct queue_node_tag {
  huffNode *node;
  struct queue_node_tag *next;
} queueNode;

// Create structure to hold queue for Huffman nodes. Only use right pointer for huff_nodes
typedef struct {
  int size;        // Number of nodes in the queue (Stop at 1)
  queueNode *head; // Smallest Freq Intended
  queueNode *tail; // Highest Freq Intended
} queue;

// Create structure to hold symbols and their variable bits
typedef struct {
  unsigned char symbol;
  char code[32];   // Hold paths up to int length
  int code_length; // Store length of code
} dict;

/* Function declarations */
queue *queue_construct();                                           // Allocate memory for the Queue
void queue_destruct(queue *Q, int option);                          // Deallocate memory for the Queue
huffTree *huff_construct();                                         // Allocate memory for the Huffman Tree
huffNode *construct_node(int in_symbol, long int num_freq);         // Allocate memory for the Huffman Node
void huff_destruct(huffTree *huff);                                 // Deallocate memory for the Huffman Tree
void huff_destruct_nodes(huffNode *node);                           // Deallocate memory for the Huffman Nodes
void q_insert(queue *Q, huffNode *new, int index);                  // Insert into the queue given sorted index & Huffman Node to store
void q_insert_sort(queue *Q, huffNode *new);                        // Sort queue from least to greatest, finds appropriate index and calls q_insert
queue *init_freq(unsigned char *indata, long int filesize);         // Create the queue to hold frequencies
huffNode *h_insert(huffNode *N1, huffNode *N2);                     // Assist huff_assemble by creating a parent node pointing to two given children
huffNode *huff_assemble(queue *freq);                               // With the queue now created, use the queue to build the Huffman Tree. Stop when only one node left in queue.
int children(huffNode *N);                                          // Helper function done by Dr.Calhoun in ECE2230, counts number of children for each node for pretty_print
void pretty_print(huffTree *T);                                     // Function done by Dr. Calhoun in ECE2230, intended to print out Binary Search Trees with ASCII characters
int huff_code(huffNode *N, unsigned char S, char *code, int index); // Find the Huffman code associated with each symbol by traversing through the tree
huffTree *create_huff_tree(unsigned char *indata, long int filesize); // Create the Huffman Tree from file input
int huff_compress(FILE *input, char *title);                        // Compress the file using Huffman Algorithm
int huff_decompress(FILE *input, char *title);                      // Decompress the file using Huffman Algorithm
