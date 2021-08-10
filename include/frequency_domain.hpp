// 
// Copyright 2021 Clemens Cords
// Created on 09.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <complex>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <fftw3.h>

namespace crisp
{
    class FourierTransformFilter;

    enum FourierTransformMode : int
    {
        SPEED = 0, BALANCED = 1, ACCURACY = 2
    };

    template<FourierTransformMode Mode = BALANCED>
    class FourierTransform
    {
        friend class FrequencyDomainFilter;
        using Value_t = typename std::conditional<Mode == SPEED, float, double>::type;

        public:
            FourierTransform() = default;

            template<typename Image_t>
            void transform_from(const Image_t&);

            template<typename Image_t>
            Image_t transform_to() const;

            template<typename Image_t>
            Image_t to_image() const;

            sf::Vector2<long> get_dc_position() const;

            std::complex<Value_t> get_coefficient(long x, long y) const;

            Value_t get_magnitude(long x, long y) const;
            Value_t get_phase_angle(long x, long y) const;

            Value_t& get_magnitude(long x, long y);
            Value_t& get_phase_angle(long x, long y);

            sf::Vector2<long> get_size() const;

            std::vector<Value_t>& get_spectrum();
            std::vector<Value_t>& get_phase_angle();

        private:
            size_t to_index(long x, long y) const;

            sf::Vector2<long> _size;
            std::vector<Value_t> _magnitude,
                                 _phase_angle;

            Value_t _min = 0, _max = 1; // log(1+x) scaled
    };
}

#include ".src/frequency_domain.inl"