#!/bin/sh
# @(MSG_ORIGIN)
# @(MSG_EDIT_WARNING)

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
    DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    [[ "$SOURCE" != /* ]] && SOURCE="$DIR/$SOURCE"
done

#if @(LIBRARY:defined) || @(PREVIEW:defined)
echo "ERROR: @(Product) is a library and cannot be run directly." >&2
exit 1
#elif @(CONSOLE:defined) || @(TEST:defined) && !@(APPTEST:defined) || !@(HOST_MAC:defined)
exec dotnet "@(Product)" "$@"
#else
exec "@(Product)" "$@"
#endif
