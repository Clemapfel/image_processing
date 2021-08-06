// 
// Copyright 2021 Clemens Cords
// Created on 05.08.21 by clem (mail@clemens-cords.com)
//

namespace crisp
{
    template<typename Dist_t>
    NoiseGenerator<Dist_t>::NoiseGenerator(size_t seed)
        : _engine(seed)
    {}

    template<typename RandomNumberDistribution_t>
    size_t NoiseGenerator<RandomNumberDistribution_t>::initialize_seed()
    {
        std::random_device device;
        return device();
    }

    template<typename RandomNumberDistribution_t>
    float NoiseGenerator<RandomNumberDistribution_t>::operator()()
    {
        float value = -2;
        while (value < -1 or value > 1)
            value = _distribution(_engine);

        return value;
    }

    inline UniformNoise::UniformNoise(size_t seed)
        : NoiseGenerator<std::uniform_real_distribution<float>>(seed)
    {
        _distribution = std::uniform_real_distribution<float>(-1, 1);
    }

    inline GaussianNoise::GaussianNoise(size_t seed)
        : NoiseGenerator<std::normal_distribution<float>>(seed)
    {
        _distribution = std::normal_distribution<float>(0, 0.5);
    }

    inline GammaNoise::GammaNoise( size_t seed)
        : NoiseGenerator<std::gamma_distribution<float>>(seed)
    {
        _distribution = std::gamma_distribution<float>(1, 0);
    }

    inline ExponentialNoise::ExponentialNoise(size_t seed)
        : NoiseGenerator<std::exponential_distribution<float>>(seed)
    {
        _distribution = std::exponential_distribution<float>(2);
    }

    float ExponentialNoise::operator()()
    {
        float value = -2;
        while (value < -1 or value > 1)
            value = _distribution(_engine) - 0.5;

        return value;
    }

    inline SaltAndPepperNoise::SaltAndPepperNoise(float salt_chance, float pepper_chance, size_t seed)
        : NoiseGenerator<SaltAndPepperDistribution>(seed)
    {
        _distribution = SaltAndPepperDistribution(salt_chance, pepper_chance);
    }
}