# Pseudocolor

Pseudocolor is transforming creating a full-spectrum rgb color image from a single-spectrum image. The best and easiest example to illustrate this is with infrared:

![](./pseudocolor_example.png)
(source: <https://mobile.twitter.com/dingoresearch/status/854228672785416193>)

This is an infrared aerial image of a forest, in the center of the image is clearing with an animal. Because the animals body temperature is higher than it's surroundings, the animal shows up lighter than the rest. While it's not hard to spot the presence of the animal with the naked eye, when flying very fast, being way more zoomed out or having an AI try to recognize it it might be a lot harder. To mitigate this we would want to highlight the animal based on it's body temperature and a very effective way to do this is to use *pseudocolor*

Crisps offers a class called ``PseudoColorTransform`` just for this occasion. It has 2 main functions:

```cpp
class PseudoColorTransform {
        
    void set_function(/.../ mapping);
    ColorImage transform(const GrayScaleImage&);
};
```
As you can see pseudocolortransforms transform a grayscale image into a color image. How this is done is controlled via set function. ``PseudoColorTransform`` offers 5 different functions you can "plug-in" via ``set_function``. Let's look at only two for now:

```cpp
static auto&& identity();
static auto&& value_range_to_hue(
        float min_gray,    // lowest intensity value transform
        float max_gray,    // highest intensity value transformed
        float hue          // hue the color will be transformed into
);
```
Identity simply transforms a grayscale image into a gray *color-image*. While visually indistinguishable in memory the color image will have 4-component vectors while the grayscale image only has a single component scalar for each pixel. 
With ``value_range_to_hue_range`` you can transform gray-values in the interval [min_gray, max_gray] into colors whos HSV representation is in the range HSV(hue, 1, 1). This is easiest to illustrate with our example:

![](./pseudocolor_example.png)
(source: <https://mobile.twitter.com/dingoresearch/status/854228672785416193>)

Again we want to highlight the animal. We would expect the infrared value on the animals skin to be the hottest place around, usually 37° is more than the grounds surface:

```cpp
auto infrared = GrayScaleImage();
infrared.create_from_file("/home/clem/Workspace/image_processing/docs/color/pseudocolor_example.png");

auto transform = PseudoColorTransform();
transform.set_function(PseudoColorTransform::value_range_to_hue(0.6, 1, 0.05));

auto color = transform.transform(infrared);
```
Here we're mapping all gray values in the range [0.6, 1] to a hue value that corresponds to red:

![](./pseudocolor_05.png)

We can see that the animal has been much more clearly highlighted. 

# Mapping to Ranges

Instead of specifying just one color you can map an entire range into another range as such:

```cpp
auto infrared = GrayScaleImage();
infrared.create_from_file("/home/clem/Workspace/image_processing/docs/color/pseudocolor_example_2.png");

auto transform = PseudoColorTransform();
transform.set_function(PseudoColorTransform::value_range_to_hue_range(0.6, 1, 0, 0.25));

auto color = transform.transform(infrared);
```

original:
![](pseudocolor_example_2.png)

transformed:
![](pseudocolor_range.png)

You can invert the hue range such that the lowest gray value is mapped to the highest hue value and vice versa by using 

```cpp
transform.set_function(PseudoColorTransform::value_range_to_inverse_hue_range(0.6, 1, 0, 0.25));
```
![](pseudocolor_range_inv.png)

While this does provide a lot of freedom crisp offers one more step in complexity and customizability. You can map multiple ranges of gray values to another corresponding range of hues. To keep syntax simple, crisps offers ``PseudoColorTransform::RangeMapping`` for just this purpose. Let's go back to the original aerial image and let's say we want to map the animal to shades of red, and the trees and ground in shades of blue for contrast:

```cpp
auto infrared = GrayScaleImage();
    infrared.create_from_file("/home/clem/Workspace/image_processing/docs/color/pseudocolor_example.png");

auto transform = PseudoColorTransform();

auto mapping = PseudoColorTransform::RangeMapping();
float animal_threshold = 0.6;
mapping.add_value_range_to_hue_range(0, animal_threshold, 0.5, 0.75); // ground
mapping.add_value_range_to_inverse_hue_range(animal_threshold, 1, 0, 0.1); // animal
transform.set_function(PseudoColorTransform::value_ranges_to_hue_ranges(mapping));

auto color = transform.transform(infrared);
```

![](pseudocolor_multi.png)

Because the ground is now blue the contrast is enhanced and it's very easy to spot the animal.

While not enforced if grayvalue ranges overlap which is chosen is undefined. 

/*
auto infrared = GrayScaleImage();
    infrared.create_from_file("/home/clem/Workspace/image_processing/docs/color/pseudocolor_example.png");

    auto transform = PseudoColorTransform();

    auto mapping = PseudoColorTransform::RangeMapping();
    float animal_threshold = 0.6;
    mapping.add_value_range_to_inverse_hue_range(0, animal_threshold, 0.5, 0.75); // ground
    mapping.add_value_range_to_hue_range(animal_threshold, 1, 0, 0.1); // animal
    transform.set_function(PseudoColorTransform::value_ranges_to_hue_ranges(mapping));

    auto color = transform.transform(infrared);

    auto sprite = Sprite();
    sprite.create_from(color);

    RenderWindow window;
    window.create(sprite.get_size().x(), sprite.get_size().y);

    while (window.is_open())
    {
        window.update();
        window.clear();
        window.draw(sprite);
        window.display();
    }
*/



