/*
 * mem_manager.c
 *
 * Status: E
 *
 *  Created on: Mar 26, 2017
 *      Author: xuefei1
 *
 *  Runtime memory checker implementations
 *
 *  The idea is every malloc() call is manually associated with an id
 *  pass in the id when calling request_malloc(), if the address pointed by id
 *  is not null, meaning you have un-freed memory, you must free this before you can
 *  allocate using the same id
 *
 *  This approach can detect leaked memory when calling malloc in a loop
 */

#include "mem_manager.h"

void* allocation_addr[MAX_ALLOCATION_COUNT];

INT32U allocation_size[MAX_ALLOCATION_COUNT];

char allocation_tag[MAX_ALLOCATION_COUNT][MAX_ALLOCATOR_TAG_SIZE];

INT32U available_id_stack[MAX_ALLOCATION_COUNT + 1];

INT32U availiable_stack_top = MAX_ALLOCATION_COUNT;

INT32U num_active_allocations = 0;

INT32U total_size = 0;

OS_EVENT* lock;

/*
 * Call init() only once
 */
void init(){
	INT8U i;
	lock = OSSemCreate(MAX_CONCURRENT_ACCESS);
	for(i = 0; i < MAX_ALLOCATION_COUNT; i++){
		allocation_addr[i] = NULL;
	}
	for(i = 1; i < MAX_ALLOCATION_COUNT + 1; i++){
		available_id_stack[i] = i;
	}
}

/*
 * Thread safety
 */
void request_lock(){
	INT8U err;
	OSSemPend(lock, LOCK_TIMEOUT, &err);
	if(err)
		printf("error pending on mem lock\n");
}

void release_lock(){
	OSSemPost(lock);
}

INT32U get_num_active_allocations(){
	return num_active_allocations;
}

/*
 * Call this function for every unique request_malloc() you have in your code
 *
 * Save this id somewhere in your code as a constant
 * And pass this id when you call request_malloc()
 */
INT32U get_new_allocation_id(){
	if(availiable_stack_top == 0){
		printf("At most can have %d number of unique malloc calls\n", MAX_ALLOCATION_COUNT);
		return ~0;
	}
	return available_id_stack[availiable_stack_top--];
}

/*
 * Allocate more memory
 */
void* request_malloc(INT32U allocation_id, INT32U size_bytes, char* tag){
	request_lock();
	void* ptr = malloc(size_bytes);
	if(ptr == NULL){
		return NULL;
	}
	INT32U id = allocation_id - 1;
	if(allocation_addr[id] != NULL){
		printf("Error, allocator %lu cannot allocate more memory unless previous allocation at %p is freed", allocation_id, allocation_addr[id]);
		return NULL;
	}
	num_active_allocations++;
	allocation_addr[id] = ptr;
	allocation_size[id] = size_bytes;
	snprintf(allocation_tag[id], MAX_ALLOCATOR_TAG_SIZE, tag);
	total_size += size_bytes;
	release_lock();
	return ptr;
}

/*
 * Free allocated memory
 */
void free_existing_malloc(INT32U allocation_id){
	request_lock();
	if(allocation_id == 0 || allocation_id > MAX_ALLOCATION_COUNT){
		return;
	}
	if(allocation_addr[allocation_id] == NULL){
		return;
	}
	INT32U id = allocation_id - 1;
	num_active_allocations--;
	total_size -= allocation_size[id];
	allocation_size[id] = 0;
	snprintf(allocation_tag[id], MAX_ALLOCATOR_TAG_SIZE, " ");
	free(allocation_addr[id]);
	allocation_addr[id] = NULL;
	release_lock();
}

/*
 * Display current memory usage
 */
void print_malloc_status(){
	INT8U i;
	INT32U end = availiable_stack_top == 0 ? 0 : availiable_stack_top - 1;
	printf("%lu currently active allocations\n", num_active_allocations);
	printf("%lu bytes memory allocated using mem_manager\n", total_size);
	printf("\n");
	for(i = MAX_ALLOCATION_COUNT; i > end; i--){
		if(allocation_addr[i] == NULL){
			continue;
		}
		printf("allocator id: %d\n", i);
		printf("address: %p\n", allocation_addr[i]);
		printf("tag: %s\n", allocation_tag[i]);
		printf("bytes allocated: %lu\n", allocation_size[i]);
		printf("\n");
	}
}



