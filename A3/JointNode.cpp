#include "JointNode.hpp"

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
	current_X = init;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	current_Y = init;
}

void JointNode::rotate_x(double amount){
	current_X = current_X + amount;
	current_X = current_X > m_joint_x.max ? m_joint_x.max : current_X;
	current_X = current_X < m_joint_x.min ? m_joint_x.min : current_X;
	//need to apply to childrens
}
void JointNode::rotate_y(double amount){
	current_Y = current_Y + amount;
	current_Y = current_Y > m_joint_x.max ? m_joint_x.max : current_Y;
	current_Y = current_Y < m_joint_x.min ? m_joint_x.min : current_Y;
	//need to apply to childrens
}

void JointNode::set_transform_from_parent(glm::mat4 m){
	if (parent_trans != m){
		parent_trans = m;
		parent_invtrans = inverse(m);
		trans = m * trans;
	}
}