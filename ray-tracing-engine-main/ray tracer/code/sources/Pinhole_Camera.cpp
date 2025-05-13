/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <raytracer/Pinhole_Camera.hpp>
#include <vector>
#include <thread>
#include <algorithm>

#include <iostream>
using namespace std;

namespace udit::raytracer
{

    void Pinhole_Camera::calculate (Buffer< Ray > & primary_rays)
    {
        auto buffer_width  = primary_rays.get_width  ();
        auto buffer_height = primary_rays.get_height ();

        Vector2 half_sensor_resolution
        (
            0.5f * static_cast< float >(buffer_width ),
            0.5f * static_cast< float >(buffer_height)
        );

        Vector2 half_sensor_size
        (
            0.5f * get_sensor_width (),
            0.5f * get_sensor_width () * half_sensor_resolution.y / half_sensor_resolution.x
        );

        auto  & transform_matrix   = transform.get_matrix   ();
        Vector3 sensor_center      = transform.get_position ();
        Vector3 focal_point        = transform_matrix * Vector4(0, 0, -focal_length, 1);
        Vector3 right_direction    = transform_matrix * Vector4(half_sensor_size.x, 0, 0, 0);
        Vector3 up_direction       = transform_matrix * Vector4(0, half_sensor_size.y, 0, 0);
        Vector3 sensor_bottom_left = sensor_center - (right_direction + up_direction);

        Vector3 horizontal_step    = right_direction / half_sensor_resolution.x;
        Vector3 vertical_step      = up_direction    / half_sensor_resolution.y;


        if (!use_multithreading_)
        {
            Vector3 scanline_start     = sensor_bottom_left;
            auto    buffer_offset      = buffer_width * buffer_height;

            for (auto row = buffer_height; row > 0; --row, scanline_start += vertical_step)
            {
                Vector3 pixel = scanline_start;

                for (auto column = buffer_width; column > 0; --column, pixel += horizontal_step)
                {
                    primary_rays[--buffer_offset] = Ray{ pixel, focal_point - pixel };
                }
            }
        }
        else if (use_multithreading_)
        {
            // Concurrent tile-based implementation
            unsigned total_pixels = buffer_width * buffer_height;

            // Choose tile size based on total resolution
            unsigned TILE_SIZE;
            if (total_pixels < 250000) // 500x500 or smaller
                TILE_SIZE = 32;
            else if (total_pixels < 1000000) // 1000x1000 or smaller
                TILE_SIZE = 64;
            else
                TILE_SIZE = 128;

            unsigned tiles_x = (buffer_width + TILE_SIZE - 1) / TILE_SIZE;
            unsigned tiles_y = (buffer_height + TILE_SIZE - 1) / TILE_SIZE;

            // Get number of hardware threads for adaptive task scheduling
            unsigned available_threads = std::thread::hardware_concurrency();
            unsigned MAX_TASKS_PER_BATCH = available_threads * 4; // Queue depth

            // Process tiles in batches
            std::vector<std::pair<unsigned, unsigned>> tile_coords;

            // Create tile coordinates
            for (unsigned ty = 0; ty < tiles_y; ++ty) {
                for (unsigned tx = 0; tx < tiles_x; ++tx) {
                    tile_coords.emplace_back(tx, ty);
                }
            }

            // Process tiles in batches to avoid overwhelming the thread pool
            for (size_t batch_start = 0; batch_start < tile_coords.size(); batch_start += MAX_TASKS_PER_BATCH) {
                size_t batch_end = std::min(batch_start + MAX_TASKS_PER_BATCH, tile_coords.size());

                // Submit this batch of tasks
                for (size_t i = batch_start; i < batch_end; ++i) {
                    unsigned tx = tile_coords[i].first;
                    unsigned ty = tile_coords[i].second;

                    // Calculate tile bounds
                    unsigned start_x = tx * TILE_SIZE;
                    unsigned start_y = ty * TILE_SIZE;
                    unsigned end_x = std::min(start_x + TILE_SIZE, buffer_width);
                    unsigned end_y = std::min(start_y + TILE_SIZE, buffer_height);

                    // Submit task for this tile
                    submit_task_([this, &primary_rays, start_x, start_y, end_x, end_y,
                        sensor_bottom_left, horizontal_step, vertical_step, focal_point]() {
                            generate_rays_for_tile(
                                primary_rays,
                                start_x, start_y,
                                end_x, end_y,
                                sensor_bottom_left,
                                horizontal_step,
                                vertical_step,
                                focal_point
                            );
                        });
                }

                // Wait for all tasks in this batch to complete
                wait_for_tasks_();
            }
        }

    }

    void Pinhole_Camera::enable_multithreading(const std::function<void(std::function<void()>)>& submit_task, const std::function<void()>& wait_for_tasks)
    {
        use_multithreading_ = true;
        submit_task_ = submit_task;
        wait_for_tasks_ = wait_for_tasks;
    }

    void Pinhole_Camera::disable_multithreading()
    {
        use_multithreading_ = false;
        submit_task_ = nullptr;
        wait_for_tasks_ = nullptr;
    }

    void Pinhole_Camera::generate_rays_for_tile(Buffer<Ray>& primary_rays, unsigned start_x, unsigned start_y, unsigned end_x, unsigned end_y, const Vector3& sensor_bottom_left, const Vector3& horizontal_step, const Vector3& vertical_step, const Vector3& focal_point)
    {
        // Process each pixel in the tile
        for (unsigned y = start_y; y < end_y; ++y) {
            for (unsigned x = start_x; x < end_x; ++x) {
                // Calculate pixel position on the sensor
                Vector3 pixel = sensor_bottom_left +
                    horizontal_step * static_cast<float>(x) +
                    vertical_step * static_cast<float>(y);

                // Calculate buffer index (flip Y because we're going from bottom to top)
                unsigned buffer_y = primary_rays.get_height() - 1 - y;
                unsigned buffer_index = buffer_y * primary_rays.get_width() + x;

                // Create and store the ray
                primary_rays[buffer_index] = Ray{ pixel, focal_point - pixel };
            }
        }
    }

}
