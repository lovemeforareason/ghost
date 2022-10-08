/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2022, Max Schlüssel <lokoxe@gmail.com>                     *
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

#include "components/panel.hpp"

void panel_t::paint()
{
    if(background == ARGB(0, 0, 0, 0))
        return;

    auto cr = graphics.getContext();
    cairo_set_source_rgba(cr, ARGB_FR_FROM(background), ARGB_FG_FROM(background), ARGB_FB_FROM(background), ARGB_FA_FROM(background));
    cairo_rectangle(cr, 0, 0, getBounds().width, getBounds().height);
    cairo_fill(cr);
}

void panel_t::setBackground(g_color_argb color)
{
    background = color;
    markFor(COMPONENT_REQUIREMENT_PAINT);
}

g_color_argb panel_t::getBackground()
{
    return background;
}
