# Forge3D unit tests (internal framework; see test_framework.hpp).

add_executable(forge_tests
    test_main.cpp
    test_framework.cpp
    foundation/test_expected.cpp
    foundation/test_log.cpp
    foundation/test_result.cpp
    foundation/test_scope_exit.cpp
    foundation/test_time.cpp
    math/test_vec2.cpp
    math/test_vec3.cpp
    math/test_vec4.cpp
    math/test_mat3.cpp
    math/test_mat4.cpp
    math/test_quat.cpp
    math/test_transform.cpp
    memory/test_linear_allocator.cpp
    memory/test_memory_tracker.cpp
    filesystem/test_path.cpp
    filesystem/test_file.cpp
    threading/test_thread_pool.cpp
    threading/test_job_system.cpp
    profiling/test_profiler.cpp
)

target_include_directories(forge_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(forge_tests PRIVATE forge::foundation)
forge_apply_target_defaults(forge_tests)

forge_add_test(forge_unit_tests forge_tests)
