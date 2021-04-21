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

#include "kernel/tasking/scheduler.hpp"
#include "kernel/memory/heap.hpp"
#include "shared/logger/logger.hpp"
#include "kernel/tasking/wait.hpp"

void schedulerInitializeLocal()
{
	taskingGetLocal()->scheduling.round = 1;
}

void schedulerNewTimeSlot()
{
	taskingGetLocal()->scheduling.round++;
}

void schedulerPrepareEntry(g_schedule_entry* entry)
{
	entry->schedulerRound = 0;
}

/**
 * This scheduler implementation keeps a round counter on the
 * local tasking structure and each schedule entry. A new round
 * starts when a new timeslot starts.
 *
 * When looking for a new task to schedule, each task in the list
 * is checked once. Only tasks that have not been scheduled in this
 * round are taken into account.
 *
 * If all tasks are waiting/were already scheduled, the idle task is run.
 */
void schedulerSchedule(g_tasking_local* local)
{
	mutexAcquire(&local->lock);

	if(!local->scheduling.current)
	{
		local->scheduling.current = local->scheduling.list->task;
		mutexRelease(&local->lock);
		return;
	}

	// Find task in list
	g_task* searchTask = local->scheduling.current;

	g_schedule_entry* entry = local->scheduling.list;
	while(entry)
	{
		if(entry->task == searchTask)
		{
			break;
		}
		entry = entry->next;
	}
	if(!entry)
	{
		entry = local->scheduling.list;
	}

	bool switched = false;
	uint32_t max = local->scheduling.taskCount;
	while(max-- > 0 && entry)
	{
		/* If this task has already done some work this round, we skip it */
		if(entry->schedulerRound >= local->scheduling.round)
		{
			entry = entry->next;
			if(!entry)
			{
				entry = local->scheduling.list;
			}
			continue;
		}

		entry->schedulerRound = local->scheduling.round;

		/* Task is running, so we can schedule it */
		g_task* task = entry->task;
		if(task->status == G_THREAD_STATUS_RUNNING)
		{
			local->scheduling.current = task;
			switched = true;
			break;
		}

		/* If the task is waiting, we must see why */
		if(task->status == G_THREAD_STATUS_WAITING)
		{
			/* Now we check if the task can be woken up again. */
			bool wakeUp = waitTryWake(task);

			if(wakeUp)
			{
				local->scheduling.current = task;
				switched = true;
				break;
			}
		}

		/* Go to the next entry */
		entry = entry->next;
		if(!entry)
		{
			entry = local->scheduling.list;
		}
	}

	if(switched)
	{
		local->scheduling.current->timesScheduled++;
	}
	else
	{
		// Nothing to schedule, idle
		local->scheduling.current = local->scheduling.idleTask;
	}

	mutexRelease(&local->lock);
}

void schedulerDump()
{
	g_tasking_local* local = taskingGetLocal();
	mutexAcquire(&local->lock);

	logInfo("%! task list:", "scheduler");
	g_schedule_entry* entry = local->scheduling.list;
	while(entry)
	{
		const char* taskState;
		if(entry->task->status == G_THREAD_STATUS_RUNNING) {
			taskState = "running";
		}
		else if(entry->task->status == G_THREAD_STATUS_UNUSED) {
			taskState = "unused";
		}
		else if(entry->task->status == G_THREAD_STATUS_DEAD) {
			taskState = "dead";
		}
		else if(entry->task->status == G_THREAD_STATUS_WAITING) {
			taskState = "waiting";
		}

		logInfo("%# process: %i, task: %i, status: %s, timesScheduled: %i", entry->task->process->id, entry->task->id, taskState, entry->task->timesScheduled);
		entry = entry->next;
	}


	mutexRelease(&local->lock);
}
