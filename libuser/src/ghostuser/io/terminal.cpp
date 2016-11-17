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

#include <ghostuser/io/terminal.hpp>
#include <ghostuser/tasking/lock.hpp>
#include <ghost.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

/**
 * Use to buffer stray input whne responses to terminal requests are read.
 */
static int* buffer = nullptr;
static int bufferedChars = 0;
static int bufferSize = 64;
static g_lock bufferLock;

/**
 *
 */
void g_terminal::setEcho(bool echo) {

	std::cout << (char) G_TERMKEY_ESC << "{" << (echo ? "1" : "0") << "e";
	std::flush(std::cout);
}

/**
 *
 */
void g_terminal::setMode(g_terminal_mode mode) {

	std::cout << (char) G_TERMKEY_ESC << "{" << mode << "m";
	std::flush(std::cout);
}

/**
 *
 */
int g_terminal::readUnbuffered() {

	int c = getc(stdin);

	// Escaped sequences
	if (c == G_TERMKEY_SUB) {
		int b1 = getc(stdin);
		int b2 = getc(stdin);
		return ((b2 << 8) | b1);
	}

	return c;
}

/**
 *
 */
void g_terminal::bufferChar(int c) {

	bufferLock.lock();

	// Create buffer
	if (buffer == 0) {
		buffer = new int[bufferSize];
		if (!buffer) {
			klog("failed to buffer char in terminal client, could not allocate buffer");
			return;
		}

		bufferedChars = 0;
	}

	// Put char in buffer
	if (bufferedChars < bufferSize) {
		buffer[bufferedChars++] = c;
	}

	bufferLock.unlock();
}

/**
 *
 */
void g_terminal::readAndBufferUntilESC() {

	int c;
	while ((c = readUnbuffered()) != G_TERMKEY_ESC) {
		bufferChar(c);
	}
	// now ESC was read
}

/**
 *
 */
int g_terminal::getChar() {

	int c;
	bufferLock.lock();

	// If there are chars in the buffer, take these
	if (buffer && bufferedChars > 0) {
		c = buffer[0];
		memcpy(buffer, &buffer[1], (bufferSize - 1) * sizeof(int));
		--bufferedChars;

	} else {
		c = readUnbuffered();
	}

	bufferLock.unlock();
	return c;
}

/**
 *
 */
void g_terminal::putChar(int c) {
	std::cout << (char) G_TERMKEY_ESC << "{" << c << "p";
	std::flush(std::cout);
}

/**
 *
 */
void g_terminal::setCursor(g_term_cursor_position position) {
	std::cout << (char) G_TERMKEY_ESC << "[" << position.y << ";" << position.x << "f";
	std::flush(std::cout);
}

/**
 *
 */
int g_terminal::readEscapedParameters(int* parameters) {

	int parameterCount = 0;

	int c;
	while (true) {
		c = readUnbuffered();

		if (c >= '0' && c <= '9') {
			if (parameterCount == 0) {
				parameterCount = 1;
			}
			if (parameterCount <= TERMINAL_STREAM_CONTROL_MAX_PARAMETERS) {
				parameters[parameterCount - 1] = parameters[parameterCount - 1] * 10;
				parameters[parameterCount - 1] += c - '0';
			}

		} else if (c == ';') {
			parameterCount++;

		} else {
			break;
		}
	}

	return c;
}

/**
 *
 */
g_term_cursor_position g_terminal::getCursor() {

	// read request
	std::cout << (char) G_TERMKEY_ESC << "[6n";
	std::flush(std::cout);

	// read response
	readAndBufferUntilESC();

	int ch = readUnbuffered();
	if (ch == '[') {
		int parameters[TERMINAL_STREAM_CONTROL_MAX_PARAMETERS];
		for (int i = 0; i < TERMINAL_STREAM_CONTROL_MAX_PARAMETERS; i++) {
			parameters[i] = 0;
		}

		int mode = readEscapedParameters(parameters);

		if (mode == 'R') {
			g_term_cursor_position result;
			result.y = parameters[0];
			result.x = parameters[1];
			return result;
		}
	}

	g_term_cursor_position pos;
	pos.y = -1;
	pos.x = -1;
	return pos;
}

/**
 *
 */
void g_terminal::moveCursorUp(int n) {
	std::cout << (char) G_TERMKEY_ESC << "[" << n << "A";
	std::flush(std::cout);
}

/**
 *
 */
void g_terminal::moveCursorDown(int n) {
	std::cout << (char) G_TERMKEY_ESC << "[" << n << "B";
	std::flush(std::cout);
}

/**
 *
 */
void g_terminal::moveCursorForward(int n) {
	std::cout << (char) G_TERMKEY_ESC << "[" << n << "C";
	std::flush(std::cout);
}

/**
 *
 */
void g_terminal::moveCursorBack(int n) {
	std::cout << (char) G_TERMKEY_ESC << "[" << n << "D";
	std::flush(std::cout);
}

/**
 *
 */
void g_terminal::setControlProcess(g_pid pid) {
	std::cout << (char) G_TERMKEY_ESC << "{" << pid << "c";
	std::flush(std::cout);
}