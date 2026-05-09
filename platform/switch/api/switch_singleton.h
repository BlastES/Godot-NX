/**************************************************************************/
/*  switch_singleton.h                                                    */
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

#ifndef SWITCH_SINGLETON_H
#define SWITCH_SINGLETON_H

#include "core/object/object.h"
#include "core/object/class_db.h"

#ifdef SWITCH_ENABLED
#include "switch_wrapper.h"
#endif


class Switch : public Object{
    private:
        GDCLASS(Switch, Object);

        struct KeyboardSwitchState {
            bool _opened;
            int _events = 0;
            u_int32_t _stringLen = 0;
            int32_t _cursorPos = 0;
        };

#ifdef SWITCH_ENABLED
        SwkbdInline _keyboard;
#endif
        KeyboardSwitchState _state;
        String _text;

        static Switch *singleton;

    protected:
        static void _bind_methods();
        
    public:
        enum SoftwareKeyboardType{
            NORMAL_KEYBOARD = 0,
            NUMPAD_KEYBOARD = 1,
            QWERTY_KEYBOARD = 2,
            LATIN_KEYBOARD = 4,
            SIMPLIFIED_CHINESE_KEYBOARD = 5,
            TRADITIONAL_CHINESE_KEYBOARD = 6,
            KOREAN_KEYBOARD = 7,
            ALL_LANGUAGES_KEYBOARD = 8,
	    };
        // Joypad stuff
        void open_gamepad_applet(int p_players = 4, bool p_single_mode = false, bool p_dual_joy = true);
        

        // Keyboard stuff
        void show_keyboard(const String &current, SoftwareKeyboardType p_type);
        void initialize_keyboard();
        void finalize_keyboard();

        void update_text(const char *str);

        const KeyboardSwitchState& keyboard_state() const { return _state;}
        KeyboardSwitchState& keyboard_state() { return _state;}

        void hide_keyboard();

        void key_event(Key key, bool pressed = true);

        void process_keyboard();


        static Switch *get_singleton();
        Switch();
        ~Switch();
};
#ifdef SWITCH_ENABLED
void keyboard_string_changed_callback(const char *str, SwkbdChangedStringArg *arg);
void keyboard_moved_cursor_callback(const char *str, SwkbdMovedCursorArg *arg);
void keyboard_decided_enter_callback(const char *str, SwkbdDecidedEnterArg *arg);
void keyboard_decided_cancel_callback();
#endif

VARIANT_ENUM_CAST(Switch::SoftwareKeyboardType);

#endif //SWITCH_SINGLETON_H