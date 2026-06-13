# forge_desktop: the Phase 2 desktop application shell. Uses the Application
# runtime (forge::app) and the logical UI/editor framework (forge::ui).

add_executable(forge_desktop src/main.cpp)
target_link_libraries(forge_desktop PRIVATE forge::foundation forge::app forge::ui)
forge_apply_target_defaults(forge_desktop)
forge_install_executable(forge_desktop)
