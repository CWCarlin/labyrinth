#pragma once

#include "renderer/allocator.h"
#include "utils/types.h"

typedef struct lbr_vertex_t {
  f32 position[3];
  f32 uv_x;
  f32 normal[3];
  f32 uv_y;
  f32 color[4];
} LbrVertex;

typedef struct lbr_mesh_buffer {
  LbrAllocatedBuffer index_buffer;
  LbrAllocatedBuffer vertex_buffer;
  VkDeviceAddress vertex_address;
} LbrMeshBuffer;

LbrMeshBuffer lbrCreateMeshBuffer(VkCommandBuffer buffer, LbrDevice* p_device, LbrGPUAllocator* p_allocator, u32* indices, u32 index_count, LbrVertex* vertices, u32 vertex_count);
