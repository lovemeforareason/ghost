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

#ifndef __TERMINAL_GUISCREEN_RASTER__
#define __TERMINAL_GUISCREEN_RASTER__

#include <ghost.h>

/**
 *
 */
class raster_t
{
  private:
	g_atom lock = g_atomic_initialize();
	uint8_t* buffer = 0;

	int width = 0;
	int height = 0;

  public:
	int getWidth() const
	{
		return width;
	}
	int getHeight() const
	{
		return height;
	}

	void scrollBy(int y);
	void resizeTo(int width, int height);
	void clean();
	void put(int x, int y, uint8_t c);

	void lockBuffer();
	uint8_t getUnlocked(int x, int y);
	void unlockBuffer();
};

#endif