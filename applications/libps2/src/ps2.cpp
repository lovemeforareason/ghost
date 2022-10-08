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

#include "libps2/ps2.hpp"
#include "ps2_intern.hpp"

#include <ghost/io.h>
#include <ghost/user.h>
#include <stdio.h>

uint8_t mouse_packet_number = 0;
uint8_t mouse_packet_buffer[4];

g_fd keyIrqIn;
g_fd mouseIrqIn;

uint32_t packets_count = 0;

void (*registeredMouseCallback)(int16_t, int16_t, uint8_t);
void (*registeredKeyboardCallback)(uint8_t);

ps2_status_t ps2Initialize(void (*mouseCallback)(int16_t, int16_t, uint8_t),
						   void (*keyboardCallback)(uint8_t))
{

	registeredMouseCallback = mouseCallback;
	registeredKeyboardCallback = keyboardCallback;

	ps2_status_t status = ps2InitializeMouse();
	if(status != G_PS2_STATUS_SUCCESS)
	{
		return status;
	}

	g_open_irq_device(1, &keyIrqIn);
	g_create_thread((void*) &ps2ReadKeyIrq);
	g_open_irq_device(12, &mouseIrqIn);
	g_create_thread((void*) &ps2ReadMouseIrq);
	return G_PS2_STATUS_SUCCESS;
}

void ps2ReadKeyIrq()
{
	uint8_t buf[1];
	for(;;)
	{
		int32_t r = g_read(keyIrqIn, buf, 1);
		ps2IrqHandler(buf[0]);
	}
}

void ps2ReadMouseIrq()
{
	uint8_t buf[1];
	for(;;)
	{
		int32_t r = g_read(mouseIrqIn, buf, 1);
		ps2IrqHandler(buf[0]);
	}
}

void ps2IrqHandler(uint8_t irq)
{
	uint8_t status;
	while(((status = ioInportByte(G_PS2_STATUS_PORT)) & 0x01) != 0)
	{
		uint8_t value = ioInportByte(G_PS2_DATA_PORT);

		if((status & 0x20) == 0)
		{
			if(registeredKeyboardCallback)
			{
				registeredKeyboardCallback(value);
			}
		}
		else
		{
			ps2HandleMouseData(value);
		}

		++packets_count;
	}
}

ps2_status_t ps2InitializeMouse()
{

	// empty input buffer
	while(ioInportByte(G_PS2_STATUS_PORT) & 0x01)
	{
		ioInportByte(G_PS2_DATA_PORT);
	}

	// activate mouse device
	ps2WaitForBuffer(PS2_OUT);
	ioOutportByte(G_PS2_STATUS_PORT, 0xA8);
	ps2WaitForBuffer(PS2_IN);
	ioInportByte(G_PS2_DATA_PORT);

	// get commando-byte, set bit 1 (enables IRQ12), send back
	ps2WaitForBuffer(PS2_OUT);
	ioOutportByte(G_PS2_STATUS_PORT, 0x20);

	ps2WaitForBuffer(PS2_IN);
	uint8_t status = (ioInportByte(G_PS2_DATA_PORT) | 0x02) & (~0x10);

	ps2WaitForBuffer(PS2_OUT);
	ioOutportByte(G_PS2_STATUS_PORT, 0x60);
	ps2WaitForBuffer(PS2_OUT);
	ioOutportByte(G_PS2_DATA_PORT, status);

	// send set-default-settings command to mouse
	if(ps2WriteToMouse(0xF6))
	{
		klog("error: mouse did not acknowledge setting defaults command");
		return G_PS2_STATUS_FAILED_INITIALIZE;
	}

	// enable the mouse
	if(ps2WriteToMouse(0xF4))
	{
		klog("mouse did not acknowledge enable-mouse command");
		return G_PS2_STATUS_FAILED_INITIALIZE;
	}

	return G_PS2_STATUS_SUCCESS;
}

void ps2HandleMouseData(uint8_t value)
{

	switch(mouse_packet_number)
	{
	case 0:
		mouse_packet_buffer[0] = value;

		if((value & 0x08) == 0)
		{
			mouse_packet_number = 0; // otherwise restart the cycle
		}
		else
		{
			mouse_packet_number = 1;
		}
		break;

	case 1:
		mouse_packet_buffer[1] = value;
		mouse_packet_number = 2;
		break;

	case 2:
		mouse_packet_buffer[2] = value;

		int8_t flags = mouse_packet_buffer[0];
		uint8_t valX = mouse_packet_buffer[1];
		uint8_t valY = mouse_packet_buffer[2];

		if((flags & (1 << 6)) || (flags & (1 << 7)))
		{
			// ignore overflowing values
		}
		else
		{
			int16_t offX = valX - ((flags << 4) & 0x100);
			int16_t offY = valY - ((flags << 3) & 0x100);

			if(registeredMouseCallback)
			{
				registeredMouseCallback(offX, -offY, flags);
			}
		}

		mouse_packet_number = 0;
		break;
	}
}

void ps2WaitForBuffer(ps2_buffer_t buffer)
{

	uint8_t requiredBit;
	uint8_t requiredValue;

	if(buffer == PS2_OUT)
	{
		requiredBit = 0x02;
		requiredValue = 0;
	}
	else if(buffer == PS2_IN)
	{
		requiredBit = 0x01;
		requiredValue = 1;
	}

	int timeout = 100;
	while(timeout--)
	{
		if((ioInportByte(G_PS2_STATUS_PORT) & requiredBit) == requiredValue)
		{
			return;
		}
		g_yield();
	}
}

int ps2WriteToMouse(uint8_t value)
{

	ps2WaitForBuffer(PS2_OUT);
	ioOutportByte(G_PS2_STATUS_PORT, 0xD4);

	ps2WaitForBuffer(PS2_OUT);
	ioOutportByte(G_PS2_DATA_PORT, value);

	ps2WaitForBuffer(PS2_IN);
	if(ioInportByte(G_PS2_DATA_PORT) != 0xFA)
	{
		return 1;
	}
	return 0;
}
