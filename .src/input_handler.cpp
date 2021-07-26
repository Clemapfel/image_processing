// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#include <render_window.hpp>
#include <input_handler.hpp>
#include <iostream>

namespace crisp
{
    bool InputHandler::states_at(size_t i, KeyID id)
    {
        assert(i == 0 or i == 1);

        if (_detail_states.at(i).find(id) == _detail_states.at(i).end())
            return false;
        else
            return _detail_states.at(i).at(id);
    }

    bool InputHandler::has_state_changed(KeyID key)
    {
        auto res = states_at(_current_state_index, key) != states_at(not _current_state_index, key);

        auto& states = _detail_states;

        if (res)
            size_t breakpoint = 0;

        return res;
    }

    bool InputHandler::is_key_down(KeyID key)
    {
        return states_at(_current_state_index, key);
    }

    bool InputHandler::was_key_pressed(KeyID key)
    {
        return is_key_down(key) and has_state_changed(key);
    }

    bool InputHandler::was_key_released(KeyID key)
    {
        return not is_key_down(key) and has_state_changed(key);
    }

    sf::Time InputHandler::get_hold_duration(KeyID key_id)
    {
        if(key_id == ALT or key_id == SHIFT or key_id == CTRL)
        {
            std::cerr << R"([WARNING] Requesting hold duration for Key with KeyID "ALT", "SHIFT" or "CTRL will always return 0 seconds)" << std::endl;
            return sf::seconds(0);
        }

        if (_hold_duration.find(key_id) == _hold_duration.end())
            return sf::seconds(0);
        else
            return _hold_duration.at(key_id);
    }

    Vector2f InputHandler::cursor_position()
    {
        return _cursor_position;
    }

    void InputHandler::set_window(RenderWindow* window)
    {
        _window = window;
    }

    void InputHandler::update(sf::Time time)
    {
        if (_window == nullptr)
            return;

        _current_state_index = not _current_state_index;

        for (auto& pair : _detail_states.at(not _current_state_index))
            _detail_states.at(_current_state_index).insert_or_assign(pair.first, pair.second);

        auto event = sf::Event();

        while (_window->pollEvent(event))
            process_event(event);

        for (auto& pair : _hold_duration)
            pair.second += time;
    }

    void InputHandler::process_event(sf::Event &event)
    {
        auto code = event.key.code;
        auto& state = _detail_states.at(_current_state_index);

        // WINDOW
        if (event.type == sf::Event::Closed)
            _window->close();

        else if (event.type == sf::Event::LostFocus)
        {}

        else if (event.type == sf::Event::GainedFocus)
        {}

        // KEYS
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Unknown)
                return;

            state.insert_or_assign(sf_key_to_id(event.key.code), true);
            state.insert_or_assign(ALT, event.key.alt);
            state.insert_or_assign(CTRL, event.key.control);
            state.insert_or_assign(SHIFT, event.key.shift);

            _hold_duration.insert({sf_key_to_id(event.key.code), sf::seconds(0)});
        }
        else if (event.type == sf::Event::KeyReleased)
        {
             if (event.key.code == sf::Keyboard::Unknown)
                 return;

            state.insert_or_assign(sf_key_to_id(event.key.code), false);
            state.insert_or_assign(ALT, event.key.alt);
            state.insert_or_assign(CTRL, event.key.control);
            state.insert_or_assign(SHIFT, event.key.shift);

            _hold_duration.erase(sf_key_to_id(event.key.code));
        }
        // MOUSE
        else if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Button::Left)
            {
                state.insert_or_assign(MOUSE_LEFT, true);
                _hold_duration.insert({MOUSE_LEFT, sf::seconds(0)});
            }
            else if (event.mouseButton.button == sf::Mouse::Button::Right)
            {
                state.insert_or_assign(MOUSE_RIGHT, true);
                _hold_duration.insert({MOUSE_RIGHT, sf::seconds(0)});
            }
            else if (event.mouseButton.button == sf::Mouse::Middle)
            {
                state.insert_or_assign(MOUSE_MIDDLE, true);
                _hold_duration.insert({MOUSE_MIDDLE, sf::seconds(0)});
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Button::Left)
            {
                state.insert_or_assign(MOUSE_LEFT, false);
                _hold_duration.erase(MOUSE_LEFT);
            }
            else if (event.mouseButton.button == sf::Mouse::Button::Right)
            {
                state.insert_or_assign(MOUSE_RIGHT, false);
                _hold_duration.erase(MOUSE_RIGHT);
            }
            else if (event.mouseButton.button == sf::Mouse::Middle)
            {
                state.insert_or_assign(MOUSE_MIDDLE, false);
                _hold_duration.erase(MOUSE_MIDDLE);
            }
        }
        else if (event.type == sf::Event::MouseMoved)
        {
            _cursor_position = _window->get_mouse_position();
        }
    }

    KeyID InputHandler::sf_key_to_id(sf::Keyboard::Key sf)
    {
        char cast = static_cast<char>(sf);

        if ((cast >= 'a' and cast <= 'z') or    // a - z keys
            (cast >= '0' and cast <= '9') or    // non numpad numbers
            (cast >= 'U' and cast <= '`') or    // F1 - F12
            (cast == ESCAPE or cast == UP or cast == DOWN or cast == LEFT or cast == RIGHT or cast == SHIFT or cast == CTRL or cast == ALT or cast == SPACE))
        {
            return static_cast<KeyID>(sf);
        }
        else
            return UNKNOWN;
    }
}

