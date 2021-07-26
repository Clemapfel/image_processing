// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <map>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <key_id.hpp>
#include <vector.hpp>

namespace crisp
{
    class RenderWindow;

    class InputHandler
    {
        friend class RenderWindow;

        public:
            InputHandler() = delete;

            static bool has_state_changed(KeyID key_id);
            static bool is_key_down(KeyID key_id);
            static bool was_key_pressed(KeyID key_id);
            static bool was_key_released(KeyID key_id);
            static sf::Time get_hold_duration(KeyID key_id);

            // @returns absolute position relative to window origin
            static Vector2f cursor_position();

        protected:
            static void set_window(RenderWindow*);
            static void update(sf::Time);

        private:
            static void process_event(sf::Event&);
            static KeyID sf_key_to_id(sf::Keyboard::Key);

            static inline RenderWindow* _window = nullptr;

            static inline Vector2f _cursor_position = Vector2f(0,0);
            static inline std::map<KeyID, sf::Time> _hold_duration = {};

            static inline size_t _current_state_index = 1;
            static inline std::array<std::map<KeyID, bool>, 2> _detail_states = std::array<std::map<KeyID, bool>, 2>();
            static bool states_at(size_t i, KeyID id);
    };
}