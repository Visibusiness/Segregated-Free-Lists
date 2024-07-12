// Copyright Vișănescu Bogdan-Emilian 313CAb 2023 - 2024
#include "sfl.h"
// create a new list
fl_t *create_dll(int allocated, int block_size)
{
	fl_t *new_fl = (fl_t *)malloc(sizeof(fl_t));
	DIE(!new_fl, "malloc failed");

	new_fl->allocated = allocated;
	new_fl->block_size = block_size;
	new_fl->size = 0;
	new_fl->head = NULL;
	new_fl->tail = NULL;
	return new_fl;
}

// fill a list with blocks of memory from start to end with blocks

void fill_dll(fl_t *list, size_t start, size_t end)
{
	for (size_t i = start; i <= end; i += list->block_size) {
		fl_node_t *new_node = create_node(i, list->block_size);
		new_node->prev = list->tail;
		new_node->data = NULL;

		if (list->tail)
			list->tail->next = new_node;
		else
			list->head = new_node;

		list->tail = new_node;
		list->size++;
	}
}

// add a list in the heap in the correct order and realloc the list_array
void add_list_in_heap(heap_t *heap, fl_node_t *new_node)
{
	int k = 0;
	fl_t *curr_list = heap->list_array[0];
	fl_t *new_list;
	if (curr_list) {
		while (curr_list->block_size < new_node->block_size) {
			k++;
			if (k == heap->nr_lists)
				break;
			curr_list = heap->list_array[k];
			if (!curr_list) {
				k = heap->nr_lists;
				break;
			}
		}
	}
	new_list = create_dll(0, new_node->block_size);
	add_node_by_address(new_node, new_list);
	heap->nr_lists++;
	heap->list_array =
	realloc(heap->list_array, heap->nr_lists * sizeof(fl_t *));
	for (int j = heap->nr_lists - 1; j > k; j--)
		heap->list_array[j] = heap->list_array[j - 1];
	heap->list_array[k] = new_list;
}

// create a new node with start and block_size
fl_node_t *create_node(size_t start, int block_size)
{
	fl_node_t *new_node = (fl_node_t *)malloc(sizeof(fl_node_t));
	DIE(!new_node, "malloc failed");

	new_node->start = start;
	new_node->block_size = block_size;
	new_node->data = NULL;
	new_node->next = NULL;
	new_node->prev = NULL;
	return new_node;
}

// remove the nth node from the list
fl_node_t *remove_nth_node(fl_t *list, int n)
{
	if (!list || !list->head)
		return NULL;

	fl_node_t *aux = list->head;
	if (list->size == 1) {
		list->head = NULL;
		list->size--;
		return aux;
	}

	if (!n) {
		list->head->next->prev = NULL;
		list->head = list->head->next;
		list->size--;
		return aux;
	}
	if (n == list->size) {
		aux = list->tail;
		list->tail->prev->next = NULL;
		list->size--;
		list->tail = list->tail->prev;
		return aux;
	}
	for (int i = 0; i < n; i++)
		aux = aux->next;
	aux->next->prev = aux->prev;
	aux->prev->next = aux->next;
	list->size--;
	return aux;
}

// find a node by address in a list and returns it
fl_node_t *find_node_by_address(size_t address, fl_t *all_list)
{
	fl_node_t *current = all_list->head;

	while (current) {
		if (address >= current->start &&
			address < current->start + current->block_size)
			return current;
		current = current->next;
	}

	return NULL;
}

// remove a node by address from a list and returns it without freeing it
fl_node_t *remove_node_by_address(size_t address, fl_t *list)
{
	fl_node_t *aux = list->head;
	if (!list->head)
		return NULL;
	if (address < aux->start || address > list->tail->start)
		return NULL;
	if (list->size == 1) {
		list->head = NULL;
		list->size--;
		return aux;
	}
	if (address == aux->start) {
		aux->next->prev = NULL;
		list->size--;
		list->head = aux->next;
		return aux;
	}
	if (address == list->tail->start) {
		aux = list->tail;
		list->tail->prev->next = NULL;
		list->size--;
		list->tail = list->tail->prev;
		return aux;
	}
	int i = 0;
	for (i = 0; i < list->size; i++) {
		if (aux->start == address)
			break;
		if (aux->next)
			aux = aux->next;
	}
	if (aux->start != address)
		return NULL;
	return remove_nth_node(list, i);
}

// check if an address is allocated in the all_list
// returns 1 if it is allocated, 0 otherwise
int is_address_allocated(size_t address, fl_t *all_list)
{
	fl_node_t *current = all_list->head;

	while (current) {
		if (address >= current->start &&
			address < current->start + current->block_size)
				return 1;
		current = current->next;
	}

	return 0;
}

// removes the empty lists from the heap
void remove_empty_lists(heap_t *heap)
{
	int i, j = 0;
	for (i = 0; i < heap->nr_lists; ++i) {
		fl_t *list = heap->list_array[i];
		if (list->size == 0) {
			free_list(list);
			heap->list_array[i] = NULL;
		} else {
			heap->list_array[j++] = list;
		}
	}
	heap->nr_lists = j;
}

// add a node in a list by address
void add_node_by_address(fl_node_t *node, fl_t *list)
{
	if (!list || !node)
		return;
	if (!list->head) {
		list->head = node;
		list->tail = node;
		list->tail->next = NULL;
		list->head->prev = NULL;
		list->size++;
		return;
	}
	fl_node_t *aux = list->head;
	if (aux->start > node->start) {
		// insert as the first node

		node->next = list->head;
		node->prev = NULL;
		list->head->prev = node;
		list->head = node;
		list->size++;
		return;
	}
	while (aux && aux->start < node->start)
		aux = aux->next;

	if (!aux) {
		// insert as the final node
		aux = list->tail;
		aux->next = node;
		node->prev = aux;
		node->next = NULL;
		list->tail = node;
		list->size++;
		return;
	}

	aux = aux->prev;
	node->next = aux->next;
	node->prev = aux;
	aux->next->prev = node;
	aux->next = node;
	list->size++;
}

// print a list
void print_list(fl_t *list)
{
	fl_node_t *aux = list->head;
	int bytes = list->block_size;
	int size = list->size;
	if (list->allocated == 0) {
		printf("Blocks with %d bytes - %d free block(s) :", bytes, size);
		while (aux) {
			printf(" 0x%lx", aux->start);
			aux = aux->next;
		}

		return;
	}
	printf("Allocated blocks :");
	while (aux) {
		printf(" (0x%lx - %d)", aux->start, aux->block_size);
		aux = aux->next;
	}
	printf("\n");
}

// print the heap
void print_heap(heap_t *heap)
{
	for (int i = 0; i < heap->nr_lists; i++) {
		print_list(heap->list_array[i]);
		printf("\n");
	}
}

// free a list
void free_list(fl_t *list)
{
	fl_node_t *aux = list->head;
	if (!list->head) {
		free(list);
		return;
	}
	fl_node_t *next = aux->next;
	for (int i = 0; i < list->size; i++) {
		next = aux->next;
		if (aux->data)
			free(aux->data);
		free(aux);
		aux = next;
	}
	free(list);
}

// deletes the first n characters from a string
void delete_first_n_chars(char *str, int n)
{
	int len = strlen(str);

	if (n >= len)
		str[0] = '\0';
	else
		memmove(str, str + n, len - n + 1);
}

// turns a string into a size_t variable
size_t my_atoi(const char *hex_str)
{
	return strtoul(hex_str, NULL, 16);
}

// turns a command into words
char **command_to_words(char *command)
{
	char *token = strtok(command, "\" ");
	char **words = (char **)malloc(MAX_WORDS * sizeof(char *));
	DIE(!words, "malloc failed");
	int index = 0;
	size_t len;
	while (token) {
		len = strlen(token) + 1;
		words[index] = malloc(len);
		DIE(!words[index], "malloc failed");
		memcpy(words[index], token, len);
		// lipseste programare defensiva
		index++;
		token = strtok(NULL, "\" ");
	}
	words[index] = NULL;
	return words;
}

// count the number of words in a phrase
int count_words(char **words)
{
	int index = 0;
	while (words[index])
		index++;
	return index;
}
