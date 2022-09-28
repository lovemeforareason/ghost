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

#ifndef __LIBWINDOW_BOUNDSLISTENER__
#define __LIBWINDOW_BOUNDSLISTENER__

#include <cstdint>

#include "libwindow/listener/listener.hpp"

class g_component;

class g_bounds_event
{
	g_component *component;
};

class g_bounds_listener : public g_listener
{
  public:
	virtual ~g_bounds_listener()
	{
	}

	virtual void process(g_ui_component_event_header *header)
	{
		g_ui_component_bounds_event *bounds_event = (g_ui_component_bounds_event *) header;
		handle_bounds_changed(bounds_event->bounds);
	}

	virtual void handle_bounds_changed(g_rectangle bounds) = 0;
};

#endif
