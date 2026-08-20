#pragma once

#include "jvmdef.h"

namespace XJVM::JIT
{

using SSAID = uint32_t;

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
	SSAID id;
	ValueType type;
};

}
