/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 16:03:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/02 18:09:18                                        */
/*             By: hle-hena                                                   */
/*                                                                            */
/*    -----                                                                   */
/*                                                                            */
/*  Copyright (c) 2026 hle-hena                                               */
/*                                                                            */
/* *************************************************************************  */

#include "ecs/assets/Geometry.hpp"
#include "api/vulkan/Buffer.hpp"
#include "utils/mathUtils.hpp"

#ifndef TINYOBJLOADER_IMPLEMENTATION
# define TINYOBJLOADER_IMPLEMENTATION
# include <assetLoader/tinyObjLoader.h>
#endif

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <glm/gtx/hash.hpp>
#include "Geometry.hpp"

namespace std {

template <>
struct hash<hel::Vertex>
{
	size_t	operator()(const hel::Vertex &vertex) const
	{
		size_t	seed = 0;
		hel::mathUtils::hashCombine(seed, vertex.position, vertex.color, vertex.normal);
		return (seed);
	}	
};

}

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
	attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});

	return (attributeDescriptions);
}

bool	Vertex::operator==(const Vertex &other) const {
	return (position == other.position && color == other.color && normal == other.normal);
}

Geometry::GeometryVectors	Geometry::loadFile(const std::string &path, bool fullLoad) {
	tinyobj::attrib_t					attrib;
	std::vector<tinyobj::shape_t>	 	shapes;
	std::vector<tinyobj::material_t>	materials;
	std::string							warn, error;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, path.c_str(), nullptr, false)) {
		std::cerr << (warn + error) << std::endl;
		return (GeometryVectors{});
	}
	GeometryVectors							vec;
	std::unordered_map<Vertex, uint32_t>	uniqueVertices;
	std::set<std::pair<uint32_t, uint32_t>>	lineIndices;
	for (auto &shape: shapes) {
		uint32_t	vertexIndexOffset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
			size_t					faceVertices = static_cast<size_t>(shape.mesh.num_face_vertices[f]);
			std::vector<uint32_t>	faceIndices;

			for (size_t v = 0; v < faceVertices; v++) {
				tinyobj::index_t	index = shape.mesh.indices[vertexIndexOffset + v];
				Vertex	vertex{};
				if (index.vertex_index >= 0) {
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
				}
				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}
				if (uniqueVertices.find(vertex) == uniqueVertices.end()) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vec.vertices.size());
					vec.vertices.push_back(vertex);
				}
				faceIndices.push_back(uniqueVertices[vertex]);
			}
			for (size_t i = 1; i < faceIndices.size() - 1; i++) {
				vec.triangleIndices.push_back(faceIndices[0]);
				vec.triangleIndices.push_back(faceIndices[i]);
				vec.triangleIndices.push_back(faceIndices[i + 1]);
			}
			if (fullLoad) {
				for (size_t i = 0; i < faceIndices.size(); i++) {
					lineIndices.insert(std::pair(
						std::min(faceIndices[i], faceIndices[(i + 1) % faceIndices.size()]),
						std::max(faceIndices[i], faceIndices[(i + 1) % faceIndices.size()])));
				}
			}
			vertexIndexOffset += faceVertices;
		}
	}
	if (fullLoad) {
		for (auto pair: lineIndices) {
			vec.lineIndices.push_back(pair.first);
			vec.lineIndices.push_back(pair.second);
		}
	}
	return (vec);
}

bool	Geometry::isLoadedFully(void) const {
	return (vertexBuffer && triangleIndexBuffer);
}

std::shared_ptr<Geometry>	Geometry::load(Device &device, const std::string &path) {
	GeometryVectors	vec = loadFile(path, false);
	if (vec.vertices.empty())
		return (nullptr);

	std::shared_ptr<Geometry>	asset = std::make_shared<Geometry>();
	asset->filePath = path;
	asset->vertexBuffer = createBuffer(device, vec.vertices,
										VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	asset->triangleIndexBuffer = createBuffer(device, vec.triangleIndices,
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	asset->triangleVertexCount = vec.triangleIndices.size();
	return (asset);
}

bool	FullGeometry::isLoadedFully(void) const {
	return (vertexBuffer && triangleIndexBuffer && lineIndexBuffer);
}

std::shared_ptr<FullGeometry>	FullGeometry::load(Device &device, const std::string &path) {
	GeometryVectors	vec = loadFile(path, true);
	if (vec.vertices.empty())
		return (nullptr);

	std::shared_ptr<FullGeometry>	asset = std::make_shared<FullGeometry>();
	asset->filePath = path;
	asset->vertexBuffer = createBuffer(device, vec.vertices,
										VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	asset->triangleIndexBuffer = createBuffer(device, vec.triangleIndices,
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	asset->triangleVertexCount = vec.triangleIndices.size();

	asset->lineIndexBuffer = createBuffer(device, vec.lineIndices,
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	asset->lineVertexCount = vec.lineIndices.size();
	asset->pointVertexCount = vec.vertices.size();
	return (asset);
}

}
