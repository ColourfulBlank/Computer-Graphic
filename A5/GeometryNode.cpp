#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
	  // m_nodeType(NodeType::GeometryNode)
{
	m_nodeType = NodeType::GeometryNode;
	Picked_material = material;
	Picked_material.kd.x = 1.0f;
	Picked_material.kd.y = 1.0f;
	Picked_material.kd.z = 0.0f;
	Picked_material.ks.x = 1.0f;
	Picked_material.ks.y = 1.0f;
	Picked_material.ks.z = 0.0f;
}

GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name,
		const NodeType type
)
	: SceneNode(name),
	meshId(meshId)
{
	m_nodeType = type;
	Picked_material = material;
	Picked_material.kd.x = 1.0f;
	Picked_material.kd.y = 1.0f;
	Picked_material.kd.z = 0.0f;
	Picked_material.ks.x = 1.0f;
	Picked_material.ks.y = 1.0f;
	Picked_material.ks.z = 0.0f;
}

void GeometryNode::set_transform_from_parent(glm::mat4 m){
	if (parent_trans != m){
		parent_trans = m;
		parent_invtrans = inverse(m);
	}
}

