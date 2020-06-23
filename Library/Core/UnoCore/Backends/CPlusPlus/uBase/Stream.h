#pragma once
#include <stdint.h>
#include <uBase/Object.h>
#include <uBase/String.h>
#include <uBase/Buffer.h>
#include <uBase/Array.h>
#include <uBase/Memory.h>

namespace uBase
{
    /** \ingroup uIO */
    enum SeekOrigin
    {
        SeekOriginBegin,
        SeekOriginCurrent,
        SeekOriginEnd
    };

    /** \ingroup uIO */
    class Stream: public Object
    {
    public:
        virtual ~Stream();

        virtual void Flush();
        virtual void Close();

        virtual bool IsClosed() const;
        virtual bool AtEnd() const;

        virtual bool CanRead() const;
        virtual bool CanWrite() const;
        virtual bool CanSeek() const;

        virtual size_t GetPosition() const;
        virtual size_t GetLength() const;

        virtual size_t Read(void* dst, size_t elmSize, size_t elmCount);
        virtual void Write(const void* src, size_t elmSize, size_t elmCount);
        virtual void Seek(ptrdiff_t offset, SeekOrigin origin);

        void ReadSafe(void* dst, size_t elmSize, size_t elmCount);

        /** Write the entire contents of another stream into this stream.
            @param source Stream to read from. */
        void WriteStream(Stream* source);

        virtual DataAccessor* CreateDataAccessor()
        {
            Buffer* buf = Buffer::Create(GetLength());
            ReadSafe(buf->Ptr(), 1, buf->Size());
            return buf;
        }
    };

    /** \ingroup uIO */
    class StreamWriter: public Object
    {
    protected:
        Stream* _stream;

    public:
        StreamWriter(Stream* stream);
        virtual ~StreamWriter();

        void SetStream(Stream* stream);
        Stream* GetStream();
    };

    /** \ingroup uIO */
    class StreamReader: public Object
    {
    protected:
        Stream* _stream;

    public:
        StreamReader(Stream* stream);
        virtual ~StreamReader();

        void SetStream(Stream* stream);
        Stream* GetStream();
    };

    /** \ingroup uIO */
    class U_DLLEXPORT IOException: public Exception {};
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, StreamClosedException, "Stream has closed");
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, StreamCantReadException, "Stream not readable");
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, StreamCantWriteException, "Stream not writeable");
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, StreamCantSeekException, "Stream not seekable");
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, EndOfStreamException, "End of Stream reached");
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, FileCantOpenException, "Couldn't open file");
    /** \ingroup uIO */
    UBASE_DECLARE_EXCEPTION2(IOException, FileNotFoundException, "File not found");
}
