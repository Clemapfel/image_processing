// 
// Copyright 2021 Clemens Cords
// Created on 18.08.21 by clem (mail@clemens-cords.com)
//

namespace crisp::Segmentation
{
    BinaryImage basic_threshold(const GrayScaleImage& image)
    {
        const float convergence_treshold = 1 / 255.f;

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
        out.create(image.get_size().x(), image.get_size().y());

        for (long x = 0; x < image.get_size().x(); ++x)
            for (long y = 0; y < image.get_size().y(); ++y)
                out(x, y) = image(x, y) > (new_threshold / 255.f);

        return out;
    }

    BinaryImage otsu_threshold(const GrayScaleImage& image)
    {
        auto histogram = Histogram<uint8_t>();
        histogram.create_from(image);

        std::map<uint8_t, std::pair<float, float>> threshold_to_sums;   // k: {cumulative_sum, intensity_sum}
        float mn = image.get_size().x() * image.get_size().y();
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
        out.create(image.get_size().x(), image.get_size().y());

        for (long x = 0; x < image.get_size().x(); ++x)
            for (long y = 0; y < image.get_size().y(); ++y)
                out(x, y) = image(x, y) > result;

        return out;
    }

    BinaryImage variable_threshold(const GrayScaleImage& image, float constant)
    {
        BinaryImage out;
        out.create(image.get_size().x(), image.get_size().y());

        std::list<float> tail;
        float current_sum = 0;

        size_t tail_length = constant * std::min(image.get_size().x(), image.get_size().y());
        size_t k = 0;

        auto update = [&](size_t x, size_t y, size_t i)
        {
            out(x, y) = image(x, y) > (current_sum / tail.size());

            if (i > tail_length)
            {
                current_sum -= tail.front();
                tail.erase(tail.begin());
            }

            tail.emplace_back(image(x, y));
            current_sum += tail.back();
        };

        int top = 0, bottom = image.get_size().x() - 1, left = 0, right = image.get_size().y() - 1;
        int direction = 1;

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

    // @complexity: amortized m*n*8*neighborhood_size, where m,n size of the image
    BinaryImage neighborhood_threshold(const GrayScaleImage& image, size_t neighborhood_size)
    {
        auto out = BinaryImage();
        out.create(image.get_size().x(), image.get_size().y());

        const int spread = 7;
        const int half_spread = spread * 0.5;

        for (long x = 0; x < image.get_size().x(); ++x)
        {
            for (long y = 0; y < image.get_size().y(); ++y)
            {
                float mean = 0;
                size_t count = 0;
                for (long i = 1; i <= spread * neighborhood_size; i += spread)
                {
                    mean += image.get_pixel_or_padding(x - i, y);
                    mean += image.get_pixel_or_padding(x + i, y);
                    mean += image.get_pixel_or_padding(x, y + i);
                    mean += image.get_pixel_or_padding(x, y - i);

                    mean += image.get_pixel_or_padding(x - i - half_spread, y - i - half_spread);
                    mean += image.get_pixel_or_padding(x + i - half_spread, y - i - half_spread);
                    mean += image.get_pixel_or_padding(x - i - half_spread, y + i - half_spread);
                    mean += image.get_pixel_or_padding(x + i - half_spread, y + i - half_spread);
                }
                mean /= (8 * neighborhood_size);
                out(x, y) = image(x, y) > mean;
            }
        }

        return out;
    }

    ColorImage superpixel_clustering(const ColorImage& image, size_t n_superpixel)
    {
        auto spacing = sqrt((image.get_size().x() * image.get_size().y()) / n_superpixel);

        float constant = spacing * spacing; // maximum expected color distance

        auto distance = [&](const Color& c_a, const Vector2ui& xy_a, const Color& c_b, const Vector2ui& xy_b)
        {
            float d_c = dist(c_a, c_b);
            float d_xy = dist(xy_a, xy_b);

            float c_term = d_c * d_c;
            float xy_term = (d_xy / spacing) * (d_xy / spacing);
            return sqrt(c_term + xy_term * constant);
        };

        std::vector<Vector2ui> centers;
        std::vector<Color> center_color_sum;
        std::vector<size_t> center_n;

        centers.reserve(n_superpixel);
        center_color_sum.reserve(n_superpixel);
        center_n.reserve(n_superpixel);

        for (long x = 0; x < image.get_size().x(); x += spacing)
        {
            for (long y = 0; y < image.get_size().y(); y += spacing)
            {
                centers.emplace_back(Vector2ui(x, y));
                center_color_sum.emplace_back(image(x, y));
                center_n.emplace_back(1);
            }
        }

        // use color image as labeling space [-1, 0, 1, ... centers.size()-1] to save memory and allocation time
        // r = label
        // g = distance
        // b = 0
        ColorImage out;
        out.create(image.get_size().x(), image.get_size().y(), Color(-1, std::numeric_limits<float>::infinity(), 0));

        while (true)
        {
            size_t max_n_changed = 0;
            for (size_t center_i = 0; center_i < centers.size(); ++center_i)
            {
                auto center = centers.at(center_i);
                size_t n_changed = 0;

                for (int i = -spacing; i < +spacing; ++i)
                {
                    for (int j = -spacing; j < +spacing; ++j)
                    {
                        if ((center.x() + i < 0 or center.x() + i >= image.get_size().x()) or  (center.y() + i < 0 or center.y() + i >= image.get_size().y()))
                            continue;

                        auto new_c = image(center.x() + i, center.y() + j);
                        auto new_pos = Vector2ui(center.x() + i, center.y() + j);

                        auto mean_color = center_color_sum.at(center_i);
                        mean_color /= center_n.at(center_i);

                        auto dist = distance(mean_color, center, new_c, new_pos);
                        if (dist < out(new_pos.x(), new_pos.y()).y())
                        {
                            out(new_pos.x(), new_pos.y()).x() = center_i;
                            out(new_pos.x(), new_pos.y()).y() = dist;

                            center_n.at(center_i) += 1;
                            center_color_sum.at(center_i) += image(new_pos.x(), new_pos.y());

                            n_changed += 1;
                        }
                    }
                }

                max_n_changed = std::max(n_changed, max_n_changed);
            }

            if (max_n_changed < 2)  //TODO: might deadlock?
                break;
        }

        for (auto& px : out)
        {
            size_t which = px.x();
            Color mean = center_color_sum.at(which);
            mean /= center_n.at(which);

            px = mean;
        }

        return out;
    }

}