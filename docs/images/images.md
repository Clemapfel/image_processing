# [Images](/include/image.hpp)

Images are obviously at the heart of image-processing so it is paramount to understand the different types of images available in crisp and how to use them. Unlike images in print or on the internet, crisp images can be thought of as m*n matrices of values where m is the width, n the height of the image. Let's first take a look at the abstract [``Image``](/include/image.hpp) class as all Images are required to implement it's functionality.

Each image has a specified value type. You can access it via ``Image::value_t``. While technically arbitrary the value type should supports all common arithmetics operations in order for other classes such as spatial filters or frequency transforms to operate on the image. Note that when exporting images for rendering their values will have to be clamped to [0, 1] however while in memory the values are not bounded.

You can access an individual pixel-element (henceforth just "pixel") via ``Value_t & Image::operator()(long x, long y)``. The function returns a reference to the corresponding pixel that can be written to. Similarly you can access a pixel in a const context by calling ``Value_t Image::operator()(long x, long y) const``, this returns a copy of the value so the original image will not be modified. If x or y are out of bounds (meaning ``x < 0 or x > m or y < 0 or y > n``) both of these operators will throw an error. 

```cpp
using Image_t = Image< //...
auto image = Image_t();

// set all pixels to black
for (long x = 0; x < image.get_size().x(); ++x)
    for (long y = 0; y < image.get_size().y; ++y)
        image(x, y) = Image_t::Value_t(0);
    
const auto const_image = Image_t();

// calculate the mean of the images values
float mean = 0;
for (long x = 0; x < image.get_size().x(); ++x)
    for (long y = 0; y < image.get_size().y; ++y)
        mean += static_cast<float>(image(x, y));
    
mean /= image.get_size().x() * image.get_size().y;
```

Images offer a third way to access pixels: ``Value_t get_pixel_or_padding(long x, long y) const``. Unlike the operators above, this function does no bounds checking, instead if x or y are out of bounds it accesses what is called "padding". Crisp supports 5 padding types:

+ ``ZERO`` padding always returns 0 (or the image value-types equivalent such as rgb(0, 0, 0) for Colors) 

![](zero_padding.png)


+ ``ONE`` padding similarly always returns 1

![](one_padding.png)

+ ``STRETCH`` padding looks for the nearest pixel that is still in the image and returns it. This effectively stretches the outer 1-pixel border of the image infinitely (the "sample" was moved to the outermost border to illustrate this effect):

![](stretch_padding.png)

+ ``REPEAT`` padding simply copy and pastes the image over and over as such:

![](repeat_padding.png)

+ ``MIRROR`` padding mirrors the image across it's border pointing towards the in-bounds area:

![](mirror_padding.png)

Choosing the correct padding method can have a significant effect on processing results so be mindful of keeping track of what padding type each image has. By default ``STRETCH`` is choosen regardless of image value-type. You can modify the padding type via ``void set_padding_type(PaddingType)`` and access it via it's getter.

Images provide an ``iterator`` for easier access in loops, the iterator starts at the top-left pixel at position (0,0) and goes through the image in row-major order. With iterators we can re-write the above code with just one loop:

```cpp
using Image_t = Image< //...
auto image = Image_t();

// set all pixels to black
for (auto& pixel : image)
    pixel = Image_t::Value_t(0);
```
Internally the iterator calls ``operator()(long, long)`` so if you want to access padding you cannot do so with just the iterator.

Now that we know the basic functionality of all images, let's look at the three types natively offered by crisp

---

## [Binary Images](/include/binary_image.hpp)

Binary images are images where each pixel has the value-type ``bool``. When binding the image for rendering ``true`` is displayed as white rgb(1, 1, 1) and ``false`` is displayed as black (rgb(0, 0, 0). This is called the *polarity* of the image and while you cannot change the way bools are interpreted for rendering you can simply invert the image. Along inverting binary images have access to all common bit operators that act as if the image was a m*n array of bits:

```cpp
// invert each pixel
BinaryImage& invert();

// element-wise not
BinaryImage operator!() const;

// element-wise and
BinaryImage operator&(const BinaryImage&) const;

// element-wise or
BinaryImage operator|(const BinaryImage&) const;

// element-wise xor
BinaryImage operator^(const BinaryImage&) const;

// element-wise and-assignment
BinaryImage& operator&=(const BinaryImage&);

// element-wise or-assignment
BinaryImage& operator|=(const BinaryImage&);

// elementwise xor-assignment
BinaryImage& operator^=(const BinaryImage&);
```

When creating a binary image from a non-binary file on the disk you need to specify a *cutoff*. Values in the original file above the cutoff will be intrepreted as ``true``, values below as ``false``. This will aslo have to be done when converting one image type into binary so be sure to think about what cutoff makes the most sense for your application. In the below image you can see the original grayscale image on the left, the for the center a cutoff of 0.5 has been chosen, on the right a cutoff of 0.75 was chosen instead. You can see the difference.

![](binary_cutoff.png)

Because binary images have the same arithmetics as m*n bit matrices they are sometimes also called *bitplanes*. To illustrate this, let's look at an example of decomposing an image into it's bitplanes. We treat each pixel of an image as an 8-bit integer, then for each pixel we put the first bit of that integer into bitplane 0, the second into bitplane 1, etc.. What results are 8 binary images that can be used for compression or watermarking. crisp has a function for just this:

//TODO

Binary image have the useful properties of simplifying a lot of mathematical processes and because they're only a bool they are far faster to operate on that any other image type. We will look more at binary images in the morphology section

--- 

## [Grayscale Images](/include/grayscale_image.hpp)

Grayscale images are images where the pixel value-type is a 32-bit float. Colloqially some might call these images "black-and-white" images but to avoid confusing them with binary images we will call them grayscale or *intensity* images.

When transforming an rgb color image from the disk or memory into grayscale the following function is called:

```cpp
auto image = GrayscaleImage();
image.create_from_file("colorfile.png")

// equivalent to:
auto as_color = ColorImage();
as_color.create_from_file("colorfile.png")
auto image = GrayscaleImage();
image.create(as_color.get_size().x(), as_color.get_size().y);

for (long x = 0; x < as_color.get_size().x(); ++x)
    for (long y = 0; y < as_color.get_size().y; ++y)
        image(x, y) = as_color(x, y).as_hsv().s;
```
We convert the original color int HSV and simply take the value component. If you are not sure what exactly this means, please (re)read the [colors](/docs/colors/colors.md) documentation before continuining.

While we don't have much to say about grascale images right now, they are the centerpiece of crisp and are used extensively in pretty much all major algorithms or features. 

---

## [Full Color Images](/include/color_image.hpp)

We already mentioned them in the previous section but lastly we should take a look at what will usually be the final product of our image processing pipeline: full color images (also called "rgba images").

For full color images each pixel is not a number but a 4-element 32-bit float vector where the first element is red, second green, third blue and the fourth is the alpha component, sometimes called transparency. Operating on full-color pixels is operating on vectors. Note that these are not 1-by-4 matrices and all arithmetic operations on them are *element-wise*.

As we saw with [colors](/docs/color/color.md) there a many representations and the color-image class makes accessing these easy. Thinking of our image as a m*n matrix where each element is on 4-element vector we could also think of our image as a m*n*4 cube. Each "layer" of the cube is a matrix of all i-ths components of a pixels vector. Each of these layers is called a *plane* so for an rgba image there would be 4 planes: red, green, blue and alpha. Each of these planes is a m*n matrix of floats (remember that our colors vectors are 4 floats) we can visualize each plane as a grayscale image. Color-images offer functions for extracting these planes directly:

```cpp
// rgb
GrayScaleImage get_red_plane() const;
GrayScaleImage get_green_plane() const;
GrayScaleImage get_blue_plane() const;

// hsv / hsl
GrayScaleImage get_hue_plane() const;
GrayScaleImage get_saturiation_plane() const;
GrayScaleImage get_value_plane() const;
GrayScaleImage get_lightness_plane() const;

// alpha
GrayScaleImage get_alpha_plane() const;
```
Often image processing functions are only defined for matrices of scalars so by splitting an rgba image up into 4 planes, you can use all tools available to grayscale images on each plane and then reassemble them via:

```cpp
void set_red_plane(const GrayScaleImage&);
void set_green_plane(const GrayScaleImage&);
void set_blue_plane(const GrayScaleImage&);

void set_hue_plane(const GrayScaleImage&);
void set_saturation_plane(const GrayScaleImage&);
void set_value_plane(const GrayScaleImage&);
void set_lightness_plane(const GrayScaleImage&);

void set_alpha_plane(const GrayScaleImage&);
```

Note that interally colors are stored in rgba format, accessing a plane that is an element not part of rgba will occur some overhead for the necessary conversion.

# Exporting / Displaying an Image

We mentioned before that to render anything in crisp we need to create a ``Sprite`` of it (for more information on sprites, see [here](/docs/render/sprites.md)) as such:

```cpp
auto sprite = Sprite();
auto image = ColorImage( //...

// bind image to a sprite:
sprite.create_from(image);

// rescale the image to twice the size
sprite.scale(2);

// save the scaled image to a file
sprite.save_to_file("./test.png");

// display the sprite in a window
auto window = RenderWindow();
window.create(sprite.get_size().x(), sprite.get_size().y);

while (window.is_open())) 
{
    window.update();
    window.clear();
    window.draw(sprite);
    window.display();
}
```

Sprites works with binary, grayscale and full-color images. If you want to also render your custom format images you will have to convert them into one of the above.

# Custom Image Formats
Let's say not working with rgb images but rather multi-spectral images, so an image has the rgb value from a light sensor, plus another x-ray sensor plus another infrared sensor. And also for some reason 32-bit floating points are not enough for your application so each of these channels should be 64-bit. An image has to have the following properties:

+ it has to inherit from ``Image<Value_t>`` and implement all virtual functions
+ it's value type needs to support all artihmetic operations
+ it needs to be convertible to a native format for display and export

Let's implement our custom 5-channel image from scratch. First we need a value type, as rgba-images are a essentially a 4-channel vector, we should take our que from them and make the value type a 5-channel vector:

```cpp
using CustomVector = Eigen::Array<double, 1, 4>
class CustomImage : public Image<CustomVector>
{
    private:
        Eigen::Matrix<CustomVector, Eigen::Dynamic, Eigen::Dynamic> _data;
}
```
We use Eigens vector and matrix format here because it has all necessary arithmetics as well as some bonus operations. You can implement your own type for both storage and elements if you want to. 

Now we need to implement the inherited pure virtaul functions. ``crisp::Image`` has 3 of these:

```cpp
// get the images dimensions
public: Vector2ui get_size() const

// access a pixel within the bounds of the image (not the padding)
protected: Value_t  get_pixel(long x, long y) const = 0;
protected: Value_t& get_pixel(long x, long y) = 0;
```

For our 5-vector image class, these are straight forward:

```cpp
using CustomVector = Eigen::Array<double, 1, 4>
class CustomImage : public Image<CustomVector>
{
    private:
        Eigen::Matrix<CustomVector, Eigen::Dynamic, Eigen::Dynamic> _data;
        
    protected:
    CustomVector get_pixel(long x, long y) const override
    {
        return _data(x, y);
    }
    
    CustomVector& get_pixel(long x, long y) override
    {
        return _data(x, y);
    }
    
    public:
    Vector2ui get_size() override
    {
        return {_data.rows(), _data.cols()};
    }
}
```
And we're done! Spatial filters and intensity transforms will work with these out of the gate as matrix-vector arithmetics are well defined, signal processing will need some tinkering as all of crisps transforms are 2d to 2d, not 5d to 2d. We can again solve this by splitting our new custom image into 5 grayscale images for each plane, red, green, blue, x-ray and infrared, and operating on each plane seperately. Similarly to render them we could create and rgba image from the first 3 channels and two grayscale images for the x-ray and infrared channel and render each as their native crisp type respectively. 

---

Hopefully this overview of images was helpful. If you need more or more specific information, please consider checking the headers directly as each public function has a detail description of what it does and what each parameter means. 

Interested in how to transform your infrared plane into a color image? Consider checking the [pseudocolor](/docs/color/color.md) tutorial. 

