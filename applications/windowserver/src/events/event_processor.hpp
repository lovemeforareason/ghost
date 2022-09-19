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

#ifndef __WINDOWSERVER_EVENTS_EVENTPROCESSOR__
#define __WINDOWSERVER_EVENTS_EVENTPROCESSOR__

#include "input/keyboard/keyboard.hpp"
#include "input/mouse/mouse.hpp"
#include <deque>

#define DEFAULT_MULTICLICK_TIMESPAN 250

/**
 * The event queue is used to store any incoming events for
 * later processing.
 */
class event_processor_t
{
  public:
    uint32_t multiclickTimespan;

    event_processor_t();

    std::deque<g_key_info> key_info_buffer;
    uint8_t key_info_buffer_lock = 0;
    void bufferKeyEvent(g_key_info keyInfo);

    std::deque<void*> command_message_buffer;
    uint8_t command_message_buffer_lock = 0;
    // void bufferCommandMessage(void* commandMessage);

    void process();
    // void process_command(g_tid sender_tid, g_ui_message_header* request_header, command_message_response_t& response_out);

    void translateKeyEvent(g_key_info& info);
    void processMouseState();
};

#endif
