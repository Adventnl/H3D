cmake_minimum_required(VERSION 3.24)

project(Forge3D
    VERSION 0.1.0
    DESCRIPTION "Forge3D - a from-scratch 3D digital content creation platform"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release RelWithDebInfo MinSizeRel)
endif()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(FORGE_BUILD_APP "Build the forge_app and forge_ui modules and the desktop shell" ON)
option(FORGE_BUILD_TESTS "Build the Forge3D unit tests" ON)
option(FORGE_BUILD_BENCHMARKS "Build the Forge3D benchmarks" ON)
option(FORGE_BUILD_TOOLS "Build the Forge3D developer tools" ON)
option(FORGE_BUILD_DOCS "Add a docs target when a documentation generator is available" OFF)
option(FORGE_ENABLE_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(FORGE_ENABLE_SANITIZERS "Enable address/undefined sanitizers (non-MSVC)" OFF)
option(FORGE_ENABLE_COVERAGE "Enable coverage instrumentation (GCC/Clang)" OFF)
option(FORGE_ENABLE_PROFILING "Enable the built-in profiler instrumentation" ON)
option(FORGE_ENABLE_TRACING "Enable Chrome-trace capture in the desktop shell" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ForgeCompilerOptions)
include(ForgeWarnings)
include(ForgeSanitizers)
include(ForgeCoverage)
include(ForgeTesting)
include(ForgeInstall)

find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# Git commit (best effort; "unknown" when git or the repo is unavailable)
# ---------------------------------------------------------------------------
set(FORGE_GIT_COMMIT "unknown")
find_package(Git QUIET)
if(Git_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE FORGE_GIT_COMMIT_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE FORGE_GIT_COMMIT_RESULT)
    if(FORGE_GIT_COMMIT_RESULT EQUAL 0 AND NOT FORGE_GIT_COMMIT_OUTPUT STREQUAL "")
        set(FORGE_GIT_COMMIT "${FORGE_GIT_COMMIT_OUTPUT}")
    endif()
endif()
message(STATUS "Forge3D: version ${PROJECT_VERSION}, commit ${FORGE_GIT_COMMIT}")

# ---------------------------------------------------------------------------
# Modules
# ---------------------------------------------------------------------------
# Dependency direction: foundation -> app -> ui -> apps. Nothing depends on a
# layer above it.
add_subdirectory(foundation)

if(FORGE_BUILD_APP)
    add_subdirectory(app)
    add_subdirectory(ui)
endif()

if(FORGE_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(FORGE_BUILD_APP)
    add_subdirectory(apps/forge_desktop)
endif()
add_subdirectory(apps/forge_cli)
add_subdirectory(apps/forge_bench)

if(FORGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(FORGE_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(FORGE_BUILD_DOCS)
    add_subdirectory(docs)
endif()
