// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
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
        return std::move([lower_bound, upper_bound](Value_t value, long x, long y) -> Value_t {return value * fabs(lower_bound - upper_bound) + std::min(lower_bound, upper_bound);});
    }

    template<typename Value_t>
    [[nodiscard]] auto&& IntensityTransform<Value_t>::smoothstep(Value_t lower_bound, Value_t upper_bound)
    {
        // source c.f. Ken Perlin, "Texturing and Modeling, Third Edition: A Procedural Approach" (3rd Edition)
        return std::move([lower_bound, upper_bound](Value_t value, long, long) -> Value_t
        {
            float lmin = float(lower_bound);
            float lmax = float(upper_bound);
            float x = float(value);

            auto clamp = [](float x, float lowerlimit, float upperlimit) -> float
            {
                if (x < lowerlimit)
                    x = lowerlimit;
                if (x > upperlimit)
                    x = upperlimit;
                return x;
            };

            x = clamp((x - lower_bound) / (upper_bound - lower_bound), 0.0, 1.0);
            float res = x * x * x * (x * (x * 6 - 15) + 10);

            return Value_t(res);
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