#pragma once

#include <vulkan/vulkan.h>

#include "renderer/allocator.h"
#include "renderer/core.h"
#include "renderer/image.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

typedef struct lbr_pool_ratio_t {
  VkDescriptorType type;
  float ratio;
} LbrPoolRatio;

typedef struct lbr_descriptor_allocator_t {
  u32 sets_per_pool;
  u32 max_pool_count;
  u32 full_pool_count;
  u32 open_pool_count;
  u32 pool_size_count;
  VkDescriptorPool* open_pools;
  VkDescriptorPool* full_pools;
  VkDescriptorPoolSize* pool_sizes;
  VkDescriptorPoolCreateFlags create_flags;
} LbrDescriptorAllocator;

LbrDescriptorAllocator lbrCreateDescriptorAllocator(u32 sets_per_pool, u32 max_pool_count, LbrPoolRatio* p_ratios, u32 ratio_count, VkDescriptorPoolCreateFlags flags);
void lbrDestroyDescriptorAllocator(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device);
void lbrDescriptorAllocatorAllocatePool(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device);
void lbrDescriptorAllocatorClearPools(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device);

typedef struct lbr_descriptor_layout_t {
  VkDescriptorSetLayoutBinding* bindings;
  u32 binding_count;
} LbrDescriptorLayout;

LbrDescriptorLayout lbrCreateDescriptorLayout(u32 binding_count);
void lbrDestroyDescriptorLayout(LbrDescriptorLayout* p_layout);
void lbrAddDescriptorBinding(LbrDescriptorLayout* p_layout, u32 binding, VkDescriptorType type);
VkDescriptorSetLayout lbrBuildDescriptorLayout(LbrDescriptorLayout* p_layout, LbrDevice* p_device, VkShaderStageFlags shader_stages);

VkDescriptorSet lbrCreateDescriptorSet(LbrDescriptorAllocator* p_allocator, LbrDevice* p_device, VkDescriptorSetLayout layout);

typedef struct lbr_descriptor_writer_t {
  VkDescriptorBufferInfo* buffer_infos;
  VkDescriptorImageInfo* image_infos;
  VkWriteDescriptorSet* descriptor_writes;
  u32 max_count;
  u32 buffer_info_count;
  u32 image_info_count;
  u32 descriptor_write_count;
} LbrDescriptorWriter;

LbrDescriptorWriter lbrCreateDescriptorWriter(u32 max_info_count);
void lbrDestroyDescriptorWriter(LbrDescriptorWriter* p_writer);
void lbrDescriptorWriterWriteBuffer(LbrDescriptorWriter* p_writer, u32 binding, LbrAllocatedBuffer* p_buffer, VkDescriptorType type);
void lbrDescriptorWriterWriteImage(LbrDescriptorWriter* p_writer, u32 binding, LbrImage* p_image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
void lbrDescriptorWriterClear(LbrDescriptorWriter* p_writer);
void lbrDescriptorWriterUpdateSet(LbrDescriptorWriter* p_writer, LbrDevice* p_device, VkDescriptorSet set);
