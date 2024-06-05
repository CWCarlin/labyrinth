#include "renderer/shader.h"

#include <sys/types.h>

#include "renderer/descriptor.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#include <SPIRV-Reflect/spirv_reflect.h>
#pragma clang diagnostic pop

#include <stdio.h>
#include <unistd.h>
#include <vulkan/vulkan_core.h>

#include "utils/logging.h"
#include "utils/types.h"

#define SHADER_FILE_PATH   "../assets/shaders/spv/"
#define VERTEX_EXTENSION   ".vert.spv"
#define FRAGMENT_EXTENSION ".frag.spv"
#define COMPUTE_EXTENSION  ".comp.spv"

static VkShaderModuleCreateInfo lbrLoadShaderCode(const char* name, VkShaderStageFlags stage) {
  usize length;
  char* file_name;

  switch (stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
      length       = strlen(SHADER_FILE_PATH) + strlen(name) + strlen(VERTEX_EXTENSION) + 1;
      file_name    = malloc(length * sizeof(char));
      file_name[0] = '\0';
      strcat(file_name, SHADER_FILE_PATH);
      strcat(file_name, name);
      strcat(file_name, VERTEX_EXTENSION);
      break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
      length       = strlen(SHADER_FILE_PATH) + strlen(name) + strlen(FRAGMENT_EXTENSION) + 1;
      file_name    = malloc(length * sizeof(char));
      file_name[0] = '\0';
      strcat(file_name, SHADER_FILE_PATH);
      strcat(file_name, name);
      strcat(file_name, FRAGMENT_EXTENSION);
      break;
    case VK_SHADER_STAGE_COMPUTE_BIT:
      length       = strlen(SHADER_FILE_PATH) + strlen(name) + strlen(COMPUTE_EXTENSION) + 1;
      file_name    = malloc(length * sizeof(char));
      file_name[0] = '\0';
      strcat(file_name, SHADER_FILE_PATH);
      strcat(file_name, name);
      strcat(file_name, COMPUTE_EXTENSION);
      break;
  }

  FILE* shader_file = fopen(file_name, "r");
  LBR_ASSERT(shader_file);

  fseek(shader_file, 0, FILE_END);
  long file_size = ftell(shader_file);
  rewind(shader_file);

  u32* buffer = malloc(file_size);
  fread(buffer, sizeof(u32), file_size, shader_file);
  fclose(shader_file);

  VkShaderModuleCreateInfo shader_module_info = {0};
  shader_module_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_info.pCode                    = buffer;
  shader_module_info.codeSize                 = file_size;

  free(file_name);

  return shader_module_info;
}

LbrShader lbrCreateShader(LbrDevice* p_device, const char* name, VkShaderStageFlags stage) {
  LbrShader shader;
  shader.stage = stage;

  VkShaderModuleCreateInfo shader_info = lbrLoadShaderCode(name, stage);
  LBR_VK_CHECK(vkCreateShaderModule(p_device->device, &shader_info, NULL, &shader.module));

  SpvReflectShaderModule module;
  spvReflectCreateShaderModule(shader_info.codeSize, shader_info.pCode, &module);

  spvReflectEnumerateDescriptorSets(&module, &shader.descriptor_count, NULL);
  SpvReflectDescriptorSet* descriptor_sets[4];
  spvReflectEnumerateDescriptorSets(&module, &shader.descriptor_count, descriptor_sets);

  for (u32 i = 0; i < shader.descriptor_count; i++) {
    const SpvReflectDescriptorSet* reflect_set = descriptor_sets[i];
    LbrDescriptorLayout layout                 = lbrCreateDescriptorLayout(reflect_set->binding_count);

    for (u32 j = 0; j < reflect_set->binding_count; j++) {
      const SpvReflectDescriptorBinding* reflect_binding = reflect_set->bindings[j];
      lbrAddDescriptorBinding(&layout, reflect_binding->binding, (VkDescriptorType)reflect_binding->descriptor_type);
    }

    shader.descriptor_layouts[i] = lbrBuildDescriptorLayout(&layout, p_device, shader.stage);
    lbrDestroyDescriptorLayout(&layout);
  }

  u32 count = 0;
  SpvReflectBlockVariable* push_constants;
  spvReflectEnumeratePushConstantBlocks(&module, &count, NULL);
  if (count > 0) {
    spvReflectEnumeratePushConstantBlocks(&module, &count, &push_constants);
    shader.push_constant.stageFlags = shader.stage;
    shader.push_constant.offset     = push_constants->offset;
    shader.push_constant.size       = push_constants->size;
    shader.use_push_constant        = true;
  } else {
    shader.use_push_constant = false;
  }

  spvReflectDestroyShaderModule(&module);

  free((void*)shader_info.pCode);

  return shader;
}

LbrShaderEffect lbrCreateShaderEffect(LbrDevice* p_device, LbrShader** pp_shader, u32 shader_count) {
  LbrShaderEffect effect = {0};
  effect.shader_count    = shader_count;

  for (u32 i = 0; i < shader_count; i++) {
    effect.p_shaders[i] = pp_shader[i];
  }

  VkPipelineLayoutCreateInfo layout_info = {0};
  layout_info.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  for (u32 i = 0; i < shader_count; i++) {
    layout_info.setLayoutCount += pp_shader[i]->descriptor_count;
    layout_info.pushConstantRangeCount += (pp_shader[i]->push_constant.size > 0);
  }

  VkDescriptorSetLayout* layouts     = malloc(layout_info.setLayoutCount * sizeof(VkDescriptorSetLayout));
  VkPushConstantRange* ranges        = malloc(layout_info.pushConstantRangeCount * sizeof(VkPushConstantRange));
  layout_info.setLayoutCount         = 0;
  layout_info.pushConstantRangeCount = 0;

  for (u32 i = 0; i < shader_count; i++) {
    for (u32 j = 0; j < pp_shader[i]->descriptor_count; j++) {
      layouts[layout_info.setLayoutCount] = pp_shader[i]->descriptor_layouts[j];
      layout_info.setLayoutCount++;
    }

    if (pp_shader[i]->use_push_constant) {
      ranges[layout_info.pushConstantRangeCount] = pp_shader[i]->push_constant;
      layout_info.pushConstantRangeCount++;
    }
  }

  layout_info.pSetLayouts         = layouts;
  layout_info.pPushConstantRanges = ranges;

  LBR_VK_CHECK(vkCreatePipelineLayout(p_device->device, &layout_info, NULL, &effect.layout));

  return effect;
}

void lbrDestroyShaderEffect(LbrShaderEffect* p_effect, LbrDevice* p_device) { vkDestroyPipelineLayout(p_device->device, p_effect->layout, NULL); }

static VkPipeline lbrCreateComputePipeline(LbrDevice* p_device, LbrShaderEffect* p_effect) {
  VkPipeline pipeline;

  VkPipelineShaderStageCreateInfo shader_stage_info = {0};
  shader_stage_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shader_stage_info.stage                           = p_effect->p_shaders[0]->stage;
  shader_stage_info.module                          = p_effect->p_shaders[0]->module;
  shader_stage_info.pName                           = "main";

  VkComputePipelineCreateInfo compute_pipeline_info = {0};
  compute_pipeline_info.sType                       = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  compute_pipeline_info.layout                      = p_effect->layout;
  compute_pipeline_info.stage                       = shader_stage_info;

  LBR_VK_CHECK(vkCreateComputePipelines(p_device->device, VK_NULL_HANDLE, 1, &compute_pipeline_info, NULL, &pipeline));

  return pipeline;
}

static void lbrSetShaders(LbrShaderEffect* p_effect, VkPipelineShaderStageCreateInfo* p_shaders) {
  p_shaders[0]        = (VkPipelineShaderStageCreateInfo){0};
  p_shaders[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  p_shaders[0].stage  = p_effect->p_shaders[0]->stage;
  p_shaders[0].module = p_effect->p_shaders[0]->module;
  p_shaders[0].pName  = "main";

  p_shaders[1]        = (VkPipelineShaderStageCreateInfo){0};
  p_shaders[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  p_shaders[1].stage  = p_effect->p_shaders[1]->stage;
  p_shaders[1].module = p_effect->p_shaders[1]->module;
  p_shaders[1].pName  = "main";
}

static void lbrSetTopology(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateInfo* p_info) {
  p_info->topology               = topology;
  p_info->primitiveRestartEnable = VK_FALSE;
}

static void lbrSetPolygon(VkPolygonMode polygon_mode, VkPipelineRasterizationStateCreateInfo* p_info) {
  p_info->polygonMode = polygon_mode;
  p_info->lineWidth   = 1.0F;
}

static void lbrSetCullMode(VkCullModeFlags cull_mode, VkPipelineRasterizationStateCreateInfo* p_info) {
  p_info->cullMode  = cull_mode;
  p_info->frontFace = VK_FRONT_FACE_CLOCKWISE;
}

static void lbrSetMSAANone(VkPipelineMultisampleStateCreateInfo* p_info) {
  p_info->sampleShadingEnable   = VK_FALSE;
  p_info->rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
  p_info->minSampleShading      = 1.0F;
  p_info->alphaToCoverageEnable = VK_FALSE;
  p_info->alphaToOneEnable      = VK_FALSE;
}

static void lbrSetDisableBlending(VkPipelineColorBlendAttachmentState* p_state) {
  p_state->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  p_state->blendEnable    = VK_FALSE;
}

static void lbrSetColorFormat(const VkFormat* p_format, VkPipelineRenderingCreateInfo* p_info) {
  p_info->colorAttachmentCount    = 1;
  p_info->pColorAttachmentFormats = p_format;
}

static void lbrSetDepthFormat(VkFormat format, VkPipelineRenderingCreateInfo* p_info) { p_info->depthAttachmentFormat = format; }

static void lbrSetDisableDepthTest(VkPipelineDepthStencilStateCreateInfo* p_info) {
  p_info->depthTestEnable       = VK_FALSE;
  p_info->depthWriteEnable      = VK_FALSE;
  p_info->depthCompareOp        = VK_COMPARE_OP_NEVER;
  p_info->depthBoundsTestEnable = VK_FALSE;
  p_info->stencilTestEnable     = VK_FALSE;
  p_info->minDepthBounds        = 0.0F;
  p_info->maxDepthBounds        = 1.0F;
}

static VkPipeline lbrCreateGraphicsPipeline(LbrDevice* p_device, LbrShaderEffect* p_effect, LbrGraphicsConfig* p_config) {
  VkPipeline pipeline;

  VkPipelineShaderStageCreateInfo shader_stages[2];
  lbrSetShaders(p_effect, shader_stages);

  VkPipelineInputAssemblyStateCreateInfo input_assembler = {0};
  input_assembler.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  lbrSetTopology(p_config->topology, &input_assembler);

  VkPipelineRasterizationStateCreateInfo rasterizer = {0};
  rasterizer.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  lbrSetPolygon(p_config->polygon_mode, &rasterizer);
  lbrSetCullMode(p_config->cull_mode, &rasterizer);

  VkPipelineRenderingCreateInfo rendering_info = {0};
  rendering_info.sType                         = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  lbrSetColorFormat(&p_config->color_format, &rendering_info);
  lbrSetDepthFormat(p_config->depth_format, &rendering_info);

  VkPipelineMultisampleStateCreateInfo multisampler = {0};
  multisampler.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  if (p_config->disable_msaa) {
    lbrSetMSAANone(&multisampler);
  }

  VkPipelineColorBlendAttachmentState color_blend = {0};
  if (p_config->disable_blending) {
    lbrSetDisableBlending(&color_blend);
  }

  VkPipelineDepthStencilStateCreateInfo depth_stencil = {0};
  depth_stencil.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  if (p_config->disable_depth_test) {
    lbrSetDisableDepthTest(&depth_stencil);
  }

  VkPipelineViewportStateCreateInfo viewport_info = {0};
  viewport_info.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.viewportCount                     = 1;
  viewport_info.scissorCount                      = 1;

  VkPipelineColorBlendStateCreateInfo color_blend_info = {0};
  color_blend_info.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable                       = VK_FALSE;
  color_blend_info.logicOp                             = VK_LOGIC_OP_COPY;
  color_blend_info.attachmentCount                     = 1;
  color_blend_info.pAttachments                        = &color_blend;

  VkPipelineVertexInputStateCreateInfo vertex_input_info = {0};
  vertex_input_info.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkDynamicState dynamic_state[2]                     = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamic_state_info = {0};
  dynamic_state_info.sType                            = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_info.pDynamicStates                   = dynamic_state;
  dynamic_state_info.dynamicStateCount                = 2;

  VkGraphicsPipelineCreateInfo graphics_pipeline_info = {0};
  graphics_pipeline_info.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphics_pipeline_info.pNext                        = &rendering_info;
  graphics_pipeline_info.pStages                      = shader_stages;
  graphics_pipeline_info.stageCount                   = 2;
  graphics_pipeline_info.pVertexInputState            = &vertex_input_info;
  graphics_pipeline_info.pInputAssemblyState          = &input_assembler;
  graphics_pipeline_info.pViewportState               = &viewport_info;
  graphics_pipeline_info.pRasterizationState          = &rasterizer;
  graphics_pipeline_info.pMultisampleState            = &multisampler;
  graphics_pipeline_info.pColorBlendState             = &color_blend_info;
  graphics_pipeline_info.pDepthStencilState           = &depth_stencil;
  graphics_pipeline_info.pDynamicState                = &dynamic_state_info;
  graphics_pipeline_info.layout                       = p_effect->layout;

  LBR_VK_CHECK(vkCreateGraphicsPipelines(p_device->device, VK_NULL_HANDLE, 1, &graphics_pipeline_info, NULL, &pipeline));

  return pipeline;
}

LbrShaderPass lbrCreateShaderPass(LbrDevice* p_device, LbrShaderEffect* p_effect, enum LbrPipelineType type, LbrGraphicsConfig* p_config) {
  LbrShaderPass pass = {0};
  pass.p_effect      = p_effect;
  pass.type          = type;

  switch (type) {
    case LBR_COMPUTE_PIPELINE:
      pass.pipeline = lbrCreateComputePipeline(p_device, p_effect);
      break;
    case LBR_GRAPHICS_PIPELINE:
      pass.pipeline = lbrCreateGraphicsPipeline(p_device, p_effect, p_config);
      break;
  }

  return pass;
}
