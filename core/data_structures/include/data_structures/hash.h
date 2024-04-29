#pragma once

#include "utility/types.h"

typedef usize (*PFN_lbrHashFunc)(void* p_data);
typedef u8 (*PFN_lbrEqualityFunc)(void* p_data_a, void* p_data_b);
