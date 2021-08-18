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
}

#include ".src/histogram.inl"