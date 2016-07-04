#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
	
	glm::mat4x4 I = glm::mat4x4(glm::vec4(1, 0, 0, 0),
				  glm::vec4(0, 1, 0, 0),
				  glm::vec4(0, 0, 1, 0),
				  glm::vec4(0, 0, 0, 1));
	rotate_trans = I;
	scale_trans = I;
	parent_trans = I;
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

void SceneNode::set_scale(const glm::mat4& m) {
	scale_trans = m;
	scale_invtrans = m;
}

void SceneNode::set_rotation(const glm::mat4& m) {
	rotate_trans = m;
	rotate_invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}
const glm::mat4& SceneNode::get_scale() const {
	return scale_trans;
}
const glm::mat4& SceneNode::get_rotation() const {
	return rotate_trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	// trans = rot_matrix * trans;
	scale_trans = rot_matrix * scale_trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	scale_trans = glm::scale(amount) * scale_trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
	// trans_without_scale = glm::translate(amount) * trans_without_scale;
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;

	os << "]\n";
	return os;
}
//---------------------------------------------------------------------------------------
void SceneNode::set_transform_from_parent(glm::mat4 m){

}

//---------------------------------------------------------------------------------------
// void SceneNode::draw_parent(){
// 	for (const SceneNode * node : root.children) { 
// 		if (node->m_nodeType != NodeType::GeometryNode)
// 			continue;
// 		node->GeometryNode::set_transform_from_parent(trans);
// 		node->GeometryNode::draw_parent();
// 	}
// }





