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

            // pre-defined transforms. Use with transform.set_function(IntensityTransform<Value_t>::function)

            // @brief the identity function: f(x) = x
            static auto&& identity();

            // @brief inverts the image values: f(x) = lmax - x
            // @param upper_bound: lmax (usually 1)
            static auto&& invert(Value_t upper_bound = Value_t(1.f));

            // @brief log-scales the intensitiy values: f(x) = c * log(x)
            // @param c: factor to be applied
            static auto&& log_scale(Value_t c = Value_t(1.f));

            // @brief power-law function: f(x) = c * (x + eps)^gamma
            // @param gamma: exponent, should be in the range (0, 1] or [1, +Inf)
            // @param c: factor to be applied
            // @param eps: gamma-offset, usually 0 for properly calibrated displays
            static auto&& power_law(float gamma, Value_t c = Value_t(1.f), Value_t eps = Value_t(0.f));

            // @brief replaces all pixels outside of the boundary interval [lmin, lmax]
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            // @param replacement: Value that intensities outside of [lmin, lmax] are mapped to
            static auto&& isolate(Value_t lower_bound, Value_t upper_bound, Value_t replacement = Value_t(0.f));

            // @brief clamps intensity values, f(x) = min(max(x, lmin), lmax)
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            static auto&& clamp(Value_t lower_bound, Value_t upper_bound);

            // @brief performs linear interpolation, f(x) = x * |lmax - lmin| + min(lmin, lmax)
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            static auto&& project(Value_t lower_bound, Value_t upper_bound);

            // @brief performs 3rd order (cubic) Hermite Interpolation in [lmin, lmax]
            // @param lower_bound: lmin
            // @param upper_bound: lmax
            static auto&& smoothstep(Value_t lower_bound, Value_t upper_bound);

        protected:
            Value_t operator()(Value_t current, long x, long y);

        private:
            std::function<Value_t(Value_t, long, long)> _function = identity();
    };

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::identity()
    {
        return std::move([](Value_t value, long x, long y) -> Value_t {return value;});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::invert(Value_t upper_bound)
    {
        return std::move([upper_bound](Value_t value, long x, long y) -> Value_t {return static_cast<Value_t>(float(upper_bound) - float(value));});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::log_scale(Value_t c)
    {
        return std::move([c](Value_t value, long x, long y) -> Value_t {return c * log_n(1 + value);});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::power_law(float gamma, Value_t c, Value_t eps)
    {
        return std::move([c, gamma](Value_t value, long x, long y) -> Value_t {return c * powf(value, gamma);});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::isolate(Value_t lower_bound, Value_t upper_bound, Value_t replacement)
    {
        return std::move([lower_bound, upper_bound, replacement](Value_t value, long x, long y) -> Value_t {return (value >= lower_bound and value <= upper_bound ? value : replacement);});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::clamp(Value_t lower_bound, Value_t upper_bound)
    {
        return std::move([lower_bound, upper_bound](Value_t value, long x, long y) -> Value_t {return std::min(std::max(value, lower_bound), upper_bound);});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::project(Value_t lower_bound, Value_t upper_bound)
    {
        return std::move([lower_bound, upper_bound](Value_t value, long x, long y) -> Value_t {return value * abs(lower_bound - upper_bound) + min(lower_bound, upper_bound);});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::smoothstep(Value_t lower_bound, Value_t upper_bound)
    {
        return std::move([lower_bound, upper_bound](Value_t value, long x, long y) -> Value_t
        {
            value = std::min(std::max(value, lower_bound), upper_bound);  // clamp
            return x * x * (3 - 2 * x);
        });
    }

    template<typename Value_t>
    void IntensityTransform<Value_t>::set_function(std::function<Value_t(Value_t, long, long)>&& lambda)
    {
        _function = lambda;
    }

    template<typename Value_t>
    Value_t IntensityTransform<Value_t>::operator()(Value_t current, long x, long y)
    {
        return _function(current, x, y);
    }

    template<typename Value_t>
    template<typename Image_t>
    void IntensityTransform<Value_t>::apply_to(Image_t& image)
    {
        static_assert(std::is_same_v<Value_t, typename Image_t::value_t>, "image and transform must work in the same value domain");

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                image(x, y) = (*this)(image(x, y), x, y);
    }

}