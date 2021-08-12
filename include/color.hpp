//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cstdint>
#include <SFML/Graphics/Color.hpp>
#include <cstdint>
#include <vector.hpp>

namespace crisp
{
    // color representation as RGB cube
    // values in [0, 1]
    struct RGB
    {
        float r = 0, // red
                g = 0, // green
                b = 0, // blue
                a = 1; // alpha
    };

    // color representation in HSV system
    // values in [0, 1]
    struct HSV
    {
        float h = 0, // hue
                s = 0, // saturation
                v = 0, // value (brightness), maximum value corresponds to a white light shining onto a colored object
                a = 1; // alpha
    };

    // color representation in HSL system
    // values in [0, 1]
    struct HSL
    {
        float h = 0, // hue
                s = 0, // saturation
                l = 0, // lightness, maximum lightness corresponds to a pure white color
                a = 1; // alpha
    };

    // color representation as one grey-scale intensity v equivalent in RGB to the grey tone (v, v, v, alpha)
    // values in [0, 1]
    struct GrayScale
    {
        float v = 0, // value
                a = 1; // alpha
    };

    // class representing 32-bit floating point colors
    class Color : public Eigen::Array<float, 1, 4>
    {
        using Vector_t = Eigen::Array<float, 1, 4>;

        public:
            // CTORS
            Color();
            Color(float);
            Color(float red, float green, float blue, float alpha = 1);
            explicit Color(RGB rgb);
            explicit Color(HSV hsv);
            explicit Color(HSL hsl);
            explicit Color(GrayScale grayscale);

            float& red();
            [[nodiscard]] float red() const;

            float& green();
            [[nodiscard]] float green() const;

            float& blue();
            [[nodiscard]] float blue() const;

            float& alpha();
            [[nodiscard]] float alpha() const;

            // @brief: transform color into different representation
            [[nodiscard]] RGB as_rgb() const;
            [[nodiscard]] HSV as_hsv() const;
            [[nodiscard]] HSL as_hsl() const;
            [[nodiscard]] GrayScale as_grayscale() const;

            // @brief: assignment from different representations
            Color & operator=(RGB);
            Color & operator=(HSV);
            Color & operator=(HSL);
            Color & operator=(GrayScale);

            using Vector_t::operator-;
            using Vector_t::operator+;
            using Vector_t::operator*;
            using Vector_t::operator/;

            using Vector_t::operator-=;
            using Vector_t::operator+=;
            using Vector_t::operator*=;
            using Vector_t::operator/=;

            using Vector_t::operator==;
            using Vector_t::operator!=;

            // @returns: negative of color equals to {1 - r, 1 - g, 1 - b, a}
            [[nodiscard]] Color invert() const;
    };

    // @brief: mix override for colors, c.f. vector.hpp for the generalized function
    // @param a: first color
    // @param b: second color of same type as a
    // @param weight: weight, 0 for 100% first color, 1 for 100% second color. Values outside of [0,1] are clamped
    template<typename Color_t>
    Color_t mix(Color_t a, Color_t b, float weight);

    // @brief: convert one color representation into another
    // @param from: any of type {RGB, HSV, HSL, GrayScale, Color}
    // @returns: any of type {RBG, HSV, HSL, GrayScale, Color}
    template<typename T1, typename T2>
    T2 convert_to(T1 from);
}

#include ".src/color.inl"

