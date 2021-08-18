# CRISP: real-time, interactive signal processing

A portable, lightweight, modern C++20 Library implementing most common features of digital image processing as mentioned in "Digital Image Processing" (R.C.Gonzales, R.E.Woods, 2017). Intended for educational purposes or personal use only.

Supports video-game-like interactivity, real-time rendering, on- and off-gpu processing on Mac, Windows and Linux. 

### Dependencies
- **fftw** (3.39): for fourier-transform computation
- **ranges-v3**: for ranges
- **Eigen** (3.3.3): for cpu-side matrix operations
- **OpenGL** (3.3+ core): for gpu-side processing
- **SFML** (2.5.1): for interactivity and interfacing with OpenGL 
- **Lua** (5.4): for code generation
- **sol** (3.0): for interfacing with lua

### Documentation
Until this section is fully formed please consider checking the headers directly. They are intentionally formatted to be very easily referenced back to for questions.

## [Images](include/image.hpp)
Images are at the heart of crisp and operations in a image processing application are performed on them as objects. In crisp there are two way of representing a picture: An **image**, a rectangular array of values, lives on the ram and operations that interact with them happen on the CPU. A **texture**, in comparison, lives entirely on the graphics cards ram (gram) and most of the operations concerning textures happen on the GPU insted. This can drastically increase performance at the cost of ease of use and flexibility. For this section we will only concern ourself with **images** which are best to think of a simply m\*n matrices of arbitrary values.

#### [binary_image.hpp](include/binary_image.hpp)
Binary images are matrices of ``bool``s, each of their elements can only be zero or one. In real-life you can think of them as black-and-white images, think a QR code, a barcode or maybe a really bad scan your teacher had to fax to themself because the printer was out of ink.
Binary images have the advantage of being fairly light memory-wise and allow for bit-arithmetics making them overall the best-suited cpu-side image to use in real-time processing. A common use-case for binary are as outputs of morphological segmentation, bitplane seperation for the purpose of compression or as elements that represent images as background- xor foreground-pixels.

#### [grayscale_image.hpp](include/grayscale_image.hpp)
Grayscale images are matrices where each element is a single float in the range [0, 1]. While at first seeming limited for modern applications, grayscale images are at the heart of any image processing pipeline because even if an image is using a completely different color system, it can usually still be represented by one or more grayscale images. For example the problem processing a full-color rgba image can be segmented into processing 4 grayscale images representing each color component respectively.

#### [rgba_image.hpp](include/rgba_image.hpp)
Which is essentially what rgba images are in crisp too, just 4 greyscale images stacked on top of each other with the first plane respresenting red, 2nd green, 3rd blue and 4th the transparency value. These are as close to actual image files as you will get, however if you actually want to save them to memory you will have to do so through a fourth image class:

### Image Examples
There's no point in processing images if we can't see them so let's do that right now:

```cpp
int main() {
  // create a render window for display
  RenderWindow window;
  window.create(1280, 740);
  
  // load an image
  auto image = GreyScaleImage();
  image.create_from_file("./test_file.png");
  
  // to display an image we need to bind it to renderable sprite
  auto sprite = Sprite();
  sprite.create_from(image);
  sprite.align_center_with(Vector2f(window.get_resolution().at(0) * 0.5f, window.get_resolution().at(1) * 0.5f));
  
while (window.is_open())
{
  auto time = window.update();
  
  window.clear();
  window.draw(sprite);
  window.display();
}
```
You may have noticed that we're loading from a .png which is obviously full-color but the image is only a greyscale image. When loading from a file the image class automatically converts the image into the correct color domain via a pre-defined process so you don't have to worry about file-compatibility issues. Even if those were a case transforming one image type into a another is as simple as casting them (though you might want to be careful as this may quadruple the space taken in ram):
```cpp
auto color_img = ColorImage(...
auto binary_img = BinaryImage(...

auto grey_img = GrayScaleImage(binary_img);
``` 

#### [color.hpp](include/color.hpp)
Support for common color representations RGB, HSV, HSL and Grayscale including conversion and mixing between any of them.

```cpp 
auto as_rgb = RGB{123, 140, 129, 255}; // elements are uint8_t
Color color = rgb;
color = color.invert();

HSV in_hsv = convert_to<HSV>(color);
HSL in_hsl = convert_to<HSL>(as_hsv);

assert(convert_to<HSL>(color) == color.as_hsl());

float in_gray = convert_to<GrayScale>(as_hsl);
auto mixed = mix<Color>(color, Color(as_gray), 0.5);
```

#### [render_window.hpp](include/render_window.hpp)
Creates a window that can be used for rendering in real-time at an arbitrary refresh rate

```cpp
crisp::RenderWindow window;
window.create(1920, 1080, true, 60); // create a 1920x1080 fullscreen-only window with a max refresh rate of 60fps
window.set_background_color(Color(255, 255, 0, 1)); // set background to magenta for better contrast

crisp::Image image = //...
crisp::Shader shader = /...
crisp::Transform transform = /...

while (window.is_open())  // closes when "x" is pressed
{
  auto time = window.update();  // update things like the keyboard state
  
  window.clear();
  window.draw(image, shader, transform);
  window.display(); // vsyncs to reach at most target 60fps
}
```

### Authors
Clemens Cords (www.clemens-cords.com)
