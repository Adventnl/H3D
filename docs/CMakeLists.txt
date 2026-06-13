# Documentation packaging. Enabled with FORGE_BUILD_DOCS=ON.
#
# Forge3D's documentation is plain Markdown under docs/. This target installs it
# alongside the binaries so packaged builds ship their docs. If Doxygen is found
# it additionally wires an API-docs target, but Doxygen is never required.

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    FILES_MATCHING PATTERN "*.md")

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    message(STATUS "Forge3D: Doxygen found; 'docs' build target available")
    add_custom_target(forge_docs
        COMMENT "Forge3D documentation lives in docs/*.md")
endif()
