#include <uBase/Buffer.h>
#include <cstdlib>
#include <cstring>

namespace uBase
{
    Buffer::Buffer(size_t size)
    {
        _size = size;
        _data = ((uint8_t*)this) + sizeof(Buffer);
    }

    void* Buffer::operator new (size_t size, size_t bufSize)
    {
        return malloc(sizeof(Buffer) + bufSize);
    }

    void Buffer::operator delete (void* buf, size_t bufSize)
    {
        free(buf);
    }

    void Buffer::operator delete (void* buf)
    {
        free(buf);
    }

    Buffer* Buffer::Create(size_t size)
    {
        return new (size) Buffer(size);
    }

    void Buffer::Delete()
    {
        this->~Buffer();
        free(this);
    }

    Buffer* Buffer::CopyFrom(const void* data, size_t size)
    {
        Buffer* b = Buffer::Create(size);
        memcpy(b->Ptr(), data, size);
        return b;
    }

    size_t Buffer::Size() const
    {
        return _size;
    }

    uint8_t* Buffer::Ptr()
    {
        return _data;
    }

    const uint8_t* Buffer::Ptr() const
    {
        return _data;
    }

    const uint8_t* Buffer::GetPtr() const
    {
        return _data;
    }

    size_t Buffer::GetSizeInBytes() const
    {
        return _size;
    }

    BufferPtr::BufferPtr(void* data, size_t size, bool ownsData)
        : _data((uint8_t*)data), _size(size), _ownsData(ownsData)
    {
    }

    BufferPtr::~BufferPtr()
    {
        if (_ownsData)
            delete [] (uint8_t*)_data;
    }

    size_t BufferPtr::Size() const
    {
        return _size;
    }

    uint8_t* BufferPtr::Ptr()
    {
        return _data;
    }

    const uint8_t* BufferPtr::Ptr() const
    {
        return _data;
    }

    const uint8_t* BufferPtr::GetPtr() const
    {
        return _data;
    }

    size_t BufferPtr::GetSizeInBytes() const
    {
        return _size;
    }

    BufferRef::BufferRef(void* data, size_t size, Object* owner)
        : _data((uint8_t*)data), _size(size), _owner(owner)
    {
        if (owner)
            owner->Retain();
    }

    BufferRef::~BufferRef()
    {
        if (_owner)
            _owner->Release();
    }

    const uint8_t* BufferRef::GetPtr() const
    {
        return _data;
    }

    size_t BufferRef::GetSizeInBytes() const
    {
        return _size;
    }
}
