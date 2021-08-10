// 
// Copyright 2021 Clemens Cords
// Created on 09.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <grayscale_image.hpp>
#include <fourier_transform.hpp>

namespace crisp
{
    class FrequencyDomainFilter
    {
        public:
            FrequencyDomainFilter(long width, long height);

            void set_function(std::function<double(long, long)>&& lambda);
            const std::vector<double>& get_values() const;
            sf::Vector2<long> get_size() const;

            template<FourierTransformMode Mode>
            void apply_to(FourierTransform<Mode>&) const;

            auto&& identity();
            
            auto&& ideal_lowpass(float cutoff_frequency, float pass_factor = 1, float reject_factor = 0);
            auto&& gaussian_lowpass(float cutoff_frequency, float pass_factor = 1, float reject_factor = 0);
            auto&& butterworth_lowpass(float cutoff_frequency, size_t order, float pass_factor = 1, float reject_factor = 0);

            auto&& ideal_highpass(float cutoff_frequency, float pass_factor = 1, float reject_factor = 0);
            auto&& gaussian_highpass(float cutoff_frequency, float pass_factor = 1, float reject_factor = 0);
            auto&& butterworth_highpass(float cutoff_frequency, size_t order, float pass_factor = 1, float reject_factor = 0);

            auto&& ideal_bandpass(float lower_cutoff, float higher_cutoff, float pass_factor = 1, float reject_factor = 0);
            auto&& gaussian_bandpass(float lower_cutoff, float higher_cutoff, float pass_factor = 1, float reject_factor = 0);
            auto&& butterworth_bandpass(float lower_cutoff, float higher_cutoff, size_t order, float pass_factor = 1, float reject_factor = 0);

            auto&& ideal_bandreject(float lower_cutoff, float higher_cutoff, float pass_factor = 1, float reject_factor = 0);
            auto&& gaussian_bandreject(float lower_cutoff, float higher_cutoff, float pass_factor = 1, float reject_factor = 0);
            auto&& butterworth_bandreject(float lower_cutoff, float higher_cutoff, size_t order, float pass_factor = 1, float reject_factor = 0);

            auto&& laplacian_first_derivative();
            
        private:
            sf::Vector2<long> _size;
            std::function<double(long, long)> _function = [](long x, long y) -> double {return 1;};
            
            float distance(long x, long y);
            template<typename T>
            static T project(T lower_bound, T upper_bound, T value)
            {
                return value * fabs(lower_bound - upper_bound) + std::min(lower_bound, upper_bound);
            }
            
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

    inline auto && FrequencyDomainFilter::identity()
    {
        return std::move([](long x, long y) -> double {return 1;});
    }


    inline auto && FrequencyDomainFilter::laplacian_first_derivative()
    {
        return std::move([](long x, long y) -> double {return -4 * M_PI * M_PI + (x*x + y*y);});
    }

    // IDEAL
    
    inline auto && FrequencyDomainFilter::ideal_lowpass(float cutoff_frequency, float pass_factor, float reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {
            return distance(x, y) < cutoff_frequency ? pass_factor : reject_factor;
        });
    }

    inline auto && FrequencyDomainFilter::ideal_highpass(float cutoff_frequency, float pass_factor, float reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {return distance(x, y) < cutoff_frequency ? reject_factor : pass_factor;});
    }

    inline auto&& FrequencyDomainFilter::ideal_bandreject(float lower_cutoff, float higher_cutoff, float pass_factor, float reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            return dist > lower_cutoff and dist < higher_cutoff ? reject_factor : pass_factor;
        });
    }

    inline auto&& FrequencyDomainFilter::ideal_bandpass(float lower_cutoff, float higher_cutoff, float pass_factor, float reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            return dist > lower_cutoff and dist < higher_cutoff ? pass_factor : reject_factor;
        });
    }

    // GAUSSIAN
    
    inline auto && FrequencyDomainFilter::gaussian_lowpass(float cutoff_frequency, float pass_factor, float reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {
            
            auto dist = distance(x, y);
            auto res = exp(-0.5 * pow(distance(x, y) / cutoff_frequency, 2));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    inline auto && FrequencyDomainFilter::gaussian_highpass(float cutoff_frequency, float pass_factor, float reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {

            auto dist = distance(x, y);
            auto res = exp(-0.5 * pow(distance(x, y) / cutoff_frequency, 2));
            return 1 - project<double>(pass_factor, reject_factor, res);
        });
    }

    inline auto&& FrequencyDomainFilter::gaussian_bandreject(float lower_cutoff, float higher_cutoff, float pass_factor,
                                                    float reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;
            auto res = 1 - exp(-1*pow((dist*dist - center*center) / (dist*width), 2));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    inline auto&& FrequencyDomainFilter::gaussian_bandpass(float lower_cutoff, float higher_cutoff, float pass_factor,
                                                    float reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;
            auto res = exp(-1*pow((dist*dist - center*center) / (dist*width), 2));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    // BUTTERWORTH
    
    inline auto && FrequencyDomainFilter::butterworth_lowpass(float cutoff_frequency, size_t order, float pass_factor, float reject_factor)
    {
        return std::move([this, cutoff_frequency, order, pass_factor, reject_factor](long x, long y) -> double {
            auto res = 1 / (1 + pow(distance(x, y) / cutoff_frequency, 2 * order));
            return project<double>(reject_factor, pass_factor, res);
        });
    }


    inline auto && FrequencyDomainFilter::butterworth_highpass(float cutoff_frequency, size_t order, float pass_factor, float reject_factor)
    {
        return std::move([this, cutoff_frequency, order, pass_factor, reject_factor](long x, long y) -> double {
            auto res = (1 / (1 + pow(distance(x, y) / cutoff_frequency, 2 * order)));
            return 1 - project<double>(pass_factor, reject_factor, res);
        });
    }

    inline auto && FrequencyDomainFilter::butterworth_bandreject(float lower_cutoff, float higher_cutoff, size_t order, float pass_factor, float reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, order, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;

            auto res = 1 / (1 + pow((dist*width)/(dist*dist-center*center), 2*order));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    inline auto && FrequencyDomainFilter::butterworth_bandpass(float lower_cutoff, float higher_cutoff, size_t order, float pass_factor, float reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, order, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;

            auto res = 1 / (1 + pow((dist*width)/(dist*dist-center*center), 2*order));
            return project<double>(reject_factor, pass_factor, 1 - res);
        });
    }




}

#include ".src/frequency_domain_filter.inl"