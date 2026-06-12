# forge_project_info: prints project, build, and compile-option information.

add_executable(forge_project_info src/main.cpp)
target_link_libraries(forge_project_info PRIVATE forge::foundation)
forge_apply_target_defaults(forge_project_info)
