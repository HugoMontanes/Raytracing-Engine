/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#include "Camera_Controller.hpp"
#include <engine/Key_Event.hpp>
#include <engine/Scene.hpp>
#include <engine/Transform.hpp>

using namespace udit::engine;

namespace udit
{

    Camera_Controller::Camera_Controller(engine::Scene & scene, engine::Id entity_id) : Controller(scene, entity_id)
    {
        transform = scene.get_component< Transform > (entity_id);
        direction = Vector3(.0f, .0f, .0f);
        speed     = Vector3(.2f, .0f, .2f);
    }

    void Camera_Controller::update (float frame_time)
    {
        update_direction ();

        transform->position.x += direction.x * speed.x * frame_time;
        transform->position.z += direction.z * speed.z * frame_time;
    }

    void Camera_Controller::update_direction ()
    {
        auto & input_event_queue = scene.get_input_event_queue ();

        while (true)
        {
            auto key_event = input_event_queue.poll ()->as< Key_Event > ();

            if (not key_event)
            {
                break;
            }
            else
            {
                if (key_event->state == Key_Event::PRESSED)
                {
                    switch (key_event->code)
                    {
                        case KEY_LEFT:  direction.x = -1.f; break;
                        case KEY_RIGHT: direction.x = +1.f; break;
                        case KEY_UP:    direction.z = -1.f; break;
                        case KEY_DOWN:  direction.z = +1.f; break;
                        default:
                            break;
                    }
                }
                else
                if (key_event->state == Key_Event::RELEASED)
                {
                    switch (key_event->code)
                    {
                        case KEY_LEFT:  if (direction.x < 0.f) direction.x = 0.f; break;
                        case KEY_RIGHT: if (direction.x > 0.f) direction.x = 0.f; break;
                        case KEY_UP:    if (direction.z < 0.f) direction.z = 0.f; break;
                        case KEY_DOWN:  if (direction.z > 0.f) direction.z = 0.f; break;
                        default:
                            break;
                    }
                }
            }
        }
    }

}
