#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name
)
	: SceneNode(name),
	  meshId(meshId)
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::set_transform_from_parent(glm::mat4 m){
	if (parent_trans != m){
		parent_trans = m;
		parent_invtrans = inverse(m);
		trans = m * trans;
	}
}

// void GeometryNode::draw_parent(){

// 	updateShaderUniforms(m_shader, *this, m_view);
// 	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
// 	BatchInfo batchInfo = m_batchInfoMap[this->meshId];

// 	//-- Now render the mesh:
// 	m_shader.enable();
// 	glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
// 	m_shader.disable();

// }