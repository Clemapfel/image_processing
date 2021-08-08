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
            void create_from(Image_t&);

            size_t at(DiscreteValue_t key) const;
            const std::map<DiscreteValue_t, size_t>& get_counts() const;

        private:
            std::map<DiscreteValue_t, size_t> _counts;
    };

    template<typename DiscreteValue_t>
    template<typename Image_t>
    void Histogram<DiscreteValue_t>::create_from(Image_t& image)
    {
        _counts.clear();
        DiscreteValue_t min = std::numeric_limits<DiscreteValue_t>::max(),
                        max = DiscreteValue_t(0);

        for (auto& px : image)
        {
            auto key = static_cast<DiscreteValue_t>(round(float(px) * float(std::numeric_limits<DiscreteValue_t>::max())));
            if (_counts.find(key) == _counts.end())
                _counts.emplace(key, 1);
            else
                _counts[key] += 1;
        }

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