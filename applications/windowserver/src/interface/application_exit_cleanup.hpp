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

#ifndef __WINDOWSERVER_INTERFACE_APPLICATIONEXITCLEANUP__
#define __WINDOWSERVER_INTERFACE_APPLICATIONEXITCLEANUP__

#include "interface/interface_receiver.hpp"

class application_exit_cleanup_handler_t
{
  private:
	g_tid pid;
	interface_receiver_t* interfaceReceiver;

  public:
	application_exit_cleanup_handler_t(g_pid pid, interface_receiver_t* interfaceReceiver) : pid(pid), interfaceReceiver(interfaceReceiver)
	{
	}

	virtual void run();
};

void interfaceApplicationExitCleanupThread(application_exit_cleanup_handler_t* handler);

#endif
