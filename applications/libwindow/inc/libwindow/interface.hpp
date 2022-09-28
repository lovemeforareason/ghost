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

#ifndef __LIBWINDOW_INTERFACE__
#define __LIBWINDOW_INTERFACE__

#include <ghost.h>
#include <libinput/keyboard/keyboard.hpp>

#include "libwindow/metrics/dimension.hpp"
#include "libwindow/metrics/rectangle.hpp"

/**
 * This UI interface specification defines the messages
 * that the active window manager must understand.
 */
#define G_UI_REGISTRATION_THREAD_IDENTIFIER "windowserver/registration"

/**
 * Size of the largest expected message
 */
#define G_UI_MAXIMUM_MESSAGE_SIZE 4096

#define G_UI_COMPONENT_TITLE_MAXIMUM 1024

/**
 * Declared in the UI unit
 */
extern bool g_ui_initialized;

/**
 * ID for a UI component
 */
typedef int32_t g_ui_component_id;

/**
 * ID for a listener
 */
typedef int32_t g_ui_listener_id;

/**
 * A protocol message always starts with the header, the message id
 */
typedef uint8_t g_ui_protocol_command_id;
const g_ui_protocol_command_id G_UI_PROTOCOL_INITIALIZATION = 1;
const g_ui_protocol_command_id G_UI_PROTOCOL_CREATE_COMPONENT = 2;
const g_ui_protocol_command_id G_UI_PROTOCOL_ADD_COMPONENT = 3;
const g_ui_protocol_command_id G_UI_PROTOCOL_SET_TITLE = 4;
const g_ui_protocol_command_id G_UI_PROTOCOL_GET_TITLE = 5;
const g_ui_protocol_command_id G_UI_PROTOCOL_SET_BOUNDS = 6;
const g_ui_protocol_command_id G_UI_PROTOCOL_SET_VISIBLE = 7;
const g_ui_protocol_command_id G_UI_PROTOCOL_SET_LISTENER = 8;
const g_ui_protocol_command_id G_UI_PROTOCOL_SET_NUMERIC_PROPERTY = 9;
const g_ui_protocol_command_id G_UI_PROTOCOL_GET_NUMERIC_PROPERTY = 10;
const g_ui_protocol_command_id G_UI_PROTOCOL_CANVAS_ACK_BUFFER_REQUEST = 11;
const g_ui_protocol_command_id G_UI_PROTOCOL_GET_BOUNDS = 12;
const g_ui_protocol_command_id G_UI_PROTOCOL_CANVAS_BLIT = 13;
const g_ui_protocol_command_id G_UI_PROTOCOL_REGISTER_DESKTOP_CANVAS = 14;
const g_ui_protocol_command_id G_UI_PROTOCOL_GET_SCREEN_DIMENSION = 15;

/**
 * Common status for requests
 */
typedef uint8_t g_ui_protocol_status;
const g_ui_protocol_status G_UI_PROTOCOL_SUCCESS = 0;
const g_ui_protocol_status G_UI_PROTOCOL_FAIL = 1;

/**
 * Component types
 */
typedef uint32_t g_ui_component_type;
const g_ui_component_type G_UI_COMPONENT_TYPE_WINDOW = 0;
const g_ui_component_type G_UI_COMPONENT_TYPE_BUTTON = 1;
const g_ui_component_type G_UI_COMPONENT_TYPE_LABEL = 2;
const g_ui_component_type G_UI_COMPONENT_TYPE_TEXTFIELD = 3;
const g_ui_component_type G_UI_COMPONENT_TYPE_CANVAS = 4;

/**
 * Types of events that can be listened to
 */
typedef uint32_t g_ui_component_event_type;
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_ACTION = 0;
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_BOUNDS = 1;
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_CANVAS_WFA = 2; // "wait for acknowledge"-event
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_KEY = 3;
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_FOCUS = 4;
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_MOUSE = 5;
const g_ui_component_event_type G_UI_COMPONENT_EVENT_TYPE_CLOSE = 6;

/**
 *
 */
typedef uint8_t g_ui_layout_manager;
#define G_UI_LAYOUT_MANAGER_GRID ((g_ui_layout_manager) 0)
#define G_UI_LAYOUT_MANAGER_FLOW ((g_ui_layout_manager) 1)

/**
 *
 */
typedef struct
{
	g_ui_protocol_command_id id;
} __attribute__((packed)) g_ui_message_header;

/**
 * Request to initialize interface communications. The window server creates a
 * delegate thread that is responsible for further communications and responds
 * with a <g_ui_initialize_response>.
 */
typedef struct
{
	g_ui_message_header header;
} __attribute__((packed)) g_ui_initialize_request;

/**
 * Response for initializing interface communications.
 *
 * @field status
 * 		whether the initialization was successful
 * @field window_server_delegate_thread
 * 		id of the thread that is responsible for further window server communication
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
	g_tid window_server_delegate_thread;
} __attribute__((packed)) g_ui_initialize_response;

/**
 * Request sent to create a component.
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_type type;
} __attribute__((packed)) g_ui_create_component_request;

/**
 * Response when creating a component.
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
	g_ui_component_id id;
} __attribute__((packed)) g_ui_create_component_response;

/**
 * Request/response for adding a child
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id parent;
	g_ui_component_id child;
} __attribute__((packed)) g_ui_component_add_child_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_component_add_child_response;

/**
 * Request/response for setting bounds
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
	g_rectangle bounds;
} __attribute__((packed)) g_ui_component_set_bounds_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_component_set_bounds_response;

/**
 * Request/response for getting bounds
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
} __attribute__((packed)) g_ui_component_get_bounds_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
	g_rectangle bounds;
} __attribute__((packed)) g_ui_component_get_bounds_response;

/**
 * Request/response for setting components (in)visible
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
	bool visible;
} __attribute__((packed)) g_ui_component_set_visible_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_component_set_visible_response;

/**
 * Request/response for setting the title on a titled component
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
	char title[G_UI_COMPONENT_TITLE_MAXIMUM];
} __attribute__((packed)) g_ui_component_set_title_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_component_set_title_response;

/**
 * Request/response for getting the title on a titled component
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
} __attribute__((packed)) g_ui_component_get_title_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
	char title[G_UI_COMPONENT_TITLE_MAXIMUM];
} __attribute__((packed)) g_ui_component_get_title_response;

/**
 * Request/response for getting a numeric property
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
	int property;
} __attribute__((packed)) g_ui_component_get_numeric_property_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
	uint32_t value;
} __attribute__((packed)) g_ui_component_get_numeric_property_response;

/**
 * Request/response for setting a numeric property
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
	int property;
	uint32_t value;
} __attribute__((packed)) g_ui_component_set_numeric_property_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_component_set_numeric_property_response;

/**
 * Request/response for acknowledging the buffer update of a canvas/blitting the canvas
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
} __attribute__((packed)) g_ui_component_canvas_ack_buffer_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
} __attribute__((packed)) g_ui_component_canvas_blit_request;

/**
 * Request to register the desktop canvas
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id canvas_id;
} __attribute__((packed)) g_ui_register_desktop_canvas_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_register_desktop_canvas_response;

/**
 * Event handler registration functions
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_id id;
	g_ui_component_event_type event_type;
	g_tid target_thread;
} __attribute__((packed)) g_ui_component_set_listener_request;

typedef struct
{
	g_ui_message_header header;
	g_ui_protocol_status status;
} __attribute__((packed)) g_ui_component_set_listener_response;

/**
 * Retrieve screen size
 */
typedef struct
{
	g_ui_message_header header;
} __attribute__((packed)) g_ui_get_screen_dimension_request;

typedef struct
{
	g_ui_message_header header;
	g_dimension size;
} __attribute__((packed)) g_ui_get_screen_dimension_response;

/**
 * Event structures
 */
typedef struct
{
	g_ui_message_header header;
	g_ui_component_event_type type;
	g_ui_component_id component_id;
} __attribute__((packed)) g_ui_component_event_header;

typedef struct
{
	g_ui_component_event_header header;
} __attribute__((packed)) g_ui_component_action_event;

typedef struct
{
	g_ui_component_event_header header;
	g_rectangle bounds;
} __attribute__((packed)) g_ui_component_bounds_event;

typedef struct
{
	g_ui_component_event_header header;
	g_address newBufferAddress;
} __attribute__((packed)) g_ui_component_canvas_wfa_event;

typedef struct
{
	g_ui_component_event_header header;
	g_key_info_basic key_info;
} __attribute__((packed)) g_ui_component_key_event;

typedef struct
{
	g_ui_component_event_header header;
	uint8_t now_focused;
} __attribute__((packed)) g_ui_component_focus_event;

typedef struct
{
	g_ui_component_event_header header;
} __attribute__((packed)) g_ui_component_close_event;

/**
 * Mouse events
 */
typedef uint8_t g_mouse_button;
#define G_MOUSE_BUTTON_NONE ((g_mouse_button) 0x0)
#define G_MOUSE_BUTTON_1 ((g_mouse_button) 0x1)
#define G_MOUSE_BUTTON_2 ((g_mouse_button) 0x2)
#define G_MOUSE_BUTTON_3 ((g_mouse_button) 0x4)

typedef uint8_t g_mouse_event_type;
#define G_MOUSE_EVENT_NONE ((g_mouse_event_type) 0)
#define G_MOUSE_EVENT_MOVE ((g_mouse_event_type) 1)
#define G_MOUSE_EVENT_PRESS ((g_mouse_event_type) 2)
#define G_MOUSE_EVENT_RELEASE ((g_mouse_event_type) 3)
#define G_MOUSE_EVENT_DRAG_RELEASE ((g_mouse_event_type) 4)
#define G_MOUSE_EVENT_DRAG ((g_mouse_event_type) 5)
#define G_MOUSE_EVENT_ENTER ((g_mouse_event_type) 6)
#define G_MOUSE_EVENT_LEAVE ((g_mouse_event_type) 7)

typedef struct
{
	g_ui_component_event_header header;
	g_point position;
	g_mouse_event_type type;
	g_mouse_button buttons;
} __attribute__((packed)) g_ui_component_mouse_event;

/**
 * Canvas shared memory header
 */
typedef struct
{
	uint16_t paintable_width;
	uint16_t paintable_height;
	uint16_t blit_x;
	uint16_t blit_y;
	uint16_t blit_width;
	uint16_t blit_height;
	g_bool ready;
} __attribute__((packed)) g_ui_canvas_shared_memory_header;

/**
 * Cairo requires the canvas memory buffer to be aligned. This constant must be used to calculate
 * the address for the canvas buffer in the canvas shared memory.
 */
#define G_UI_CANVAS_SHARED_MEMORY_HEADER_SIZE ((sizeof(g_ui_canvas_shared_memory_header) - sizeof(g_ui_canvas_shared_memory_header) % sizeof(g_address)) + sizeof(g_address))

#endif
