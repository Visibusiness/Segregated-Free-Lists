// Copyright Vișănescu Bogdan-Emilian 313CAb 2023 - 2024

// libraries

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
// macros
#define MAX_WORDS 100
#define MAX_LEN 600
#define DIE(assertion, calldescription)   \
	do {								  \
		if (assertion) {                  \
										  \
			fprintf(stderr, "(%s, %d): ", \
					__FILE__, __LINE__);  \
			perror(calldescription);      \
			exit(errno);                  \
		}                                 \
	} while (0)

// structures

// fl_t is a structure that represents a list of free or allocated blocks
typedef struct fl_t {
	int allocated;
	struct fl_node_t *head;
	struct fl_node_t *tail;
	int block_size;
	int size;
} fl_t;

// heap_t is a structure that represents the heap
typedef struct heap_t {
	struct fl_t **list_array;
	size_t start_adress;
	int nr_lists;
	int bytes_per_list;
	int bool_rec;
	size_t free_blocks;
	int heap_memory;
	int free_mem;
	int fragm;
	int malloc_calls;
	int free_calls;
} heap_t;
// fl_node_t is a structure that represents a block of memory

typedef struct fl_node_t {
	size_t start;
	int block_size;
	void *data;
	struct fl_node_t *next;
	struct fl_node_t *prev;
} fl_node_t;

// list functions for general use
size_t compile_mother_block(heap_t *heap, size_t address);
void fill_dll(fl_t *list, size_t start, size_t end);
fl_node_t *create_node(size_t start, int block_size);
fl_t *create_dll(int allocated, int block_size);
fl_node_t *remove_nth_node(fl_t *list, int n);
void add_node_by_address(fl_node_t *node, fl_t *list);
fl_node_t *find_node_by_address(size_t address, fl_t *all_list);
fl_node_t *remove_node_by_address(size_t address, fl_t *list);
void remove_empty_lists(heap_t *heap);
void print_list(fl_t *list);
int is_address_allocated(size_t address, fl_t *all_list);

// init_heap used functions

heap_t *init_heap(char **words);

// MALLOC function

void MALLOC(heap_t *heap, char **words, fl_t *all_list);

// free_mem function

void free_mem(char **words, heap_t *heap, fl_t *all_list);
void bonus_case(fl_node_t *removed_node, heap_t *heap);

// read function

void read(heap_t *heap, fl_t *all_list, char **words, char *command);

// write function

void write(heap_t *heap, fl_t *all_list, char **words, char *command);

//  dump_memory function

void dump_memory(heap_t *heap, fl_t *all_list);

// non-main functions
void delete_first_n_chars(char *str, int n);
int check_seg_fault(heap_t *heap, fl_t *all_list, size_t address,
					int nr_bytes, char *command, char **words);
void write_in_blocks(heap_t *heap, fl_t *all_list,
					 size_t address, char *data, int nr_bytes, char *command,
					 char **words);
int count_words(char **words);
void destroy_heap(heap_t *heap, fl_t *all_list, char *command, char **words);
void free_list(fl_t *list);
void add_list_in_heap(heap_t *heap, fl_node_t *new_node);

void print_heap(heap_t *heap);
size_t my_atoi(const char *hex_str);
char **command_to_words(char *command);
