# CRISP: real-time, interactive signal processing

A portable, lightweight, modern C++20 Library implementing most common features of digital image processing as mentioned in "Digital Image Processing" (R.C.Gonzales, R.E.Woods, 2017). Intended for educational purposes or personal use only.

Supports video-game-like interactivity, real-time rendering, on- and off-gpu processing on Mac, Windows and Linux. 

### Dependencies
- **fftw** (3.39): for fourier-transform computation
- **Eigen** (3.3.3): for matrix operations
- **OpenGL** (3.3+ core): for gpu-side processing
- **SFML** (2.5.1): for interactivity and interfacing with OpenGL 
- **Lua** (5.4): for code generation
- **sol** (3.0): for interfacing with lua
- 
### Documentation
Until this section is fully formed please consider checking the headers directly. They are intentionally formatted to be very easily referenced back to for questions.

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

### [render_window.hpp](include/render_window.hpp)
Creates a window that can be used for rendering in real-time at an arbitrary refresh rate

```cpp
RenderWindow window;
window.create(1920, 1080, true, 60); // create a 1920x1080 fullscreen-only window with a max refresh rate of 60fps
window.set_background_color(Color(255, 255, 0, 1)); // set background to magenta for better contrast

crisp::Image image = //...
crisp::Shader shader = /...
crisp::Transform transform = /...

while (window.is_open())  // closes when "x" is pressed
{
  auto time = window.update();  // update things like the keyboard state
  
  window.draw(&image, &shader, transform);
  
  window.clear();
  window.display();
}
```

### Authors
Clemens Cords (www.clemens-cords.com)
