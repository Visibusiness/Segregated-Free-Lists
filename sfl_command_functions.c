// Copyright Vișănescu Bogdan-Emilian 313CAb 2023 - 2024
#include "sfl.h"
// initialize the heap with blocks of memory in order starting from start_adress
// and with block_size starting from 8 and doubling at each step
heap_t *init_heap(char **words)
{
	heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
	DIE(!heap, "malloc heap failed");
	// initialize the heap with given values
	heap->start_adress = my_atoi(words[1]);
	heap->nr_lists = atoi(words[2]);
	heap->bytes_per_list = atoi(words[3]);
	heap->bool_rec = atoi(words[4]);
	// compute the total memory of the heap
	heap->heap_memory = heap->bytes_per_list * heap->nr_lists;
	heap->free_mem = heap->nr_lists * heap->bytes_per_list;
	heap->free_calls = 0;
	heap->fragm = 0;
	heap->malloc_calls = 0;
	heap->free_blocks = 0;

	size_t start;
	size_t block_size;
	size_t end;

	heap->list_array = (fl_t **)malloc(heap->nr_lists * sizeof(fl_t *));
	DIE(!heap->list_array, "malloc list_array failed");
	for (int i = 0; i < heap->nr_lists; i++) {
		start = i * heap->bytes_per_list + heap->start_adress;
		block_size = (size_t)1 << (3 + i);
		end = start + heap->bytes_per_list - block_size;
		// create a list of blocks of memory
		heap->list_array[i] = create_dll(0, block_size);
		DIE(!heap->list_array[i], "malloc list_array[i] failed");
		// fill the list with blocks of memory
		fill_dll(heap->list_array[i], start, end);
		heap->free_blocks += heap->list_array[i]->size;
	}

	return heap;
}

// allocate memory for a block of memory of nr_bytes size
// this function moves the block from the free list to the allocated list
// and fragments the block if needed
void MALLOC(heap_t *heap, char **words, fl_t *all_list)
{
	// get the number of bytes to allocate
	int nr_bytes_all = atoi(words[1]);
	int i = 0;
	int ok = 0;
	int j = 0;
	// check if the number of bytes is bigger than the biggest block size
	if (nr_bytes_all > heap->list_array[heap->nr_lists - 1]->block_size) {
		printf("Out of memory\n");
		return;
	}
	// find the list that has the block size bigger than the number of bytes
	while (nr_bytes_all > heap->list_array[i]->block_size &&
		   i != heap->nr_lists - 1)
		i++;
	// get the first block from the list
	fl_t *curr = heap->list_array[i];
	// remove the block with the smallest address from the list
	fl_node_t *removed_head = remove_nth_node(curr, 0);
	// update the heap values
	heap->malloc_calls++;
	heap->free_mem -= nr_bytes_all;
	heap->free_blocks--;
	// check if fragmenting is needed
	if (removed_head->block_size == nr_bytes_all) {
		remove_empty_lists(heap);
		add_node_by_address(removed_head, all_list);
		return;
	}
	// update the heap values
	heap->fragm++;
	heap->free_blocks++;
	// fragment the block in two blocks one add it to the heap coresponding
	// list and the other to the all_list
	int new_size = removed_head->block_size - nr_bytes_all;
	removed_head->block_size = nr_bytes_all;
	// add one block to the all_list
	add_node_by_address(removed_head, all_list);

	// add the other block to the heap coresponding list

	size_t start = removed_head->start + nr_bytes_all;
	fl_node_t *new_node = create_node(start, new_size);
	for (j = 0; j < heap->nr_lists; j++) {
		if (heap->list_array[j]->block_size == new_size) {
			ok = 1;
			break;
		}
	}
	if (ok == 1) {
		add_node_by_address(new_node, heap->list_array[j]);
		remove_empty_lists(heap);
		return;
	}
	add_list_in_heap(heap, new_node);
	// removes the empty lists from the heap
	remove_empty_lists(heap);
}

// free the block of memory at the address
// this function moves the block from the allocated list to the free list
// and merges the block with the adjacent blocks if bool_rec is 1
void free_mem(char **words, heap_t *heap, fl_t *all_list)
{ // get the address of the block to free
	size_t address = my_atoi(words[1]);
	// remove the block from the all_list
	fl_node_t *removed_node = remove_node_by_address(address, all_list);
	// bonus case
	if (heap->bool_rec == 1) {
		bonus_case(removed_node, heap);
		return;
	}
	// normal case
	if (heap->bool_rec == 0) {
		int i;
		fl_t *curr;
		if (!removed_node) {
			printf("Invalid free\n");
			return;
		}
		// update the heap values
		heap->free_calls++;
		heap->free_blocks++;
		heap->free_mem += removed_node->block_size;
		// add the block to the heap coresponding list
		for (i = 0; i < heap->nr_lists; i++) {
			curr = heap->list_array[i];
			if (curr->block_size == removed_node->block_size) {
				add_node_by_address(removed_node, curr);
				return;
			}
		}
		// if there is no list with the block size of the removed block
		// create a new list with the block size of the removed block
		// and add the block to the list and the list to the heap
		add_list_in_heap(heap, removed_node);
	}
}

// bonus case for free_mem function
// this function merges the block with the adjacent blocks
void bonus_case(fl_node_t *removed_node, heap_t *heap)
{
	// create a new block of memory with the same size as the removed block
	fl_node_t *new_node = malloc(sizeof(fl_node_t));
	DIE(!new_node, "malloc new_node failed");
	new_node->start = removed_node->start;
	new_node->block_size = removed_node->block_size;
	new_node->data = malloc(removed_node->block_size);
	DIE(!new_node->data, "malloc new_node->data failed");
	// update the heap values
	heap->free_calls++;
	// address of the removed block
	size_t address = removed_node->start;
	free(removed_node->data);
	free(removed_node);
	// compute the list size, mother block size, the
	// main address and the end address
	int list_number = (address - heap->start_adress) / heap->bytes_per_list;
	size_t list_start_address = heap->start_adress +
	list_number * heap->bytes_per_list;
	int mother_block_size = 8 << list_number;

	size_t main_address = list_start_address + ((address - list_start_address) /
	mother_block_size) * mother_block_size;
	size_t end_address = mother_block_size + main_address;
	// remove the adjacent blocks from the lists
	// and update the new block size and address
	for (int i = 0; i < heap->nr_lists; i++) {
		fl_t *curr = heap->list_array[i];
		fl_node_t *aux = curr->head;

		while (aux) {
			fl_node_t *next = aux->next;
			// adjacent block on the left
			if (aux->start == new_node->start + new_node->block_size &&
				aux->start < end_address) {
				new_node->block_size = new_node->block_size + aux->block_size;
				fl_node_t *node = remove_node_by_address(aux->start, curr);
				heap->free_blocks--;
				heap->free_mem -= aux->block_size;
				if (node->data)
					free(node->data);
				free(node);
			}
			// adjacent block to the right
			else if (aux->start + aux->block_size == new_node->start &&
					 aux->start >= main_address) {
				new_node->start = aux->start;
				new_node->block_size = new_node->block_size + aux->block_size;
				fl_node_t *node = remove_node_by_address(aux->start, curr);
				heap->free_mem -= aux->block_size;
				heap->free_blocks--;
				if (node->data)
					free(node->data);
				free(node);
			}

			aux = next;
		}
	}
	// remove the empty lists from the heap
	remove_empty_lists(heap);
	// add the new block to the all_list if the list exists
	for (int i = 0; i < heap->nr_lists; i++) {
		fl_t *curr = heap->list_array[i];
		if (curr->block_size == new_node->block_size) {
			add_node_by_address(new_node, curr);
			heap->free_mem += new_node->block_size;
			heap->free_blocks++;
			return;
		}
	}
	// create a new list with the block size
	// of the new block if the list does not exist
	add_list_in_heap(heap, new_node);
	heap->free_blocks++;
	heap->free_mem += new_node->block_size;
}

// read nr_bytes from the address
void read(heap_t *heap, fl_t *all_list, char **words, char *command)
{
	// get the address and the number of bytes to read
	size_t address = my_atoi(words[1]);
	int nr_bytes = atoi(words[2]);
	// check if the address is valid
	check_seg_fault(heap, all_list, address, nr_bytes, command, words);
	// find the block that contains the address
	fl_node_t *block = find_node_by_address(address, all_list);
	// read the bytes from the block
	while (block && nr_bytes > 0) {
		// offset represents the position of the address in the block
		size_t offset = address - block->start;
		// bytes_to_read represents the number of bytes
		// to read from the block on each iteration
		int bytes_to_read = (nr_bytes < (int)(block->block_size - offset)) ?
		nr_bytes : (int)(block->block_size - offset);
		for (int i = offset; i < (int)offset + bytes_to_read; i++)
			if (((char *)block->data))
				printf("%c", ((char *)block->data)[i]);
		address += bytes_to_read;
		nr_bytes -= bytes_to_read;
		block = block->next;
	}
	printf("\n");
}

// write nr_bytes to the address
void write(heap_t *heap, fl_t *all_list, char **words, char *command)
{ // get the address and the number of bytes to write
	size_t address = my_atoi(words[1]);
	int nr_bytes = atoi(words[count_words(words) - 1]);
	// get the phrase to write
	char *phrase = malloc(MAX_LEN * sizeof(char));
	DIE(!phrase, "malloc phrase failed");
	phrase[0] = '\0';
	int i = 2;
	int end = count_words(words) - 1;
	while (i < end) {
		strcat(phrase, words[i]);
		if (i != end - 1)
			strcat(phrase, " ");
		i++;
	}
	if ((size_t)nr_bytes < strlen(phrase))
		phrase[nr_bytes] = '\0';
	if ((size_t)nr_bytes > strlen(phrase))
		nr_bytes = strlen(phrase);
	write_in_blocks(heap, all_list, address, phrase, nr_bytes, command, words);
	free(phrase);
}

void write_in_blocks(heap_t *heap, fl_t *all_list, size_t address, char *data,
					 int nr_bytes, char *command, char **words)
{
	// check if the address is valid
	check_seg_fault(heap, all_list, address, nr_bytes, command, words);
	// find the block that contains the address
	fl_node_t *block = find_node_by_address(address, all_list);
	// aux is used for concatenating the data with the block data
	char *aux = malloc(MAX_LEN * sizeof(char));
	DIE(!aux, "malloc aux failed");
	if (!aux)
		return;

	aux[0] = '\0';
	int ok = 0;
	// write the bytes to the block
	while (block && nr_bytes > 0) {
		// bytes_to_write represents the number of bytes on each iteration
		int bytes_to_write = (nr_bytes < block->block_size) ?
		nr_bytes : block->block_size;

		if (block->data) {
			strcat(aux, block->data);
			ok = 1;
		}
		// delete the first bytes_to_write characters from aux
		delete_first_n_chars(aux, strlen(data));

		if (block->data)
			free(block->data);

		block->data =
		(char *)malloc((bytes_to_write + strlen(aux) + 1) * sizeof(char));
		DIE(!block->data, "malloc block->data failed");
		if (!block->data) {
			free(aux);
			return;
		}
		// copy the data to the block
		memcpy(block->data, data, bytes_to_write);
		((char *)block->data)[bytes_to_write] = '\0';
		// concatenate the data with the block data if needed
		if (ok)
			strcat(block->data, aux);

		address += bytes_to_write;
		data += bytes_to_write;
		nr_bytes -= bytes_to_write;

		block = block->next;
	}

	free(aux);
}

// dump the memory of the heap and the all_list
void dump_memory(heap_t *heap, fl_t *all_list)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", heap->heap_memory);
	printf("Total allocated memory: %d bytes\n",
		   heap->heap_memory - heap->free_mem);
	printf("Total free memory: %d bytes\n", heap->free_mem);
	printf("Free blocks: %ld\n", heap->free_blocks);
	printf("Number of allocated blocks: %d\n", all_list->size);
	printf("Number of malloc calls: %d\n", heap->malloc_calls);
	printf("Number of fragmentations: %d\n", heap->fragm);
	printf("Number of free calls: %d\n", heap->free_calls);
	print_heap(heap);
	print_list(all_list);
	printf("-----DUMP-----\n");
}

// frees memory allocated for the heap and the all_list and exits the program
void destroy_heap(heap_t *heap, fl_t *all_list, char *command, char **words)
{
	for (int i = 0; i < heap->nr_lists; i++)
		free_list(heap->list_array[i]);
	free(heap->list_array);
	free(heap);
	free_list(all_list);
	free(command);
	for (int i = 0; words[i]; i++)
		free(words[i]);

	free(words);
	exit(0);
}

// check if the address is valid
int check_seg_fault(heap_t *heap, fl_t *all_list,
					size_t address, int nr_bytes, char *command, char **words)
{
	if (!all_list || !all_list->head) {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(heap, all_list);
		destroy_heap(heap, all_list, command, words);
		return 1;
	}

	fl_node_t *block = find_node_by_address(address, all_list);

	if (!block) {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(heap, all_list);
		destroy_heap(heap, all_list, command, words);
		return 1;
	}

	for (int i = 0; i < nr_bytes; i++)
		if (!is_address_allocated(address + i, all_list)) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(heap, all_list);
			destroy_heap(heap, all_list, command, words);
			return 1;
	}

	return 0;
}
