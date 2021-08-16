// 
// Copyright 2021 Clemens Cords
// Created on 13.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image_segment.hpp>
#include <histogram.hpp>
#include <grayscale_image.hpp>
#include <binary_image.hpp>

#include <list>

namespace crisp::Segmentation
{
    BinaryImage basic_threshold(const GrayScaleImage& image, float convergence_treshold = 1 / 255.f)
    {
        auto histogram = Histogram<uint8_t>();
        histogram.create_from(image);

        uint8_t old_threshold = histogram.median() * 255;
        uint8_t new_threshold = 0;
        float total_sum = 0,
              left_sum = 0,
              right_sum = 0;

        size_t total_n = 0,
               left_n = 0,
               right_n = 0;

        float left_mean, right_mean;

        for (uint8_t i = 0; i < uint8_t(255); ++i)
        {
            float value = histogram.at(i) * (i / 255.f);
            if (i < old_threshold)
            {
                left_n += histogram.at(i);
                left_sum += value;
            }
            else
            {
                right_n += histogram.at(i);
                right_sum += value;
            }
        }

        left_mean = left_sum / left_n;
        right_mean = right_sum / right_n;
        new_threshold = (left_mean + 0.5 * (abs(right_mean - left_mean))) * 255;

        // TODO: optimize by only adding/subtracting necessary parts from left/right sum
        while (std::abs<float>((old_threshold / 255.f) - (new_threshold / 255.f)) > convergence_treshold)
        {
            old_threshold = new_threshold;
            left_n = 0; left_sum = 0; right_n = 0; right_sum = 0;

            for (uint8_t i = 0; i < uint8_t(255); ++i)
            {
                float value = histogram.at(i) * (i / 255.f);
                if (i < old_threshold)
                {
                    left_n += histogram.at(i);
                    left_sum += value;
                }
                else
                {
                    right_n += histogram.at(i);
                    right_sum += value;
                }
            }

            left_mean = left_sum / left_n;
            right_mean = right_sum / right_n;
            new_threshold = (left_mean + 0.5 * (abs(right_mean - left_mean))) * 255;
        }

        auto out = BinaryImage();
        out.create(image.get_size().x, image.get_size().y);

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                out(x, y) = image(x, y) > (new_threshold / 255.f);

        return out;
    }

    BinaryImage otsu_threshold(const GrayScaleImage& image)
    {
        auto histogram = Histogram<uint8_t>();
        histogram.create_from(image);

        std::map<uint8_t, std::pair<float, float>> threshold_to_sums;   // k: {cumulative_sum, intensity_sum}
        float mn = image.get_size().x * image.get_size().y;
        float global_mean = 0;

        for (uint8_t k = 0; k < 255; ++k)
        {
            float cumulative_sum = 0;
            float intensity_sum = 0;
            size_t n = 0;
            for (uint8_t i = 0; i <= k; ++i)
            {
                float p_i = histogram.at(i) / mn;
                cumulative_sum += p_i;
                intensity_sum += p_i * i;
                n += histogram.at(i);

                if (k == 254)
                    global_mean += p_i * i;
            }
            threshold_to_sums.emplace(k, std::make_pair(cumulative_sum, intensity_sum));
        }

        uint8_t max_k = 0;
        float max_sigma = 0;
        for (auto& pair : threshold_to_sums)
        {
            auto p_i = pair.second.first;
            auto local_mean = pair.second.second;
            auto sigma = pow(global_mean * p_i - local_mean, 2) / (p_i * (1 - p_i));

            if (sigma > max_sigma)
            {
                max_sigma = sigma;
                max_k = pair.first;
            }
        }

        float result = max_k / 255.f;

        auto out = BinaryImage();
        out.create(image.get_size().x, image.get_size().y);

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                out(x, y) = image(x, y) > result;

        return out;
    }

    BinaryImage variable_threshold(const GrayScaleImage& image, float constant = 1)
    {
        BinaryImage out;
        out.create(image.get_size().x, image.get_size().y);

        size_t tail_length = image.get_size().x * image.get_size().y * 0.2;
        std::list<float> tail;
        float current_sum = 0;

        auto update = [&](size_t x, size_t y, size_t i)
        {
            out(x, y) = image(x, y) > (current_sum / tail_length);

            std::cout << (current_sum / tail_length) << std::endl;

            if (i > tail_length)
            {
                current_sum -= tail.front();
                tail.erase(tail.begin());
                tail.emplace_back(image(x, y));
                current_sum += tail.back();
            }
            else
            {
                tail.emplace_back(image(x, y));
                current_sum += tail.back();
            }
        };

        int top = 0, bottom = image.get_size().x - 1, left = 0, right = image.get_size().y - 1;
        int direction = 1;

        size_t k = 0;
        while (top <= bottom and left <= right)
        {
            if (direction == 1)
            {
                for (int i = left; i <= right; ++i, k++)
                    update(top, i, k);

                ++top;
                direction = 2;
            }

            else if (direction == 2)
            {
                for (int i = top; i <= bottom; ++i, k++)
                    update(i, right, k);

                --right;
                direction = 3;
            }

            else if (direction == 3)
            {
                for (int i = right; i >= left; --i, k++)
                    update(bottom, i, k);

                --bottom;
                direction = 4;
            }
            else if (direction == 4)
            {
                for (int i = bottom; i >= top; --i)
                    update(i, left, k);

                ++ left;
                direction = 1;
            }
        }

        return out;
    }
}