#include "utils/logging.h"

#ifdef LOGGING
#include <stdio.h>
#define ANSI_GREEN		"\x1b[38;5;28m"
#define ANSI_YELLOW		"\x1b[38;5;227m"
#define ANSI_RED		"\x1b[38;5;196m"
#define ANSI_RESET		"\x1b[0m"

void VulkanLog(const char* color, const char* type, const char* severity, const char* message) {
	printf("%sVulkan %s %s: %s\n" ANSI_RESET, color, type, severity, message);
}


VKAPI_ATTR VkBool32 lbrLogVulkanValidation(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagBitsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	...
	) {
	char* message_type;
	switch (type) {
	case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
		message_type = "General";
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
		message_type = "Validation";
		break;
	case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
		message_type = "Performance";
		break;
	default:
		message_type = "";
		break;
	}

	switch (severity) {
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		VulkanLog(ANSI_RESET, message_type, "Verbose", callback_data->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		VulkanLog(ANSI_GREEN, message_type, "Info", callback_data->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		VulkanLog(ANSI_YELLOW, message_type, "Warning", callback_data->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		VulkanLog(ANSI_RED, message_type, "Error", callback_data->pMessage);
		break;
	default:
		break;
	}

	return VK_FALSE;
}

void lbrLogAssertionFailure(const char* expr, const char* file, int line) {
	printf(ANSI_RED "Assertion Failure %s @ line %d: %s\n" ANSI_RESET, file, line, expr);
}

void lbrLogInfo(const char* message, const char* file, int line) {
	printf(ANSI_GREEN "Info: %s @ line %d: %s\n" ANSI_RESET, file, line, message);
}

void lbrLogWarning(const char* message, const char* file, int line) {
	printf(ANSI_YELLOW "Warning: %s @ line %d: %s\n" ANSI_RESET, file, line, message);
}

void lbrLogError(const char* message, const char* file, int line) {
	printf(ANSI_RED "Error: %s @ line %d: %s\n" ANSI_RESET, file, line, message);
}

#endif
