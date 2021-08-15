# Colors & Colors Transformations

While it's nice to work with grayscale images, processing and working with colors is an integral part of image processing.

Crisp offers 4 different color representations: RGB, HSV, HSL and GrayScale. All of these include an alpha component. **All componens of all representations are 32-bit floats in range [0, 1]**. 

Let's go through these one-by-one:

## RGB
```
struct RGB
{
    float r = 0, // red
          g = 0, // green
          b = 0, // blue
          a = 1; // alpha
};
```
RGB is the most commonly used color representation and simulates how colored light behaves when mixed. It is how crisp handles colors internally. RGB or more precisely RGBA colors have 4 components: red, green, blue and alpha. Alpha is the "transparency" component which becomes important in rendering. 

## HSV & HSL
```
struct HSV
{
    float h = 0, // hue
          s = 0, // saturation
          v = 0, // value
          a = 1; // alpha
};

struct HSL
{
    float h = 0, // hue
          s = 0, // saturation
          l = 0, // lightness
          a = 1; // alpha
};
```
HSV stands for "hue, saturation, value" and HSL for "hue, saturation, lightness". Both are an attempt of representing colors more like humans would describe them. You wouldn't state the color of your car in terms of it's rgb fractions, you would call it "light blue" or "a really dark red". Let's first focus on hue:

Hue is the actual color, it is traditionally a value from 0 to 360 but is normalized to 0 to 1 in crisp for consistency. 0 is the minimum visible light frequency, usually called red and 360 is the maximum visible light frequency, usually violet or pink. When moving from 0 to 1 in hue, you can imagine it as moving from left to right on the spectrum.

![](https://www.orcagrowfilm.com/v/vspfiles/templates/taste/images/wavelength-nm.jpg)

Saturation can be most easily conceptualizes as how much pigment is in a base color, let's say you start with a purel white color. As you mix in red pigment it will go from faintly red to light red and as you reach 100% saturation the color will be a pure red. You can't go any darker than pure red by putting in more red and this is what saturation does, at 0 it is no pigment, at 1 it is only pigment.

HSV and HSL have a third component and conceptualizing these is not as easy. For an in-depth discussion of their relation, consider reading their [excellent wikipedia article](https://en.wikipedia.org/wiki/HSL_and_HSV). I like to conceptualize the relationship between lightness and value as such:

![](https://www.researchgate.net/profile/Abdulkareem-Althwaini/publication/323586342/figure/fig1/AS:601267550318606@1520364763395/The-HSV-colour-model-mapped-to-a-cylinder-and-The-HSL-colour-model-mapped-to-a-cylinder.png)

In HSV a value of 0 means your mix of pigments is 100% black. Regardless of the other colors or white color, the resulting pigment is only black. As you increaes HSV's value component, you remove black from the mixture. At 0.5 you still have 50% black in it, at 0 you have no black pigments, the color is "pure" and visually can be described as more intense.

In HSL a lightness of 0 is equivalent to an HSV value of 0: pure black. In fact HSV(x, x, 0) and HSL(x, x, 0) are visually indistinguishable. The difference becomes clear when you increase lightness. From HSL.l = 0 to HSL.l = 0.5 you can again visualize it as removing black pigment from a mixture, except now at only 0.5 (instead of HSV.v = 1 before) you have no black pigments left. Thus HSV(x, x, 1) = HSL(x, x, 0.5). Once you go past HSL.l = 0.5 you're not adding back in white such and much like HSL(x, x, 0) is always pure black, HSL(x, x, 1) is always pure white.

I get that these colors systems are confusing but as you work with them you will find it useful to convert one into the other. For example to convert and rgb color into black and white the easiest way is to convert it to HSV, the HSV.s component to 0 and then convert it back into rgb. 

# Conversion

Crisp offers the following template:
```
template<typename T1, typename T2>
    T2 convert_to(T1 from);
``` 
Where T1 are any of {RGB, HSV, HSL, GrayScale, crisp::Color} and T1 are also any of {RGB, HSV, HSL, GrayScale, crisp::Color}. So for example:
```cpp
RGB rgb_col = //...
        
HSV hsv_col = convert_to<HSV>(rgb_col);
HSL hsl_col = convert_to<HSL>(hsv_col);
// etc.
```
You can convert any color representation into any other representation via ``convert_to<>``. To add your own color system you can simply implement crisps convert_to with partial or full template specialization for each pair of possible colors. 

# The Color class

Acting solely in the vector-like types above can be bothersome and internally crisps uses a color "dummy" class, called ``crisp::Color``. ``Color`` can interact with any other representation and does all the conversion automatically:

```cpp
// CTORS
Color(float red, float green, float blue, float alpha = 1);
explicit Color(RGB rgb);
explicit Color(HSV hsv);
explicit Color(HSL hsl);
explicit Color(GrayScale grayscale);

// assignment
Color & operator=(Color);
Color & operator=(RGB);
Color & operator=(HSV);
Color & operator=(HSL);
Color & operator=(GrayScale);

// conversion
RGB Color::as_rgb() const;
HSV Color::as_hsv() const;
HSL Color::as_hsl() const;
GrayScale Color::as_grayscale() const;
```

This makes it convenient because we don't have to deal with ``convert_to`` everytime, though you can still do that. To accomplish our goal from earlier of converting and RGB color into black-and-white we can do the following:

```cpp
RGB color = //...

HSV as_hsv = conver_to<HSV>(origin_color);
as_hsv.s = 0;
color = convert_to<RGB>(as_hsv);
``` 
or if we start out with a ``crisp::Color`` (as we would when for example using ``crisp::ColorImage``):

```cpp
Color color = //...
HSV as_hsv = color.to_hsv();
as_hsv.s = 0;
Color = as_hsv;
```

Because of the expressive convert_to function crisps makes handling colors in so many representations easy.

## Member Access

Note that internally Color still uses the rgba format, to access the respective r, g, b and a members use:

```cpp
float& red();
float& green();
float& blue();
float& alpha();

// example:
Color color = //...
color.red() = 1;
color.blue() = color.red() - color.blue();
```

## Arithmetics

``Colors`` allow for arithmetics, so ``color * color`` is valid. Both +, -, *, /, their respective assignemtn and boolean comparison operators all work by comparing the colors rgba components element-wise. So ``color * color`` computes the outer product of two 4-element vectors.

To invert a color, simply call ``crisp::Color::invert() const``. Internally this simply executes ``color = 1 - color``.

## Mixing

You can mix two colors via: 
```cpp
template<typename Color_t>
Color_t mix(Color_t a, Color_t b, float weight);
```
While the color representation is arbitrary, both colors must have the same representation.

--- 

For a practical example of how to use color, consider visiting the [Pseudocolor Tutorial](/docs/color/pseudocolor.md).