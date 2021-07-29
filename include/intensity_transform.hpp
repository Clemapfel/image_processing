// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <functional>

namespace crisp
{
    // functor object that applies a function that maps a pixels value to a function of it's current value and coordinates
    template<typename Value_t>
    class IntensityTransform
    {
        public:
            IntensityTransform() = default;

            template<typename Image_t>
            void apply_to(Image_t& image);

            void set_function(std::function<Value_t(Value_t, long, long)>&& lambda);

            [[nodiscard]] static inline auto&& identity()
            {
                return std::move([](Value_t value, long x, long y) -> Value_t {return value;});
            }

            [[nodiscard]] static inline auto&& invert(Value_t upper_bound = Value_t(1.f))
            {
                return std::move([upper_bound](Value_t value, long x, long y) -> Value_t {return static_cast<Value_t>(float(upper_bound) - float(value));});
            }

            [[nodiscard]] static inline auto&& log_scale(Value_t c = Value_t(1.f))
            {
                return std::move([c](Value_t value, long x, long y) -> Value_t {return c * log_n(1 + value);});
            }

            [[nodiscard]] static inline auto&& power_law(float gamma, Value_t c = Value_t(1.f))
            {
                return std::move([c, gamma](Value_t value, long x, long y) -> Value_t {return c * powf(value, gamma);});
            }

            [[nodiscard]] static inline auto&& isolate(Value_t lower_bound, Value_t upper_bound, Value_t replacement)
            {
                return std::move([lower_bound, upper_bound, replacement](Value_t value, long x, long y) -> Value_t {return (value >= lower_bound and value <= upper_bound ? value : replacement);});
            }

            [[nodiscard]] static inline auto&& clamp(Value_t lower_bound, Value_t upper_bound)
            {
                return std::move([lower_bound, upper_bound](Value_t value, long x, long y) -> Value_t {return std::min(std::max(value, lower_bound), upper_bound);});
            }

        protected:
            Value_t operator()(Value_t current, long x, long y);

        private:
            std::function<Value_t(Value_t, long, long)> _function = identity();
    });

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