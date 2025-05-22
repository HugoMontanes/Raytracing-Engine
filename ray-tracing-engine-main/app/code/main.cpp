/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>
#include <thread>

#include <engine/Control.hpp>
#include <engine/Key_Event.hpp>
#include <engine/Path_Tracing.hpp>
#include <engine/Starter.hpp>
#include <engine/Scene.hpp>
#include <engine/Window.hpp>
#include <engine/Entity.hpp>
#include <engine/Thread_Pool_Manager.hpp>

#include "Camera_Controller.hpp"

using namespace std;
using namespace udit;
using namespace udit::engine;

namespace
{
    // Scene loading functions - these create the 3D content for ray tracing

    void load_camera(Scene& scene)
    {
        // Create a camera entity with transform and ray tracing components
        auto& entity = scene.create_entity();

        // Add transform component for position, rotation, and scaling
        scene.create_component< Transform >(entity);

        // Add ray tracing camera component with APS-C sensor and 16mm focal length
        scene.create_component< Path_Tracing::Camera >(
            entity,
            Path_Tracing::Camera::Sensor_Type::APS_C,
            16.f / 1000.f  // Convert millimeters to meters
        );

        // Add interactive camera controller for user input
        std::shared_ptr< Controller > camera_controller =
            std::make_shared< Camera_Controller >(scene, entity.id);

        scene.create_component< Control::Component >(entity, camera_controller);

        std::cout << "Camera loaded with interactive controls" << std::endl;
    }

    void load_ground(Scene& scene)
    {
        // Create a ground plane for the scene
        auto& entity = scene.create_entity();

        scene.create_component< Transform >(entity);

        auto model_component = scene.create_component< Path_Tracing::Model >(entity);

        // Create a diffuse material with a neutral gray color
        auto ground_material = model_component->add_diffuse_material(
            Path_Tracing::Color(.4f, .4f, .5f)  // Slightly blue-gray ground
        );

        // Add a horizontal plane pointing upward
        model_component->add_plane(Vector3{ 0, 1, 0 }, ground_material);

        std::cout << "Ground plane loaded" << std::endl;
    }

    void load_shape(Scene& scene)
    {
        // Create a sphere object in the scene
        auto& entity = scene.create_entity();

        scene.create_component< Transform >(entity);

        auto model_component = scene.create_component< Path_Tracing::Model >(entity);

        // Create a bright diffuse material for the sphere
        auto sphere_material = model_component->add_diffuse_material(
            Path_Tracing::Color(.8f, .8f, .8f)  // Light gray sphere
        );

        // Add a sphere with 25cm radius
        model_component->add_sphere(.25f, sphere_material);

        std::cout << "Sphere loaded" << std::endl;
    }

    void load_additional_objects(Scene& scene)
    {
        // Add more interesting objects to showcase the ray tracing

        // Metallic sphere
        {
            auto& entity = scene.create_entity();
            auto transform = scene.create_component< Transform >(entity);
            transform->position = Vector3(0.7f, 0.25f, -0.5f);  // Position to the right

            auto model_component = scene.create_component< Path_Tracing::Model >(entity);
            auto metallic_material = model_component->add_metallic_material(
                Path_Tracing::Color(.7f, .6f, .5f),  // Bronze-ish color
                0.1f  // Low diffusion for mirror-like reflections
            );
            model_component->add_sphere(.2f, metallic_material);

            std::cout << "Metallic sphere loaded" << std::endl;
        }

        // Another diffuse sphere
        {
            auto& entity = scene.create_entity();
            auto transform = scene.create_component< Transform >(entity);
            transform->position = Vector3(-0.7f, 0.15f, -0.3f);  // Position to the left

            auto model_component = scene.create_component< Path_Tracing::Model >(entity);
            auto colored_material = model_component->add_diffuse_material(
                Path_Tracing::Color(.2f, .8f, .3f)  // Bright green
            );
            model_component->add_sphere(.15f, colored_material);

            std::cout << "Colored sphere loaded" << std::endl;
        }
    }

    void load_concurrently(Scene& scene)
    {
        // Demonstrate concurrent loading using your thread pool system
        // This shows how multiple scene components can be loaded in parallel

        auto& loading_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::LOADING);

        std::vector<std::future<void>> loading_futures;

        // Load camera task
        loading_futures.push_back(
            loading_pool.submit([&scene]()
                {
                    std::cout << "Loading camera on thread: " << std::this_thread::get_id() << std::endl;
                    load_camera(scene);
                })
        );

        // Load ground task
        loading_futures.push_back(
            loading_pool.submit([&scene]() {
                std::cout << "Loading ground on thread: " << std::this_thread::get_id() << std::endl;
                load_ground(scene);
                })
        );

        // Load main shape task
        loading_futures.push_back(
            loading_pool.submit([&scene]() {
                std::cout << "Loading main shape on thread: " << std::this_thread::get_id() << std::endl;
                load_shape(scene);
                })
        );

        // Load additional objects task
        loading_futures.push_back(
            loading_pool.submit([&scene]() {
                std::cout << "Loading additional objects on thread: " << std::this_thread::get_id() << std::endl;
                load_additional_objects(scene);
                })
        );

        // Wait for all loading tasks to complete
        std::cout << "Waiting for all scene components to load..." << std::endl;

        for (auto& future : loading_futures)
        {
            future.wait();
        }

        std::cout << "All scene components loaded successfully" << std::endl;
    }

    void configure_continuous_rendering(Scene& scene)
    {
        // This is where we set up the continuous framebuffer update system

        auto path_tracing = scene.get_subsystem<Path_Tracing>();
        if (!path_tracing) {
            std::cerr << "Error: Path_Tracing subsystem not available" << std::endl;
            return;
        }

        // Configure ray tracing quality
        // More rays per pixel = higher quality but slower rendering
        path_tracing->set_rays_per_pixel(4);  // Start with 1 ray per pixel for fast updates

        // Enable continuous rendering with adaptive frame rate
        // The system will update the display at this rate regardless of ray tracing speed

        // Detect display refresh rate or use sensible defaults
        // You might want to query the actual display refresh rate here
        float target_fps = 60.0f;  // Target 60 FPS for smooth interaction

        std::cout << "Enabling continuous rendering at " << target_fps << " FPS" << std::endl;
        path_tracing->enable_continuous_rendering(target_fps);

        // Print configuration summary
        std::cout << "Ray tracing configuration:" << std::endl;
        std::cout << "  - Rays per pixel: " << path_tracing->get_rays_per_pixel() << std::endl;
        std::cout << "  - Display FPS: " << path_tracing->get_display_fps() << std::endl;
        std::cout << "  - Continuous rendering: " <<
            (path_tracing->is_continuous_rendering_enabled() ? "enabled" : "disabled") << std::endl;
    }

    void print_performance_info()
    {
        // Print information about the threading setup
        auto& thread_manager = Thread_Pool_Manager::get_instance();

        std::cout << "\nPerformance Information:" << std::endl;
        std::cout << "  - Hardware threads: " << std::thread::hardware_concurrency() << std::endl;
        std::cout << "  - Rendering threads: " <<
            thread_manager.get_pool(Thread_Pool_Type::RENDERING).get_thread_count() << std::endl;
        std::cout << "  - Loading threads: " <<
            thread_manager.get_pool(Thread_Pool_Type::LOADING).get_thread_count() << std::endl;
        std::cout << "  - Input threads: " <<
            thread_manager.get_pool(Thread_Pool_Type::INPUT).get_thread_count() << std::endl;
    }

    void print_usage_instructions()
    {
        std::cout << "\nUsage Instructions:" << std::endl;
        std::cout << "  - Use arrow keys to move the camera" << std::endl;
        std::cout << "  - The image will continuously improve in quality as more ray samples are computed" << std::endl;
        std::cout << "  - Display updates happen at a constant rate independent of ray computation speed" << std::endl;
        std::cout << "  - Close the window or press Ctrl+C to exit" << std::endl;
    }

    void engine_application()
    {
        // Create the main application window
        // The window size affects ray tracing performance - larger windows need more computation
        Window window("Ray Tracing Engine - Continuous Updates Demo", 1024, 600);

        // Create the main scene that will manage all rendering and updates
        Scene scene(window);

        std::cout << "Starting Ray Tracing Engine with Continuous Framebuffer Updates" << std::endl;

        // Load all scene content concurrently for faster startup
        load_concurrently(scene);

        // Configure the continuous rendering system
        configure_continuous_rendering(scene);

        // Print helpful information
        print_performance_info();
        print_usage_instructions();

        std::cout << "\nStarting main rendering loop..." << std::endl;

        // Run the main application loop
        // This will continue until the user closes the window
        // The continuous update system will keep the display smooth
        scene.run();

        std::cout << "Application shutting down..." << std::endl;

        // Cleanup happens automatically when objects go out of scope
        // The continuous rendering system will shut down gracefully
    }
}

int main(int, char* [])
{
    // Start the engine with our application function
    // This initializes SDL, thread pools, and other engine subsystems
    engine::starter.run(engine_application);

    return 0;
}