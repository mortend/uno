#pragma once
#include <uBase/Stream.h>

namespace uBase
{
    /** \ingroup uIO */
    class BufferStream: public Stream
    {
        DataAccessor* _buf;
        bool _read, _write, _closed;
        ptrdiff_t _pos;

    public:
        BufferStream(DataAccessor* buf, bool canRead, bool canWrite);
        virtual ~BufferStream();

        virtual void Close();

        virtual bool IsClosed() const;
        virtual bool AtEnd() const;

        virtual bool CanRead() const;
        virtual bool CanWrite() const;
        virtual bool CanSeek() const;

        virtual size_t Read(void* dst, size_t elmSize, size_t elmCount);
        virtual void Write(const void* dst, size_t elmSize, size_t elmCount);
        virtual void Seek(ptrdiff_t offset, SeekOrigin origin);

        virtual size_t GetPosition() const;
        virtual size_t GetLength() const;

        virtual DataAccessor* CreateDataAccessor();
    };
}
