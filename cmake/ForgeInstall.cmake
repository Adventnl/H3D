# Install rules for Forge3D targets.
#
# Usage:
#   forge_install_executable(<target>)
#   forge_install_library(<target>)

include(GNUInstallDirs)

function(forge_install_executable target)
    install(TARGETS ${target}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

function(forge_install_library target)
    install(TARGETS ${target}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()
