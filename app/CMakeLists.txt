# forge_app: the application runtime, events, input, commands, operators,
# modal tools, preferences and the window abstraction. Depends only on
# forge::foundation (never on ui).

add_library(forge_app STATIC
    src/app_context.cpp
    src/application.cpp
    src/application_config.cpp
    src/command.cpp
    src/command_stack.cpp
    src/event.cpp
    src/event_bus.cpp
    src/input.cpp
    src/keymap.cpp
    src/modal_tool.cpp
    src/operator.cpp
    src/operator_registry.cpp
    src/preferences.cpp
    src/runtime.cpp
    src/shortcut.cpp
    src/window.cpp
)
add_library(forge::app ALIAS forge_app)

target_include_directories(forge_app PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(forge_app PUBLIC forge::foundation)

forge_apply_target_defaults(forge_app)
forge_install_library(forge_app)
