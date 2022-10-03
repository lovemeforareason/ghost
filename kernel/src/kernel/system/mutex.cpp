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

#include "shared/system/mutex.hpp"
#include "kernel/kernel.hpp"
#include "kernel/system/interrupts/interrupts.hpp"
#include "kernel/system/system.hpp"
#include "kernel/tasking/tasking.hpp"
#include "shared/logger/logger.hpp"
#include "shared/video/console_video.hpp"

volatile int mutexInitializerLock = 0;
#define G_MUTEX_INITIALIZED 0xFEED

#define SPINLOCK_ACQUIRE(lock)                        \
	while(!__sync_bool_compare_and_swap(&lock, 0, 1)) \
		asm volatile("pause");
#define SPINLOCK_RELEASE(lock) lock = 0;
#define MUTEX_GUARD                               \
	if(mutex->initialized != G_MUTEX_INITIALIZED) \
		mutexErrorUninitialized(mutex);

void mutexErrorUninitialized(g_mutex* mutex)
{
	kernelPanic("%! %i: tried to use uninitialized mutex %h", "mutex", processorGetCurrentId(), mutex);
}

void _mutexInitialize(g_mutex* mutex)
{
	SPINLOCK_ACQUIRE(mutexInitializerLock);

	if(mutex->initialized == G_MUTEX_INITIALIZED)
		logWarn("%! initializing mutex %x twice", "mutex", mutex);

	mutex->initialized = G_MUTEX_INITIALIZED;
	mutex->lock = 0;
	mutex->depth = 0;
	mutex->owner = -1;

	SPINLOCK_RELEASE(mutexInitializerLock);
}

void mutexAcquire(g_mutex* mutex)
{
	MUTEX_GUARD;

	uint32_t owner = systemIsReady() ? taskingGetCurrentTask()->id : -processorGetCurrentId();

	while(!mutexTryAcquire(mutex, owner))
	{
		if(systemIsReady())
			taskingYield();
		else
			asm volatile("pause");
	}
}

bool mutexTryAcquire(g_mutex* mutex, uint32_t owner)
{
	MUTEX_GUARD;

	bool set = false;

	int intr = interruptsAreEnabled();
	if(intr)
		interruptsDisable();

	SPINLOCK_ACQUIRE(mutex->lock);

	if(mutex->depth == 0 || mutex->owner == owner)
	{
		++mutex->depth;
		mutex->owner = owner;
		set = true;
	}

	SPINLOCK_RELEASE(mutex->lock);

	if(intr)
		interruptsEnable();

	return set;
}

void mutexRelease(g_mutex* mutex)
{
	MUTEX_GUARD;

	int intr = interruptsAreEnabled();
	if(intr)
		interruptsDisable();

	SPINLOCK_ACQUIRE(mutex->lock);

	if(mutex->depth > 0)
	{
		--mutex->depth;
		if(mutex->depth == 0)
		{
			mutex->depth = 0;
			mutex->owner = -1;
		}
	}

	SPINLOCK_RELEASE(mutex->lock);

	if(intr)
		interruptsEnable();
}
