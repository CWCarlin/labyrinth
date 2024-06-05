#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "renderer/core.h"

enum LbrPipelineType {
  LBR_COMPUTE_PIPELINE,
  LBR_GRAPHICS_PIPELINE,
};

typedef struct lbr_shader_t {
  VkShaderModule module;
  VkDescriptorSetLayout descriptor_layouts[4];
  VkPushConstantRange push_constant;
  VkShaderStageFlags stage;
  u32 descriptor_count;
  bool use_push_constant;
} LbrShader;

LbrShader lbrCreateShader(LbrDevice* p_device, const char* name, VkShaderStageFlags stage);
void lbrDestroyShader(LbrShader* p_shader, LbrDevice* p_device);

typedef struct lbr_shader_effect_t {
  VkPipelineLayout layout;
  LbrShader* p_shaders[2];
  u32 shader_count;
} LbrShaderEffect;

LbrShaderEffect lbrCreateShaderEffect(LbrDevice* p_device, LbrShader** pp_shader, u32 shader_count);
void lbrDestroyShaderEffect(LbrShaderEffect* p_effect, LbrDevice* p_device);

typedef struct lbr_graphics_config_t {
  VkPrimitiveTopology topology;
  VkPolygonMode polygon_mode;
  VkCullModeFlags cull_mode;
  VkFormat color_format;
  VkFormat depth_format;
  bool disable_msaa;
  bool disable_blending;
  bool disable_depth_test;
} LbrGraphicsConfig;

typedef struct lbr_shader_pass_t {
  LbrShaderEffect* p_effect;
  VkPipeline pipeline;
  enum LbrPipelineType type;
} LbrShaderPass;

LbrShaderPass lbrCreateShaderPass(LbrDevice* p_device, LbrShaderEffect* p_effect, enum LbrPipelineType type, LbrGraphicsConfig* p_config);
void lbrDestroyShaderPass();
