#include <uBase/ArrayStream.h>
#include <cstring>

namespace uBase
{
    ArrayStream::ArrayStream(size_t initCapacity)
    {
        _buf.Reserve(initCapacity);
    }

    bool ArrayStream::CanWrite() const
    {
        return true;
    }

    void ArrayStream::Write(const void* src, size_t elmSize, size_t elmCount)
    {
        _buf.Expand();
        size_t loc = _buf.Length();
        _buf.Resize(loc + elmSize * elmCount);
        memcpy(_buf.Ptr() + loc, src, elmSize * elmCount);
    }

    size_t ArrayStream::GetLength() const
    {
        return _buf.Length();
    }

    const uint8_t* ArrayStream::GetPtr() const
    {
        return _buf.Ptr();
    }

    Buffer* ArrayStream::CopyBuffer() const
    {
        return Buffer::CopyFrom(_buf.Ptr(), _buf.Length());
    }

    void ArrayStream::Clear()
    {
        _buf.Clear();
    }
}
