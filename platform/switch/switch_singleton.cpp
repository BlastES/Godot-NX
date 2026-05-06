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

#include "switch_wrapper.h"

#ifdef SWITCH_ENABLED

void Switch::open_gamepad_applet(int p_players, bool p_single_mode, bool p_dual_joy){
    //ERR_FAIL_COND_MSG(p_min_players > p_max_players || p_min_players < 0, "min_players must be >=0 and <=max_players");
    //ERR_FAIL_COND_MSG(p_max_players > 8, "max_players must be >=min_players and <=8");
    
    HidLaControllerSupportArg arg;

    hidLaCreateControllerSupportArg(&arg);

    print_line(arg.hdr.player_count_min);
    print_line(arg.hdr.player_count_max);
    print_line(arg.hdr.enable_single_mode);
    print_line(arg.hdr.enable_permit_joy_dual);
    print_line("");

    /*
    arg.hdr.player_count_min = p_players;
    arg.hdr.player_count_max = p_players;
    arg.hdr.enable_single_mode = p_single_mode;
    arg.hdr.enable_permit_joy_dual= p_dual_joy;
    */

    print_line(arg.hdr.player_count_min);
    print_line(arg.hdr.player_count_max);
    print_line(arg.hdr.enable_single_mode);
    print_line(arg.hdr.enable_permit_joy_dual);

    Result res = hidLaShowControllerSupport(NULL, &arg);
    if(res == LibnxError_LibAppletBadExit){
        print_line("hid connection interface terminated BAD");

    }else{
        print_line("hid connection interface terminated GOOD");
    }
}

#endif