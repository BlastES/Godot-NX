/**************************************************************************/
/*  api.cpp                                                               */
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

#include "api.h"

#ifdef SWITCH_ENABLED
#include "os_switch.h"
#include "switch_wrapper.h"
#endif
#include "switch_singleton.h"

#include "core/config/engine.h"

static Switch *switch_singleton;

void register_switch_api(){
    GDREGISTER_ABSTRACT_CLASS(Switch);
    switch_singleton = memnew(Switch);
    Engine::get_singleton()->add_singleton(Engine::Singleton("Switch", switch_singleton));
}

void unregister_switch_api(){
    memdelete(switch_singleton);
}

Switch *Switch::singleton = nullptr;

Switch *Switch::get_singleton(){
    return singleton;
}

Switch::Switch(){
    ERR_FAIL_COND_MSG(singleton != nullptr, "Switch singleton already exist.");
    singleton = this;
    print_line("Switch singleton initialize");
}

Switch::~Switch() {}

void Switch::_bind_methods(){
    ClassDB::bind_method(D_METHOD("open_gamepad_applet", "n_players", "single_mode", "dual_joy"), &Switch::open_gamepad_applet, DEFVAL(4), DEFVAL(false), DEFVAL(true));
}

#ifndef SWITCH_ENABLED

void Switch::open_gamepad_applet(int p_players, bool p_single_mode, bool p_dual_joy){
    //ERR_FAIL_COND_MSG(p_min_players > p_max_players || p_min_players < 0, "min_players must be >=0 and <=max_players");
    //ERR_FAIL_COND_MSG(p_max_players > 8, "max_players must be >=min_players and <=8");
}

#endif