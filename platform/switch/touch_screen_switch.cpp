/**************************************************************************/
/*  touch_screen_switch.cpp                                               */
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

#include "touch_screen_switch.h"

#include <iostream>

TouchScreenSwitch::TouchScreenSwitch() {
}

TouchScreenSwitch::~TouchScreenSwitch() {
}

void TouchScreenSwitch::initialize() {
	hidInitializeTouchScreen();
}

void TouchScreenSwitch::process() {
	auto drop_touches = _touches;

	HidTouchScreenState state;
	if (hidGetTouchScreenStates(&state, 1)) {

		//for (int i = 0; i < state.count; i++) {
		//	std::cout << i << " [" << state.touches[i].finger_id << "] " << state.touches[i].attributes << " ("
		//			  << state.touches[i].x << "," << state.touches[i].y << ")" << std::endl;
		//}

		for (int i = 0; i < state.count; i++) {
			const int id = state.touches[i].finger_id;

			drop_touches.erase(id);
			auto it = _touches.find(id);

			if (it == _touches.end()) { //new touch
				_touches[id] = state.touches[i];
			}

			Vector2 pos(_touches[id].x, _touches[id].y);
			Ref<InputEventScreenTouch> ev;
			ev.instantiate();
			ev->set_index(id);
			ev->set_position(pos);
			ev->set_pressed(true);
			Input::get_singleton()->parse_input_event(ev);
		}
	}

	for (auto &it : drop_touches) {
		const int id = it.first;
        Vector2 pos(_touches[id].x, _touches[id].y);

		_touches.erase(id);

        Ref<InputEventScreenTouch> ev;
		ev.instantiate();
		ev->set_index(id);
        ev->set_position(pos);
		ev->set_pressed(false);
		Input::get_singleton()->parse_input_event(ev);
	}
}