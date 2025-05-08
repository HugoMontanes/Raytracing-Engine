/*
 * Copyright © 2025+ ÁRgB (angel.rodriguez@udit.es)
 *
 * Distributed under the Boost Software License, version 1.0
 * See ./LICENSE or www.boost.org/LICENSE_1_0.txt
 */

#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace udit::engine
{

    template< typename OBJECT_TYPE >
    class Object_Pool
    {
    public:

        using Object_Type = OBJECT_TYPE;
        using index_t     = unsigned int;

        class Pointer
        {
            friend class  Object_Pool;

            Object_Type * object;
            index_t       index;

        public:

            Pointer() : Pointer(nullptr)
            {
            }

            Pointer(const nullptr_t )
            {
                reset ();
            }

        private:

            Pointer(Object_Type * given_object, index_t given_index)
            {
                object = given_object;
                index  = given_index;
            }

        public:

            Object_Type * get ()
            {
                return object;
            }

            const Object_Type * get () const
            {
                return object;
            }

            void reset ()
            {
                object = nullptr;
                index  = 0;
            }

        public:

            Pointer & operator = (const nullptr_t )
            {
                reset ();
            }

            Object_Type & operator * ()
            {
                return *object;
            }

            Object_Type * operator -> ()
            {
                return  object;
            }

            const Object_Type & operator * () const
            {
                return *object;
            }

            const Object_Type * operator -> () const
            {
                return  object;
            }

            bool operator == (const Pointer & other) const
            {
                return this->object == other.object;
            }
            bool operator != (const Pointer & other) const { return this->object != other.object; }
            bool operator == (const std::nullptr_t ) const { return this->object == nullptr;      }
            bool operator != (const std::nullptr_t ) const { return this->object != nullptr;      }

            explicit operator bool () const
            {
                return object != nullptr;
            }
        };

        class Iterator
        {
        };

    private:

        static constexpr size_t pool_capacity    =   1;
        static constexpr size_t segment_size     =  64;
        static constexpr size_t segment_shift    =   6;
        static constexpr size_t segment_mask     =  63;
        static constexpr size_t object_size      =  sizeof(Object_Type);
        static constexpr size_t object_alignment = alignof(Object_Type) >= sizeof(size_t) ? alignof(Object_Type) : sizeof(size_t);
        static constexpr size_t object_padding   = (object_size / object_alignment) * (object_alignment + (object_size % object_alignment > 0));

        template< size_t OBJECT_SIZE, size_t OBJECT_PADDING >
        struct Padded_Object
        {
            public:  std::byte data   [OBJECT_SIZE   ];
            private: std::byte padding[OBJECT_PADDING];
        };

        template< size_t OBJECT_SIZE >
        struct Padded_Object< OBJECT_SIZE, 0 >
        {
            std::byte data[OBJECT_SIZE];
        };

        using Element = Padded_Object< object_size, object_padding >;

        class Segment
        {
            using  Container = std::array< Element, segment_size >;

            static constexpr uint64_t full   =  0;
            static constexpr uint64_t empty  = ~full;
            static constexpr uint64_t l_one  =  1ull << (segment_size - 1);
            static constexpr uint64_t l_zero = ~l_one;

            uint64_t bitmap;

            std::unique_ptr< Container > elements;

        public:

            Segment() : elements{std::make_unique< Container > ()}
            {
                bitmap = empty;
            }

            bool is_full () const
            {
                return bitmap == full;
            }

            index_t allocate ()
            {
                auto index = static_cast< index_t >(std::countl_zero (bitmap));

                bitmap &= ~(l_one >> index);

                return index;
            }

            void free (index_t index)
            {
                bitmap |= l_one >> index;
            }

            Element & operator [] (unsigned int index)
            {
                return (*elements)[index];
            }
        };

        class Pool
        {
            std::vector< Segment > segments;

            size_t first_free_segment;

        public:

            Pool() : first_free_segment{0}
            {
                segments.reserve (pool_capacity);
                segments.resize  (1);
            }

            index_t allocate ()
            {
                index_t allocated_index = segments[first_free_segment].allocate () + static_cast< index_t >(first_free_segment * segment_size);

                while (segments[first_free_segment].is_full ())
                {
                    if (++first_free_segment == segments.size ())
                    {
                        segments.emplace_back ();
                        break;
                    }
                }

                return allocated_index;
            }

            void free (const index_t index)
            {
                index_t segment_index = index >> segment_shift;

                segments[segment_index].free (index & segment_mask);

                if (segment_index < first_free_segment)
                {
                    first_free_segment = segment_index;
                }
            }

            Element & operator [] (index_t index)
            {
                return segments[index >> segment_shift][index & segment_mask];
            }
        };

    private:

        Pool pool;

    public:

        Object_Pool()
        {
        }

        Iterator begin ()
        {
            return {};
        }

        Iterator end ()
        {
            return {};
        }

    public:

        template< typename ...ARGUMENTS >
        Pointer allocate (const ARGUMENTS & ...arguments)
        {
            auto index = pool.allocate ();

            return
            {
                new (pool[index].data) Object_Type(arguments...),
                index
            };
        }

        void free (Pointer & pointer)
        {
            if (pointer)
            {
                pointer.object->~Object_Type ();

                pool.free (pointer.index);
            }
        }

    };

}
