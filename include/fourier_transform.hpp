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
            Image_t spectrum_to_image() const;

            template<typename Image_t>
            Image_t phase_angle_to_image() const;

            std::complex<Value_t> get_coefficient(long x, long y) const;

            Value_t get_component(long x, long y) const;
            Value_t get_phase_angle(long x, long y) const;

            Value_t& get_component(long x, long y);
            Value_t& get_phase_angle(long x, long y);

            Value_t get_dc_component() const;

            sf::Vector2<long> get_size() const;

        protected:
            std::vector<Value_t>& get_spectrum();
            std::vector<Value_t>& get_phase_angle();

        private:
            size_t to_index(long x, long y) const;

            sf::Vector2<long> _size;
            std::vector<Value_t> _spectrum,
                                 _phase_angle;

            Value_t _min_spectrum = 0, _max_spectrum = 1; // log(1+x) scaled
    };
}

#include ".src/fourier_transform.inl"