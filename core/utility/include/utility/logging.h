#pragma once

#ifdef LOGGING
#include "utility/types.h"

void lbrLogAssertionFailure(const char* expr, const char* file, u32 line);
void lbrLogInfo(const char* message, const char* file, u32 line);
void lbrLogWarning(const char* message, const char* file, u32 line);
void lbrLogError(const char* message, const char* file, u32 line);

#define lbrDebugBreak() __asm("int3")

#define ASSERT(expr)							\
	if (expr) {}								\
	else {										\
		labLogAssertionFailure(					\
			#expr,								\
			__FILE__,							\
			__LINE__,							\
		);										\
		lbrDebugBreak();						\
	}

#define LOG_INFO(message)						\
	lbrLogInfo(#message, __FILE__, __LINE__);

#define LOG_WARN(message)						\
	lbrLogWarning(#message, __FILE__, __LINE__);

#define LOG_ERROR(message)						\
	lbrLogError(#message, __FILE__, __LINE__);	\
	lbrDebugBreak();

#else
#define ASSERT(expr)
#define LOG_INFO(message)
#define LOG_WARN(message)
#define LOG_ERROR(message)
#endif
