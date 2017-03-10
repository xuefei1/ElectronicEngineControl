/*
 * heap_trace.h
 *
 * Status: UC
 *
 *  Created on: Feb 3, 2017
 *      Author: Nicholas
 *
 *  Contains helper function definitions
 */

#ifndef HEAP_TRACE_H_
#define HEAP_TRACE_H_

#include "proj_config.h"

void* HeapTrace_Malloc(size_t size);
void HeapTrace_Free(void* memIn);
size_t HeapTrace_MaxAllocation();
size_t HeapTrace_MaxRunningAllocation();
size_t HeapTrace_LeakedAllocations();
void HeapTrace_PrintDiag();

#endif /* UTIL_H_ */