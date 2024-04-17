#pragma once

#ifdef LOGGING
#include "vulkan/vulkan_core.h"

VKAPI_ATTR VkBool32 lbrLogVulkanValidation(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagBitsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	...
	);

void lbrLogAssertionFailure(const char* expr, const char* file, int line);
void lbrLogInfo(const char* message, const char* file, int line);
void lbrLogWarning(const char* message, const char* file, int line);
void lbrLogError(const char* message, const char* file, int line);

#define lbrDebugBreak() asm("int3")

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
