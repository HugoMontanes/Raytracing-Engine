/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <iostream>

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

    void load_camera (Scene & scene)
    {
        auto & entity = scene.create_entity ();

        scene.create_component< Transform > (entity);
        scene.create_component< Path_Tracing::Camera > (entity, Path_Tracing::Camera::Sensor_Type::APS_C, 16.f / 1000.f);

        std::shared_ptr< Controller > camera_controller = std::make_shared< Camera_Controller > (scene, entity.id);

        scene.create_component< Control::Component > (entity, camera_controller);
    }

    void load_ground (Scene & scene)
    {
        auto & entity = scene.create_entity ();

        scene.create_component< Transform > (entity);

        auto model_component = scene.create_component< Path_Tracing::Model > (entity);

        model_component->add_plane (Vector3{0, -1, 0}, model_component->add_diffuse_material (Path_Tracing::Color(.4f, .4f, .5f)));
    }

    void load_shape (Scene & scene)
    {
        auto & entity = scene.create_entity ();

        scene.create_component< Transform > (entity);

        auto model_component = scene.create_component< Path_Tracing::Model > (entity);

        model_component->add_sphere (.25f, model_component->add_diffuse_material (Path_Tracing::Color(.8f, .8f, .8f)));
    }

    void load_concurrently (Scene & scene)
    {
        //Get loading thread pool
        auto& loading_pool = Thread_Pool_Manager::get_instance().get_pool(Thread_Pool_Type::LOADING);

        //Create a vector to store futures
        std::vector<std::future<void>> loading_futures;

        //Load camera task
        loading_futures.push_back(
            loading_pool.submit([&scene]()
                {
                    std::cout << "Loading camera on thread: " << std::this_thread::get_id() << std::endl;
                    load_camera(scene);
                    std::cout << "Camera loaded" << std::endl;
                })
        );

        //Load ground task
        loading_futures.push_back(
            loading_pool.submit([&scene]() {
                std::cout << "Loading ground on thread: " << std::this_thread::get_id() << std::endl;
                load_ground(scene);
                std::cout << "Ground loaded" << std::endl;
                })
        );

        // Load shape task
        loading_futures.push_back(
            loading_pool.submit([&scene]() {
                std::cout << "Loading shape on thread: " << std::this_thread::get_id() << std::endl;
                load_shape(scene);
                std::cout << "Shape loaded" << std::endl;
                })
        );

        //Wait for all loading tasks to complete
        std::cout << "Waiting for all scene components to load..." << std::endl;

        for (auto& future : loading_futures)
        {
            future.wait(); //Block until each task is complete
        }

        std::cout << "All scene components loaded successfully" << std::endl;
    }

    void engine_application ()
    {
        Window window("Ray Tracing Engine", 1024, 600);

        Scene scene(window);

        load_concurrently (scene);

        scene.run ();
    }

}
int main (int , char * [])
{
    engine::starter.run (engine_application);

    return 0;
}
