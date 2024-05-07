#pragma once

#include "utils/types.h"

typedef usize (*PFN_lbrHashFunc)(const void* p_data);
typedef u8 (*PFN_lbrEqualityFunc)(const void* p_data_a, const void* p_data_b);
