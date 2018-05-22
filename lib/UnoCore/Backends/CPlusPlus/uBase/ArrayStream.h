#pragma once
#include <uBase/Stream.h>

namespace uBase
{
    /** \ingroup uIO */
    class ArrayStream: public Stream
    {
    protected:
        Array<uint8_t> _buf;

    public:
        ArrayStream(size_t initCapacity = 1024);

        virtual bool CanWrite() const;
        virtual void Write(const void* src, size_t elmSize, size_t elmCount);

        virtual size_t GetLength() const;

        const uint8_t* GetPtr() const;
        Buffer* CopyBuffer() const;

        void Clear();
    };
}
