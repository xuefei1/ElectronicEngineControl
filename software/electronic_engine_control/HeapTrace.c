/*
 * Copyright (C) 2015  Brendan Bruner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * bbruner@ualberta.ca
 */
/**
 * @file HeapTrace.c
 * @author Brendan Bruner
 * @date Jun 8, 2015
 * @modified by Nicholas Li
 * @Status: UC
 */
#include "heap_trace.h"

static size_t maxAllocation = 0;
static size_t runningAllocation = 0;
static size_t maxRunningAllocation = 0;
static size_t leakedAllocations = 0;

/**
 * Wrapper for malloc that will track dynamic memory allocation.
 * Memory allocated this way must be freed with HeapTrace_Free().
 */
void* HeapTrace_Malloc(size_t size)
{
  void* mem;

  mem = malloc(sizeof(size_t) + size);
  if(mem == NULL)
    {
      return NULL;
    }

  *((size_t*) mem) = size;

  ++leakedAllocations;
  if(size > maxAllocation)
    {
      maxAllocation = size;
    }
  runningAllocation += size;
  if(runningAllocation > maxRunningAllocation)
    {
      maxRunningAllocation = runningAllocation;
    }

  return (mem + sizeof(size_t));
}

/**
 * Free memory allocated by HeapTrace_Malloc().
 */
void HeapTrace_Free(void* memIn)
{
  void* memActual;
  size_t size;

  if(memIn == NULL)
    {
      return;
    }

  memActual = memIn - sizeof(size_t);
  size = *((size_t*) memActual);

  runningAllocation = runningAllocation - size;
  leakedAllocations = leakedAllocations - 1;

  free(memActual);
}

size_t HeapTrace_MaxAllocation()
{
  return maxAllocation;
}

size_t HeapTrace_MaxRunningAllocation()
{
  return maxRunningAllocation;
}

size_t HeapTrace_LeakedAllocations()
{
  return leakedAllocations;
}

/**
 * Print the current state of dynamic memory usage.
 */
void HeapTrace_PrintDiag()
{
  printf("\nLargest allocation request: \t\t%zu\n", HeapTrace_MaxAllocation());
  printf("Largest heap usage at one point: \t%zu\n", HeapTrace_MaxRunningAllocation());
  printf("Allocations currently leaked: \t\t%zu\n", HeapTrace_LeakedAllocations());
}
