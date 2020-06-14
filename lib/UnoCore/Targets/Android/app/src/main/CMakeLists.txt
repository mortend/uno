cmake_minimum_required(VERSION 3.4.1)
project(@(Project.Name:quoteSpace))

include(AndroidNdkModules)
include_directories(include@(IncludeDirectory:quoteSpace:join('', ' ')))
link_directories(@(LinkDirectory:quoteSpace:join(' ')))
add_definitions(@(PreprocessorDefinition:join(' ', '-D')))

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=@(CStandard)")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=@(CppStandard) -Wno-unused-value -Wno-dangling-else -Wno-switch -Wno-invalid-offsetof -fexceptions -frtti")

# Strip release builds to reduce file size.
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
endif()

# Using response file as workaround for long paths on Windows.
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_C_RESPONSE_FILE_LINK_FLAG "@")
set(CMAKE_CXX_RESPONSE_FILE_LINK_FLAG "@")
set(CMAKE_NINJA_FORCE_RESPONSE_FILE 1 CACHE INTERNAL "")

# Find libraries extracted from AARs, if applicable.
#if @(Gradle.Dependency.NativeImplementation:IsRequired)
link_directories("@(OutputDirectory:path)/app/build/native/jni/${ANDROID_ABI}")
#endif

add_library(@(Activity.Name) SHARED
    @(HeaderFile:joinSorted('\n    ', '"include/', '"'))
    @(SourceFile:joinSorted('\n    ', '"jni/', '"')))

target_link_libraries(@(Activity.Name)
    @(LinkLibrary:join('\n    ', '"', '"'))
    @(SharedLibrary:join('\n    ', '"', '"'))
    @(LinkOrderedStaticLibraries:join('\n    ', '"', '"')))

@(SharedLibrary:quoteSpace:join('\n', 'file(COPY ', ' DESTINATION "@(OutputDirectory:path)/app/src/main/jniLibs/${ANDROID_ABI}")'))
