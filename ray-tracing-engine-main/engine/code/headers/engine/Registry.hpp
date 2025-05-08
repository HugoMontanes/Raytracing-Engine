/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <cassert>
#include <cstddef>
#include <memory>

#include <engine/Id.hpp>

namespace udit::engine
{

    template< class CLASS, size_t MAX_REGISTRY_SIZE >
    class Registry final
    {
    public:

        struct Metadata
        {
            using Factory = std::unique_ptr< CLASS > (*) (Scene & scene);
            using Name    = const char *;

            Name    name;
            Id      id;
            Id      hash;
            Factory factory;
        };

    private:

        Metadata entries[MAX_REGISTRY_SIZE];
        Id       counter;

    public:

        Registry() : entries{}, counter(0)
        {
        }

        size_t size () const
        {
            return counter;
        }

        Id add (Metadata::Name name, Metadata::Factory factory);

        const Metadata * metadata_by_id   (Id id);
        const Metadata * metadata_by_name (Id name_hash);

    };

    template< class CLASS, size_t MAX_REGISTRY_SIZE >
    Id Registry< CLASS, MAX_REGISTRY_SIZE >::add (Metadata::Name name, Metadata::Factory factory)
    {
        assert(counter < MAX_REGISTRY_SIZE);

        Id id = counter++;

        /*Metadata * metadata =*/ new (&entries[id]) Metadata{ name, id, 0 /*HASH of name*/, factory };

        for (Id index = 0, end = id; index < end; ++index)
        {
            //assert(metadata->hash != entries[index].hash);
        }

        return id;
    }

    template< class CLASS, size_t MAX_REGISTRY_SIZE >
    const typename Registry< CLASS, MAX_REGISTRY_SIZE >::Metadata * Registry< CLASS, MAX_REGISTRY_SIZE >::metadata_by_id (Id id)
    {
        return &entries[id];
    }

    template< class CLASS, size_t MAX_REGISTRY_SIZE >
    const typename Registry< CLASS, MAX_REGISTRY_SIZE >::Metadata * Registry< CLASS, MAX_REGISTRY_SIZE >::metadata_by_name (Id name_hash)
    {
        for (Id index = 0, end = counter; index < end; ++index)
        {
            if (entries[index].hash == name_hash) return &entries[index];
        }

        return nullptr;
    }

}
