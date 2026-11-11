cmake_minimum_required(VERSION 3.26)

project(soft_ray_tracer LANGUAGES CXX)

# ThirdParty/lib contains prebuilt Windows MSVC x64 libraries.
if(NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Select an MSVC x64 kit (ThirdParty libraries require Windows MSVC x64).")
endif()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use a separate build directory, for example: cmake -S . -B out/build/cmake")
endif()

get_property(BDT_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT BDT_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build configuration" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
if(CMAKE_GENERATOR MATCHES "Ninja")
    # Preserve the compiler's actual output bytes for Ninja's header tracking.
    # CMake's automatic code-page conversion can corrupt Korean /showIncludes.
    set(probe_dir "${CMAKE_BINARY_DIR}/CMakeFiles/bdt_showincludes")
    file(MAKE_DIRECTORY "${probe_dir}")
    file(WRITE "${probe_dir}/probe.h" "\n")
    file(WRITE "${probe_dir}/probe.cpp" "#include \"probe.h\"\n")
    execute_process(
        COMMAND "${CMAKE_CXX_COMPILER}" /nologo /showIncludes /c probe.cpp
        WORKING_DIRECTORY "${probe_dir}"
        OUTPUT_VARIABLE probe_output
        ERROR_VARIABLE probe_error
        RESULT_VARIABLE probe_result
        ENCODING NONE
    )
    if(probe_result EQUAL 0 AND probe_output MATCHES "(^|\n)([^:\n][^:\n]+:[^:\n]*[^: \n][^: \n]:?[ \t]+)([A-Za-z]:\\\\|\\./|\\.\\\\|/)")
        set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_MATCH_2}")
    else()
        message(FATAL_ERROR "Could not detect MSVC header dependency output: ${probe_error}")
    endif()
endif()

set(BDT_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty")
if(NOT IS_DIRECTORY "${BDT_THIRDPARTY_DIR}/include" OR
   NOT IS_DIRECTORY "${BDT_THIRDPARTY_DIR}/lib")
    message(FATAL_ERROR "Place ThirdParty/include and ThirdParty/lib in the repository root. See README.md.")
endif()

add_library(bdt_options INTERFACE)
target_include_directories(bdt_options INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_options(bdt_options INTERFACE /utf-8 /W3 /sdl /permissive-
    "$<$<NOT:$<CONFIG:Debug>>:/Gy;/Oi>"
)
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    target_compile_options(bdt_options INTERFACE /MP)
endif()
target_compile_definitions(bdt_options INTERFACE
    UNICODE _UNICODE $<IF:$<CONFIG:Debug>,_DEBUG,NDEBUG>
)

# Match Debug libraries to Debug, and Release libraries to all other configurations.
set(BDT_LIBRARY_DIR "${BDT_THIRDPARTY_DIR}/lib/$<IF:$<CONFIG:Debug>,Debug,Release>")
add_library(bdt_thirdparty INTERFACE)
target_include_directories(bdt_thirdparty INTERFACE "${BDT_THIRDPARTY_DIR}/include")
target_compile_definitions(bdt_thirdparty INTERFACE KHRONOS_STATIC)
target_link_libraries(bdt_thirdparty INTERFACE
    "${BDT_LIBRARY_DIR}/glfw3.lib"
    "${BDT_LIBRARY_DIR}/glad.lib"
    "${BDT_LIBRARY_DIR}/imgui.lib"
    "${BDT_LIBRARY_DIR}/$<IF:$<CONFIG:Debug>,assimp-vc145-mtd.lib,assimp-vc145-mt.lib>"
    "${BDT_LIBRARY_DIR}/$<IF:$<CONFIG:Debug>,zlibstaticd.lib,zlibstatic.lib>"
    "${BDT_LIBRARY_DIR}/ktx.lib"
    "${BDT_LIBRARY_DIR}/astcenc-avx2-static.lib"
    opengl32 user32 gdi32 shell32
)

# Only scan source module directories, excluding generated files and ThirdParty.
# CONFIGURE_DEPENDS detects additions/removals on the next build.
function(bdt_collect_sources output directory)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/*.inl"
    )
    list(FILTER sources EXCLUDE REGEX "/pch\\.cpp$")
    set(${output} ${sources} PARENT_SCOPE)
endfunction()

function(bdt_add_module name)
    bdt_collect_sources(sources "${name}")
    add_library(${name} STATIC ${sources})
    target_compile_definitions(${name} PRIVATE _LIB)
    target_precompile_headers(${name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/${name}/pch.h")
    target_link_libraries(${name} PUBLIC bdt_options bdt_thirdparty ${ARGN})
endfunction()

# All module targets and their dependencies are defined here; no subdirectory CMakeLists.
bdt_add_module(Core)
bdt_add_module(Resources Core)
bdt_add_module(Rendering Resources)
bdt_add_module(RayTracing Rendering)
bdt_add_module(Editor Rendering)
target_include_directories(Editor PUBLIC "${BDT_THIRDPARTY_DIR}/include/imgui")
bdt_add_module(Pipelines RayTracing Rendering)
bdt_add_module(Runtime RayTracing Rendering Editor)

bdt_collect_sources(app_sources soft_ray_tracer)
add_executable(soft_ray_tracer ${app_sources})
target_compile_definitions(soft_ray_tracer PRIVATE _CONSOLE)
target_precompile_headers(soft_ray_tracer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/soft_ray_tracer/pch.h")
target_link_libraries(soft_ray_tracer PRIVATE Runtime Pipelines Editor)

# Run on every application build so shader/asset-only edits are copied too.
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
    add_custom_target(bdt_copy_assets
        COMMAND "${CMAKE_COMMAND}" -E copy_directory_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
            "$<TARGET_FILE_DIR:soft_ray_tracer>/Assets"
        COMMENT "Syncing Assets next to soft_ray_tracer"
        VERBATIM
    )
    add_dependencies(soft_ray_tracer bdt_copy_assets)
else()
    message(WARNING "Place Assets in the repository root before running the application. See README.md.")
endif()

set_target_properties(soft_ray_tracer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:soft_ray_tracer>"
)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT soft_ray_tracer)
