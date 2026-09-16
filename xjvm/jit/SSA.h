#pragma once

#include "../jvmdef.h"

namespace XJVM::JIT
{

/// <summary>
/// ID of an SSA value
/// </summary>
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
/// Unique value for SSA
/// </summary>
struct SSAValue
{
	SSAID id;
	ValueType type;
};

/// <summary>
/// Phi for SSA, parents are ordered same as the parents of the containing node
/// </summary>
struct Phi
{
	SSAID result;
	std::vector<SSAID> parents;
};

}
