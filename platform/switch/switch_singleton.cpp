/**************************************************************************/
/*  switch_singleton.cpp                                                  */
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

#include "api/switch_singleton.h"
#include "core/input/input.h"

#include <iostream>

#ifdef SWITCH_ENABLED

void Switch::open_gamepad_applet(int p_players, bool p_single_mode, bool p_dual_joy){
    //ERR_FAIL_COND_MSG(p_min_players > p_max_players || p_min_players < 0, "min_players must be >=0 and <=max_players");
    //ERR_FAIL_COND_MSG(p_max_players > 8, "max_players must be >=min_players and <=8");
    
    HidLaControllerSupportArg arg;

    hidLaCreateControllerSupportArg(&arg);

    arg.hdr.player_count_min = p_players-1;
    arg.hdr.player_count_max = p_players;
    arg.hdr.enable_single_mode = p_single_mode;
    arg.hdr.enable_permit_joy_dual= p_dual_joy;

	// Should use the regular version, but the forSystem always shows the UI and the regular version is bugged.
    Result res = hidLaShowControllerSupportForSystem(NULL, &arg, NULL);
    if(res == LibnxError_LibAppletBadExit){
        print_line("hid connection interface terminated BAD");

    }else{
        print_line("hid connection interface terminated GOOD");
    }
}

void Switch::initialize_keyboard() {
	swkbdInlineCreate(&_keyboard);

	swkbdInlineLaunchForLibraryApplet(&_keyboard, SwkbdInlineMode_AppletDisplay, 0);
	swkbdInlineSetChangedStringCallback(&_keyboard, keyboard_string_changed_callback);
	swkbdInlineSetMovedCursorCallback(&_keyboard, keyboard_moved_cursor_callback);
	swkbdInlineSetDecidedEnterCallback(&_keyboard, keyboard_decided_enter_callback);
	swkbdInlineSetDecidedCancelCallback(&_keyboard, keyboard_decided_cancel_callback);
}

void Switch::finalize_keyboard() {
	swkbdInlineClose(&_keyboard);
}

void Switch::update_text(const char *str) {
	_text = str;
}

void Switch::show_keyboard(const String &current, SoftwareKeyboardType p_type) {
	if (!_state._opened) {
		_state._opened = true;
		_text = current;
		SwkbdAppearArg arg;
		swkbdInlineMakeAppearArg(&arg, SwkbdType_Normal);
		swkbdInlineSetInputText(&_keyboard, current.utf8().get_data());
		swkbdInlineSetCursorPos(&_keyboard, current.size() - 1);
		swkbdInlineAppear(&_keyboard, &arg);
	}
}

void Switch::hide_keyboard() {
	_state._opened = false;
	swkbdInlineDisappear(&_keyboard);
}

void Switch::key_event(Key key, bool pressed) {
	Ref<InputEventKey> ev;
	ev.instantiate();
	ev->set_echo(false);
	ev->set_pressed(pressed);
	ev->set_keycode(key);
	Input::get_singleton()->parse_input_event(ev);
};

void Switch::process_keyboard()
{
	swkbdInlineUpdate(&_keyboard, NULL);
}

void keyboard_string_changed_callback(const char *str, SwkbdChangedStringArg *arg) {
	std::cout << "keyboard_string_changed_callback: " << arg->stringLen << " " << str << std::endl;
	// We get a string changed event on appear, and another one on setting text.
	if (Switch::get_singleton()->keyboard_state()._events) {
		Switch::get_singleton()->keyboard_state()._events--;
		return;
	}
	
	if (arg->stringLen < Switch::get_singleton()->keyboard_state()._stringLen) {
		Switch::get_singleton()->key_event(Key::BACKSPACE);
	} else if (arg->stringLen > 0) {
		Switch::get_singleton()->key_event((Key)(str[arg->stringLen - 1] - 32));
	}

	Switch::get_singleton()->keyboard_state()._stringLen = arg->stringLen;
	Switch::get_singleton()->update_text(str);
}

void keyboard_moved_cursor_callback(const char *str, SwkbdMovedCursorArg *arg) {
	std::cout << "keyboard_moved_cursor_callback: " << arg->cursorPos << " " << str << std::endl;
	if (arg->cursorPos < Switch::get_singleton()->keyboard_state()._cursorPos) {
		Switch::get_singleton()->key_event(Key::LEFT);
	} else {
		Switch::get_singleton()->key_event(Key::RIGHT);
	}
	Switch::get_singleton()->keyboard_state()._cursorPos = arg->cursorPos;
}

void keyboard_decided_enter_callback(const char *str, SwkbdDecidedEnterArg *arg) {
	std::cout << "keyboard_decided_enter_callback: " << str << std::endl;

	Switch::get_singleton()->key_event(Key::ENTER, true);
	Switch::get_singleton()->keyboard_state()._opened = false;
	Switch::get_singleton()->call_deferred("emit_signal", SNAME("keyboard_string_result"), str);
}

void keyboard_decided_cancel_callback() {
	Switch::get_singleton()->keyboard_state()._opened = false;
}

#endif