#pragma once

#include "jvmdef.h"

namespace XJVM::JIT
{

/// <summary>
/// Value type
/// </summary>
enum class ValueType
{
    Int,
    Float,
    Double,
    Long,
    Reference
};

/// <summary>
/// 
/// </summary>
struct SSAValue
{
	uint32_t id;
	ValueType type;
};

}
