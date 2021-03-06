// 
// Copyright 2021 Clemens Cords
// Created on 05.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <random>
#include <detail/salt_and_pepper_distribution.hpp>

namespace crisp
{
    // uncorrelated noise generator
    // takes a probablitity density functions and returns a pseudo-random number following that PDF
    template<typename RandomNumberDistribution_t>
    class NoiseGenerator
    {
        public:
            // @brief Constructor the noise generator and initialize it with a seed
            // @param seed: (optional) the seed, if none is specified, std::random_device::operator() is called instead
            NoiseGenerator(size_t seed);

            // @brief specificy the minimal and maximal response of the noise generator
            // @param min: the lower bound
            // @param max: the upper bound
            // @notes by default the bounds are [-1, 1]
            void set_interval(float min, float max);

            // @brief get a random value
            // @returns a float in range [min_value, max_value]
            // @notes the PDFs are configured such that their mean is abs(max - min) / 2
            //        their stddev (sigma) such that: mean - 3*sigma = min and mean + 3*sigma = max.
            //        though unlikely this means that values outisde of [min, max] can accur
            virtual float operator()();

        protected:
            float _min = -1, _max = 1;
            static size_t initialize_seed();
            
            std::mt19937 _engine;
            RandomNumberDistribution_t _distribution;
    };

    // generates uniformly distributed noise
    struct UniformNoise : public NoiseGenerator<std::uniform_real_distribution<float>>
    {
        // @overload, c.f. NoiseGenerator<...>::NoiseGenerator(size_t)
        UniformNoise(size_t = initialize_seed());
    };

    struct GaussianNoise : public NoiseGenerator<std::normal_distribution<float>>
    {
        // @overload, c.f. NoiseGenerator<...>::NoiseGenerator(size_t)
        GaussianNoise(size_t = initialize_seed());

        // @brief construct guassian noise
        // @param mean: the mean value (center of the distribution)
        // @param sigma: the standard deviation
        // @param seed: the seed to initialize the rng with
        GaussianNoise(float mean, float sigma, size_t seed = initialize_seed());
    };

    struct GammaNoise : public NoiseGenerator<std::gamma_distribution<float>>
    {
        // @overload, c.f. NoiseGenerator<...>::NoiseGenerator(size_t)
        GammaNoise(size_t = initialize_seed());

        // @brief construct gamma noise
        // @param alpha: the shape parameter of the distribution
        // @param beta: the scale parameter of the distribution
        // @param seed: the seed to initialize the rng with
        GammaNoise(float alpha, float beta, size_t seed = initialize_seed());

        // @overload, c.f. NoiseGenerator<...>::operator()()
        float operator()() override;
    };

    // salt-and-pepper noise, randomly distributed -n or n impulses
    // -n is called "pepper" because it will usually set and affected pixel to 0, +n will set it to white "salt"
    class SaltAndPepperDistribution;
    struct SaltAndPepperNoise : public NoiseGenerator<SaltAndPepperDistribution>
    {
        // @brief initialize the salt-and-pepper-noise generator
        // @param salt_chance: the likelyhood of a positive impulse being returned, may be 0
        // @param pepper_chance: the likelyhood of a negative impulse being returned, may be 0
        // @param : the seed
        SaltAndPepperNoise(float salt_chance, float pepper_chance, size_t = initialize_seed());

        // @overload, c.f. NoiseGenerator<...>::operator()()
        // @returns -min if pepper, +max if salt
        float operator()() override;
    };

    // ##############################################################################################

    // TODO: noise that is a function of the pixels position
    /*
    template<typename Function_t>
    class SpatiallyCorrelatedNoiseGenerator
    {
        public:
            SpatiallyCorrelatedNoiseGenerator(size_t offset);

            float operator()(long x, long y);

            template<typename Lambda_t>
            void set_function(Lambda_t&&);

            static auto&& sine(float amplitude_min, float amplitude_max, float frequency, float phase, float angle_dg);
            static auto&& triangle(float amplitude_min, float amplitude_max, float frequency, float phase, float angle_dg);
            static auto&& square(float amplitude_min, float amplitude_max, float frequency, float phase, float angle_dg);

        private:
            Function_t _function;
    };
     */


}

#include ".src/noise_generator.inl"