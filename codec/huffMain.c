/* huffTree.c - Huffman Compression and Decompression */

#include "huffTree.h"

/* Function to write the Huffman tree to a file for decompression */
void write_huff_tree(huffNode *node, FILE *output) {
  if (node == NULL) {
    return;
  }
  if (node->left == NULL && node->right == NULL) {
    fprintf(output, "1%c", node->symbol); // Leaf node indicator
  } else {
    fprintf(output, "0"); // Internal node indicator
  }
  write_huff_tree(node->left, output);
  write_huff_tree(node->right, output);
}

/* Read Huffman tree from file */
huffNode *read_huff_tree(FILE *input) {
  int c = fgetc(input);
  if (c == '1') { // Leaf node
    char symbol = fgetc(input);
    return construct_node(symbol, 0);
  } else if (c == '0') { // Internal node
    huffNode *node = construct_node(-1, 0);
    node->left = read_huff_tree(input);
    node->right = read_huff_tree(input);
    return node;
  }
  return NULL;
}

/* Compress a file using Huffman coding, optimized for executables */
int huff_compress(FILE *input, char *title) {
  FILE *output;
  char *newTitle, *extension, *newExtension;
  unsigned char *indata;
  long int filesize;
  huffTree *huff;
  queue *freq;
  dict table[256];

  // Read file into buffer
  fseek(input, 0, SEEK_END);
  filesize = ftell(input);
  fseek(input, 0, SEEK_SET);
  indata = (unsigned char *)malloc(filesize);
  if (indata == NULL) {
    return 1;
  }
  fread(indata, 1, filesize, input);
  fclose(input);

  // Create Huffman Tree
  freq = init_freq(indata, filesize);
  huff = create_huff_tree(indata, filesize);

  // Generate Huffman codes
  for (int i = 0; i < 256; i++) {
    table[i].symbol = i;
    table[i].code[0] = '\0'; // Initialize as empty
  }

  queueNode *N = freq->head;
  while (N != NULL) {
    huff_code(huff->root, N->node->symbol, table[N->node->symbol].code, 0);
    table[N->node->symbol].code_length = strlen(table[N->node->symbol].code);
    N = N->next;
  }

  // Handle new file name
  printf("Compressing %s...\n", title);
  newExtension = ".huff";
  extension = strrchr(title, '.'); // Find location of file extension in title
  if (extension != NULL) {
    *extension = '\0'; // Get rid of old extension, if it exists
  }
  newTitle = (char *)malloc(strlen(newExtension) + strlen(title) + 1); // Allocate enough space to store newTitle, char is 1 byte & include null terminator
  strcpy(newTitle, title);
  strcat(newTitle, newExtension); // Create a new file name to preserve the old file

  // Open output file in binary mode
  char outname[256];
  sprintf(outname, "%s", newTitle);
  output = fopen(outname, "wb");
  if (output == NULL) {
    return 1;
  }
  // Write tree to output as raw binary
  write_huff_tree(huff->root, output);

  // Write file size (for accurate decompression)
  fwrite(&filesize, sizeof(long int), 1, output);

  // Write encoded data
  unsigned char buffer = 0;
  int bit_count = 0;

  for (long int i = 0; i < filesize; i++) {
    char *code = table[indata[i]].code;
    for (int j = 0; code[j] != '\0'; j++) {
      buffer = (buffer << 1) | (code[j] - '0'); // Convert char '0' or '1' to bit
      bit_count++;

      if (bit_count == 8) {
        fwrite(&buffer, 1, 1, output);
        bit_count = 0;
        buffer = 0;
      }
    }
  }

  // Handle leftover bits
  if (bit_count > 0) {
    buffer <<= (8 - bit_count);
    fwrite(&buffer, 1, 1, output);
  }

  // Close the file, free the data, and leave
  fclose(output);
  free(newTitle);
  free(indata);
  queue_destruct(freq, 1);
  huff_destruct(huff);
  return 0;
}

/* Decompress a Huffman encoded file, optimized for executables */
int huff_decompress(FILE *input, char *title) {

  // Handle new file name
  char *newTitle, *extension, *newExtension;
  printf("Decompressing %s...\n", title);
  newExtension = ".u";
  extension = strrchr(title, '.'); // Find location of file extension in title
  if (extension != NULL) {
    *extension = '\0'; // Get rid of old extension, if it exists
  }
  newTitle = (char *)malloc(strlen(newExtension) + strlen(title) + 1); // Allocate enough space to store newTitle, char is 1 byte & include null terminator
  strcpy(newTitle, title);
  strcat(newTitle, newExtension); // Create a new file name to preserve the old file

  // Open output file in binary mode
  FILE *output;
  char outname[256];
  sprintf(outname, "%s", newTitle);
  output = fopen(outname, "wb");
  if (output == NULL) {
    return 1;
  }

  // Read Huffman tree
  huffNode *root = read_huff_tree(input);

  // Read original file size
  long int filesize;
  fread(&filesize, sizeof(filesize), 1, input);

  // Decode data
  huffNode *node = root;
  int c, total_bytes_written = 0;

  while (total_bytes_written < filesize) {
    c = fgetc(input);
    if (c == EOF) {
      break;
    }

    for (int i = 7; i >= 0; i--) {
      int bit = (c >> i) & 1;
      node = (bit == 0) ? node->left : node->right;

      if (node->left == NULL && node->right == NULL) {
        fputc(node->symbol, output);
        node = root;
        total_bytes_written++;
        if (total_bytes_written >= filesize)
          break; // Stop when original size is reached
      }
    }
  }

  // Close the files and free the data
  fclose(input);
  fclose(output);
  free(newTitle);
  huff_destruct_nodes(root);
  return 0;
}

/* Handle user input & begin file compression/decompression with Huffman Algorithm */
int main(int argc, char *argv[]) {
  FILE *input;

  // Verify User Input
  if (argc != 3) {
    printf("Usage: ./huffTree [-c|-d] [filename]\n");
    return 1;
  }

  // Verify File Open
  input = fopen(argv[2], "rb");
  if (input == NULL) {
    printf("Error opening file: %s\n", argv[2]);
    return 1;
  }

  // Handle User Command, begin compression/decompression
  if (strcmp(argv[1], "-c") == 0) {
    huff_compress(input, argv[2]);
  } else if (strcmp(argv[1], "-d") == 0) {
    huff_decompress(input, argv[2]);
  } else {
    printf("Invalid option. Use '-c' for compression, or '-d' for decompression.\n");
    return 1;
  }

  return 0;
}
