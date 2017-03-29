#include <stdlib.h>
#include <stdio.h>
#include "mem_manager.c"

void main(int argc, char *argv[]){
	//1st case
	printf("1st test case:\n");
	int allocation_id = 16;
	int size_bytes = 32;
	char* tag = "A";
	
	mem_manager_init();
	
	int new_id = get_new_allocation_id();
	printf("new id is %d\n", new_id);

	int new_number = get_num_active_allocations();
	printf("new number is %d\n", new_number);

	request_malloc(new_id, size_bytes, tag);
	
	print_malloc_status();
	free_existing_malloc(new_id);
	printf("\n");
	print_malloc_status();
	
	//2nd case
	printf("2nd test case:\n");
	int allocation_id = 32;
	int size_bytes = 32;
	char* tag = "C";
	
	mem_manager_init();
	
	int new_id = get_new_allocation_id();
	printf("new id is %d\n", new_id);

	int new_number = get_num_active_allocations();
	printf("new number is %d\n", new_number);

	request_malloc(new_id, size_bytes, tag);
	
	print_malloc_status();
	printf("\n");
	free_existing_malloc(new_id);
	printf("\n");
	print_malloc_status();
	
	//3rd case
	printf("3rd test case:\n");
	int allocation_id = 64;
	int size_bytes = 64;
	char* tag = "B";
	
	mem_manager_init();
	
	int new_id = get_new_allocation_id();
	printf("new id is %d\n", new_id);

	int new_number = get_num_active_allocations();
	printf("new number is %d\n", new_number);

	request_malloc(new_id, size_bytes, tag);
	
	print_malloc_status();
	printf("\n");
	free_existing_malloc(new_id);
	printf("\n");
	print_malloc_status();
}