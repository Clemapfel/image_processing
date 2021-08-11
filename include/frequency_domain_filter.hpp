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
            void set_offset(long x_dist_from_center, long y_dist_from_center, bool force_symmetry = true);

            FrequencyDomainFilter operator+(const FrequencyDomainFilter&) const;
            FrequencyDomainFilter operator*(const FrequencyDomainFilter&) const;
            FrequencyDomainFilter operator-(const FrequencyDomainFilter&) const;
            FrequencyDomainFilter operator/(const FrequencyDomainFilter&) const;

            FrequencyDomainFilter& operator+=(const FrequencyDomainFilter&);
            FrequencyDomainFilter& operator*=(const FrequencyDomainFilter&);
            FrequencyDomainFilter& operator-=(const FrequencyDomainFilter&);
            FrequencyDomainFilter& operator/=(const FrequencyDomainFilter&);
            
            virtual double & operator()(long x, long y);
            virtual double operator()(long x, long y) const;

            const std::vector<double>& get_values() const;
            sf::Vector2<long> get_size() const;

            template<FourierTransformMode Mode>
            void apply_to(FourierTransform<Mode>&) const;

            auto&& identity();
            
            auto&& ideal_lowpass(double cutoff_frequency, double pass_factor = 1, double reject_factor = 0);
            auto&& gaussian_lowpass(double cutoff_frequency, double pass_factor = 1, double reject_factor = 0);
            auto&& butterworth_lowpass(double cutoff_frequency, size_t order, double pass_factor = 1, double reject_factor = 0);

            auto&& ideal_highpass(double cutoff_frequency, double pass_factor = 1, double reject_factor = 0);
            auto&& gaussian_highpass(double cutoff_frequency, double pass_factor = 1, double reject_factor = 0);
            auto&& butterworth_highpass(double cutoff_frequency, size_t order, double pass_factor = 1, double reject_factor = 0);

            auto&& ideal_bandpass(double lower_cutoff, double higher_cutoff, double pass_factor = 1, double reject_factor = 0);
            auto&& gaussian_bandpass(double lower_cutoff, double higher_cutoff, double pass_factor = 1, double reject_factor = 0);
            auto&& butterworth_bandpass(double lower_cutoff, double higher_cutoff, size_t order, double pass_factor = 1, double reject_factor = 0);

            auto&& ideal_bandreject(double lower_cutoff, double higher_cutoff, double pass_factor = 1, double reject_factor = 0);
            auto&& gaussian_bandreject(double lower_cutoff, double higher_cutoff, double pass_factor = 1, double reject_factor = 0);
            auto&& butterworth_bandreject(double lower_cutoff, double higher_cutoff, size_t order, double pass_factor = 1, double reject_factor = 0);

            auto&& laplacian_first_derivative();
            
        private:
            sf::Vector2<long> _size;
            bool _offset_symmetrical = true;
            sf::Vector2<long> _offset = {0,0};
            std::function<double(long, long)> _function = [](long x, long y) -> double {return 1;};
            
            double distance(long x, long y);

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

    inline void FrequencyDomainFilter::set_offset(long x_dist_from_center, long y_dist_from_center, bool force_symmetry)
    {
        _offset_symmetrical = force_symmetry;
        _offset = {x_dist_from_center, y_dist_from_center};
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

        for (long x = 0, i = 0; x < _size.x; ++x)
            for (long y = 0; y < _size.y; ++y, ++i)
                _values.emplace_back(_function(x - _offset.x, y - _offset.y) * (_offset_symmetrical ? _function(x + _offset.x, y + _offset.y) : 1));

        _values_initialized = true;
    }
    
    inline double FrequencyDomainFilter::distance(long x_in, long y_in)
    {
        auto x = x_in - _size.x / 2.f;
        auto y = y_in - _size.y / 2.f;
        return sqrt(x*x + y*y);
    }

    inline auto && FrequencyDomainFilter::identity()
    {
        return std::move([](long x, long y) -> double {return 1;});
    }

    // IDEAL
    inline auto && FrequencyDomainFilter::ideal_lowpass(double cutoff_frequency, double pass_factor, double reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {
            return distance(x, y) < cutoff_frequency ? pass_factor : reject_factor;
        });
    }

    inline auto && FrequencyDomainFilter::ideal_highpass(double cutoff_frequency, double pass_factor, double reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {return distance(x, y) < cutoff_frequency ? reject_factor : pass_factor;});
    }

    inline auto&& FrequencyDomainFilter::ideal_bandreject(double lower_cutoff, double higher_cutoff, double pass_factor, double reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            return dist > lower_cutoff and dist < higher_cutoff ? reject_factor : pass_factor;
        });
    }

    inline auto&& FrequencyDomainFilter::ideal_bandpass(double lower_cutoff, double higher_cutoff, double pass_factor, double reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            return dist > lower_cutoff and dist < higher_cutoff ? pass_factor : reject_factor;
        });
    }

    // GAUSSIAN
    
    inline auto && FrequencyDomainFilter::gaussian_lowpass(double cutoff_frequency, double pass_factor, double reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {
            
            auto dist = distance(x, y);
            auto res = exp(-0.5 * pow(distance(x, y) / cutoff_frequency, 2));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    inline auto && FrequencyDomainFilter::gaussian_highpass(double cutoff_frequency, double pass_factor, double reject_factor)
    {
        return std::move([this, cutoff_frequency, pass_factor, reject_factor](long x, long y) -> double {

            auto dist = distance(x, y);
            auto res = exp(-0.5 * pow(distance(x, y) / cutoff_frequency, 2));
            return 1 - project<double>(pass_factor, reject_factor, res);
        });
    }

    inline auto&& FrequencyDomainFilter::gaussian_bandreject(double lower_cutoff, double higher_cutoff, double pass_factor,
                                                    double reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;
            auto res = 1 - exp(-1*pow((dist*dist - center*center) / (dist*width), 2));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    inline auto&& FrequencyDomainFilter::gaussian_bandpass(double lower_cutoff, double higher_cutoff, double pass_factor,
                                                    double reject_factor)
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
    
    inline auto && FrequencyDomainFilter::butterworth_lowpass(double cutoff_frequency, size_t order, double pass_factor, double reject_factor)
    {
        return std::move([this, cutoff_frequency, order, pass_factor, reject_factor](long x, long y) -> double {
            auto res = 1 / (1 + pow(distance(x, y) / cutoff_frequency, 2 * order));
            return project<double>(reject_factor, pass_factor, res);
        });
    }


    inline auto && FrequencyDomainFilter::butterworth_highpass(double cutoff_frequency, size_t order, double pass_factor, double reject_factor)
    {
        return std::move([this, cutoff_frequency, order, pass_factor, reject_factor](long x, long y) -> double {
            auto res = (1 / (1 + pow(distance(x, y) / cutoff_frequency, 2 * order)));
            return 1 - project<double>(pass_factor, reject_factor, res);
        });
    }

    inline auto && FrequencyDomainFilter::butterworth_bandreject(double lower_cutoff, double higher_cutoff, size_t order, double pass_factor, double reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, order, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;

            auto res = 1 / (1 + pow((dist*width)/(dist*dist-center*center), 2*order));
            return project<double>(reject_factor, pass_factor, res);
        });
    }

    inline auto && FrequencyDomainFilter::butterworth_bandpass(double lower_cutoff, double higher_cutoff, size_t order, double pass_factor, double reject_factor)
    {
        return std::move([this, lower_cutoff, higher_cutoff, order, pass_factor, reject_factor](long x, long y) -> double {
            auto dist = distance(x, y);
            auto width = higher_cutoff - lower_cutoff;
            auto center = lower_cutoff + width/2;

            auto res = 1 / (1 + pow((dist*width)/(dist*dist-center*center), 2*order));
            return project<double>(reject_factor, pass_factor, 1 - res);
        });
    }

    template<FourierTransformMode Mode>
    void FrequencyDomainFilter::apply_to(FourierTransform<Mode>& fourier) const
    {
        assert(fourier.get_size().x == get_size().x and fourier.get_size().y == get_size().y);

        if (not _values_initialized)
            initialize();

        for (long x = 0, i = 0; x < fourier.get_size().x; ++x)
            for (long y = 0; y < fourier.get_size().y; ++y, ++i)
                fourier.get_component(x, y) *= _values.at(i);
    }

    inline FrequencyDomainFilter FrequencyDomainFilter::operator+(const FrequencyDomainFilter& other) const
    {
        assert(_size == other._size);

        auto out = FrequencyDomainFilter(_size.x, _size.y);
        out._values.clear();

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
                out._values.push_back(_values.at(i) + other._values.at(i));

        return out;
    }

    inline FrequencyDomainFilter FrequencyDomainFilter::operator-(const FrequencyDomainFilter& other) const
    {
        assert(_size == other._size);

        auto out = FrequencyDomainFilter(_size.x, _size.y);
        out._values.clear();

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
                out._values.push_back(_values.at(i) - other._values.at(i));

        return out;
    }

    inline FrequencyDomainFilter FrequencyDomainFilter::operator*(const FrequencyDomainFilter& other) const
    {
        assert(_size == other._size);

        auto out = FrequencyDomainFilter(_size.x, _size.y);
        out._values.clear();

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
                out._values.push_back(_values.at(i) * other._values.at(i));

        return out;
    }

    inline FrequencyDomainFilter FrequencyDomainFilter::operator/(const FrequencyDomainFilter& other) const
    {
        assert(_size == other._size);

        auto out = FrequencyDomainFilter(_size.x, _size.y);
        out._values.clear();

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
                out._values.push_back(_values.at(i) / other._values.at(i));

        return out;
    }

    inline FrequencyDomainFilter& FrequencyDomainFilter::operator+=(const FrequencyDomainFilter& other)
    {
        assert(_size == other._size);

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
            _values.at(i) = _values.at(i) + other._values.at(i);

        return *this;
    }

    inline FrequencyDomainFilter& FrequencyDomainFilter::operator-=(const FrequencyDomainFilter& other)
    {
        assert(_size == other._size);

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
            _values.at(i) = _values.at(i) - other._values.at(i);

        return *this;
    }

    inline FrequencyDomainFilter& FrequencyDomainFilter::operator*=(const FrequencyDomainFilter& other)
    {
        assert(_size == other._size);

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
            _values.at(i) = _values.at(i) * other._values.at(i);

        return *this;
    }

    inline FrequencyDomainFilter& FrequencyDomainFilter::operator/=(const FrequencyDomainFilter& other)
    {
        assert(_size == other._size);

        if (not _values_initialized)
            initialize();

        if (not other._values_initialized)
            other.initialize();

        for (size_t i = 0; i < _values.size(); ++i)
            _values.at(i) = _values.at(i) / other._values.at(i);

        return *this;
    }

    double& FrequencyDomainFilter::operator()(long x, long y)
    {
        return _values.at(y + _size.y * x);
    }

    double FrequencyDomainFilter::operator()(long x, long y) const
    {
        return _values.at(y + _size.y * x);
    }
}

#include ".src/frequency_domain_filter.inl"