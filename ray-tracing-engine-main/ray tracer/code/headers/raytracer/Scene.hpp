/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include <raytracer/declarations.hpp>
#include <raytracer/Memory_Pool.hpp>

namespace udit::raytracer
{

    class Scene
    {
        using Camera_Ptr          = std::unique_ptr< Camera          >;
        using Material_Ptr        = std::unique_ptr< Material        >;
        using Material_List       = std::vector    < Material_Ptr    >;
        using Model_Ptr           = std::unique_ptr< Model           >;
        using Model_List          = std::vector    < Model_Ptr       >;
        using Sky_Environment_Ptr = std::unique_ptr< Sky_Environment >;

    public:

        template< typename ITERATOR >
        class Iterator_Template
        {
            ITERATOR iterator;

        public:

            Iterator_Template(const ITERATOR & given_iterator) : iterator(given_iterator)
            {
            }

        public:

            Iterator_Template & operator ++ ()
            {
                ++iterator;
                return *this;
            }

        public:

                  Model & operator *  ()       { return  **iterator; }
            const Model & operator *  () const { return  **iterator; }
                  Model * operator -> ()       { return &**iterator; }
            const Model * operator -> () const { return &**iterator; }

        public:

            bool operator == (const Iterator_Template & other) const { return this->iterator == other.iterator; }
            bool operator != (const Iterator_Template & other) const { return this->iterator != other.iterator; }
        };

        using       Iterator = Iterator_Template< Model_List::      iterator >;
        using Const_Iterator = Iterator_Template< Model_List::const_iterator >;

    private:

        Camera_Ptr          camera;
        Memory_Pool         intersectable_pool;
        Material_List       materials;
        Model_List          models;
        Sky_Environment_Ptr sky_environment;

        unsigned hash;                              // Permitiría saber si se han añadido y/o quitado elementos
                                                    // Podría hacerse sumando un ID autonumérico al añadir y restándolo al quitar
    public:

        Camera * get_camera ()
        {
            return camera.get ();
        }

        const Camera * get_camera () const
        {
            return camera.get ();
        }

        Sky_Environment * get_sky_environment ()
        {
            return sky_environment.get ();
        }

        const Sky_Environment * get_sky_environment () const
        {
            return sky_environment.get ();
        }

        Iterator begin ()
        {
            return models.begin ();
        }

        Iterator end ()
        {
            return models.end ();
        }

        Const_Iterator begin () const
        {
            return models.begin ();
        }

        Const_Iterator end () const
        {
            return models.end ();
        }

    public:

        template< class CLASS, typename ...ARGUMENTS >
        CLASS * create (const ARGUMENTS & ...arguments);

    };


    template< class CLASS, typename ...ARGUMENTS >
    inline CLASS * Scene::create (const ARGUMENTS & ...arguments)
    {
        if constexpr (std::is_base_of< Camera, CLASS >::value)
        {
            camera = std::make_unique< CLASS > (arguments...);

            return static_cast< CLASS * >(camera.get ());
        }
        else
        if constexpr (std::is_base_of< Intersectable, CLASS >::value)
        {
            return intersectable_pool.allocate< CLASS > (arguments...);
        }
        else
        if constexpr (std::is_base_of< Material, CLASS >::value)
        {
            materials.emplace_back (std::make_unique< CLASS > (arguments...));

            return static_cast< CLASS * >(materials.back ().get ());
        }
        else
        if constexpr (std::is_base_of< Model, CLASS >::value)
        {
            models.emplace_back (std::make_unique< CLASS > (arguments...));

            return static_cast< CLASS * >(models.back ().get ());
        }
        else
        if constexpr (std::is_base_of< Sky_Environment, CLASS >::value)
        {
            sky_environment = std::make_unique< CLASS > (arguments...);

            return static_cast< CLASS * >(sky_environment.get ());
        }
    }

}
