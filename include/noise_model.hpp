// 
// Copyright 2021 Clemens Cords
// Created on 05.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <random>
#include <detail/salt_and_pepper_distribution.hpp>

namespace crisp
{
    template<typename RandomNumberDistribution_t>
    class NoiseGenerator
    {
        public:
            NoiseGenerator(size_t seed);
            virtual float operator()();

        protected:
            static size_t initialize_seed();
            
            std::mt19937 _engine;
            RandomNumberDistribution_t _distribution;
    };
    
    struct UniformNoise : public NoiseGenerator<std::uniform_real_distribution<float>>
    {
        UniformNoise(size_t seed = initialize_seed());
    };

    struct GaussianNoise : public NoiseGenerator<std::normal_distribution<float>>
    {
        GaussianNoise(size_t seed = initialize_seed());
    };

    struct GammaNoise : public NoiseGenerator<std::gamma_distribution<float>>
    {
        GammaNoise(size_t seed = initialize_seed());
    };

    struct ExponentialNoise : public NoiseGenerator<std::exponential_distribution<float>>
    {
        ExponentialNoise(size_t seed = initialize_seed());
        float operator()() override;
    };

    class SaltAndPepperDistribution;
    struct SaltAndPepperNoise : public NoiseGenerator<SaltAndPepperDistribution>
    {
        SaltAndPepperNoise(float salt_chance, float pepper_chance, size_t seed = initialize_seed());
    };
}

#include ".src/noise_generator.inl"