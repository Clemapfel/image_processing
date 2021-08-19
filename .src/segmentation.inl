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

        std::map<std::string, float> color_pair_to_dist;

        auto color_dist = [&](const Color& color_a, const Color& color_b) -> float
        {
            static std::string out_a = "123";

            out_a.at(0) = (char(color_a.red() * 255));
            out_a.at(1) = (char(color_a.green() * 255));
            out_a.at(2) = (char(color_a.blue() * 255));

            static std::string out_b = "123";

            out_b.at(0) = (char(color_b.red() * 255));
            out_b.at(1) = (char(color_b.green() * 255));
            out_b.at(2) = (char(color_b.blue() * 255));

            std::string key;
            key.reserve(6);
            if (out_a < out_b)
                key = out_a + out_b;
            else
                key = out_b + out_a;

            if (color_pair_to_dist.find(key) == color_pair_to_dist.end())
            {
                if (out_a == out_b)
                    color_pair_to_dist.emplace(key, 0);
                else
                    color_pair_to_dist.emplace(key, dist(color_a, color_b));
            }

            return color_pair_to_dist.at(key);
        };

        struct Cluster
        {
            Vector2ui center;
            Color color_sum;
            Vector2f xy_sum;
            size_t n;
            size_t last_n_changed = 0;
            bool converged = false;
        };

        std::map<size_t, Cluster> clusters;

        for (long x = spacing / 2, cluster_i = 0; x < image.get_size().x(); x += spacing)
        {
            for (long y = spacing / 2; y < image.get_size().y(); y += spacing, cluster_i++)
            {
               clusters.emplace(cluster_i, Cluster{Vector2ui(x, y), image(x, y), Vector2f(x, y), 1});
            }
        }

        // use color image as labeling space [-1, 0, 1, ... centers.size()-1] to save memory and allocation time
        // r = label
        // g = distance
        // b = 0
        ColorImage out;
        out.create(image.get_size().x(), image.get_size().y(), Color(-1, std::numeric_limits<float>::infinity(), 0));

        std::map<int, int> do_merge; // old_id: new_id, n_passes

        int n_changed_before = 0;
        int n_changed = 0;
        do
        {
            n_changed_before = n_changed;
            n_changed = 0;
            for (auto& pair : clusters)
            {
                if (pair.second.converged == true)
                    continue;

                size_t n_local_changed = 0;
                size_t cluster_i = pair.first;
                auto& center = pair.second.center;
                Color mean_color = pair.second.color_sum;
                mean_color /= pair.second.n;

                Vector2f mean_pos = pair.second.xy_sum;
                mean_pos /= pair.second.n;

                for (int i = -spacing; i < +spacing; ++i)
                {
                    for (int j = -spacing; j < +spacing; ++j)
                    {
                         if ((center.x() + i < 0 or center.x() + i >= image.get_size().x())
                            or (center.y() + j < 0 or center.y() + j >= image.get_size().y()))
                             continue;

                         int old_i = out(center.x() + i, center.y() + j).x();
                         bool join_always = false;

                         if (old_i == cluster_i)
                            continue;
                         else if (do_merge.find(old_i) != do_merge.end())
                             join_always = true;

                         auto pos = Vector2ui(center.x() + i, center.y() + j);

                         auto col_dist = color_dist(mean_color, image(pos.x(), pos.y())) * 2;
                         auto xy_dist = dist(mean_pos, Vector2f(pos.x(), pos.y())) / (2*spacing);
                         auto new_dist = col_dist + xy_dist;

                         auto old_dist = out(pos.x(), pos.y()).y();

                         if (new_dist < old_dist)
                         {
                             out(pos.x(), pos.y()).x() = cluster_i;
                             out(pos.x(), pos.y()).y() = new_dist;

                             pair.second.n += 1;
                             pair.second.color_sum += image(pos.x(), pos.y());
                             pair.second.xy_sum += Vector2f(pos.x(), pos.y());

                             if (old_i != -1)
                             {
                                 auto& old = clusters.at(old_i);
                                 old.n -= 1;
                                 old.color_sum -= image(pos.x(), pos.y());
                                 old.xy_sum -= Vector2f(pos.x(), pos.y());
                             }

                             mean_color = pair.second.color_sum;
                             mean_color /= pair.second.n;

                             mean_pos = pair.second.xy_sum;
                             mean_pos /= pair.second.n;

                             n_changed += 1;
                             n_local_changed += 1;
                         }
                    }
                }
            }

            std::cout << n_changed << std::endl;

        } while (std::abs<int>(n_changed_before - n_changed) > 4*spacing);

        for (auto& px : out)
        {
            size_t which = px.x();
            const auto& cluster = clusters.at(which);

            Color mean = cluster.color_sum;
            mean /= cluster.n;
            px = mean;
        }

        return out;
    }


    /*
     *
                             // schedule merge
                             if (old_i != -1)
                             {
                                 auto& old = clusters.at(old_i);
                                 auto old_color = old.sum;
                                 old_color /= float(old.n);
                                 if (color_dist(mean_color, old_color) < 0.01)
                                 {
                                     clusters.at(cluster_i).sum += old.sum;
                                     clusters.at(cluster_i).n += old.n;
                                     n_changed += old.n;

                                     do_merge.emplace(old_i, std::make_pair(cluster_i, 0));
                                 }
                             }

     */
}