/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 16:03:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/02/10 20:13:21                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/assets/Geometry.hpp"
#include "api/vulkan/Buffer.hpp"

#ifndef TINYOBJLOADER_IMPLEMENTATION
# define TINYOBJLOADER_IMPLEMENTATION
# include "api/objectLoader/tinyObjLoader.h"
#endif

#include <iostream>

namespace	hel {

std::vector<VkVertexInputBindingDescription>	Vertex::getBindingDescriptions(void) {
	std::vector<VkVertexInputBindingDescription>	bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return (bindingDescriptions);
}

std::vector<VkVertexInputAttributeDescription>	Vertex::getAttributeDescriptions(void) {
	std::vector<VkVertexInputAttributeDescription>	attributeDescriptions{};

	attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
	attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});

	return (attributeDescriptions);
}



std::shared_ptr<Geometry>	Geometry::load(Device &device, const std::string &path) {
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>	 	shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string							warn, error;

	std::vector<Vertex>	vertices;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, path.c_str())) {
		std::cerr << (warn + error) << std::endl;
		return (nullptr);
	}
	for (auto &shape: shapes) {
		for (auto &index: shape.mesh.indices) {
			if (index.vertex_index >= 0) {
				Vertex	vertex{};

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.color= {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2]
				};
				vertices.push_back(vertex);
			}
			
		}
	}


	return (std::shared_ptr<Geometry>(new Geometry{path, createBuffer<Vertex>(device, vertices),
			nullptr, static_cast<uint32_t>(vertices.size())}));
}

}
