/*
 * mem_manager.h
 *
 * Status: E
 *
 *  Created on: Mar 26, 2017
 *      Author: xuefei1
 *
 *  Header file for run time memory checker
 */

#ifndef MEM_MANAGER_H_
#define MEM_MANAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include "includes.h"

#define INVALID_MEM_ADDR			NULL

/*
 * Supports upto MAX_ALLOCATION_COUNT different malloc calls
 * If the same malloc call is used in a loop, make sure they have the same allocator id
 */
#define MAX_ALLOCATION_COUNT		32

#define MAX_ALLOCATOR_TAG_SIZE		128

#define MAX_CONCURRENT_ACCESS		1

#define LOCK_TIMEOUT				0

void mem_manager_init();

INT32U get_new_allocation_id();

INT32U get_num_active_allocations();

void* request_malloc(INT32U allocation_id, INT32U size_bytes, char* tag);

void free_existing_malloc(INT32U allocation_id);

void print_malloc_status();

#endif /* MEM_MANAGER_H_ */
