/* *************************************************************************  */
/*                                                                            */
/*                                                                            */
/*  File: Geometry.cpp                                                        */
/*  Project: Hel Engine                                                       */
/*  Created: 2026/02/10 16:03:26 by hle-hena                                  */
/*                                                                            */
/*  Last Modified: 2026/04/30 21:00:23                                        */
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

#include <unordered_map>
#include <algorithm>
#include "Geometry.hpp"
#include "utils/VFS.hpp"

namespace std {

template <>
struct hash<hel::Vertex>
{
	size_t	operator()(const hel::Vertex &vertex) const
	{
		size_t	seed = 0;
		hel::mathUtils::hashCombine(seed, vertex.position.x, vertex.position.y,
			vertex.position.z, vertex.uv.x, vertex.uv.y, vertex.normal.x,
			vertex.normal.y, vertex.normal.z, vertex.color.x, vertex.color.y,
			vertex.color.z);
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
	attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
	attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
	attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});

	return (attributeDescriptions);
}

bool	Vertex::operator==(const Vertex &other) const {
	return (position == other.position && uv == other.uv
			&& normal == other.normal && color == other.color);
}

Geometry::GeometryVectors	Geometry::loadFile(const std::string &modelName, bool fullLoad) {
	std::string	baseDir = "assets/models/" + modelName + "/";

	tinyobj::ObjReaderConfig	readerConfig;
	readerConfig.mtl_search_path = baseDir;

	tinyobj::ObjReader	reader;
	std::string			filePath = VFS::getFilepath(baseDir + modelName + ".obj");
	if (!reader.ParseFromFile(filePath, readerConfig)) {
		if (!reader.Error().empty())
			std::cerr << "TinyObj error: " << reader.Error() << std::endl;
		return {};
	}
	auto	&attrib = reader.GetAttrib();
	auto	&shapes = reader.GetShapes();
	auto	&materials = reader.GetMaterials();

	GeometryVectors							vec;
	std::unordered_map<Vertex, uint32_t>	uniqueVertices;
	Submesh									*submesh = nullptr;

	for (auto &mat: materials) {
		if (mat.diffuse_texname.empty())
			vec.materialPaths.push_back("assets/images/white_square.png");
		else
			vec.materialPaths.push_back(baseDir + mat.diffuse_texname);
	}

	std::set<std::pair<uint32_t, uint32_t>>	lineIndices;
	bool									hasNormals = !attrib.normals.empty();
	uint32_t	triIndexOffset = 0;
	uint32_t	lineIndexOffset = 0;
	for (auto &shape: shapes) {
		int			lastMaterialID = -1;
		uint32_t	vertexIndexOffset = 0;

		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
			int		currentMaterialID = shape.mesh.material_ids[f];
			currentMaterialID = currentMaterialID > 0 ? currentMaterialID : 0;
			if (f == 0 || currentMaterialID != lastMaterialID) {
				submesh = &vec.submeshes.emplace_back();
				submesh->materialID = static_cast<uint32_t>(currentMaterialID);
				submesh->triFirstIndex = triIndexOffset;
				submesh->lineFirstIndex = lineIndexOffset;
				lastMaterialID = currentMaterialID;
			}

			size_t					faceVertices = static_cast<size_t>(shape.mesh.num_face_vertices[f]);
			std::vector<uint32_t>	faceIndices;

			for (size_t v = 0; v < faceVertices; v++) {
				tinyobj::index_t	index = shape.mesh.indices[vertexIndexOffset + v];
				Vertex	vertex{};
				if (index.vertex_index >= 0) {
					uint32_t	vertIndex = static_cast<uint32_t>(index.vertex_index);
					vertex.position = {
						attrib.vertices[3 * vertIndex + 0],
						attrib.vertices[3 * vertIndex + 1],
						attrib.vertices[3 * vertIndex + 2]
					};

					vertex.color = {
						attrib.colors[3 * vertIndex + 0],
						attrib.colors[3 * vertIndex + 1],
						attrib.colors[3 * vertIndex + 2]
					};
				}
				if (index.normal_index >= 0) {
					uint32_t	normIndex = static_cast<uint32_t>(index.normal_index);
					vertex.normal = {
						attrib.normals[3 * normIndex + 0],
						attrib.normals[3 * normIndex + 1],
						attrib.normals[3 * normIndex + 2]
					};
				}
				if (index.texcoord_index >= 0) {
					uint32_t	texIndex = static_cast<uint32_t>(index.texcoord_index);
					vertex.uv = {
						attrib.texcoords[2 * texIndex + 0],
						1 - attrib.texcoords[2 * texIndex + 1]
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
				triIndexOffset += 3;
				submesh->triIndexCount += 3;
			}
			if (fullLoad) {
				for (size_t i = 0; i < faceIndices.size(); i++) {
					lineIndices.insert(std::pair(
						std::min(faceIndices[i], faceIndices[(i + 1) % faceIndices.size()]),
						std::max(faceIndices[i], faceIndices[(i + 1) % faceIndices.size()])));
					lineIndexOffset += 2;
					submesh->lineIndexCount += 2;
				}
			}
			vertexIndexOffset += static_cast<uint32_t>(faceVertices);
		}
	}

	if (!hasNormals) {
		for (size_t i = 0; i < vec.triangleIndices.size(); i += 3) {
			uint32_t i0 = vec.triangleIndices[i];
			uint32_t i1 = vec.triangleIndices[i + 1];
			uint32_t i2 = vec.triangleIndices[i + 2];

			glm::vec3 v0 = vec.vertices[i0].position;
			glm::vec3 v1 = vec.vertices[i1].position;
			glm::vec3 v2 = vec.vertices[i2].position;

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;
			glm::vec3 faceNormal = glm::cross(edge1, edge2);

			vec.vertices[i0].normal += faceNormal;
			vec.vertices[i1].normal += faceNormal;
			vec.vertices[i2].normal += faceNormal;
		}

		for (auto& vertex : vec.vertices) {
			if (glm::length(vertex.normal) > 0.0f) {
				vertex.normal = glm::normalize(vertex.normal);
			}
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

std::shared_ptr<Geometry>	Geometry::load(Device &device, const std::string &modelName) {
	GeometryVectors	vec = loadFile(modelName, false);
	if (vec.vertices.empty())
		return (nullptr);

	std::shared_ptr<Geometry>	asset = std::make_shared<Geometry>();
	asset->modelName = modelName;
	asset->vertexBuffer = createBuffer(device, vec.vertices,
										VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	asset->triangleIndexBuffer = createBuffer(device, vec.triangleIndices,
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	asset->triangleVertexCount = static_cast<uint32_t>(vec.triangleIndices.size());
	asset->submeshes = std::move(vec.submeshes);
	asset->materialPaths = std::move(vec.materialPaths);
	if (asset->materialPaths.empty())
		asset->materialPaths.push_back("assets/images/white_square.png");
	return (asset);
}

bool	FullGeometry::isLoadedFully(void) const {
	return (vertexBuffer && triangleIndexBuffer && lineIndexBuffer);
}

std::shared_ptr<FullGeometry>	FullGeometry::load(Device &device, const std::string &modelName) {
	GeometryVectors	vec = loadFile(modelName, true);
	if (vec.vertices.empty())
		return (nullptr);

	std::shared_ptr<FullGeometry>	asset = std::make_shared<FullGeometry>();
	asset->modelName = modelName;
	asset->vertexBuffer = createBuffer(device, vec.vertices,
										VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	asset->triangleIndexBuffer = createBuffer(device, vec.triangleIndices,
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	asset->triangleVertexCount = static_cast<uint32_t>(vec.triangleIndices.size());

	asset->lineIndexBuffer = createBuffer(device, vec.lineIndices,
										VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	asset->lineVertexCount = static_cast<uint32_t>(vec.lineIndices.size());
	asset->submeshes = std::move(vec.submeshes);
	asset->materialPaths = std::move(vec.materialPaths);
	if (asset->materialPaths.empty())
		asset->materialPaths.push_back("assets/images/white_square.png");
	return (asset);
}

}
