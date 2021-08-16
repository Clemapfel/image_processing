// 
// Copyright 2021 Clemens Cords
// Created on 06.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <type_traits>
#include <limits>
#include <cstdint>
#include <map>

namespace crisp
{
    template<typename DiscreteValue_t = uint8_t>
    class Histogram
    {
        public:
            Histogram() = default;

            template<typename Image_t>
            void create_from(const Image_t&);

            size_t at(DiscreteValue_t key) const;
            const std::map<DiscreteValue_t, size_t>& get_counts() const;

            float mean() const;
            float median() const;
            float min_count() const;
            float max_count() const;
            float min_value() const;
            float max_value() const;

        private:
            std::map<DiscreteValue_t, size_t> _counts;

            float _mean,
                  _median,
                  _min_intensity,
                  _max_intensity;

            size_t _min_count,
                   _max_count;
    };

    template<typename DiscreteValue_t>
    template<typename Image_t>
    void Histogram<DiscreteValue_t>::create_from(const Image_t& image)
    {
        _counts.clear();
        const DiscreteValue_t min = std::numeric_limits<DiscreteValue_t>::max(),
                        max = DiscreteValue_t(0);

        _mean = 0;
        _min_intensity = 1;
        _max_intensity = 0;

        size_t n = 0;
        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                auto value = float(image(x, y));
                auto key = static_cast<DiscreteValue_t>(round(
                        value * float(std::numeric_limits<DiscreteValue_t>::max())));
                if (_counts.find(key) == _counts.end())
                    _counts.emplace(key, 1);
                else
                    _counts[key] += 1;

                n += 1;
                _mean += value;
                _min_intensity = std::min(_min_intensity, value);
                _max_intensity = std::max(_max_intensity, value);
            }
        }

        auto sum = _mean;
        _mean = _mean / n;

        _min_count = max;
        _max_count = 0;

        _median = -1;
        size_t current_sum = 0;
        size_t i = 0;
        for (const auto& pair : _counts)
        {
            _min_count = std::min(_min_count, pair.second);
            _max_count = std::max(_max_count, pair.second);

            current_sum += pair.second;
            if (current_sum >= 0.5 * n and _median == -1)
                _median = (i / 255.f);

            i += 1;
        }
    }

    template<typename DiscreteValue_t>
    float Histogram<DiscreteValue_t>::mean() const
    {
        return _mean;
    }

    template<typename DiscreteValue_t>
    float Histogram<DiscreteValue_t>::median() const
    {
        return _median;
    }

    template<typename DiscreteValue_t>
    float Histogram<DiscreteValue_t>::min_value() const
    {
        return _min_intensity;
    }

    template<typename DiscreteValue_t>
    float Histogram<DiscreteValue_t>::max_value() const
    {
        return _max_intensity;
    }

    template<typename DiscreteValue_t>
    float Histogram<DiscreteValue_t>::min_count() const
    {
        return _min_count;
    }

    template<typename DiscreteValue_t>
    float Histogram<DiscreteValue_t>::max_count() const
    {
        return _max_count;
    }

    template<typename DiscreteValue_t>
    const std::map<DiscreteValue_t, size_t>& Histogram<DiscreteValue_t>::get_counts() const
    {
        return _counts;
    }

    template<typename DiscreteValue_t>
    size_t Histogram<DiscreteValue_t>::at(DiscreteValue_t key) const
    {
        auto it = _counts.find(key);
        if (it != _counts.end())
            return it->second;
        else
            return 0;
    }
}