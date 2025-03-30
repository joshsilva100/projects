/* huffTree.c
 * Huffman Tree
 *
 * Purpose: To compress/decompress a file, specifically golfcore.ppm, using Huffman
 *
 * Assumptions: User knows that code uses Huffman method
 *
 * The program accepts one command line argument, including the name of the file
 */

#include "huffTree.h"

/* Construct the Queue */
queue *queue_construct() {
  queue *freq = (queue *)malloc(sizeof(queue)); // Allocate Memory

  // Assign default values for Queue
  freq->size = 0;
  freq->head = NULL;
  freq->tail = NULL;

  return freq;
}

/* Destruct the Queue 
 * Q: Queue that stores Huffman Nodes 
 * option: Extra argument given if user also wants to destroy the Huffman Nodes inside each queue node
 */
void queue_destruct(queue *Q, int option) {
  assert(Q != NULL);

  queueNode *N = Q->head;
  queueNode *N_Next; // Make sure to not lose next node
  int size = Q->size;

  // Go through queue and free all pointers. Do not need to free Huffman Nodes! 
  for (int i = 0; i < size; i++) {
    // If there is a next node, save its location
    if (i < (size - 1)) {
      N_Next = N->next;
    } else
      N_Next = NULL; 
    if (option == 1) free(N->node); // Added in to deal with scenario where queue is not used to construct Huffman tree
    free(N); 
    N = N_Next;
  }  
  
  

  free(Q);
}

/* Construct the Huffman Tree */
huffTree *huff_construct() {
  huffTree *huff = (huffTree *)malloc(sizeof(huffTree)); // Allocate Memory

  // Assign default values for tree
  huff->root = NULL;
  huff->syms = 0;

  return huff;
}

/* Construct a Huffman Node 
 * in_symbol: Symbol to be inputted into Huffman Node 
 * num_freq: Frequency of the symbol being inputted 
 * 
 * Returns node, the Huffman node constructed 
 */
huffNode *construct_node(int in_symbol, long int num_freq) {
  if (num_freq < 0) {
    assert(num_freq > 0);
  }
  huffNode *node = (huffNode *)malloc(sizeof(huffNode)); // Allocate Memory
  node->symbol = in_symbol;                              // Assign symbol. If greater than/equal to LENGTH, does not store an actual symbol (NOT ALL NODES NEED SYMBOLS)
  node->freq = num_freq;                                 // Assign frequency, all nodes need this;
  node->left = NULL;
  node->right = NULL;

  return node;
}

/* Destruct the Huffman Tree */
void huff_destruct(huffTree *huff) {
  assert(huff != NULL);

  huff_destruct_nodes(huff->root); // Free all nodes first
  free(huff);                      // Deallocate Memory
}

/* Destruct the Huffman Nodes */
void huff_destruct_nodes(huffNode *node) {
  huffNode *rover1;
  huffNode *rover2;

  // Process to free node & call function to free any children
  if (node != NULL) {
    // Node is valid, store locations of its children
    rover1 = node->left;
    rover2 = node->right;

    // Free the node
    free(node); // Deallocate Memory

    // Recursively destroy the node's children!
    huff_destruct_nodes(rover1);
    huff_destruct_nodes(rover2);
  }
}

/* Insert new queue node into the queue based on the given index
 * Q: Queue storing Huffman Nodes
 * new: Huffman node to be added to queue
 * index: Position for Huffman node to be added to 
 *  
 * Originally created for ECE2230 Project 
 */
void q_insert(queue *Q, huffNode *new, int index) {
  assert(Q != NULL);
  assert(new != NULL);

  // Create new queue node
  queueNode *q_node = (queueNode *)malloc(sizeof(queueNode)); // Allocate Memory
  q_node->node = new;                                         // Store huffman node
  // Queue is empty
  if (Q->head == NULL) {
    // Set up new node, no other nodes to point to.
    q_node->next = NULL;
    // Set up head & tail. Only one node, both head and tail will point to new q_node
    Q->head = q_node;
    Q->tail = q_node;
  }
  // Inserting at the head
  else if (index == 0) {
    // Set up new node
    q_node->next = Q->head; // Point to previous head
    // Set up head
    Q->head = q_node;
  }
  // Inserting at the tail
  else if (index >= (Q->size)) {
    // Set up new node.
    q_node->next = NULL;
    // Set up tail
    Q->tail->next = q_node; // Previous tail points to new tail
    Q->tail = q_node;
  }
  // Position is not valid
  else if (index < 0) {
    return;
  }
  // Inserting in the middle
  else {
    // Go through linked list until position before index is found
    queueNode *N = Q->head;
    // Find the node
    for (int i = 0; i <= index; i++) {
      if (i == (index - 1)) {
        // Set up new node
        q_node->next = N->next;
        // Update previous node
        N->next = q_node;
      }
      N = N->next;
    }
  }

  (Q->size)++; // Indicate that a node has been added
}

/* Inserts the Huffman Node at the proper position. Following least to greatest frequencies
 * Q: Queue storing Huffman nodes
 * new: Huffman node to be added to queue
 * Note: If the Node equals in frequency to another node, will be inserted after
 */
void q_insert_sort(queue *Q, huffNode *new) {
  assert(Q != NULL);
  assert(new != NULL);

  queueNode *N = Q->head;
  int i = 0;

  /*Queue is sorted from smallest to largest
   *Will run until next node has larger frequency, or queue has ended
   */
  while ((Q->size > 0) && (N != NULL) && ((N->node->freq) < (new->freq))) {
    i++;
    N = N->next;
  }

  // Let q_insert handle inserting to the queue
  q_insert(Q, new, i);
}

/* Initialize the dictionary to represent one byte values. Count frequencies
 * dict[]: Dictionary to hold frequencies of 1 bytes values
 * indata: Pointer to the input buffer, which stores all data from file to be compressed/decompressed
 * filesize: Size of the input file 
 *  
 * Returns freq, a queue that stores all Symbols & their frequencies for use in Huffman Tree construction
 */
queue *init_freq(unsigned char *indata, long int filesize) {
  assert(indata != NULL);

  long int dict[LENGTH] = {0}; // Ensure dictionary is initialized
  huffNode *node;

  // Count character frequencies
  for (long int j = 0; j < filesize; j++) {
    dict[indata[j]]++;
  }

  queue *freq = queue_construct(); // Create queue to hold frequency nodes

  // Add non-zero frequencies into the queue
  for (int k = 0; k < LENGTH; k++) {
    if (dict[k] > 0) {
      node = construct_node(k, dict[k]);
      q_insert_sort(freq, node);
    }
  }

  return freq;
}

/* Create a new parent node for the two given nodes. Save combined frequency & assign travel paths
 * N1: "Smaller" of the two nodes. Both are the lowest two frequencies
 * N2: "Larger" of the two nodes. Both are the lowest two frequencies 
 *  
 * Returns parent, a new Huffman parent node that points to two input nodes  
 */
huffNode *h_insert(huffNode *N1, huffNode *N2) {
  assert(N1 != NULL);
  assert(N2 != NULL);

  huffNode *parent;
  long int newFreq;
  newFreq = (N1->freq) + (N2->freq); // Combine the two frequencies

  parent = construct_node(LENGTH, newFreq); // Create the parent node with the sum of their children's frequencies. Give impossible symbol to indicate no symbol in node
  parent->left = N1;                        //"Smaller" child is left
  parent->right = N2;                       //"Larger" child is right

  return parent; // Return parent for queue to store
}

/* Begin creating the Huffman Tree with the assistance of the queue 
 * Q: A queue of symbols & their frequencies, use to assist with production of tree  
 * 
 * Returns root to finished Huffman Tree
 */
huffNode *huff_assemble(queue *Q) {
  assert(Q != NULL);

  queueNode *Q1;
  queueNode *Q2;
  huffNode *parent;

  while (Q->size > 1) {
    // Save the two smallest frequencies
    Q1 = Q->head;
    Q2 = Q->head->next;
    assert(Q1 != NULL);
    assert(Q2 != NULL);

    // Create new parent to combine nodes
    parent = h_insert(Q1->node, Q2->node);

    // Get rid of first two nodes in queue
    Q->head = Q2->next;    // Assign new position for head
    Q->size = Q->size - 2; // Indicate that we've removed two nodes
    free(Q1);
    free(Q2);

    // Insert new node into the queue. Sorted from least to greatest frequencies
    q_insert_sort(Q, parent);
  }

  assert(Q->head != NULL);

  return Q->head->node; // Returns root of new Huffman Tree
}

/* Recursive function to count children
 * Function is from Dr.Calhoun's ECE2230 Course
 */
int children(huffNode *N) {
  if (N == NULL)
    return 0;
  return 1 + children(N->left) + children(N->right);
}

/* Prints the tree to the terminal in ASCII art
 * Function is from Dr.Calhoun's ECE2230 Course
 */
void pretty_print(huffTree *T) {
  typedef struct queue_tag {
    huffNode *N;
    int level;
    int list_sum;
  } Queue;

  T->syms = (T->syms * 2) - 1;

  Queue Q[T->syms];
  int q_head = 0;
  int q_tail = 0;
  int i, j;
  int current_level = 0;
  int col_cnt = 0;
  huffNode *N;

  Q[q_tail].N = T->root;
  Q[q_tail].level = 0;
  Q[q_tail].list_sum = 0;
  q_tail++;
  for (i = 0; i < T->syms; i++) {
    assert(q_head < T->syms);
    N = Q[q_head].N;
    if (Q[q_head].level > current_level) {
      printf("\n");
      current_level++;
      col_cnt = 0;
    }
    int left_ch = children(N->left);
    int my_pos = 1 + left_ch + Q[q_head].list_sum;
    int left_child_pos = my_pos;
    if (N->left != NULL)
      left_child_pos = 1 + Q[q_head].list_sum + children(N->left->left);
    int right_child_pos = my_pos;
    if (N->right != NULL)
      right_child_pos = my_pos + 1 + children(N->right->left);
    for (j = col_cnt + 1; j <= right_child_pos; j++) {
      if (j == my_pos)

        if (left_child_pos < my_pos)
          if (N->symbol < 10)
            printf("--%d: %ld", N->symbol, N->freq);
          else if (N->symbol < 100)
            printf("-%d: %ld", N->symbol, N->freq);
          else
            printf("%d: %ld", N->symbol, N->freq);
        else
          printf("%d: %ld", N->symbol, N->freq);
      else if (j == left_child_pos)
        // printf("  |");
        printf("  /");
      else if (j > left_child_pos && j < my_pos)
        printf("---");
      else if (j > my_pos && j < right_child_pos)
        printf("---");
      else if (j == right_child_pos)
        // printf("--|");
        printf("-\\ ");
      else
        printf("   ");
    }
    col_cnt = right_child_pos;

    if (N->left != NULL) {
      Q[q_tail].N = N->left;
      Q[q_tail].level = Q[q_head].level + 1;
      Q[q_tail].list_sum = Q[q_head].list_sum;
      q_tail++;
    }
    if (N->right != NULL) {
      Q[q_tail].N = N->right;
      Q[q_tail].level = Q[q_head].level + 1;
      Q[q_tail].list_sum = Q[q_head].list_sum + left_ch + 1;
      q_tail++;
    }
    q_head++;
  }
  printf("\n");
}

/* Give back code to symbol in Huffman Tree
 * N: Huffman Node, starts at root of tree & contains symbols & their frequencies
 * S: Symbol we intend to find
 * code: Array of strings, meant to store the travel path to the found symbol
 * index: Current index of our code array, meant to increment as we go futher down the tree 
 *  
 * Return value is not important to outside functions, meant for use in recursion IF statements for writing code
 */
int huff_code(huffNode *N, unsigned char S, char *code, int index) {
  assert(N != NULL);

  // Leaf Node found, will contain a symbol if Huffman Tree, check for match
  if (N->left == NULL && N->right == NULL) {
    if (N->symbol == S) {
      code[index] = '\0'; // Base Case: Indicate end of string
      return 1;
    }
    return 0; // Not at this node
  }

  // Go left, add 0 to the path if found
  code[index] = '0';
  if (huff_code(N->left, S, code, index + 1))
    return 1;

  // Go right, add 1 to the path if found
  code[index] = '1';
  if (huff_code(N->right, S, code, index + 1))
    return 1;

  return 0; // Get rid of warning, chance of symbol not found
}

/* Function to create the Huffman Tree 
 * indata: Input buffer that stores filedata given by user 
 * filesize: Size of the filedata given by user 
 *
 * Returns a completed Huffman Tree  
 */
huffTree *create_huff_tree(unsigned char *indata, long int filesize) {
  queue *freq = init_freq(indata, filesize);

  huffTree *huff = huff_construct();
  huff->syms = freq->size; // Grab # of symbols in queue
  huff->root = huff_assemble(freq);

  queue_destruct(freq, 0);
  return huff;
}
