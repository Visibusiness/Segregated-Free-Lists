# Segregated Free Lists

This project implements a memory heap using segregated free lists.
The heap is organized into an array of lists, with each list containing
memory blocks of a specific size range. The main functionalities
provided by the project are:

- INIT_HEAP <start_address> <nr_lists> <nr_bytes_per_list> <reconstruction_switch>
- MALLOC <nr_bytes>
- FREE <address>
- DUMP_MEMORY
- READ <address> <nr_bytes>
- WRITE <address> <TEXT> <nr_bytes>
- DESTROY_HEAP

## Description

The project's primary objective is to manage a memory heap and perform
operations on memory blocks within it. The heap organization utilizes an
array of doubly-linked lists, allowing efficient allocation and deallocation
of memory blocks. Throughout the project, the following key aspects were addressed:

- Working with doubly-linked lists.
- Interacting with data structures for memory management.
- Dynamically allocating and managing memory for lists and blocks.
- Conducting debugging to ensure optimal functionality and reliability of the code.

## Main Functions

### INIT_HEAP

Initializes the memory heap starting from the specified <start_address>,
creating <nr_lists> lists with <nr_bytes_per_list> bytes per list. Additionally,
the <reconstruction_switch> parameter determines whether to reconstruct fragmented
blocks during deallocation.

### MALLOC

Allocates a memory block of size <nr_bytes>. The function selects the block with the
smallest address that can accommodate the requested size. If fragmentation occurs, the block
is split, and the allocated portion is moved to the allocated list while the remaining fragment
stays in its original list.

### FREE

Deallocates a memory block with the specified <address>. The block is moved from the allocated
list back to the heap. If the <reconstruction_switch> is enabled and the block was fragmented
during allocation, reconstruction is performed.

### DUMP_MEMORY

Displays detailed information about the memory heap, including the allocation status of blocks
and the contents of allocated memory.

### WRITE

Writes the provided <TEXT> to the memory block starting from the specified <address>,
for a given number of <nr_bytes>.

### READ

Reads and prints the contents of memory starting from the specified <address>,
for a given number of <nr_bytes>.

### DESTROY_HEAP

Frees all memory associated with the heap and resets the data structures.

This README provides an overview of the Segregated Free Lists project,
explaining its functionalities and key components.

Author: Vișănescu Bogdan-Emilian
