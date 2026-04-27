/**************************************************************************/
/*  joypad_switch.cpp                                                     */
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

#include "joypad_switch.h"

#include <iostream>

//when only both joy-con are use as a single controller (general case)
const PadMappingSwitch JoypadSwitch::switch_joy_dual_button_map = {
	{
		{ HidNpadButton_A, JoyButton::B },
		{ HidNpadButton_B, JoyButton::A },
		{ HidNpadButton_X, JoyButton::Y },
		{ HidNpadButton_Y, JoyButton::X },
		{ HidNpadButton_StickL, JoyButton::LEFT_STICK },
		{ HidNpadButton_StickR, JoyButton::RIGHT_STICK },
		{ HidNpadButton_L, JoyButton::LEFT_SHOULDER },
		{ HidNpadButton_R, JoyButton::RIGHT_SHOULDER },
		{ HidNpadButton_Plus, JoyButton::START },
		{ HidNpadButton_Minus, JoyButton::BACK },
		{ HidNpadButton_Left, JoyButton::DPAD_LEFT },
		{ HidNpadButton_Up, JoyButton::DPAD_UP },
		{ HidNpadButton_Right, JoyButton::DPAD_RIGHT },
		{ HidNpadButton_Down, JoyButton::DPAD_DOWN },
		{ HidNpadButton_LeftSL, JoyButton::PADDLE1 },
		{ HidNpadButton_RightSR, JoyButton::PADDLE3 },
		{ HidNpadButton_RightSL, JoyButton::PADDLE2 },
		{ HidNpadButton_RightSR, JoyButton::PADDLE4 }
	},
	{
		{ HidNpadButton_ZL, JoyAxis::TRIGGER_LEFT },
		{ HidNpadButton_ZR, JoyAxis::TRIGGER_RIGHT },
	}
};

//when only right joy-con is use as a controller horizontally
const PadMappingSwitch JoypadSwitch::switch_joy_right_button_map = {
	{
		{ HidNpadButton_A, JoyButton::A },
		{ HidNpadButton_B, JoyButton::X },
		{ HidNpadButton_X, JoyButton::B },
		{ HidNpadButton_Y, JoyButton::Y },
		{ HidNpadButton_StickR, JoyButton::LEFT_STICK },
		{ HidNpadButton_R, JoyButton::PADDLE2 },
		{ HidNpadButton_ZR, JoyButton::PADDLE1 },
		{ HidNpadButton_Plus, JoyButton::START },
		{ HidNpadButton_RightSL, JoyButton::LEFT_SHOULDER },
		{ HidNpadButton_RightSR, JoyButton::RIGHT_SHOULDER },
	},
	{
		{ HidNpadButton_RightSL, JoyAxis::TRIGGER_LEFT },
		{ HidNpadButton_RightSR, JoyAxis::TRIGGER_RIGHT },
	}
};

//when only left joy-con is use as a controller horizontally
const PadMappingSwitch JoypadSwitch::switch_joy_left_button_map = {
	{
		
		{ HidNpadButton_Left, JoyButton::A },
		{ HidNpadButton_Up, JoyButton::X },
		{ HidNpadButton_Down, JoyButton::B },
		{ HidNpadButton_Right, JoyButton::Y },
		{ HidNpadButton_StickL, JoyButton::LEFT_STICK },
		{ HidNpadButton_L, JoyButton::PADDLE2 },
		{ HidNpadButton_ZL, JoyButton::PADDLE1 },
		{ HidNpadButton_Minus, JoyButton::START },
		{ HidNpadButton_LeftSL, JoyButton::LEFT_SHOULDER },
		{ HidNpadButton_LeftSR, JoyButton::RIGHT_SHOULDER },
	},
	{
		{ HidNpadButton_LeftSL, JoyAxis::TRIGGER_LEFT },
		{ HidNpadButton_LeftSR, JoyAxis::TRIGGER_RIGHT },
	}
};

void JoypadSwitch::initialize() {
	print_line("JoypadSwitch::initialize");

	Input::get_singleton()->set_use_accumulated_input(false);
	Input::get_singleton()->set_use_input_buffering(false);

	//accept up to 8 controllers, all modes
	padConfigureInput(_pads.size(), HidNpadStyleSet_NpadStandard);
	// first controler initialized as is #1 AND handheld
	padInitialize(&_pads[0], HidNpadIdType_No1, HidNpadIdType_Handheld);
	// from 2 -> 8 controller controler initialized as is #N
	for (uint8_t i = 1; i < _pads.size(); i++) {
		_pads[i].id = i;
		padInitialize(&_pads[i], HidNpadIdType(i));
	}
}

void JoypadSwitch::open_pad(PadStateSwitch &pad) {
	print_line("JoypadSwitch::open_pad(" + String::num(pad.id) + ")");
	print_line("open pads size: " + _pads.size());

	pad.initialized = true;
	bool solo = false;
	pad.name = "switch-pad-" + String::num(pad.id);
	if (pad.style_set & HidNpadStyleTag_NpadJoyLeft) {
		pad.mapping = switch_joy_left_button_map;
		pad.name += "::solo-left";
		solo = true;
	} else if (pad.style_set & HidNpadStyleTag_NpadJoyRight) {
		pad.mapping = switch_joy_right_button_map;
		pad.name += "::solo-right";
		solo = true;
	} else if (pad.style_set & HidNpadStyleTag_NpadJoyDual) {
		pad.mapping = switch_joy_dual_button_map;
		pad.name += "::dual";
	} else if (pad.style_set & HidNpadStyleTag_NpadFullKey) {
		pad.mapping = switch_joy_dual_button_map;
		pad.name += "::pro";
	} else if (pad.style_set & HidNpadStyleTag_NpadHandheld) {
		pad.mapping = switch_joy_dual_button_map;
		pad.name += "::handheld";
	} else {
		pad.mapping = switch_joy_dual_button_map;
		pad.name += "::other";
	}

	if (solo) {
		HidNpadControllerColor color;
		hidGetNpadControllerColorSingle((HidNpadIdType)pad.id, &color);
		pad.name += "::#" + String::num_int64(color.main, 16);
	} else {
		HidNpadControllerColor color_l, color_r;
		hidGetNpadControllerColorSplit((HidNpadIdType)pad.id, &color_l, &color_r);
		pad.name += "::#" + String::num_int64(color_l.main, 16);
		pad.name += "::#" + String::num_int64(color_r.main, 16);
	}

	Input::get_singleton()->joy_connection_changed(pad.id, true, pad.name);
	std::cout << "joy_connection_changed pad(" << pad.id << ") "
			  << "name(" << pad.name.utf8().get_data() << ") "
			  << "read_handheld(" << pad.read_handheld << ") "
			  << "active_handheld(" << pad.active_handheld << ") "
			  << "attributes(" << pad.attributes << ") "
			  << "style_set(" << pad.style_set << ")" << std::endl;
}

void JoypadSwitch::close_pad(PadStateSwitch &pad) {
	pad.initialized = false;
	Input::get_singleton()->joy_connection_changed(pad.id, false, pad.name);
}

void JoypadSwitch::process() {
	Input* input = Input::get_singleton();

	for (uint8_t i = 0; i < _pads.size(); i++) {
		PadStateSwitch &pad = _pads[i];
		padUpdate(&pad);

		u64 kDown = padGetButtonsDown(&pad);
		u64 kUp = padGetButtonsUp(&pad);

		if (!pad.initialized && kDown) {
			open_pad(pad);
		}
		if (!padIsConnected(&pad) && pad.initialized){
			close_pad(pad);
		}

		for (const auto &button : pad.mapping.first) {
			if (kDown & button.first) {
				input->joy_button(pad.id, button.second, true);
			}
			if (kUp & button.first) {
				input->joy_button(pad.id, button.second, false);
			}
		}

		for (const auto &axis : pad.mapping.second) {
			if (kDown & axis.first) {
				input->joy_axis(pad.id, axis.second, 1);
			}
			if (kUp & axis.first) {
				input->joy_axis(pad.id, axis.second, 0);
			}
		}

		HidAnalogStickState leftStick = pad.sticks[0];
		HidAnalogStickState rightStick = pad.sticks[1];

		if (pad.style_set & HidNpadStyleTag_NpadJoyLeft) {
			// only left stick available and rotated 90 anti-clock wise
			input->joy_axis(i, JoyAxis::LEFT_Y, -(float)(leftStick.x) / float(JOYSTICK_MAX));
			input->joy_axis(i, JoyAxis::LEFT_X, -(float)(leftStick.y) / float(JOYSTICK_MAX));
		} else if (pad.style_set & HidNpadStyleTag_NpadJoyRight) {
			// only left stick available and rotated 90 clock wise
			input->joy_axis(i, JoyAxis::LEFT_Y, (float)(rightStick.x) / float(JOYSTICK_MAX));
			input->joy_axis(i, JoyAxis::LEFT_X, (float)(rightStick.y) / float(JOYSTICK_MAX));
		} else {
			// both sticks no rotations
			input->joy_axis(i, JoyAxis::LEFT_X, (float)(leftStick.x) / float(JOYSTICK_MAX));
			input->joy_axis(i, JoyAxis::LEFT_Y, -(float)(leftStick.y) / float(JOYSTICK_MAX));
			input->joy_axis(i, JoyAxis::RIGHT_X, (float)(rightStick.x) / float(JOYSTICK_MAX));
			input->joy_axis(i, JoyAxis::RIGHT_Y, -(float)(rightStick.y) / float(JOYSTICK_MAX));
		}
	}
}

JoypadSwitch::JoypadSwitch() {
}