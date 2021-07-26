# CRISP: real-time, interactive signal processing library

A portable, lightweight modern C++ Library implementing most common features of digital image processing as described in "Digital Image Processing" (R.C.Gonzales, R.E.Woods, 2017). Intended for educational purposes or personal use only.

Supports on- and off-gpu processing of 8-bit color images and modern C++ features such as ranges for image processing pipelines.

### Dependencies
- **fftw** (3.39): for fourier-transform computation
- **Eigen** (3.3.3): for matrix operations
- **SFML** (2.5.1): for interactivity and interfacing with OpenGL 
- **Lua** (5.4): for code generation
- **sol2** (3.0): for interfacing with lua

### Documentation
Until this section is fully formed please consider checking the headers directly. They are intentionally formatted to be very easily referenced back to for questions.

#### [color.hpp](include/color.hpp)
Support for common color representations RGB, HSV, HSL and Grayscale including conversion and mixing between any of these.

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

### Authors
Clemens Cords (www.clemens-cords.com)
