/**************************************************************************/
/*  keyboard_switch.h                                                     */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef KEYBOARD_SWITCH_H
#define KEYBOARD_SWITCH_H

#include "switch_wrapper.h"

#include <core/input/input.h>

struct KeyboardSwitchState {
	bool _opened;
	int _events = 0;
	u32 _stringLen = 0;
	s32 _cursorPos = 0;
};

class KeyboardSwitch {
private:
	SwkbdInline _keyboard;
	KeyboardSwitchState _state;

	static KeyboardSwitch* _instance;

protected:
	KeyboardSwitch();
	virtual ~KeyboardSwitch();

public:
	static KeyboardSwitch* get();

	void initialize();

	const KeyboardSwitchState& state() const { return _state;}
	KeyboardSwitchState& state() { return _state;}

	void show(const String &current);
	void hide();

	void key_event(Key key, bool pressed = true);

	void process();
};

void keyboard_string_changed_callback(const char *str, SwkbdChangedStringArg *arg);
void keyboard_moved_cursor_callback(const char *str, SwkbdMovedCursorArg *arg);
void keyboard_decided_enter_callback(const char *str, SwkbdDecidedEnterArg *arg);
void keyboard_decided_cancel_callback();

#endif // KEYBOARD_SWITCH_H