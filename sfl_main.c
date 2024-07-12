// Copyright Vișănescu Bogdan-Emilian 313CAb 2023 - 2024
#include "sfl.h"
// this is the main function of the program
int main(void)
{
	// allocate memory for the command
	char *command = malloc(MAX_LEN * sizeof(char));
	DIE(!command, "malloc failed");
	// this is the command breaked into words
	char **words;
	// create the list of allocated blocks
	fl_t *all_list = create_dll(1, 0);
	// initialize the heap
	heap_t *heap;
	while (1) {
		// read the command
		scanf(" %[^\n]", command);
		// break the command into words
		words = command_to_words(command);
		// check the command and execute it
		if (!strcmp(words[0], "INIT_HEAP") && count_words(words) == 5)
			heap = init_heap(words);
		else if (!strcmp(words[0], "MALLOC") && count_words(words) == 2)
			MALLOC(heap, words, all_list);
		else if (!strcmp(words[0], "FREE") && count_words(words) == 2)
			free_mem(words, heap, all_list);
		else if (!strcmp(words[0], "DUMP_MEMORY") && count_words(words) == 1)
			dump_memory(heap, all_list);
		else if (!strcmp(words[0], "DESTROY_HEAP") && count_words(words) == 1)
			destroy_heap(heap, all_list, command, words);
		else if (!strcmp(words[0], "WRITE"))
			write(heap, all_list, words, command);
		else if (!strcmp(words[0], "READ") && count_words(words) == 3)
			read(heap, all_list, words, command);
		// free the memory allocated for the command
		for (int i = 0; words[i] ; i++)
			free(words[i]);
		free(words);
	}
	return 0;
}
