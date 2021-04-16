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

#include "ghost/user.h"
#include "__internal.h"

static g_bool handlingIrq = false;

/**
 * Bootstrap routine that is called everytime an IRQ is handled by a userspace handler.
 * The kernel passes the address of the handler to call and the IRQ number as parameters.
 */
void __g_enter_irq_handler(void (*handler)(uint8_t), uint8_t irq) {
	handlingIrq = true;
	handler(irq);
	handlingIrq = false;
}

/**
 *
 */
g_register_irq_handler_status g_register_irq_handler(uint8_t irq, void (*handler)(uint8_t)) {
	g_syscall_register_irq_handler data;
	data.irq = irq;
	data.handlerAddress = (uintptr_t) handler;
	data.entryAddress = (uintptr_t) __g_enter_irq_handler;
	data.returnAddress = (uintptr_t) __g_restore_interrupted_state_callback;
	g_syscall(G_SYSCALL_REGISTER_IRQ_HANDLER, (uint32_t) &data);
	return data.status;
}

g_bool g_is_handling_irq() {
	return handlingIrq;
}
