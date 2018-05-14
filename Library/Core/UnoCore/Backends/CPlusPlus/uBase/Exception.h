#pragma once
#include <uBase/Config.h>
#include <uBase/String.h>
#include <exception>

namespace uBase
{
    /** \ingroup uException */
    class U_DLLEXPORT Exception : public std::exception
    {
    public:
        String _message;
        Exception() {}
        Exception(const String& message): _message(message) {}
        virtual ~Exception() throw() {}
        virtual const char* what() const throw() { return _message.Ptr(); }
        const String& GetMessage() const throw() { return _message; }
    };

    /** \addtogroup uException */
    /** @{ */
#define UBASE_DECLARE_EXCEPTION2(BASE, NAME, MSG) \
    class U_DLLEXPORT NAME: public BASE \
    { \
    public: \
        NAME(const uBase::String& msg) { _message = uBase::String(MSG) + ": " + msg; } \
        NAME() { _message = MSG; } \
    }
#define UBASE_DECLARE_EXCEPTION(NAME, MSG) \
    UBASE_DECLARE_EXCEPTION2(uBase::Exception, NAME, MSG)
    /** @} */

    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(OverflowException, "Overflow");
    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(IndexOutOfRangeException, "Index out of range");
    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(NullPointerException, "Null pointer access attempt");
    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(BadDeleteException, "Attemt to delete invalid object");
    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(FormatException, "Invalid format");
    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(ArgumentException, "Illegal argument");
    /** \ingroup uException */
    UBASE_DECLARE_EXCEPTION(NotSupportedException, "Not supported");
}
