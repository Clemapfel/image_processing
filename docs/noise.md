# [Noise](include/noise_model.hpp)

Noise is a common contributor to image degradation should be addressed as it can significantly impact further processing down the line. To test the robustness of an image pipeline when it comes to different types of noise, crisp offers a suite of noise generators that can be used to simulate noise to be added to a noise-free original image so that then results can be compared to the known optimal solution.

There are two types of noise in crips: spacially invariate and periodic noise. Let's look at the former first:

"Spacially invariate" means that the function that generates the noise is not a function of the position of the image. Another way to look at this is that domain of the noise function is an infinite plane of noise and all the function does is randomly sample that plane everytime it is called. Crisp has 6 different build-in noise-types but it's easy to customize your own should you want to.

## Noise Generator
All noise models generate their values through a ``NoiseGenerator``. It takes the noises probability density function (PDF) as a template argument and has 3 functions:

``NoiseGenerator(size_t seed)``: Constructs the generator and seeds it, if no seed is specified it is randomly generated via a ``std::random_device`` 

``set_interval(float min, float max)``: This functions determines the strength of the noise. By default the generator returns values in [0, 1], however it is often useful to have a smaller range or a range containing negative values. Calling ``set_interval(-0.3, +0.5)`` will modify the noise generators results such that they are in [-0.3, +0.5] with mean +0.1.

Note that while internally an images pixels can have arbitrary intensity values once you bind them for rendering they are clamped to [0, 1]. For advice on how to deal with this see the chapter on intensity transforms.

``float operator()()``: Activates the generator and returns a float in ``[min, max]``. 

## Types of Invariate Noise

![](noise.png)
*Figure 1:* Different types of noise and their intensity histograms (mean in red). From left to right: No Noise, Uniform, Gaussian, Gamma (Erlang), Exponential, Salt-and-Pepper *(figure generated with: [noise.cpp](docs/noise.cpp))*

Crisp supports 6 types of noise by default. With their default arguments (that is the default values of the parameters of their PDF) all noise types except salt-and-pepper have a mean of 0.5 and a standard deviation sigma such that 3 * sigma = 0.5. This assures that most of the generated values fall into the range [0, 1] and that the user can always expect the mean for an arbitrary range [min, max] to be at (max - min) * 0.5. While not strictly enforced it is recommend to stick to this form when implementing custom noise models via inheritance from ``NoiseGenerator``.

### Uniform Noise

Uniform noise adheres to the [real uniform distribution](https://en.wikipedia.org/wiki/Continuous_uniform_distribution). It only has a default constructor, if you want ranges outside of [0, 1], please use ``UniformNoise::set_interval``.

### Gaussian Noise 

Gaussian noise follows the [normal distribution](https://en.wikipedia.org/wiki/Normal_distribution) with mean 0.5 and standard deviation of (0.5 / 3) as mentioned above. To specify a custom mean and standard deviation, use ``GaussianNoise(float mean, float sigma, size_t seed)``

### Gamma Noise

Gamma noise follows the [erlang distribution](https://en.wikipedia.org/wiki/Normal_distribution) with discrete shape parameter alpha = 2 and scale parameter beta such that the mean falls on 0.5. From functional analysis we know that the erlang distribution approaches the [exponential distribution](https://en.wikipedia.org/wiki/Exponential_distribution) around shape parameter 1 so we have an entire family of distributions covered. Again the parameters of the distribution can be manually specified via ``GammaNoise(float alpha, float beta, size_t seed)``.

### Salt and Pepper noise
More formally called data-drop-out (pepper) or spike (salt) noise salt-and-pepper noise models a situation where a visual sensor receives values of exactly 0 or exactly 1 for a certain pixel on accident. You can manually specify the chance for salt and/or pepper respectively via ``SaltAndPepperNoise(float salt_chance, float pepper_chance, size_t seed)``, with salt_chance = 0.1 and pepper_chance = 0.3 the noise will behave in a way where 10% of the time a spike of 1 is returned, 30% of the time a drop-out of -1 is returned and 60% of the time the generator returns 0;

## Variate Noise

TODO
