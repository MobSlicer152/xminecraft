/// XJVM public API header

#pragma once

#pragma warning(push)
#pragma warning(disable : 4200, justification : "zero-length arrays are useful")

#include "ClassFile.h"
#include "FieldDescriptor.h"
#include "InstructionReader.h"
#include "JarFile.h"
#include "OffsetSpan.h"
#include "class_structs.h"
#include "hash.h"
#include "jit/BlockFinder.h"
#include "jvmdef.h"
#include "util.h"

#pragma warning(pop)
