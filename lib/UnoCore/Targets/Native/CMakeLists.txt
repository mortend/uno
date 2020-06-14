cmake_minimum_required(VERSION 3.6)
project(@(Project.Name:quoteSpace))
include_directories(@(HeaderDirectory)@(IncludeDirectory:quoteSpace:join('', ' ')))
link_directories(@(LinkDirectory:quoteSpace:join(' ')))
add_definitions(@(PreprocessorDefinition:join(' ', '-D')))

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))

if (UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=@(CStandard)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=@(CppStandard)")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -Wno-unused-value -Wno-unused-variable -Wno-dangling-else -Wno-switch")
endif()

if (APPLE)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fobjc-arc")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}@(Xcode.Framework:join('', ' -Wl,-framework,')) -w")
    set(CMAKE_MACOSX_RPATH 1)
elseif (MSVC)
    add_definitions(-DUNICODE -D_UNICODE -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /MP /wd4065 /wd4101 /wd4390 /wd4674")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build Configurations (forced)" FORCE)
    set(EXTRA_FILES "natvis/uno.natstepfilter" "natvis/uno.natvis"@('!LIBRARY':Defined:test(' "rc/icon.rc"', '')))
elseif (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group")
endif()

@(CMake.SourceGroups)

#if @(LIBRARY:defined)
add_library(@(Project.Name:quoteSpace) SHARED
#else
add_executable(@(Project.Name:quoteSpace)@(WIN32:Defined:test(' WIN32', ''))
#endif
    @(HeaderFile:joinSorted('\n    ', '"@(HeaderDirectory)/', '"'))
    @(SourceFile:joinSorted('\n    ', '"@(SourceDirectory)/', '"'))
    ${EXTRA_FILES})

target_link_libraries(@(Project.Name:quoteSpace) @(LinkLibrary:join(' '))
    @(LinkLibrary.Debug:join(' ', 'debug '))
    @(LinkLibrary.Release:join(' ', 'optimized ')))

@(SharedLibrary:join('\n', 'file(COPY "', '" DESTINATION ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))'))
#if @(X64:defined)
@(SharedLibrary.x64:join('\n', 'file(COPY "', '" DESTINATION ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))'))
#elif @(X86:defined)
@(SharedLibrary.x86:join('\n', 'file(COPY "', '" DESTINATION ${PROJECT_SOURCE_DIR}/@(BinaryDirectory))'))
#endif

if (MSVC)
    set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT @(Project.Name:quoteSpace))
endif()
