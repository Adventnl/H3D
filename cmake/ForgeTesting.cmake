# Test registration helpers for Forge3D.
#
# Usage:
#   forge_add_test(<name> <target> [extra args...])
#
# Registers a CTest entry that runs the given executable target with the
# provided arguments. Tests are expected to return a non-zero exit code on
# failure (the internal test framework does this).

function(forge_add_test name target)
    add_test(NAME ${name} COMMAND ${target} ${ARGN})
    set_tests_properties(${name} PROPERTIES TIMEOUT 300)
endfunction()
