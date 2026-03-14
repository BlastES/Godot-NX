/**************************************************************************/
/*  keyboard_switch.cpp                                                   */
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

#include "keyboard_switch.h"

#include <iostream>

KeyboardSwitch* KeyboardSwitch::_instance = nullptr;

KeyboardSwitch::KeyboardSwitch() {
}

KeyboardSwitch::~KeyboardSwitch() {
}

KeyboardSwitch *KeyboardSwitch::get() {
	if (_instance == nullptr) {
		_instance = new KeyboardSwitch();
	}
	return _instance;
}

void KeyboardSwitch::initialize() {
	swkbdInlineCreate(&_keyboard);

	swkbdInlineLaunchForLibraryApplet(&_keyboard, SwkbdInlineMode_AppletDisplay, 0);
	swkbdInlineSetChangedStringCallback(&_keyboard, keyboard_string_changed_callback);
	swkbdInlineSetMovedCursorCallback(&_keyboard, keyboard_moved_cursor_callback);
	swkbdInlineSetDecidedEnterCallback(&_keyboard, keyboard_decided_enter_callback);
	swkbdInlineSetDecidedCancelCallback(&_keyboard, keyboard_decided_cancel_callback);
}

void KeyboardSwitch::show(const String &current) {
	if (!_state._opened) {
		_state._opened = true;
		SwkbdAppearArg arg;
		swkbdInlineMakeAppearArg(&arg, SwkbdType_Normal);
		swkbdInlineSetInputText(&_keyboard, current.utf8().get_data());
		swkbdInlineSetCursorPos(&_keyboard, current.size() - 1);
		swkbdInlineAppear(&_keyboard, &arg);
	}
}

void KeyboardSwitch::hide() {
	_state._opened = false;
	swkbdInlineDisappear(&_keyboard);
}

void KeyboardSwitch::key_event(Key key, bool pressed) {
	Ref<InputEventKey> ev;
	ev.instantiate();
	ev->set_echo(false);
	ev->set_pressed(pressed);
	ev->set_keycode(key);
	Input::get_singleton()->parse_input_event(ev);
};

void KeyboardSwitch::process()
{
	swkbdInlineUpdate(&_keyboard, NULL);
}

void keyboard_string_changed_callback(const char *str, SwkbdChangedStringArg *arg) {
	std::cout << "keyboard_string_changed_callback: " << arg->stringLen << " " << str << std::endl;

	// We get a string changed event on appear, and another one on setting text.
	if (KeyboardSwitch::get()->state()._events) {
		KeyboardSwitch::get()->state()._events--;
	} else {
		if (arg->stringLen < KeyboardSwitch::get()->state()._stringLen) {
			KeyboardSwitch::get()->key_event(Key::BACKSPACE);
		} else if (arg->stringLen > 0) {
			KeyboardSwitch::get()->key_event((Key)str[arg->stringLen - 1]);
		}
	}
	KeyboardSwitch::get()->state()._stringLen = arg->stringLen;
}

void keyboard_moved_cursor_callback(const char *str, SwkbdMovedCursorArg *arg) {
	std::cout << "keyboard_moved_cursor_callback: " << arg->cursorPos << " " << str << std::endl;

	if (arg->cursorPos < KeyboardSwitch::get()->state()._cursorPos) {
		KeyboardSwitch::get()->key_event(Key::LEFT);
	} else {
		KeyboardSwitch::get()->key_event(Key::RIGHT);
	}
	KeyboardSwitch::get()->state()._cursorPos = arg->cursorPos;
}

void keyboard_decided_enter_callback(const char *str, SwkbdDecidedEnterArg *arg) {
	std::cout << "keyboard_decided_enter_callback: " << str << std::endl;

	KeyboardSwitch::get()->key_event(Key::ENTER, true);
	KeyboardSwitch::get()->state()._opened = false;
}

void keyboard_decided_cancel_callback() {
	KeyboardSwitch::get()->state()._opened = false;
}