# forge_cli: command-line diagnostics and utilities.

add_executable(forge_cli src/main.cpp)
target_link_libraries(forge_cli PRIVATE forge::foundation)
forge_apply_target_defaults(forge_cli)
forge_install_executable(forge_cli)
