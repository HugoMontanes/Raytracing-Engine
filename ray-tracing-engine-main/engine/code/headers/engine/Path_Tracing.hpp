/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <memory>

#include <engine/Entity.hpp>
#include <engine/Stage.hpp>
#include <engine/Subsystem.hpp>
#include <engine/Transform.hpp>

#include <raytracer/Camera.hpp>
#include <raytracer/Diffuse_Material.hpp>
#include <raytracer/Linear_Space.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/Metallic_Material.hpp>
#include <raytracer/Model.hpp>
#include <raytracer/Path_Tracer.hpp>
#include <raytracer/Scene.hpp>
#include <raytracer/Sky_Environment.hpp>

namespace udit::engine
{

    class Path_Tracing : public Subsystem
    {
    public:

        class Stage : public engine::Stage
        {
            Path_Tracing * subsystem;

        public:

            Stage(Scene & scene) : engine::Stage(scene)
            {
                subsystem = nullptr;
            }

            void prepare ()      override;
            void compute (float) override;

        private:

            void update_component_transforms ();
        };

        friend class Stage;

    public:

        using Color    = raytracer::Color;
        using Material = raytracer::Material;

        struct Camera : public Component
        {
            using Sensor_Type = raytracer::Camera::Sensor_Type;

            raytracer::Camera * instance = nullptr;
        };

        struct Model : public Component
        {
            raytracer::Model * instance = nullptr;
            raytracer::Scene * path_tracer_scene = nullptr;

            Material * add_diffuse_material  (const Color & color);
            Material * add_metallic_material (const Color & color, float diffusion);
            void       add_sphere            (float radius, Material * material);
            void       add_plane             (const Vector3 & normal, Material * material);
        };

    private:

        Component_Store< Camera > camera_components;
        Component_Store< Model  >  model_components;

        raytracer::Path_Tracer    path_tracer;
        raytracer::Scene          path_tracer_scene;
        raytracer::Linear_Space   path_tracer_space;

        unsigned int              rays_per_pixel;

    public:

        Path_Tracing(Scene & scene);

    public:

        void set_rays_per_pixel (unsigned new_rays_per_pixel)
        {
            rays_per_pixel = new_rays_per_pixel;
        }

    public:

        Component * create_camera_component (Entity & entity, Camera::Sensor_Type sensor_type, float focal_length);
        Component * create_model_component  (Entity & entity);

    };

}
