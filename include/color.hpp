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
            // @brief construct color equal to rgba(0, 0, 0, 1)
            Color();

            // @brief construct color
            // @param : value v such that the resulting color is equal to rgba(v, v, v, 1)
            Color(float);

            // @brief construct color
            // @param red: the red component
            // @param green: the green component
            // @param blue: the blue component
            // @param alpha: the transparency component
            Color(float red, float green, float blue, float alpha = 1);

            // @brief construct color from RGB representation
            explicit Color(RGB rgb);

            // @brief construct color from HSV representation
            explicit Color(HSV hsv);

            // @brief construct color from HSL representation
            explicit Color(HSL hsl);

            // @brief construct color from GrayScale representation
            explicit Color(GrayScale grayscale);

            // @brief expose the red component
            // @returns writable reference to the component
            float& red();

            // @brief access red component
            // @returns value of the component
            [[nodiscard]] float red() const;

            // @brief expose the green component
            // @returns writable reference to the component
            float& green();

            // @brief access green component
            // @returns value of the component
            [[nodiscard]] float green() const;

            // @brief expose the blue component
            // @returns writable reference to the component
            float& blue();

            // @brief access blue component
            // @returns value of the component
            [[nodiscard]] float blue() const;

            // @brief expose the transparency component
            // @returns writable reference to the component
            float& alpha();

            // @brief access transparency component
            // @returns value of the component
            [[nodiscard]] float alpha() const;

            // @brief convert to RGB representation
            // @returns color in rgba format
            [[nodiscard]] RGB as_rgb() const;

            // @brief convert to HSV representation
            // @returns color in hsva format
            [[nodiscard]] HSV as_hsv() const;

            // @brief convert to HSL representation
            // @returns color in hsla format
            [[nodiscard]] HSL as_hsl() const;

            // @brief convert to GrayScale representation
            // @returns color in grayscale format including transparency
            [[nodiscard]] GrayScale as_grayscale() const;

            // @brief assign RGB representation to self
            // @param : RGB representation
            // @returns reference to self after assignemnt
            Color & operator=(RGB);

            // @brief assign HSV representation to self
            // @param : HSV representation
            // @returns reference to self after assignemnt
            Color & operator=(HSV);

            // @brief assign HSL representation to self
            // @param : HSL representation
            // @returns reference to self after assignemnt
            Color & operator=(HSL);

            // @brief assign GrayScale representation to self
            // @param : GrayScale representation
            // @returns reference to self after assignemnt
            Color & operator=(GrayScale);

            // @brief perform element-wise addition
            // @param : another color
            // @returns vector of values after operation
            using Vector_t::operator+;

            // @brief perform element-wise subtraction
            // @param : another color
            // @returns vector of values after operation
            using Vector_t::operator-;

            // @brief perform element-wise inner product
            // @param : another color
            // @returns vector of values after operation
            using Vector_t::operator*;

            // @brief perform element-wise inner division
            // @param : another color
            // @returns vector of values after operation
            using Vector_t::operator/;

            // @brief addition assignment
            // @param : another color
            // @returns reference to self after assignment
            using Vector_t::operator+=;

            // @brief subtraction assignment
            // @param : another color
            // @returns reference to self after assignment
            using Vector_t::operator-=;

            // @brief inner product assignment
            // @param : another color
            // @returns reference to self after assignment
            using Vector_t::operator*=;

            // @brief inner division assignment
            // @param : another color
            // @returns reference to self after assignment
            using Vector_t::operator/=;

            // @brief boolean element-wise comparison
            // @param : another color
            // @returns true if all components of both colors are equal, false otherwise
            using Vector_t::operator==;

            // @brief boolean element-wise comparison
            // @param : another color
            // @returns false if all components of both colors are equal, true otherwise
            using Vector_t::operator!=;

            // @brief invert color
            // @returns: Color equal to {1 - r, 1 - g, 1 - b, a}
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

