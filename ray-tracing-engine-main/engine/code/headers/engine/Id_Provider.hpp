/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#ifndef UDIT_ID_PROVIDER_HEADER
#define UDIT_ID_PROVIDER_HEADER

    #include <vector>
    #include <engine/Id.hpp>

    namespace udit::engine
    {

        class Id_Provider
        {

            static constexpr size_t pool_capacity =  32;        // Número de segmentos reservados de antemano
            static constexpr size_t segment_size  = 256;
            static constexpr size_t segment_shift =   8;
            static constexpr size_t segment_mask  = 255;

            struct Node
            {
                Node * next;
                Id     id;
            };

            class Segment
            {
                std::vector< Node > nodes;

            public:

                Segment(size_t segment_index) : nodes{segment_size}
                {
                    Id id = static_cast< Id >(segment_index * segment_size);

                    for (auto & node : nodes) node = { &node + 1, id++ };

                    nodes.back ().next = nullptr ;
                }

                Node & first_node ()
                {
                    return nodes.front ();
                }

                Node & operator [] (Id index)
                {
                    return nodes[index];
                }
            };

            class Pool
            {
                std::vector< Segment > segments;

            public:

                Pool()
                {
                    segments.reserve (pool_capacity);
                }

                Node * extend ()
                {
                    segments.emplace_back (segments.size ());
                    return &segments.back ().first_node ();
                }

                Node & operator [] (Id index)
                {
                    return segments[index >> segment_shift][index & segment_mask];
                }
            };

        private:

            Pool   pool;
            Node * first_node;

        public:

            Id_Provider()
            {
                first_node = nullptr;
            }

            Id allocate_id ()
            {
                if (not first_node) first_node = pool.extend ();

                Id id = first_node->id;

                first_node = first_node->next;

                return id;
            }

            void release (const Id id)
            {
                Node & node = pool[id];

                  node.next = first_node;

                 first_node = &node;
            }
        };

    }

#endif
