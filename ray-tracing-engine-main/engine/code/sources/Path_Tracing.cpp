/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>

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

    template<>
    Subsystem::Unique_Ptr Subsystem::create< Path_Tracing > (Scene & scene)
    {
        return std::make_unique< Path_Tracing > (scene);
    }

    template<>
    Stage::Unique_Ptr Stage::create< Path_Tracing > (Scene & scene)
    {
        return std::make_unique< Path_Tracing::Stage > (scene);
    }

    template<>
    template<>
    Id Registrar< Subsystem >::record< Path_Tracing > ()
    {
        return registry ().add ("Path_Tracing", Subsystem::create< Path_Tracing >);
    }

    template<>
    template<>
    Id Registrar< Stage >::record< Path_Tracing > ()
    {
        return registry ().add ("Path_Tracing::Stage", Stage::create< Path_Tracing >);
    }

    template<>
    Id Subsystem::setup< Path_Tracing > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Subsystem::record< Path_Tracing > ();
        }

        return id;
    }

    template<>
    Id Subsystem::id_of< Path_Tracing > ()
    {
        return Subsystem::setup< Path_Tracing > ();
    }

    template<>
    Id Stage::setup< Path_Tracing > ()
    {
        static Id id = INVALID_ID;

        if (not_valid (id))
        {
            id = Stage::record< Path_Tracing > ();
        }

        return id;
    }

    template<>
    Id Stage::id_of< Path_Tracing > ()
    {
        return Stage::setup< Path_Tracing > ();
    }

    template<>
    Id Subsystem::id_of< Path_Tracing::Camera > ()
    {
        return Subsystem::setup< Path_Tracing > ();
    }

    template<>
    Id Subsystem::id_of< Path_Tracing::Model > ()
    {
        return Subsystem::setup< Path_Tracing > ();
    }

    Path_Tracing::Path_Tracing(Scene & scene)
    :
        Subsystem(scene),
        path_tracer_space(path_tracer_scene),
        rays_per_pixel(1)
    {
        path_tracer_scene.create< raytracer::Skydome > (raytracer::Color{.5f, .75f, 1.f}, raytracer::Color{1, 1, 1});
    }

    template< >
    Component * Subsystem::create_component< Path_Tracing::Camera >
    (
        Entity & entity,
        const Path_Tracing::Camera::Sensor_Type & sensor_type,
        const float & focal_length
    )
    {
        return static_cast< Path_Tracing * >(this)->create_camera_component (entity, sensor_type, focal_length);
    }

    Component * Path_Tracing::create_camera_component (Entity & entity, Camera::Sensor_Type sensor_type, float focal_length)
    {
        auto camera = camera_components.allocate (entity.id);

        camera->instance = path_tracer_scene.create< raytracer::Pinhole_Camera > (sensor_type, focal_length);

        return camera;
    }

    template< >
    Component * Subsystem::create_component< Path_Tracing::Model > (Entity & entity)
    {
        return static_cast< Path_Tracing * >(this)->create_model_component (entity);
    }

    Component * Path_Tracing::create_model_component (Entity & entity)
    {
        auto model = model_components.allocate (entity.id);

        model->instance = path_tracer_scene.create< raytracer::Model > ();
        model->path_tracer_scene = &path_tracer_scene;

        return model;
    }


    void Path_Tracing::Stage::prepare ()
    {
        subsystem = scene.get_subsystem< Path_Tracing > ();
    }

    void Path_Tracing::Stage::compute(float)
    {
        if (subsystem)
        {
            auto& window = scene.get_window();
            auto   viewport_width = window.get_width();
            auto   viewport_height = window.get_height();

            update_component_transforms();

            // Get the rendering thread pool
            auto& thread_pool = engine::Thread_Pool_Manager::get_instance().get_pool(engine::Thread_Pool_Type::RENDERING);

            // Create a vector to store futures for all tasks
            std::vector<std::future<void>> futures;
            futures.reserve(100); // Reserve space for tasks to avoid reallocations

            // Create a task submitter that uses our thread pool
            auto submit_task = [&thread_pool, &futures](std::function<void()> task) {
                auto future = thread_pool.submit(engine::Task_Priority::NORMAL, std::move(task));
                futures.push_back(std::move(future));
                };

            // Create a task waiter
            auto wait_for_tasks = [&futures]() {
                for (auto& future : futures) {
                    future.wait();
                }
                futures.clear();
                };

            // Enable multithreading for the camera's primary ray generation
            auto camera = subsystem->path_tracer_scene.get_camera();
            if (auto pinhole_camera = dynamic_cast<raytracer::Pinhole_Camera*>(camera)) {
                pinhole_camera->enable_multithreading(submit_task, wait_for_tasks);
            }

            // Enable multithreading in the path tracer
            subsystem->path_tracer.enable_multithreading(submit_task, wait_for_tasks);

            subsystem->path_tracer.trace
            (
                subsystem->path_tracer_space,
                viewport_width,
                viewport_height,
                subsystem->rays_per_pixel
            );

            // Disable multithreading for future calls
            subsystem->path_tracer.disable_multithreading();

            if (auto pinhole_camera = dynamic_cast<raytracer::Pinhole_Camera*>(camera)) {
                pinhole_camera->disable_multithreading();
            }

            window.blit_rgb_float
            (
                subsystem->path_tracer.get_snapshot().data(),
                viewport_width,
                viewport_height
            );
        }
    }

    void Path_Tracing::Stage::update_component_transforms ()
    {
        // Get the rendering thread pool
        auto& thread_pool = engine::Thread_Pool_Manager::get_instance().get_pool(engine::Thread_Pool_Type::RENDERING);

        // Vector to store futures for synchronization
        std::vector<std::future<void>> futures;

        // Create a task submitter function
        auto submit_task = [&thread_pool, &futures](std::function<void()> task) {
            auto future = thread_pool.submit(engine::Task_Priority::HIGH, std::move(task));
            futures.push_back(std::move(future));
            };

        // Create a task waiter function
        auto wait_for_tasks = [&futures]() {
            for (auto& future : futures) {
                future.wait();
            }
            futures.clear();
            };

        // Process each camera in parallel
        for (auto& camera : subsystem->camera_components)
        {
            submit_task([this, &camera]() {
                auto transform = subsystem->scene.get_component<Transform>(camera.entity_id);
                if (transform) {
                    camera.instance->transform.set_position(transform->position);
                    camera.instance->transform.set_rotation(transform->rotation);
                    camera.instance->transform.set_scales(transform->scales);
                }
                });
        }

        // Process each model in parallel (fixed to use model_components instead of camera_components)
        for (auto& model : subsystem->model_components)
        {
            submit_task([this, &model]() {
                auto transform = subsystem->scene.get_component<Transform>(model.entity_id);
                if (transform) {
                    model.instance->transform.set_position(transform->position);
                    model.instance->transform.set_rotation(transform->rotation);
                    model.instance->transform.set_scales(transform->scales);
                }
                });
        }

        // Wait for all tasks to complete
        wait_for_tasks();
    }

    Path_Tracing::Material * Path_Tracing::Model::add_diffuse_material  (const Color & color)
    {
        return path_tracer_scene->create< raytracer::Diffuse_Material > (color);
    }

    Path_Tracing::Material * Path_Tracing::Model::add_metallic_material (const Color & color, float diffusion)
    {
        return path_tracer_scene->create< raytracer::Metallic_Material > (color, diffusion);
    }

    void Path_Tracing::Model::add_sphere (float radius, Material * material)
    {
        instance->add (path_tracer_scene->create< raytracer::Sphere > (Vector3{0.f,  0.f, -1.f}, radius, material));
    }

    void Path_Tracing::Model::add_plane (const Vector3 & normal, Material * material)
    {
        instance->add (path_tracer_scene->create< raytracer::Plane > (Vector3{0.f, .25f,  0.f}, normal, material));
    }

}
