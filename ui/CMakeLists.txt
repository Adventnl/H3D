# forge_ui: the logical UI/editor framework — workspaces, screens, areas,
# regions, editors, widgets, layout, themes, menus, toolbars and status bars.
# Depends on forge::foundation and forge::app. It must NOT depend on any future
# scene/geometry/gpu/render module.

add_library(forge_ui STATIC
    src/area.cpp
    src/docking.cpp
    src/editor.cpp
    src/editor_registry.cpp
    src/layout.cpp
    src/menu.cpp
    src/panel.cpp
    src/rect.cpp
    src/region.cpp
    src/screen.cpp
    src/status_bar.cpp
    src/theme.cpp
    src/toolbar.cpp
    src/widget.cpp
    src/workspace.cpp
    src/workspace_registry.cpp
)
add_library(forge::ui ALIAS forge_ui)

target_include_directories(forge_ui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(forge_ui PUBLIC forge::foundation forge::app)

forge_apply_target_defaults(forge_ui)
forge_install_library(forge_ui)
