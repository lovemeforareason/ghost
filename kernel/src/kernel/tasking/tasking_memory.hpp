/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __KERNEL_TASKING_MEMORY__
#define __KERNEL_TASKING_MEMORY__

#include "kernel/tasking/tasking.hpp"

#define G_TASKING_MEMORY_INTERRUPT_STACK_PAGES 2
#define G_TASKING_MEMORY_KERNEL_STACK_PAGES 2
#define G_TASKING_MEMORY_USER_STACK_PAGES 10

/**
 * Extends the heap of the task by an amount.
 */
bool taskingMemoryExtendHeap(g_task* task, int32_t amount, uint32_t* outAddress);

/**
 * Creates the stacks for a newly created task.
 *
 * For a task running on kernel-level, only the main stack is created. On interrupt
 * handling the current state is always pushed on top of this stack.
 *
 * For user-space tasks, there is a user-space stack and a dedicated interrupt stack
 * that is switched to when handling interrupts.
 */
void taskingMemoryCreateStacks(g_task* task);

/**
 * Creates a new page directory to use for a new process. Clones the kernel space
 * into the page directory, maps the lower memory and adds recursive mapping.
 *
 * @return the physical address of the directory
 */
g_physical_address taskingMemoryCreatePageDirectory();

/**
 * Creates and maps a stack.
 */
g_stack taskingMemoryCreateStack(g_address_range_pool* addressRangePool, uint32_t tableFlags, uint32_t pageFlags, int pages);

#endif
