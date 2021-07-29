// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <functional>

namespace crisp
{
    // functor object that elementwise transforms pixels into a value that is a function of the current value and the pixels coordinates
    template<typename Value_t>
    class IntensityTransform
    {
        public:
            IntensityTransform() = default;

            // @brief applies transformation to image
            // @param image: the image to be transform, must have the same value domain as the transform
            // @returns void
            template<typename Image_t>
            void apply_to(Image_t& image);

            // @brief specify the function for the transform
            // @param lambda: a function-like object such as an anonymous lambda of the correct signature
            // @returns void
            void set_function(std::function<Value_t(Value_t, long, long)>&& lambda);

            // pre-defined transforms. Use with transform.set_function(IntensityTransform<Value_t>::foo);

            // @brief the identity function: f(x) = x
            // @returns lambda meant as argument for set_function
            static auto&& identity();

            // @brief inverts the image values: f(x) = lmax - x
            // @param upper_bound: lmax (usually 1)
            // @returns lambda meant as argument for set_function
            static auto&& invert(Value_t upper_bound = Value_t(1.f));

            // @brief log-scales the intensitiy values: f(x) = c * log(x)
            // @param c: factor to be applied
            // @returns lambda meant as argument for set_function
            static auto&& log_scale(Value_t c = Value_t(1.f));

            // @brief power-law function: f(x) = c * (x + eps)^gamma
            // @param gamma: exponent, should be in the range (0, 1] or [1, +Inf)
            // @param c: factor to be applied
            // @param eps: gamma-offset, usually 0 for properly calibrated displays
            // @returns lambda meant as argument for set_function
            static auto&& power_law(float gamma, Value_t c = Value_t(1.f), Value_t eps = Value_t(0.f));

            // @brief replaces all pixels outside of the boundary interval [lmin, lmax]
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            // @param replacement: Value that intensities outside of [lmin, lmax] are mapped to
            // @returns lambda meant as argument for set_function
            static auto&& isolate(Value_t lower_bound, Value_t upper_bound, Value_t replacement = Value_t(0.f));

            // @brief clamps intensity values, f(x) = min(max(x, lmin), lmax)
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            // @returns lambda meant as argument for set_function
            static auto&& clamp(Value_t lower_bound, Value_t upper_bound);

            // @brief performs linear interpolation, f(x) = x * |lmax - lmin| + min(lmin, lmax)
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            // @returns lambda meant as argument for set_function
            static auto&& project(Value_t lower_bound, Value_t upper_bound);

            // @brief performs Hermite Interpolation in [lmin, lmax]
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            // @returns lambda meant as argument for set_function
            static auto&& smoothstep(Value_t lower_bound, Value_t upper_bound);

        protected:
            Value_t operator()(Value_t current, long x, long y);

        private:
            std::function<Value_t(Value_t, long, long)> _function = identity();
    };
}

#include ".src/intensity_transform.inl"