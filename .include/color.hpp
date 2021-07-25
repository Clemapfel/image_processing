//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <cstdint>

namespace cip
{
    // color representation as RGB cube
    // values in [0, 255)
    struct RGB
    {
        uint8_t r, // red
                g, // green
                b, // blue
                a; // alpha
    };

    // color representation in HSV system
    // values in [0, 255)
    struct HSV
    {
        uint8_t h, // hue (chroma)
                s, // saturation
                v, // value
                a; // alpha
    };

    // color representation in CYMK system common in printing
    // values in [0, 255)
    struct CMYK
    {
        uint8_t c, // cyan
                m, // magenta
                y, // yellow
                k; // black
    };

    // color representation as one grey-scale intensity v equivalent in RGB to the grey tone (v, v, v, alpha)
    // values in [0, 255)
    struct GrayScale
    {
        uint8_t v, // value
                a; // alpha
    };

    // class representing 8-bit colors
    class Color : protected sf::Color
    {
        public:
            // values identical to those of the RGB representation
            using sf::Color::r;
            using sf::Color::g;
            using sf::Color::b;
            using sf::Color::a;

            // CTORS
            Color();
            Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
            Color(RGB rgb);
            Color(HSV hsv);
            Color(GrayScale grayscale);

            // @brief: transform color into different representation
            [[nodiscard]] RGB && as_rgb();
            [[nodiscard]] HSV && as_hsv();
            [[nodiscard]] CMYK && as_cmyk();
            [[nodiscard]] GrayScale && as_grayscale();

            // @brief: assignment from different representations
            Color & operator=(RGB);
            Color & operator=(HSV);
            Color & operator=(CMYK);
            Color & operator=(GrayScale);

            // @brief: comparison operator, compares element-wise as if the color was a RGB vector
            bool operator==(const Color&) const;
            bool operator!=(const Color&) const;

            // @returns: negative of color equals to {1 - r, 1 - g, 1 - b, a}
            [[nodiscard]] Color && invert() const;
    };

    // @brief: convert one representation to another in the form "convert T1 to T2"
    // @param from: original representation of type T1
    // @returns: converted representation of type T2
    // @note: Will trigger a static assertion unless T1 and T2 are any of {Color, RGB, HSV; CMYK, GrayScale}
    template<typename T1, typename T2>
    extern T2&& convert(T1 from);

    // @brief: mix override for colors, c.f. vector.hpp for the generalized function
    // @param a: first color
    // @param b: second color
    // @param weight: weight, 0 for 100% first color, 1 for 100% second color. Values outside of [0,1] are clamped
    Color && mix(Color a, Color b, float weight);
}

#include ".src/color.inl"