/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include <engine/Control.hpp>
#include <engine/Key_Event.hpp>
#include <engine/Path_Tracing.hpp>
#include <engine/Starter.hpp>
#include <engine/Scene.hpp>
#include <engine/Window.hpp>
#include <engine/Entity.hpp>

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

    void load (Scene & scene)
    {
        load_camera (scene);
        load_ground (scene);
        load_shape  (scene);
    }

    void engine_application ()
    {
        Window window("Ray Tracing Engine", 1024, 600);

        Scene scene(window);

        load (scene);

        scene.run ();
    }

}

int main (int , char * [])
{
    engine::starter.run (engine_application);

    return 0;
}
