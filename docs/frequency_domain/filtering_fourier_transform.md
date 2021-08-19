# Fourier Transform

Filtering and processing in the *frequency domain* is a highly useful too as it facilitates certain techniques that would be very laborous in the spatial domain. 

To transform an image into it's fourier transform, simple construct the transform and use ``transform_from`` 

```cpp
auto image = GrayScaleImage();
image.load_from_file(// ...

auto fourier = FourierTransform<BALANCED>();
fourier.transform_from(image);
```
This only works with one-dimensional images, that is images that only have 1 arithmetic value per pixel. To transform higher dimensional consider using the [approriate fftw functions](https://www.fftw.org/fftw3_doc/Multi_002dDimensional-DFTs-of-Real-Data.html#Multi_002dDimensional-DFTs-of-Real-Data) as crisp already uses the 2d version internally. 

To visually inspect a transforms spectrum (the magnitude of it's complex terms) use ``to_image`` and bind the resulting image to a sprite for rendering

```cpp
auto spectrum_sprite = Sprite();
spectrum_sprite.create_from(spectrum.to_image<GrayScaleImage>());

// draw sprite here
```

The original image:

![](../opal_color.png)

An it's fourier spectrum:

![](fourier_spectrum.png)

Note converting the spectrum to an image automatically normalizes the resulting intensity values for ease of use.

For an image of size ``m*n``, it's fourier transform will be of size ``2*m*n``. This is to avoid [wraparound error](https://dsp.stackexchange.com/questions/3444/why-images-need-to-be-padded-before-filtering-in-frequency-domain). In crisp, the fourier spectrum is center, that is it's dc component is at ``m*n*0.5``. While not technically necessary this aids in visual analysis and filter design as we will see shortly. You can access the actual components of the transform via these functions:

```cpp
// access the complex coefficients
std::complex<Value_t> get_coefficient(long x, long y) const;

// access the coefficents magnitude
Value_t& get_component(long x, long y);
Value_t get_component(long x, long y) const;

// access the coefficients phase angle
Value_t get_phase_angle(long x, long y) const;
Value_t& get_phase_angle(long x, long y);
```

The dc component can be accessed either with ``get_component(spectrum.get_size().x() * 0.5, spectrum.get_size().y * 0.5)`` or simply ``get_dc_component``.

You may have noticed that when constructing the spectrum it takes a template argument. These dictate the internal behavior and have significant impact on the performance of the conversion both to and from an image:

+ ``SPEED``: this setting using the fastest heuristic for planning and uses 32-bit floats instead of 64-bit doubles for calculations. This is the fastest setting that crisps allows and the only one suitable for real-time rendering, however especially due to rounding error parasitic complex components and other artifacting is common
+ ``BALANCED``: this setting is recommend for non-realtime use. It uses a decently accurate heuristic and 64-bit doubles internally. It is about 2.5 times slower than the ``SPEED`` setting
+ ``ACCURACY``: this setting uses a heuristic that measures the optimal plan for the transform and results in the highest quality transform possible. This comes at a hefty performance cost as it is about 10 times slower than the speed setting. It is only recommended in applications where numerical precision is paramount

# Filtering in the Frequency Domain

Filtering in the frequency domain means multiplying the fourier spectrum with a 2d-function and then transforming the modified spectrum back into an image. For this purpose crisp offers the ``FrequencyDomainFilter`` class. It works very similarly to ``SpatialFilter`` in that it is multi-use because you can easily "plug-in" certain functions to change the filter shape. To construct a filter we need to specify the size of the spectrum that we want to filter along with it:

```cpp
auto spectrum = //...
auto filter = FrequencyDomainFilter(spectrum.get_size().x(), spectrum.get_size().y);
```

Note that only allocates all the memory when it is *applied*, not when it is constructed. 

Next we need to specify a filter shape. By default the shape is ``identiy``, which is simply all 1s meaning multiplying with a spectrum will result in that spectrum. 

To set the filter shape we use ``set_function``. This function takes a lambda of type ``double(long, long)`` where the arguments are the x and y coordinate of the spectrum component and the returned double is the filters value at that point.

To aid in filter design crisps allows us to render a filter by using:

```cpp
auto filter = //...
auto sprite = Sprite();
sprite.create_from(filter);

// render sprite
```

This is not only useful for the user but also because it makes explaining the different filter shapes much easier. Imagine a picture of an image like a view of a distribution function from the top, where the center is, again, over the dc component of the spectrum. Crisp offers 4 types of filters:

+ *low-pass* filters attenuate (reject) high frequencies towards the outer edges of the spectrum and pass low frequencies towards the center

![](ideal_lowpass.png)
+ *high-pass* attenuate low frequencies and pass high frequencies
  
![](ideal_highpass.png)
+ *band-pass* attenuate frequencies outside a specified band and pass frequencies inside it
  
![](ideal_bandpass.png)
+ *band-reject* attenuate frequencies inside the band and pass frequencies outside of it

![](ideal_bandreject.png)

After specifying the type you can apply the filter to a spectrum like we did in the images above like so:
```cpp
auto filter = FrequencyDomainFilter(transform.get_size().x(), transform.get_size().y);

// set the type:
filter.set_function(filter.ideal_bandpass(transform.get_size().x() * 0.2, transform.get_size().x() * 0.3));
    
// apply to transform
filter.apply_to(transform);

// bind for rendering
auto filter_sprite = Sprite();
filter_sprite.create_from(transform);
```

Each filter type has 2 default arguments called ``pass_factor`` and ``reject_factor``. These modify the maximum and minimum value of the filter in a way where if we specify ``pass_factor = 2`` then a low-pass filter will boost low-frequencies by a factor of 2, and a high-pass filter will boost *high* frequencies by a factor of 2. Similary if we specify ``reject_factor = 0.5`` then a band-pass filter will half frequencies outside of the band and vice-versa with a band-reject. 

![](bandpass_15_05.png)

```cpp
filter.set_function(filter.ideal_bandpass(transform.get_size().x() * 0.2, transform.get_size().x() * 0.3, 1.5, 0.5));
```

Here we boost frequencies inside the band by 50% and instead of completely rejecting frequencies outside of the band we multiply them by 0.5. 

Other than filter type (low-pass, high-pass, etc.) you can also specify the filter *shape*. The reason for this will become evident in a moment. Crisp offers 3 filter shapes natively:

+ *ideal* filters have a frequency response that immediately transitions from one region to the other. The above examples are all ideal filters, you can think of them as solid geometrical shapes (or ring / vertical pillars in 3d space).
![](ideal_lowpass_pure.png)
An ideal low-pass filter
  
+ *gaussian* filters are best for most purposes, their frequency response transitions from passing to rejecting region according to the gaussian distribution:
![](gaussian_lowpass.png) 
A gaussian low-pass filter
  
+ *butterworth* filters take an extra argument other than the frequency cutoff called it's *order*. Butterworth filters are designed such that the higher the order the closer they become to ideal filters. This makes them very "customizable" and their generation tends to be a lot faster than gaussian filters because they don't have to deal with exponential and squareroot operations:
![](butterworth_1_lowpass.png)
A butterworth low-pass filter of order 1
  
![](butterworth_2_lowpass.png)
A butterworth low-pass filter of order 2

![](butterworth_10_lowpass.png)
A butterworth low-pass filter of order 10

# Combining Filters

To offer absolute flexibility in filter design crisp offers all arithmetic operators for two filters:

  
```cpp
FrequencyDomainFilter operator+(const FrequencyDomainFilter&) const;
FrequencyDomainFilter operator-(const FrequencyDomainFilter&) const;
FrequencyDomainFilter operator*(const FrequencyDomainFilter&) const;
FrequencyDomainFilter operator/(const FrequencyDomainFilter&) const;

FrequencyDomainFilter& operator+=(const FrequencyDomainFilter&);
FrequencyDomainFilter& operator-=(const FrequencyDomainFilter&);
FrequencyDomainFilter& operator*=(const FrequencyDomainFilter&);
FrequencyDomainFilter& operator/=(const FrequencyDomainFilter&);
```

For example if we wanted to get from our gaussian low-pass to the inverse, a guassian highpass we could do:

```cpp
auto gaussian_lp = FrequencyDomainFilter(//...
gaussian_lp.set_function(gaussian_lp.gaussian_lowpass(//...

// filter is identity by default
auto gaussian_hp = FrequencyDomainFilter(gaussian_lp.get_size().x(), gaussian_lp.get_size().y) - gaussian_lp;
``` 

If you still can't get your perfect filter working you can also just directly access each filter intensity directly by exposing their values with:

```cpp
const std::vector<double>& FrequencyDomainFilter::get_values() const;
```
These values are in row-major order meaning for a filter of width m*n the first index corresponds to the element at the top left, the m-ths element is at the top right, the center (dc) element is at index 0.5*n + 0.5*n*m, etc.. 

# Seeing it in action

Lastly let's actually filter an image:

```cpp
// load an image
auto image = GrayScaleImage();
image.create_from_file("../opal_color.png");

// calculate the transform
auto transform = FourierTransform<SPEED>();
transform.transform_from(image);

// set up a filter and filter the spectrum
auto filter = FrequencyDomainFilter(image.get_size().x(), image.get_size().y);
filter.set_function(filter.ideal_lowpass(filter.get_size().x() * 0.2));
filter.apply_to(transform);

// transform back into an image
image = transform.transform_to<GrayScaleImage>();

// bind for drawing and display in a window
auto sprite = Sprite();
sprite.create_from(image);

RenderWindow render_window(sprite.get_size().x(), sprite.get_size().y);

while (window.is_open())
{
    window.update();
    window.clear();
    window.draw(sprite);
    window.display();
}
```

![](opal_filtered.png)

As expected we observe smoothing because high frequencies which represent fine detail are rejected. We furthermore observe "ringing" typical for ideal filters. By instead using a gaussian lowpass filter we can achieve a much more useful result:

![](opal_filtered_no_ringing.png)






