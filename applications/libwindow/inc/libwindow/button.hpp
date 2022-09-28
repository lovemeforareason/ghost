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

#ifndef __LIBWINDOW_BUTTON__
#define __LIBWINDOW_BUTTON__

#include <cstdint>

#include "libwindow/action_component.hpp"
#include "libwindow/component.hpp"
#include "libwindow/titled_component.hpp"

class g_button : public g_component, public g_titled_component, public g_action_component
{
  protected:
	g_button(uint32_t id) : g_component(id), g_titled_component(id), g_action_component(this, id)
	{
	}

  public:
	static g_button* create();

	void setEnabled(bool enabled);
	bool isEnabled();
};

#endif
