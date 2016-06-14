#pragma once

#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode(
		const std::string & meshId,
		const std::string & name
	);

	void set_transform_from_parent(glm::mat4 m);
	// void draw_parent(); 

	Material material;
	Material Picked_material;

	// Mesh Identifier. This must correspond to an object name of
	// a loaded .obj file.
	std::string meshId;
};
