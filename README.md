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
Images are at the heart of crisp and most of operations and math is performed on them as objects. In crisp there are two way of representing a picture: An **image** a rectangular array of values that lives in the ram and operations that interact with them happen on the CPU. A **texture** in comparison lives on the graphics cards ram (gram) and most of the operations concerning textures happen on the GPU which may drastically increase performance at the cost of ease of use and flexibility. For this section we will only concern ourself with **images** which are best to think of a simply m\*n matrices.

#### [binary_image.hpp](include(binary_image.hpp)
Binary images are matrices of ``bool``s, each of their elements can only be zero or one. In real-life you can think of them as black-and-white images, think a QR code or barcode or maybe a really bad scan your teacher handed out because the printer was out of ink. 
Binary images have the advantage of being fairly light memory-wise, a common use-case for binary images is morphological segmentation or bitplane seperation for the purpose of compression.

#### [grayscale_image.hpp](include/grayscale_image.hpp)
Grayscale images are matrices where each element is a single float in the range [0, 1]. While at first seeming limited for modern applications grayscale images are at the heart of any image processing pipeline because any other color system can be segmented into them. If you want to process a full color rgba image you can just split each component into it's plane and process those as grayscale images. 

#### [rgba_image.hpp](include/rgba_image.hpp)
Which is essentially what rgba images are, just 4 greyscale images stacked on top of each other with plane 1 respresenting red, 2 green, 3 blue and 4 the transparency value. These are as close to actual image files as you will get, you can however convert any picture representation mentioned so far into any commonly available file format

### Image Examples
TODO


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
