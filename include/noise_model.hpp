// 
// Copyright 2021 Clemens Cords
// Created on 05.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <random>

namespace crisp
{
    template<typename RandomNumberDistribution_t>
    class NoiseGenerator
    {
        public:
            float operator()();

        private:
            std::mt19937_64 _engine;
            RandomNumberDistribution_t _distribution;
    };

    // gaussian
    template<>
    struct NoiseGenerator<std::normal_distribution<float>>
    {
        NoiseGenerator(float mean, float sigma);
    };
    using GaussianNoiseGenerator = NoiseGenerator<std::normal_distribution<float>>;

    // erlang gamma
    template<>
    struct NoiseGenerator<std::gamma_distribution<float>>
    {
        NoiseGenerator(float alpha, float beta);
    };
    using GammaNoiseGenerator = NoiseGenerator<std::gamma_distribution<float>>;

    // exponential
    template<>
    struct NoiseGenerator<std::exponential_distribution<float>>
    {
        NoiseGenerator(float lambda);
    };
    using ExponentialNoiseGenerator = NoiseGenerator<std::exponential_distribution<float>>;

    // uniform
    template<>
    struct NoiseGenerator<std::uniform_real_distribution<float>>
    {
        NoiseGenerator();
    };
    using UniformNoiseGenerator = NoiseGenerator<std::uniform_real_distribution<float>>;

    // salt and pepper
    class SaltAndPepperDistribution;
    template<>
    struct NoiseGenerator<SaltAndPepperDistribution>
    {
        NoiseGenerator(float salt_chance, float pepper_chance);
    };
    using SaltAndPepperNoiseGenerator = NoiseGenerator<SaltAndPepperDistribution>;
}

#include ".src/noise_generator.inl"