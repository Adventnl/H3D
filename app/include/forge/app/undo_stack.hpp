#pragma once

// UndoStack is the same machinery as CommandStack. The two names exist because
// different parts of the codebase think of it differently: the command system
// "executes" commands, while higher layers "undo" them. They are one type so
// the application owns a single history.

#include "forge/app/command_stack.hpp"

namespace forge
{

using UndoStack = CommandStack;

} // namespace forge
