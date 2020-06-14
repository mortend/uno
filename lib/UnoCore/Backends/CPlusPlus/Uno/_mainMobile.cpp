// @(MSG_ORIGIN)
// @(MSG_EDIT_WARNING)

#include <Uno/ObjectModel.h>

@(TypeObjects.Declaration:joinSorted())
void uInitRtti(uType*(*factories[])());

void uInitRtti()
{
    static uType*(*factories[])() =
    {
        @(TypeObjects.FunctionPointer:joinSorted('\n        ', '', ','))
        nullptr
    };

    uInitRtti(factories);
}

@(Main.Include:joinSorted('\n', '#include <', '>'))

void uStartApp()
{
    @(Main.Body:Indent(' ', 4))
}
