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
    // fourier transform of an image

    // optimization mode of the transform
    enum FourierTransformMode : int
    {
        SPEED = 0,      // prioritizes speed, high likelihood of rounding errors but suitable for real-time applications
        BALANCED = 1,   // recommended, heuristic DFT fit suboptimal, minimal rounding errors
        ACCURACY = 2    // unsuitable for real-time use, heuristic DFT optimal, minimal rounding errors

        // slowdown: speed: 1x, balanced: ~2.5x, accuracy: ~10x
    };

    // class storing the *centered* fourier transform in polar form, that is each coefficients magnitude and phase angle
    template<FourierTransformMode Mode = BALANCED>
    class FourierTransform
    {
        friend class FrequencyDomainFilter;
        using Value_t = typename std::conditional<Mode == SPEED, float, double>::type;

        public:
            FourierTransform() = default;

            // @brief creates fourier transform from an image
            // @param : image to be transformed
            //
            // @note given an image of size m*n the fourier transform will have m*n*2 components. This is to eliminate wrap-around error.
            //       Be aware that the type of padding used for the image can have significant impact on the transforms output
            template<typename Image_t>
            void transform_from(const Image_t&);

            // @brief transform back into an image
            // @returns image after transform
            //
            // @note this does not deallocate the transform
            template<typename Image_t>
            Image_t transform_to() const;

            // @brief visualizes the transforms spectrum as an image
            // @returns log(1+x)-scaled grayscale image of size m*n*2 where m, n size of the original transformed image
            template<typename Image_t>
            Image_t spectrum_to_image() const;

            // @brief visualize the transforms phase angles as an image
            // @returns linearly scaled grayscale image of size m*n*2 where m, n size of the original transformed image
            template<typename Image_t>
            Image_t phase_angle_to_image() const;

            // @brief get the complex coefficient at the specified position, (0,0) being the top-left origin
            // @param x: the row index, range [0, 2*m]
            // @param y: the column index, range [0, 2*n]
            // @returns std::complex
            //
            // @note Interally the coefficients are stored in polar form meaning that a performance overhead will be present if
            //       accessing them via this function instead of get_component, get_phase_angle
            std::complex<Value_t> get_coefficient(long x, long y) const;

            // @brief const-access the component of the spectrum at the specified position, (0,0) being the top-left origin
            // @param x: the row index, range [0, 2*m]
            // @param y: the column index, range [0, 2*n]
            // @returns coefficients magnitude, float if in SPEED mode, double otherwise
            Value_t get_component(long x, long y) const;

            // @brief const-acces the phase angle at the specified position, (0,0) being the top-left origin
            // @param x: the row index, range [0, 2*m]
            // @param y: the column index, range [0, 2*n]
            // @returns angle in radians, float if in SPEED mode, double otherwise
            Value_t get_phase_angle(long x, long y) const;

            // @overload non-const version of get_component(long, long)
            Value_t& get_component(long x, long y);

            // @overload non-const version of get_phase_angle(long, long)
            Value_t& get_phase_angle(long x, long y);

            // @brief access the dc-component (the value at the spectrums center)
            // @returns float if in SPEED mode, double otherwise
            //
            // @note equivalent to calling get_component(p/2, q/2) where p,q size of the transform
            Value_t get_dc_component() const;

            // @brief get the size of the transforms
            // @returns sf::Vector where .x is the width, .y the size of the transform
            sf::Vector2<long> get_size() const;

        protected:
            std::vector<Value_t>& get_spectrum();
            std::vector<Value_t>& get_phase_angle();

        private:
            size_t to_index(long x, long y) const;

            sf::Vector2<long> _size;
            std::vector<Value_t> _spectrum,
                                 _phase_angle;

            Value_t _min_spectrum = 0, _max_spectrum = 1; // already log(1+x) scaled
    };
}

#include ".src/fourier_transform.inl"