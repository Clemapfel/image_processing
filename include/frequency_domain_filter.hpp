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

            auto&& identity();
            
            auto&& ideal_lowpass(float cutoff_frequency);
            auto&& gaussian_lowpass(float cutoff_frequency);
            auto&& butterworth_lowpass(float cutoff_frequency, size_t order);

            auto&& ideal_highpass(float cutoff_frequency);
            auto&& gaussian_highpass(float cutoff_frequency);
            auto&& butterworth_highpass(float cutoff_frequency, size_t order);
            
        private:
            sf::Vector2<long> _size;
            std::function<double(long, long)> _function = [](long x, long y) -> double {return 1;};
            
            float distance(long x, long y);
            
            void initialize() const;
            mutable bool _values_initialized = false;
            mutable std::vector<double> _values;
    };

    inline FrequencyDomainFilter::FrequencyDomainFilter(long width, long height)
        : _size(width, height)
    {}

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
    
    inline float FrequencyDomainFilter::distance(long x_in, long y_in)
    {
        auto x = x_in - _size.x / 2.f;
        auto y = y_in - _size.y / 2.f;
        return sqrt(x*x + y*y);
    }

    // ##
    
    inline auto && FrequencyDomainFilter::identity()
    {
        return std::move([](long x, long y) -> double {return 1;});
    }
    
    inline auto && FrequencyDomainFilter::ideal_lowpass(float cutoff_frequency)
    {
        return std::move([this, cutoff_frequency](long x, long y) -> double {return distance(x, y) < cutoff_frequency ? 1 : 0;});
    }
    
    inline auto && FrequencyDomainFilter::gaussian_lowpass(float cutoff_frequency)
    {
        return std::move([this, cutoff_frequency](long x, long y) -> double {
            
            auto dist = distance(x, y);
            return exp(-0.5 * pow(distance(x, y) / cutoff_frequency, 2));
        });
    }
    
    inline auto && FrequencyDomainFilter::butterworth_lowpass(float cutoff_frequency, size_t order)
    {
        return std::move([this, cutoff_frequency, order](long x, long y) -> double {
            return 1 / (1 + pow(distance(x, y) / cutoff_frequency, 2 * order));
        });
    }

    inline auto && FrequencyDomainFilter::ideal_highpass(float cutoff_frequency)
    {
        return std::move([this, cutoff_frequency](long x, long y) -> double {return distance(x, y) < cutoff_frequency ? 0 : 1;});
    }

    inline auto && FrequencyDomainFilter::gaussian_highpass(float cutoff_frequency)
    {
        return std::move([this, cutoff_frequency](long x, long y) -> double {

            auto dist = distance(x, y);
            return 1 - exp(-0.5 * pow(distance(x, y) / cutoff_frequency, 2));
        });
    }

    inline auto && FrequencyDomainFilter::butterworth_highpass(float cutoff_frequency, size_t order)
    {
        return std::move([this, cutoff_frequency, order](long x, long y) -> double {
            return 1 - (1 / (1 + pow(distance(x, y) / cutoff_frequency, 2 * order)));
        });
    }


}

#include ".src/frequency_domain_filter.inl"