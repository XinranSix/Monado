#pragma once

#include <cstdint>
#include <iostream>

namespace Monado {
    using KeyCode = uint16_t;
    namespace Key {
        enum : uint16_t {
            // From glfw3.h
            Space = 32,
            Apostrophe = 39, /* ' */
            Comma = 44,      /* , */
            Minus = 45,      /* - */
            Period = 46,     /* . */
            Slash = 47,      /* / */

            D0 = 48, /* 0 */
            D1 = 49, /* 1 */
            D2 = 50, /* 2 */
            D3 = 51, /* 3 */
            D4 = 52, /* 4 */
            D5 = 53, /* 5 */
            D6 = 54, /* 6 */
            D7 = 55, /* 7 */
            D8 = 56, /* 8 */
            D9 = 57, /* 9 */

            Semicolon = 59, /* ; */
            Equal = 61,     /* = */

            A = 65,
            B = 66,
            C = 67,
            D = 68,
            E = 69,
            F = 70,
            G = 71,
            H = 72,
            I = 73,
            J = 74,
            K = 75,
            L = 76,
            M = 77,
            N = 78,
            O = 79,
            P = 80,
            Q = 81,
            R = 82,
            S = 83,
            T = 84,
            U = 85,
            V = 86,
            W = 87,
            X = 88,
            Y = 89,
            Z = 90,

            LeftBracket = 91,  /* [ */
            Backslash = 92,    /* \ */
            RightBracket = 93, /* ] */
            GraveAccent = 96,  /* ` */

            World1 = 161, /* non-US #1 */
            World2 = 162, /* non-US #2 */

            /* Function keys */
            Escape = 256,
            Enter = 257,
            Tab = 258,
            Backspace = 259,
            Insert = 260,
            Delete = 261,
            Right = 262,
            Left = 263,
            Down = 264,
            Up = 265,
            PageUp = 266,
            PageDown = 267,
            Home = 268,
            End = 269,
            CapsLock = 280,
            ScrollLock = 281,
            NumLock = 282,
            PrintScreen = 283,
            Pause = 284,
            F1 = 290,
            F2 = 291,
            F3 = 292,
            F4 = 293,
            F5 = 294,
            F6 = 295,
            F7 = 296,
            F8 = 297,
            F9 = 298,
            F10 = 299,
            F11 = 300,
            F12 = 301,
            F13 = 302,
            F14 = 303,
            F15 = 304,
            F16 = 305,
            F17 = 306,
            F18 = 307,
            F19 = 308,
            F20 = 309,
            F21 = 310,
            F22 = 311,
            F23 = 312,
            F24 = 313,
            F25 = 314,

            /* Keypad */
            KP0 = 320,
            KP1 = 321,
            KP2 = 322,
            KP3 = 323,
            KP4 = 324,
            KP5 = 325,
            KP6 = 326,
            KP7 = 327,
            KP8 = 328,
            KP9 = 329,
            KPDecimal = 330,
            KPDivide = 331,
            KPMultiply = 332,
            KPSubtract = 333,
            KPAdd = 334,
            KPEnter = 335,
            KPEqual = 336,

            LeftShift = 340,
            LeftControl = 341,
            LeftAlt = 342,
            LeftSuper = 343,
            RightShift = 344,
            RightControl = 345,
            RightAlt = 346,
            RightSuper = 347,
            Menu = 348
        };

        inline std::ostream &operator<<(std::ostream &os, KeyCode keyCode) {
            os << static_cast<int32_t>(keyCode);
            return os;
        }
    } // namespace Key
} // namespace Monado

// From glfw3.h
#define MND_KEY_SPACE ::Monado::Key::Space
#define MND_KEY_APOSTROPHE ::Monado::Key::Apostrophe /* ' */
#define MND_KEY_COMMA ::Monado::Key::Comma           /* , */
#define MND_KEY_MINUS ::Monado::Key::Minus           /* - */
#define MND_KEY_PERIOD ::Monado::Key::Period         /* . */
#define MND_KEY_SLASH ::Monado::Key::Slash           /* / */
#define MND_KEY_0 ::Monado::Key::D0
#define MND_KEY_1 ::Monado::Key::D1
#define MND_KEY_2 ::Monado::Key::D2
#define MND_KEY_3 ::Monado::Key::D3
#define MND_KEY_4 ::Monado::Key::D4
#define MND_KEY_5 ::Monado::Key::D5
#define MND_KEY_6 ::Monado::Key::D6
#define MND_KEY_7 ::Monado::Key::D7
#define MND_KEY_8 ::Monado::Key::D8
#define MND_KEY_9 ::Monado::Key::D9
#define MND_KEY_SEMICOLON ::Monado::Key::Semicolon /* ; */
#define MND_KEY_EQUAL ::Monado::Key::Equal         /* = */
#define MND_KEY_A ::Monado::Key::A
#define MND_KEY_B ::Monado::Key::B
#define MND_KEY_C ::Monado::Key::C
#define MND_KEY_D ::Monado::Key::D
#define MND_KEY_E ::Monado::Key::E
#define MND_KEY_F ::Monado::Key::F
#define MND_KEY_G ::Monado::Key::G
#define MND_KEY_H ::Monado::Key::H
#define MND_KEY_I ::Monado::Key::I
#define MND_KEY_J ::Monado::Key::J
#define MND_KEY_K ::Monado::Key::K
#define MND_KEY_L ::Monado::Key::L
#define MND_KEY_M ::Monado::Key::M
#define MND_KEY_N ::Monado::Key::N
#define MND_KEY_O ::Monado::Key::O
#define MND_KEY_P ::Monado::Key::P
#define MND_KEY_Q ::Monado::Key::Q
#define MND_KEY_R ::Monado::Key::R
#define MND_KEY_S ::Monado::Key::S
#define MND_KEY_T ::Monado::Key::T
#define MND_KEY_U ::Monado::Key::U
#define MND_KEY_V ::Monado::Key::V
#define MND_KEY_W ::Monado::Key::W
#define MND_KEY_X ::Monado::Key::X
#define MND_KEY_Y ::Monado::Key::Y
#define MND_KEY_Z ::Monado::Key::Z
#define MND_KEY_LEFT_BRACKET ::Monado::Key::LeftBracket   /* [ */
#define MND_KEY_BACKSLASH ::Monado::Key::Backslash        /* \ */
#define MND_KEY_RIGHT_BRACKET ::Monado::Key::RightBracket /* ] */
#define MND_KEY_GRAVE_ACCENT ::Monado::Key::GraveAccent   /* ` */
#define MND_KEY_WORLD_1 ::Monado::Key::World1             /* non-US #1 */
#define MND_KEY_WORLD_2 ::Monado::Key::World2             /* non-US #2 */

/* Function keys */
#define MND_KEY_ESCAPE ::Monado::Key::Escape
#define MND_KEY_ENTER ::Monado::Key::Enter
#define MND_KEY_TAB ::Monado::Key::Tab
#define MND_KEY_BACKSPACE ::Monado::Key::Backspace
#define MND_KEY_INSERT ::Monado::Key::Insert
#define MND_KEY_DELETE ::Monado::Key::Delete
#define MND_KEY_RIGHT ::Monado::Key::Right
#define MND_KEY_LEFT ::Monado::Key::Left
#define MND_KEY_DOWN ::Monado::Key::Down
#define MND_KEY_UP ::Monado::Key::Up
#define MND_KEY_PAGE_UP ::Monado::Key::PageUp
#define MND_KEY_PAGE_DOWN ::Monado::Key::PageDown
#define MND_KEY_HOME ::Monado::Key::Home
#define MND_KEY_END ::Monado::Key::End
#define MND_KEY_CAPS_LOCK ::Monado::Key::CapsLock
#define MND_KEY_SCROLL_LOCK ::Monado::Key::ScrollLock
#define MND_KEY_NUM_LOCK ::Monado::Key::NumLock
#define MND_KEY_PRINT_SCREEN ::Monado::Key::PrintScreen
#define MND_KEY_PAUSE ::Monado::Key::Pause
#define MND_KEY_F1 ::Monado::Key::F1
#define MND_KEY_F2 ::Monado::Key::F2
#define MND_KEY_F3 ::Monado::Key::F3
#define MND_KEY_F4 ::Monado::Key::F4
#define MND_KEY_F5 ::Monado::Key::F5
#define MND_KEY_F6 ::Monado::Key::F6
#define MND_KEY_F7 ::Monado::Key::F7
#define MND_KEY_F8 ::Monado::Key::F8
#define MND_KEY_F9 ::Monado::Key::F9
#define MND_KEY_F10 ::Monado::Key::F10
#define MND_KEY_F11 ::Monado::Key::F11
#define MND_KEY_F12 ::Monado::Key::F12
#define MND_KEY_F13 ::Monado::Key::F13
#define MND_KEY_F14 ::Monado::Key::F14
#define MND_KEY_F15 ::Monado::Key::F15
#define MND_KEY_F16 ::Monado::Key::F16
#define MND_KEY_F17 ::Monado::Key::F17
#define MND_KEY_F18 ::Monado::Key::F18
#define MND_KEY_F19 ::Monado::Key::F19
#define MND_KEY_F20 ::Monado::Key::F20
#define MND_KEY_F21 ::Monado::Key::F21
#define MND_KEY_F22 ::Monado::Key::F22
#define MND_KEY_F23 ::Monado::Key::F23
#define MND_KEY_F24 ::Monado::Key::F24
#define MND_KEY_F25 ::Monado::Key::F25

/* Keypad */
#define MND_KEY_KP_0 ::Monado::Key::KP0
#define MND_KEY_KP_1 ::Monado::Key::KP1
#define MND_KEY_KP_2 ::Monado::Key::KP2
#define MND_KEY_KP_3 ::Monado::Key::KP3
#define MND_KEY_KP_4 ::Monado::Key::KP4
#define MND_KEY_KP_5 ::Monado::Key::KP5
#define MND_KEY_KP_6 ::Monado::Key::KP6
#define MND_KEY_KP_7 ::Monado::Key::KP7
#define MND_KEY_KP_8 ::Monado::Key::KP8
#define MND_KEY_KP_9 ::Monado::Key::KP9
#define MND_KEY_KP_DECIMAL ::Monado::Key::KPDecimal
#define MND_KEY_KP_DIVIDE ::Monado::Key::KPDivide
#define MND_KEY_KP_MULTIPLY ::Monado::Key::KPMultiply
#define MND_KEY_KP_SUBTRACT ::Monado::Key::KPSubtract
#define MND_KEY_KP_ADD ::Monado::Key::KPAdd
#define MND_KEY_KP_ENTER ::Monado::Key::KPEnter
#define MND_KEY_KP_EQUAL ::Monado::Key::KPEqual

#define MND_KEY_LEFT_SHIFT ::Monado::Key::LeftShift
#define MND_KEY_LEFT_CONTROL ::Monado::Key::LeftControl
#define MND_KEY_LEFT_ALT ::Monado::Key::LeftAlt
#define MND_KEY_LEFT_SUPER ::Monado::Key::LeftSuper
#define MND_KEY_RIGHT_SHIFT ::Monado::Key::RightShift
#define MND_KEY_RIGHT_CONTROL ::Monado::Key::RightControl
#define MND_KEY_RIGHT_ALT ::Monado::Key::RightAlt
#define MND_KEY_RIGHT_SUPER ::Monado::Key::RightSuper
#define MND_KEY_MENU ::Monado::Key::Menu
