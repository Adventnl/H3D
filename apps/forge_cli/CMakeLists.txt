# forge_cli: command-line diagnostics and utilities. Gains the Phase 2 commands
# (list-workspaces/editors/operators, keymap, preferences) when the app/ui
# modules are built.

add_executable(forge_cli src/main.cpp)
target_link_libraries(forge_cli PRIVATE forge::foundation)

if(TARGET forge::app AND TARGET forge::ui)
    target_link_libraries(forge_cli PRIVATE forge::app forge::ui)
    target_compile_definitions(forge_cli PRIVATE FORGE_CLI_HAS_APP=1)
else()
    target_compile_definitions(forge_cli PRIVATE FORGE_CLI_HAS_APP=0)
endif()

forge_apply_target_defaults(forge_cli)
forge_install_executable(forge_cli)
