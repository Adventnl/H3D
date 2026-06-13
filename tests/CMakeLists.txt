# Forge3D unit tests (internal framework; see test_framework.hpp).

set(FORGE_TEST_SOURCES
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

# Phase 2 app/ui tests are added only when those modules are built.
if(TARGET forge::app AND TARGET forge::ui)
    list(APPEND FORGE_TEST_SOURCES
        app/test_application.cpp
        app/test_command_stack.cpp
        app/test_event_bus.cpp
        app/test_input.cpp
        app/test_keymap.cpp
        app/test_modal_tool.cpp
        app/test_operator_registry.cpp
        app/test_preferences.cpp
        app/test_undo_stack.cpp
        app/test_window.cpp
        ui/test_area_split.cpp
        ui/test_editor_registry.cpp
        ui/test_layout.cpp
        ui/test_screen.cpp
        ui/test_theme.cpp
        ui/test_widgets.cpp
        ui/test_workspace.cpp
        ui/test_workspace_registry.cpp
    )
endif()

add_executable(forge_tests ${FORGE_TEST_SOURCES})

target_include_directories(forge_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(forge_tests PRIVATE forge::foundation)
if(TARGET forge::app AND TARGET forge::ui)
    target_link_libraries(forge_tests PRIVATE forge::app forge::ui)
endif()
forge_apply_target_defaults(forge_tests)

forge_add_test(forge_unit_tests forge_tests)
