/* bst.c 
 * Joshua Silva
 * jysilva@clemson.edu
 * ECE 2230 Spring 2024
 * MP5
 *
 * Implementation of tree interface for basic binary tree
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "bst.h"

#define MYMAX(a, b) (a > b ? a : b)


// counters for statistics
static BST_Stats stats;  


/* definitions for use in bst.c only */
void ugly_print(BST_Node *N, int level, int policy);
int bst_debug_validate_rec(BST_Node *N, int min, int max, int *count);
int rec_height(BST_Node *N);
int children(BST_Node *N);
void pretty_print(BST *T); 

/* private functions */ 
int bst_find_key(BST *T, BST_Key key, Data elem_ptr); //Assist in finding key in tree, replacing it with new elem if found
int bst_bs_tree_insert(BST *T, BST_Key key, Data elem_ptr); //Assist in inserting node in Tree with BS_Tree policy
BST_Node* bst_insert_elem(BST *T, BST_Node *rover, BST_Key key, Data elem_ptr); //Assist in inserting node for both policies 
void bst_destruct_nodes(BST_Node *node); //Destroy nodes, and their data, that are located in Tree 
void bst_int_path_len_calc(BST_Node *rover, int *int_path_len, int depth); //Calculate the internal path length of the tree 
BST_Node* bst_rotate_left(BST_Node *node1, BST_Node *node2); //Rotate subtree left
BST_Node* bst_rotate_right(BST_Node *node1, BST_Node *node2); //Rotate subtree right


/* Finds the tree element with the matching key and returns the data that is
 * stored in this node in the tree.
 *
 * T - tree to search in key - key to find inside T
 *
 * RETURNS pointer to the data stored in the found node or NULL if no match is
 * found
 */
Data bst_access(BST *T, BST_Key key)
{
    assert(T != NULL);

    stats.CompCalls = 0; 
    BST_Node *rover = T -> root; 

    //Roam through tree until we find key
    while (rover != NULL) 
    {   
        (stats.CompCalls)++;

        //Found key
        if ((rover -> key) == key) 
        { 
            return rover->data_ptr; //Found key
        } 

        //Key not found, proceed to next appropriate node
        else 
        { 
            (stats.CompCalls)++;

            //Go left if key is less than current node
            if ((rover -> key) > key) 
            { 
                rover = rover -> left;
            }

            //Go right if key is greater than current node
            else
            { 
                rover = rover -> right;
            }
        }
    } 

    return NULL; //Function did not return data found in node with matching key
}

/* Creates the header block for the tree with the provided management policy,
 * and initializes it with default 'blank' data.
 *
 * tree_policy - tree management policy to use either AVL or BST.
 *
 * RETURNS pointer to the newly created tree
 */
BST *bst_construct(int tree_policy)
{
    //Make sure an appropriate tree policy has been given
    assert(tree_policy == BS_TREE || tree_policy == AVL); 
    
    /*Create header block & initialize it with default data*/
    
    //Allocate memory for new tree 
    BST *tree = (BST *) malloc(sizeof(BST)); 

    //Assign default values for tree
    tree -> policy = tree_policy; //Set policy 
    tree -> size = 0; 
    tree -> root = NULL; 
    
    //Assign default values for tree stats
    tree -> stats.CompCalls = 0; 
    tree -> stats.NumRotations = 0;

    return tree; //Return pointer to new tree
}


/* Free all items stored in the tree including the memory block with the data
 * and the BST_Node structure.  Also frees the header block.  
 *
 * T - tree to destroy
 */
void bst_destruct(BST *T)
{
    bst_destruct_nodes(T -> root); //Free all nodes and data contained within them
    free(T); //Free header block
} 

/* Free all items stored in the tree including the memory block with the data
 * and the BST_Node structure. Start at the beginning and recursively go down 
 * left and right subtrees, freeing each node as we go down the tree
 *
 * rover - node to destroy
 */ 
void bst_destruct_nodes(BST_Node *node) 
{ 
    //Prepare rovers to left and right pointers of current node
    BST_Node *rover1; 
    BST_Node *rover2; 

    //Proceed to free node & call function to free potential children.
    if (node != NULL) 
    { 
        //Node is valid, store locations of its children
        rover1 = node -> left; 
        rover2 = node -> right; 

        //Free the node and its data
        free(node->data_ptr); 
        free(node); 
        
        //Go on to destroy the node's children!
        bst_destruct_nodes(rover1); 
        bst_destruct_nodes(rover2); 
    }
}

/* Insert Data into the tree with the associated key. Insertion MUST
 * follow the tree's property (i.e., AVL or BST)
 *
 * T - tree to insert into
 * key - search key to determine if key is in the tree
 * elem_ptr - data to be stored at tree node indicated by key
 *
 * RETURNS 0 if key is found and element is replaced, and 1 if key is not found
 * and element is inserted
 */
int bst_insert(BST *T, BST_Key key, Data elem_ptr)
{
    stats.CompCalls = 0;
    stats.NumRotations = 0; 
    int insert_key; //Let user know if node was replaced or inserted

    //AVL Policy
    if (T->policy == AVL) {
        insert_key = bst_avl_insert(T, key, elem_ptr);
    } 

    //BS_Tree Policy
    else
    {
        insert_key = bst_bs_tree_insert(T, key, elem_ptr);
    }
    
#ifdef VALIDATE
        bst_debug_validate(T);
#endif
    return insert_key;
}

/* Find key located in Tree & replace element if found.
 * 
 * T - tree to insert into
 * key - search key to determine if key is in the tree
 * elem_ptr - data to be stored at tree node indicated by key
 * 
 * RETURNS 0 if key is found and element is replaced, and 1 if key is not found
 */
int bst_find_key(BST *T, BST_Key key, Data elem_ptr) 
{ 
    BST_Node *rover = T -> root; //Create a rover to navigate through Tree
    int replace = 1;

    //Roam through tree until we can determine if key does or does not already exist
    while (rover != NULL && replace == 1) 
    { 
        (stats.CompCalls)++; //Comparison Call for current node
        
        //Found key, replace previous node's element with new element
        if ((rover -> key) == key) 
        { 
            replace = 0; //Found key 
            free(rover -> data_ptr); //Get rid of old data 
            rover -> data_ptr = elem_ptr; //Assign new element
        } 

        //Key not found, proceed to next appropriate node
        else 
        { 
            (stats.CompCalls)++; //Comparison Call for next node 
            
            //Go left if key is less than current node
            if ((rover -> key) > key) 
            { 
                rover = rover -> left;
            }

            //Go right if key is greater than current node
            else
            { 
                rover = rover -> right;
            }
        }
    } 

    return replace;
} 

/* Insert Data into the tree with the associated key. Insertion MUST
 * follow the tree's balance property. 
 * 
 * Recursively go down list until proper location for insertion is found. If AVL balance 
 * is determined as we go back up towards to root
 *
 * rover - Current Node that we're in
 * key - search key to determine if key is in the tree (in this case it is not in the tree yet)
 * elem_ptr - data to be stored at tree node indicated by key 
 *  
 * Recursively return current node back to its parent, will return root/new root to where function is called
 */
BST_Node* bst_insert_elem(BST *T, BST_Node *rover, BST_Key key, Data elem_ptr) 
{   
    //Prepare for AVL balancing
    int left_height; 
    int right_height;
    int balance;

    //At the end of tree, Base Case, initialize new node
    if (rover == NULL) 
    {   
        BST_Node *node = (BST_Node *) malloc(sizeof(BST_Node)); //Allocate memory for new node 

        //Initialize new node with given key, element, and default left/right pointers
        node -> key = key; 
        node -> data_ptr = elem_ptr; 
        node -> left = NULL; 
        node -> right = NULL; 
        rover = node; //Set left/right of previous node to new inserted node
    } 

    //Keep navigating to appropriate position
    else 
    {
        
        //Go left if key is less than current node, if appropriate position is found, parent will point to new child
        if ((rover -> key) > key) 
        { 
            rover -> left = bst_insert_elem(T, rover -> left, key, elem_ptr);
        }

        //Go right if key is greater than current node, if appropriate position is found, parent will point to new child
        else
        { 
            rover -> right = bst_insert_elem(T, rover -> right, key, elem_ptr);
        } 
    } 

    /* Assign new height to node. Should be one greater of tallest child, -1 if no node for balancing reasons 
     * Will apply to BST as well, although BST tree will not make use of parameters for balancing
     */
    left_height = ((rover -> left) != NULL) ? rover -> left -> height: -1; 
    right_height = ((rover -> right) != NULL) ? rover -> right -> height: -1; 
    balance = left_height - right_height; //2 for left heavy, -2 for right heavy tree
    rover -> height = 1 + MYMAX(left_height, right_height); //Update node height

    //For every insert in an AVL tree, check if tree is balanced, otherwise rotate appropriately
    if ((T -> policy) == AVL) 
    { 
        /* Left Heavy */ 

        //LL rotation 
        if (balance >= 2 && key < (rover -> left -> key)) 
        { 
            rover = bst_rotate_right(rover, rover ->left); //abc, Rotate 'a' and 'b'
        } 

        //LR rotation
        else if (balance >= 2 && key > (rover -> left -> key)) 
        { 
            rover -> left = bst_rotate_left(rover -> left, rover -> left -> right); //abc, Rotate 'b' and 'c'
            rover = bst_rotate_right(rover, rover -> left); //abc, Rotate 'a' and 'b'
        }

        /* Right Heavy */ 

        //RR rotation
        else if (balance <= -2 && key > (rover -> right -> key)) 
        { 
            rover = bst_rotate_left(rover, rover -> right); //abc, Rotate 'a' and 'b'
        } 

        //RL rotation
        else if (balance <= -2 && key < (rover -> right -> key)) 
        { 
            rover -> right = bst_rotate_right(rover -> right, rover -> right -> left); //abc, Rotate 'b' and 'c'
            rover = bst_rotate_left(rover, rover -> right); //abc, Rotate 'a' and 'b'
        }
    }

    return rover; //Return current node, will return new root to function that calls bst_insert_elem
} 

/* Rotate subtree left to maintain balance of AVL tree
 *
 * node1 - Top node to be rotated. 
 * node2 - Bottom node to be rotated.
 */
BST_Node* bst_rotate_left(BST_Node *node1, BST_Node *node2) 
{ 
    BST_Node *left_subtree; //If node2 has a left subtree, transfer it over to node1 right ptr
    int left_height; 
    int right_height;
    (stats.NumRotations)++; //Let stats block know that a rotation has occurred 

    //Check if node violating balance has a left subtree 
    left_subtree = node2 -> left; //Store left subtree 
    node2 -> left = node1; //Rotate 2nd node left
    node1 -> right = left_subtree; 

    //Update node1 heights
    left_height = ((node1 -> left) != NULL) ? node1 -> left -> height: -1; 
    right_height = ((node1 -> right) != NULL) ? node1 -> right -> height: -1; 
    node1 -> height = 1 + MYMAX(left_height, right_height); 
    
    //Update node2 heights
    left_height = ((node2 -> left) != NULL) ? node2 -> left -> height: -1; 
    right_height = ((node2 -> right) != NULL) ? node2 -> right -> height: -1; 
    node2 -> height = 1 + MYMAX(left_height, right_height); 

    return node2; //Return node2, which will rotate node1 to the left
} 

/* Rotate subtree right to maintain balance of AVL tree
 *
 * node1 - Top node to be rotated. 
 * node2 - Bottom node to be rotated. 
 */
BST_Node* bst_rotate_right(BST_Node *node1, BST_Node *node2)
{ 
    BST_Node *right_subtree; //If node2 has a right subtree, transfer it over to node1 left ptr
    int left_height; 
    int right_height;
    (stats.NumRotations)++; //Let stats block know that a rotation has occurred 

    //Check if node violating balance has a right subtree 
    right_subtree = node2 -> right; //Store right subtree 
    node2 -> right = node1; //Rotate 2nd node right
    node1 -> left = right_subtree; 

    //Update node1 heights
    left_height = ((node1 -> left) != NULL) ? node1 -> left -> height: -1; 
    right_height = ((node1 -> right) != NULL) ? node1 -> right -> height: -1; 
    node1 -> height = 1 + MYMAX(left_height, right_height); 
    
    //Update node2 heights
    left_height = ((node2 -> left) != NULL) ? node2 -> left -> height: -1; 
    right_height = ((node2 -> right) != NULL) ? node2 -> right -> height: -1; 
    node2 -> height = 1 + MYMAX(left_height, right_height); 

    return node2; //Return node2, which will rotate node1 to the right
}

/* Insert Data into the tree with the associated key. Insertion MUST
 * follow the tree's property AVL. This function should be called from
 * bst_insert for AVL tree's inserts.
 *
 * T - tree to insert into
 * key - search key to determine if key is in the tree
 * elem_ptr - data to be stored at tree node indicated by key
 *
 * RETURNS 0 if key is found and element is replaced, and 1 if key is not found
 * and element is inserted
 */
int bst_avl_insert(BST *T, BST_Key key, Data elem_ptr)
{
    int replace = bst_find_key(T, key, elem_ptr); //Check if key already exists, if so replace old node
    
    //Key was not found, insert element into tree
    if (replace == 1) 
    { 
        (T->size)++; //Added another node 
        T -> root = bst_insert_elem(T, T -> root, key, elem_ptr); //Call insert_elem function to handle new insert
    }

    return replace;
}

/* Insert Data into a BST tree with the associated key. 
 * This function should be called from bst_insert for BS_TREE tree's inserts.
 *
 * T - tree to insert into
 * key - search key to determine if key is in the tree
 * elem_ptr - data to be stored at tree node indicated by key
 *
 * RETURNS 0 if key is found and element is replaced, and 1 if key is not found
 * and element is inserted
 */
int bst_bs_tree_insert(BST *T, BST_Key key, Data elem_ptr)
{
    //Call find_key to see if key already exists in Tree
    int replace = bst_find_key(T, key, elem_ptr); //Check if key already exists, if so replace old node
    
    //Key was not found, insert element into tree
    if (replace == 1) 
    { 
        (T->size)++; //Added another node 
        T -> root = bst_insert_elem(T, T -> root, key, elem_ptr); //Call insert_elem function to handle new insert 
    }

    return replace;
}

/* DO NOT NEED TO IMPLEMENT FOR REGULAR ASSIGNMENT. ONLY FOR EXTRA ASSIGNMENT.
 *
 * Removes the item in the tree with the matching key.
 * 
 * T - pointer to tree
 * key - search key for particular node in the tree 'T'
 *
 * RETURNS the pointer to the data memory block and free the BST_Node memory
 * block.  If the key is not found in the tree, return NULL.  If the tree's 
 * policy is AVL, then ensure all nodes have the AVL property.
 *
 */
Data bst_remove(BST *T, BST_Key key)
{
    Data dp = NULL;
    stats.CompCalls = 0;
    stats.NumRotations = 0;
    if (T->policy == AVL)
	    dp = NULL; /*TODO: AVL remove */
    else
	    dp = NULL; /*TODO: BST remove */

    /*TODO: update tree stats*/

#ifdef VALIDATE
        bst_debug_validate(T);
#endif
    return dp;
}


/* RETURNS the number of keys in the tree */
int bst_size(BST *T)
{
    //Make sure a tree is being given
    assert(T != NULL); 

    return (T -> size); //Return amount of keys in the tree
}

/* RETURNS the number of key comparisons  */
int bst_key_comps(BST *T)
{
    return (stats.CompCalls); //Return number of key comparisons
}

/* RETURNS the computed internal path length of the tree T */
int bst_int_path_len(BST *T)
{
    //Make sure a tree is being given
    assert(T != NULL);  

    //Prepare pointer for incrementing depth of each node in IPL calc function
    int int_path_len = 0; 
    int *int_path_len_ptr = &int_path_len; //Store location of IPL variable for future increments

    //Call calc function to recursively go through each node of the list and add its depth to IPL
    bst_int_path_len_calc(T -> root, int_path_len_ptr, int_path_len); 
   
    return int_path_len; 
} 

/* Compute the internal path length of the tree T, keeping track of the given nodes depth */
void bst_int_path_len_calc(BST_Node *rover, int *int_path_len, int depth) 
{ 
    if (rover != NULL) 
    {
        (*int_path_len) += depth; //Update IPL with depth of current node
        bst_int_path_len_calc(rover -> left, int_path_len, depth + 1); //Add depth of left subtree nodes
        bst_int_path_len_calc(rover -> right, int_path_len, depth + 1); //Add depth of right subtree nodes
    }
}

/* RETURNS the number of rotations from the last remove*/
int bst_rotations(BST *T)
{
    return (stats.NumRotations); //Return number of rotations
}


/* prints the tree T */
void bst_debug_print_tree(BST *T)
{
    ugly_print(T->root, 0, T->policy);//XTRA
    printf("\n");
    if (T->size < 64)
	pretty_print(T);
}

/* basic print function for a binary tree 
 *
 * N - node of tree to print
 * level - level in which the node resides
 * policy - BST or AVL
 */
void ugly_print(BST_Node *N, int level, int policy)
{
    int i;
    if (N == NULL) return;
    ugly_print(N->right, level+1, policy);
    if (policy == AVL) {
	    for (i = 0; i<level; i++) printf("       ");
	        printf("%5d-%d\n", N->key, N->height);
    } else {
	    for (i = 0; i<level; i++) printf("     ");
	        printf("%5d\n", N->key);
    }
    ugly_print(N->left, level+1, policy);
}


/* Basic validation function for tree T */
void bst_debug_validate(BST *T)
{
    int size = 0;
    assert(bst_debug_validate_rec(T->root, INT_MIN, INT_MAX, &size) == TRUE); 
    assert(size == T->size);
    if (T->policy == AVL)
	    rec_height(T->root);
}


/* A tree validation function based on node position
 *
 * N - node to validate tree
 * min - temp min for it's left subtree min
 * max - temp max for it's right subtree max
 */
int bst_debug_validate_rec(BST_Node *N, int min, int max, int *count)
{
    if (N == NULL)
        return TRUE;
    if (N->key <= min || N->key >= max)
        return FALSE;
    assert(N->data_ptr != NULL);
    *count += 1;
    return bst_debug_validate_rec(N->left, min, N->key, count) &&
        bst_debug_validate_rec(N->right, N->key, max, count);
}

/* Verifies AVL tree properties */
int rec_height(BST_Node *N)
{
    if (N == NULL)
	    return 0;
    int lh = rec_height(N->left);
    int rh = rec_height(N->right); 
    int lean = lh - rh; 
    assert(lean == 0 || lean == 1 || lean == -1);
    return 1 + MYMAX(lh, rh); 

}





/* Recursive function to count children */
int children(BST_Node *N)
{
    if (N == NULL) return 0;
    return 1 + children(N->left) + children(N->right);
}



/* Prints the tree to the terminal in ASCII art*/
void pretty_print(BST *T)
{
    typedef struct queue_tag {
	    BST_Node *N;
	    int level;
	    int list_sum;
    } Queue;

    Queue Q[T->size];
    int q_head = 0;
    int q_tail = 0;
    int i, j;
    int current_level = 0;
    int col_cnt = 0;
    BST_Node *N;

    Q[q_tail].N = T->root;
    Q[q_tail].level = 0;
    Q[q_tail].list_sum = 0;
    q_tail++;
    for (i = 0; i < T->size; i++)
    {
	assert(q_head < T->size);
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
	for (j = col_cnt + 1; j <= right_child_pos; j++)
	{
	    if (j == my_pos)
		if (left_child_pos < my_pos)
		    if (N->key < 10)
			printf("--%d", N->key);
		    else if (N->key < 100)
			printf("-%d", N->key);
		    else
			printf("%d", N->key);
		else
		    printf("%3d", N->key);
	    else if (j == left_child_pos)
		//printf("  |");
		printf("  /");
	    else if (j > left_child_pos && j < my_pos)
		printf("---");
	    else if (j > my_pos && j < right_child_pos)
		printf("---");
	    else if (j == right_child_pos)
		//printf("--|");
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


