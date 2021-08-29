# Segmentation

Segmenting an image into parts and processing these parts individually is what allows modern image libraries and algorithms to work, however reliably doing so depends on a huge number of environmental factos and the image itself. To address this crisps offers a broad variety of algorithms that excel in one situation but not others to hopefully allow a user to have the confidence that at least one of the algorithsm outlined here will do the job. 

First let's talk about the main output of the segmentation algorithms, ``ImageSegment``s

## [ImageSegment](../../include/image_segment.hpp)

In ``crisp`` and image segment is simply a set of pixel coordinates, internally it is just a typedef for ``std::set`` with a custom order operator. The set is ordered such that the top-most, left-most pixel is the first pixel as such:

TODO: image of numbering

Unlike ``ImageRegion``, ``ImageSegment`` puts not constraints of connectivity on it's pixels, the only property it enforces is that each pixel only occurs exactly once in the same segment (that is, it is not a multiset).

ImageSegments can be extracted from Images via ``decompose_into_segments``. This function takes an image and iterates through it, retuning a set of pixles for each **connected** component in the image. If you would like a segment that simply represents all pixels of a certain color you can do so via a simple for loop:

```cpp
GrayScaleImage image;
image.create(//...



for (long y = 0; y < image.get_size().y; ++y
    for (long x = 0; x < image.get_size().x; ++x)
        