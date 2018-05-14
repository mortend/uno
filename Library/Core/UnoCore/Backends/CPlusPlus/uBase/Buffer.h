#pragma once
#include <stdint.h>
#include <uBase/Object.h>
#include <cstddef>
#include <new>

namespace uBase
{
    /** \ingroup uBuffer */
    class DataAccessor: public Object
    {
    public:
        virtual const uint8_t* GetPtr() const = 0;
        virtual size_t GetSizeInBytes() const = 0;
    };

    /** \ingroup uBuffer
        A buffer of bytes.
        A buffer can not be resized or relocated, so other objects can depend on the pointer to the buffer data. */
    class Buffer: public DataAccessor
    {
        size_t _size;
        uint8_t* _data;

        Buffer(size_t size);
        void* operator new (size_t size, size_t bufSize);
        void operator delete (void* buf, size_t bufSize);
        void operator delete (void* buf);

    public:
        static Buffer* Create(size_t size);
        static Buffer* CopyFrom(const void* data, size_t size);

        virtual void Delete();

        /** Returns the size of the buffer, in bytes.
            @return The size of the buffer, in bytes. */
        size_t Size() const;

        /** Allows the buffer to be used as pointer.
            This pointer will never change or become invalid during the lifetime of the buffer.
            @returns A pointer to the data in the buffer. */
        uint8_t* Ptr();
        const uint8_t* Ptr() const;

        virtual const uint8_t* GetPtr() const;
        virtual size_t GetSizeInBytes() const;
    };

    /** \ingroup uBuffer */
    class BufferPtr: public DataAccessor
    {
        uint8_t* _data;
        size_t _size;
        bool _ownsData;

    public:
        BufferPtr(void* data, size_t size, bool ownsData);
        virtual ~BufferPtr();

        size_t Size() const;

        uint8_t* Ptr();
        const uint8_t* Ptr() const;

        virtual const uint8_t* GetPtr() const;
        virtual size_t GetSizeInBytes() const;
    };

    /** \ingroup uBuffer */
    class BufferRef: public DataAccessor
    {
        uint8_t* _data;
        size_t _size;
        Object* _owner;

    public:
        BufferRef(void* data, size_t size, Object* owner);
        virtual ~BufferRef();

        virtual const uint8_t* GetPtr() const;
        virtual size_t GetSizeInBytes() const;
    };
}
