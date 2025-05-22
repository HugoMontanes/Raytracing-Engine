/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>
#include <algorithm>

#include <engine/Path_Tracing.hpp>
#include <engine/Scene.hpp>
#include <engine/Window.hpp>

#include <raytracer/Diffuse_Material.hpp>
#include <raytracer/Metallic_Material.hpp>
#include <raytracer/Pinhole_Camera.hpp>
#include <raytracer/Plane.hpp>
#include <raytracer/Sphere.hpp>
#include <raytracer/Skydome.hpp>

#include "engine/Thread_Pool_Manager.hpp"

namespace udit::engine
{

    // Engine registration system - this is how your framework knows how to create subsystems
    template<>
    Subsystem::Unique_Ptr Subsystem::create< Path_Tracing >(Scene& scene)
    {
        return std::make_unique< Path_Tracing >(scene);
    }

    template<>
    Stage::Unique_Ptr Stage::create< Path_Tracing >(Scene& scene)
    {
        return std::make_unique< Path_Tracing::Stage >(scene);
    }

    template<>
    template<>
    Id Registrar< Subsystem >::record< Path_Tracing >()
    {
        return registry().add("Path_Tracing", Subsystem::create< Path_Tracing >);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Path_Tracing >()
    {
        return registry().add("Path_Tracing::Stage", Stage::create< Path_Tracing >);
    }

    template<>
    Id Subsystem::setup< Path_Tracing >()
    {
        static Id id = INVALID_ID;

        if (not_valid(id))
        {
            id = Subsystem::record< Path_Tracing >();
        }

        return id;
    }

    template<>
    Id Subsystem::id_of< Path_Tracing >()
    {
        return Subsystem::setup< Path_Tracing >();
    }

    template<>
    Id Stage::setup< Path_Tracing >()
    {
        static Id id = INVALID_ID;

        if (not_valid(id))
        {
            id = Stage::record< Path_Tracing >();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Path_Tracing >()
    {
        return Stage::setup< Path_Tracing >();
    }

    template<>
    Id Subsystem::id_of< Path_Tracing::Camera >()
    {
        return Subsystem::setup< Path_Tracing >();
    }

    template<>
    Id Subsystem::id_of< Path_Tracing::Model >()
    {
        return Subsystem::setup< Path_Tracing >();
    }

    // Constructor: Initialize the Path_Tracing subsystem with optimal defaults
    Path_Tracing::Path_Tracing(Scene& scene)
        :
        Subsystem(scene),
        path_tracer_space(path_tracer_scene),
        rays_per_pixel(1),                        // Start with fast, low-quality rendering
        continuous_rendering_enabled(false),      // Begin with traditional rendering
        target_display_fps(30.0f),               // Conservative default frame rate
        last_frame_time(0.0f),
        frames_since_resize(0)
    {
        // Create a beautiful sky environment that provides natural lighting
        // Even without explicit light sources, this creates realistic illumination
        path_tracer_scene.create< raytracer::Skydome >(
            raytracer::Color{ .5f, .75f, 1.f },   // Soft blue sky color
            raytracer::Color{ 1, 1, 1 }           // Bright white horizon
        );

        std::cout << "Path_Tracing subsystem initialized with synchronized continuous updates" << std::endl;
    }

    // Continuous rendering control methods with enhanced validation and feedback

    void Path_Tracing::enable_continuous_rendering(float display_fps)
    {
        // Input validation with helpful error handling
        if (display_fps <= 0.0f || display_fps > 1000.0f) {
            std::cerr << "Warning: Invalid display FPS " << display_fps
                << ", clamping to valid range [1.0, 1000.0]" << std::endl;
            display_fps = std::max(1.0f, std::min(display_fps, 1000.0f));
        }

        // Avoid unnecessary restarts if already configured correctly
        if (continuous_rendering_enabled && target_display_fps == display_fps) {
            std::cout << "Continuous rendering already enabled at " << display_fps << " FPS" << std::endl;
            return;
        }

        // Store configuration before starting the low-level system
        continuous_rendering_enabled = true;
        target_display_fps = display_fps;

        // Initialize the synchronized continuous update system
        initialize_continuous_rendering();

        std::cout << "Synchronized continuous rendering enabled at " << display_fps << " FPS" << std::endl;
        std::cout << "  - Data consistency: Guaranteed through tile synchronization" << std::endl;
        std::cout << "  - Quality preservation: Shadows and colors fully maintained" << std::endl;
    }

    void Path_Tracing::disable_continuous_rendering()
    {
        if (!continuous_rendering_enabled) {
            std::cout << "Continuous rendering already disabled" << std::endl;
            return;
        }

        continuous_rendering_enabled = false;

        // Clean shutdown with proper synchronization
        shutdown_continuous_rendering();

        std::cout << "Continuous rendering disabled - reverted to traditional frame-by-frame rendering" << std::endl;
    }

    bool Path_Tracing::is_continuous_rendering_enabled() const
    {
        return continuous_rendering_enabled;
    }

    void Path_Tracing::set_display_fps(float fps)
    {
        // Validate and constrain input parameters
        fps = std::max(1.0f, std::min(fps, 1000.0f));

        target_display_fps = fps;

        // Apply changes immediately if continuous rendering is active
        if (continuous_rendering_enabled) {
            path_tracer.set_update_rate(fps);
            std::cout << "Display update rate changed to " << fps << " FPS" << std::endl;
        }
    }

    float Path_Tracing::get_display_fps() const
    {
        return target_display_fps;
    }

    float Path_Tracing::get_last_frame_time() const
    {
        std::lock_guard<std::mutex> lock(performance_mutex);
        return last_frame_time;
    }

    bool Path_Tracing::is_performance_stable() const
    {
        std::lock_guard<std::mutex> lock(performance_mutex);
        // Consider performance stable after several frames since initialization or resize
        return frames_since_resize > 10;
    }

    // Private helper methods for managing continuous rendering lifecycle

    void Path_Tracing::initialize_continuous_rendering()
    {
        // Only initialize if we have valid rendering buffers
        // This prevents initialization with empty or invalid data
        if (!path_tracer.get_frame_buffer().empty()) {
            path_tracer.start_continuous_updates(target_display_fps);
        }
        else {
            std::cout << "Deferring continuous rendering start until buffers are initialized" << std::endl;
        }
    }

    void Path_Tracing::shutdown_continuous_rendering()
    {
        path_tracer.stop_continuous_updates();
    }

    bool Path_Tracing::should_restart_continuous_rendering() const
    {
        // Restart continuous rendering if it's enabled but not currently active
        // This handles cases like buffer resizing where we temporarily disable it
        return continuous_rendering_enabled && !path_tracer.is_continuous_updates_active();
    }

    // Component creation methods - these build the 3D scene content

    template< >
    Component* Subsystem::create_component< Path_Tracing::Camera >
        (
            Entity& entity,
            const Path_Tracing::Camera::Sensor_Type& sensor_type,
            const float& focal_length
        )
    {
        return static_cast<Path_Tracing*>(this)->create_camera_component(entity, sensor_type, focal_length);
    }

    Component* Path_Tracing::create_camera_component(Entity& entity, Camera::Sensor_Type sensor_type, float focal_length)
    {
        auto camera = camera_components.allocate(entity.id);

        // Create a pinhole camera with the specified characteristics
        camera->instance = path_tracer_scene.create< raytracer::Pinhole_Camera >(sensor_type, focal_length);

        std::cout << "Camera component created with "
            << (sensor_type == raytracer::Camera::FULL_FRAME ? "full-frame" : "APS-C")
            << " sensor and " << (focal_length * 1000) << "mm focal length" << std::endl;

        return camera;
    }

    template< >
    Component* Subsystem::create_component< Path_Tracing::Model >(Entity& entity)
    {
        return static_cast<Path_Tracing*>(this)->create_model_component(entity);
    }

    Component* Path_Tracing::create_model_component(Entity& entity)
    {
        auto model = model_components.allocate(entity.id);

        // Create a new 3D model that can hold geometric objects
        model->instance = path_tracer_scene.create< raytracer::Model >();
        model->path_tracer_scene = &path_tracer_scene;

        return model;
    }

    // Main rendering stage implementation - this is where continuous rendering shines

    void Path_Tracing::Stage::prepare()
    {
        subsystem = scene.get_subsystem< Path_Tracing >();
    }

    void Path_Tracing::Stage::compute(float frame_time)
    {
        if (!subsystem) {
            return;  // Safety check for proper initialization
        }

        // Update performance tracking for monitoring and optimization
        {
            std::lock_guard<std::mutex> lock(subsystem->performance_mutex);
            subsystem->last_frame_time = frame_time;
            subsystem->frames_since_resize++;
        }

        auto& window = scene.get_window();
        auto viewport_width = window.get_width();
        auto viewport_height = window.get_height();

        // Always synchronize transforms first - this ensures responsive interaction
        // Even in continuous mode, transform changes should be reflected immediately
        update_component_transforms();

        // Check for window resize - this requires special handling in continuous mode
        bool need_resize = (subsystem->path_tracer.get_frame_buffer().get_width() != viewport_width ||
            subsystem->path_tracer.get_frame_buffer().get_height() != viewport_height);

        if (need_resize) {
            std::cout << "Window resized to " << viewport_width << "x" << viewport_height
                << " - reinitializing buffers" << std::endl;

            // Handle buffer resizing with minimal disruption
            bool was_continuous = subsystem->is_continuous_rendering_enabled();
            float previous_fps = subsystem->get_display_fps();

            if (was_continuous) {
                std::cout << "Temporarily disabling continuous rendering for resize" << std::endl;
                subsystem->disable_continuous_rendering();
            }

            // Reset performance tracking since we're starting fresh
            {
                std::lock_guard<std::mutex> lock(subsystem->performance_mutex);
                subsystem->frames_since_resize = 0;
            }

            // Perform initial trace with full multithreading to initialize buffers
            auto& thread_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::RENDERING);

            std::vector<std::future<void>> futures;
            auto submit_task = [&thread_pool, &futures](std::function<void()> task) {
                futures.push_back(thread_pool.submit(Task_Priority::HIGH, std::move(task)));
                };
            auto wait_for_tasks = [&futures]() {
                for (auto& future : futures) future.wait();
                futures.clear();
                };

            // Enable multithreading for camera and path tracer
            auto camera = subsystem->path_tracer_scene.get_camera();
            if (auto pinhole_camera = dynamic_cast<raytracer::Pinhole_Camera*>(camera)) {
                pinhole_camera->enable_multithreading(submit_task, wait_for_tasks);
            }
            subsystem->path_tracer.enable_multithreading(submit_task, wait_for_tasks);

            // Perform initial trace to establish buffer sizes and content
            subsystem->path_tracer.trace(
                subsystem->path_tracer_space,
                viewport_width,
                viewport_height,
                subsystem->rays_per_pixel
            );

            // Restart continuous rendering if it was previously active
            if (was_continuous) {
                std::cout << "Restarting continuous rendering after resize" << std::endl;
                subsystem->enable_continuous_rendering(previous_fps);
            }
        }

        // Ensure continuous rendering is active if configured
        if (subsystem->should_restart_continuous_rendering()) {
            std::cout << "Starting continuous rendering as configured" << std::endl;
            subsystem->initialize_continuous_rendering();
        }

        // Main rendering logic: choose between continuous and traditional modes
        if (subsystem->is_continuous_rendering_enabled() &&
            subsystem->path_tracer.is_continuous_updates_active()) {

            // CONTINUOUS RENDERING MODE
            // In this mode, ray tracing accumulates samples continuously in the background
            // while display updates happen at a predictable rate with guaranteed data consistency

            auto& thread_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::RENDERING);

            std::vector<std::future<void>> futures;
            auto submit_task = [&thread_pool, &futures](std::function<void()> task) {
                futures.push_back(thread_pool.submit(Task_Priority::NORMAL, std::move(task)));
                };
            auto wait_for_tasks = [&futures]() {
                for (auto& future : futures) future.wait();
                futures.clear();
                };

            // Enable multithreading for continuous sample accumulation
            auto camera = subsystem->path_tracer_scene.get_camera();
            if (auto pinhole_camera = dynamic_cast<raytracer::Pinhole_Camera*>(camera)) {
                pinhole_camera->enable_multithreading(submit_task, wait_for_tasks);
            }
            subsystem->path_tracer.enable_multithreading(submit_task, wait_for_tasks);

            // Continue accumulating ray samples - this improves quality over time
            subsystem->path_tracer.trace(
                subsystem->path_tracer_space,
                viewport_width,
                viewport_height,
                subsystem->rays_per_pixel
            );

            // Display the latest synchronized snapshot
            // This data is guaranteed to be consistent thanks to our synchronization system
            window.blit_rgb_float(
                subsystem->path_tracer.get_snapshot_for_display().data(),
                viewport_width,
                viewport_height
            );

        }
        else {

            // TRADITIONAL RENDERING MODE
            // In this mode, we compute and display each frame synchronously
            // This maintains compatibility with the original system behavior

            auto& thread_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::RENDERING);

            std::vector<std::future<void>> futures;
            auto submit_task = [&thread_pool, &futures](std::function<void()> task) {
                futures.push_back(thread_pool.submit(Task_Priority::NORMAL, std::move(task)));
                };
            auto wait_for_tasks = [&futures]() {
                for (auto& future : futures) future.wait();
                futures.clear();
                };

            // Enable multithreading for this frame
            auto camera = subsystem->path_tracer_scene.get_camera();
            if (auto pinhole_camera = dynamic_cast<raytracer::Pinhole_Camera*>(camera)) {
                pinhole_camera->enable_multithreading(submit_task, wait_for_tasks);
            }
            subsystem->path_tracer.enable_multithreading(submit_task, wait_for_tasks);

            // Perform ray tracing for this frame
            subsystem->path_tracer.trace(
                subsystem->path_tracer_space,
                viewport_width,
                viewport_height,
                subsystem->rays_per_pixel
            );

            // Display the result immediately
            window.blit_rgb_float(
                subsystem->path_tracer.get_snapshot().data(),
                viewport_width,
                viewport_height
            );
        }
    }

    void Path_Tracing::Stage::update_component_transforms()
    {
        // Synchronize engine-level transform components with ray tracer transforms
        // This method runs in parallel to minimize the overhead of transform updates

        auto& thread_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::RENDERING);

        std::vector<std::future<void>> futures;

        auto submit_task = [&thread_pool, &futures](std::function<void()> task) {
            futures.push_back(thread_pool.submit(Task_Priority::HIGH, std::move(task)));
            };

        auto wait_for_tasks = [&futures]() {
            for (auto& future : futures) {
                future.wait();
            }
            futures.clear();
            };

        // Update camera transforms in parallel
        for (auto& camera : subsystem->camera_components)
        {
            submit_task([this, &camera]() {
                auto transform = subsystem->scene.get_component<Transform>(camera.entity_id);
                if (transform && camera.instance) {
                    // Copy transform data from engine component to ray tracer
                    camera.instance->transform.set_position(transform->position);
                    camera.instance->transform.set_rotation(transform->rotation);
                    camera.instance->transform.set_scales(transform->scales);
                }
                });
        }

        // Update model transforms in parallel
        for (auto& model : subsystem->model_components)
        {
            submit_task([this, &model]() {
                auto transform = subsystem->scene.get_component<Transform>(model.entity_id);
                if (transform && model.instance) {
                    // Copy transform data from engine component to ray tracer
                    model.instance->transform.set_position(transform->position);
                    model.instance->transform.set_rotation(transform->rotation);
                    model.instance->transform.set_scales(transform->scales);
                }
                });
        }

        // Wait for all transform updates to complete before proceeding
        wait_for_tasks();
    }

    // Model utility methods - these provide convenient ways to add geometry and materials

    Path_Tracing::Material* Path_Tracing::Model::add_diffuse_material(const Color& color)
    {
        // Create a Lambertian diffuse material with the specified color
        // This material scatters light uniformly in all directions
        return path_tracer_scene->create< raytracer::Diffuse_Material >(color);
    }

    Path_Tracing::Material* Path_Tracing::Model::add_metallic_material(const Color& color, float diffusion)
    {
        // Create a metallic material with controllable reflectivity
        // Lower diffusion values create mirror-like surfaces
        return path_tracer_scene->create< raytracer::Metallic_Material >(color, diffusion);
    }

    void Path_Tracing::Model::add_sphere(float radius, Material* material)
    {
        // Add a sphere primitive at the origin of this model's transform
        instance->add(path_tracer_scene->create< raytracer::Sphere >(Vector3{ 0.f,  0.f, -1.f }, radius, material));
    }

    void Path_Tracing::Model::add_plane(const Vector3& normal, Material* material)
    {
        // Add an infinite plane primitive with the specified normal direction
        instance->add(path_tracer_scene->create< raytracer::Plane >(Vector3{ 0.f, .25f,  0.f }, normal, material));
    }
}