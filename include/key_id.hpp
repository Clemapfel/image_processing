// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SFML/Window/Keyboard.hpp>

namespace crisp
{
    // enum that does not support all characters to assure portability and support all keyboard types
    enum KeyID : char
    {
        A = 'a',
        B = 'b',
        C = 'c',
        D = 'd',
        E = 'e',
        F = 'f',
        G = 'g',
        H = 'h',
        I = 'i',
        J = 'j',
        K = 'k',
        L = 'l',
        M = 'm',
        N = 'n',
        O = 'o',
        P = 'p',
        Q = 'q',
        R = 'r',
        S = 's',
        T = 't',
        U = 'u',
        V = 'v',
        W = 'w',
        X = 'x',
        Y = 'y',
        Z = 'z',
        NUM_0 = '0',
        NUM_1 = '1',
        NUM_2 = '2',
        NUM_3 = '3',
        NUM_4 = '4',
        NUM_5 = '5',
        NUM_6 = '6',
        NUM_7 = '7',
        NUM_8 = '8',
        NUM_9 = '9',
        F1 = sf::Keyboard::Key::F1,
        F2 = sf::Keyboard::Key::F2,
        F3 = sf::Keyboard::Key::F3,
        F4 = sf::Keyboard::Key::F4,
        F5 = sf::Keyboard::Key::F5,
        F6 = sf::Keyboard::Key::F6,
        F7 = sf::Keyboard::Key::F7,
        F8 = sf::Keyboard::Key::F8,
        F9 = sf::Keyboard::Key::F9,
        F10 = sf::Keyboard::Key::F10,
        F11 = sf::Keyboard::Key::F11,
        F12 = sf::Keyboard::Key::F12,
        ESCAPE = sf::Keyboard::Key::Escape,
        UP = sf::Keyboard::Key::Up,
        DOWN = sf::Keyboard::Key::Down,
        LEFT = sf::Keyboard::Key::Left,
        RIGHT = sf::Keyboard::Key::Right,
        SHIFT = sf::Keyboard::Key::LShift,
        CTRL = sf::Keyboard::Key::LControl,
        ALT = sf::Keyboard::Key::LAlt,
        SPACE = sf::Keyboard::Key::Space,
        MOUSE_LEFT = char(254),
        MOUSE_RIGHT = char(253),
        MOUSE_MIDDLE = char(252),
        UNKNOWN = char(0)
    };
}