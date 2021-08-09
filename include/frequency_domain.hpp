// 
// Copyright 2021 Clemens Cords
// Created on 09.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <vector.hpp>

namespace crisp
{
    class FourierTransformFilter;
    enum FourierTransformMode : bool {ACCURACY = true, SPEED = false};

    template<bool AccuracyOrSpeed = FourierTransformMode::ACCURACY>
    class FourierTransform
    {
        public:

    };

    template<>
    class FourierTransform<FourierTransformMode::ACCURACY>
    {
        public:
            FourierTransform
            template<typename Image_t>
            void create_from(const Image_t&);

            std::complex<double> get_coefficient(long x, long y);
            double get_magnitude(long x, long y);
            double get_phase_angle(long x, long y);

            template<typename Image_t>
            Image_t to_image() const;

        private:
            std::vector<double> _magnitude,
                                _phase_angle;
    };

    template<>
    class FourierTransform<FourierTransformMode::SPEED>
    {
        public:

        private:
            std::vector<std::complex<float>> _coefficients;
    };
}