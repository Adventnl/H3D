# forge_desktop: the (future) desktop application entry point.

add_executable(forge_desktop src/main.cpp)
target_link_libraries(forge_desktop PRIVATE forge::foundation)
forge_apply_target_defaults(forge_desktop)
forge_install_executable(forge_desktop)
