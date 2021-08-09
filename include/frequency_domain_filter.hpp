// 
// Copyright 2021 Clemens Cords
// Created on 09.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <grayscale_image.hpp>
#include <frequency_domain.hpp>

namespace crisp
{
    class FrequencyDomainFilter
    {
        public:
            FrequencyDomainFilter(long width, long height);

            void set_function(std::function<double(long, long)>&& lambda);
            const std::vector<double>& get_values() const;
            sf::Vector2<long> get_size() const;

            static auto&& identity();

        private:
            sf::Vector2<long> _size;
            std::function<double(long, long)> _function = [](long x, long y) -> double {return 1;};

            void initialize() const;
            mutable bool _values_initialized = false;
            mutable std::vector<double> _values;
    };

    inline FrequencyDomainFilter::FrequencyDomainFilter(long width, long height)
        : _size(width, height)
    {}

    inline auto && FrequencyDomainFilter::identity()
    {
        return std::move([](long x, long y) -> double {return 1;});
    }

    inline const std::vector<double> & FrequencyDomainFilter::get_values() const
    {
        if (not _values_initialized)
            initialize();

        return _values;
    }

    inline void FrequencyDomainFilter::set_function(std::function<double(long, long)>&& f)
    {
        _function = f;
        _values_initialized = false;
    }

    inline sf::Vector2<long> FrequencyDomainFilter::get_size() const
    {
        return _size;
    }

    inline void FrequencyDomainFilter::initialize() const
    {
        _values.clear();
        _values.reserve(_size.x * _size.y);

        size_t i = 0;
        for (long x = 0; x < _size.x; ++x)
            for (long y = 0; y < _size.y; ++y, ++i)
                _values.emplace_back(_function(x, y));

        _values_initialized = true;
    }


}

#include ".src/frequency_domain_filter.inl"