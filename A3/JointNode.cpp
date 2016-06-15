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
	current_X = 0;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	current_Y = 0;
}
void JointNode::upDate_x(double amount){
	current_X = amount;
}
void JointNode::upDate_y(double amount){
	current_Y = amount;
}
void JointNode::rotate_x(double amount){
	current_X = current_X + amount;
	const float PI = 3.14159265f;
	current_X = glm::degrees(current_X) > m_joint_x.max ? m_joint_x.max /180.0f * PI: current_X;
	current_X = glm::degrees(current_X) < m_joint_x.min ? m_joint_x.min /180.0f * PI: current_X;
	//need to apply to childrens
	upDateRotation();
}
void JointNode::rotate_y(double amount){
	current_Y = current_Y + amount;
	// std::cout << current_Y <<" " << glm::degrees(current_Y) << std::endl;
	const float PI = 3.14159265f;
	current_Y = glm::degrees(current_Y) > m_joint_x.max ? m_joint_x.max/180.0f * PI : current_Y;
	current_Y = glm::degrees(current_Y) < m_joint_x.min ? m_joint_x.min/180.0f * PI : current_Y;
	//need to apply to childrens
	upDateRotation();
}

void JointNode::set_transform_from_parent(glm::mat4 m){
	if (parent_trans != m){
		parent_trans = m;
		parent_invtrans = inverse(m);
		// trans = m * trans;
	}
}

glm::mat4 JointNode::get_joint_transform(int on) const{
	if (on == 1) {
		return parent_trans * rotate_trans * trans ;
	} else {
		return parent_trans  * rotate_trans * trans;
	}
}

void JointNode::upDateRotation(){
	glm::mat4 I = glm::mat4x4(glm::vec4(1, 0, 0, 0),
				  glm::vec4(0, 1, 0, 0),
				  glm::vec4(0, 0, 1, 0),
				  glm::vec4(0, 0, 0, 1));

	glm::mat4 Rotation_z = glm::mat4x4 (glm::vec4(cos(current_Y), sin(current_Y),0 ,0),
								glm::vec4(-sin(current_Y), cos(current_Y), 0, 0),
								glm::vec4(0,0,1,0),
								glm::vec4(0,0,0,1) );
	glm::mat4 Rotation_x = glm::mat4x4 (glm::vec4(1, 0, 0, 0),
								 glm::vec4(0, cos(current_X), sin(current_X), 0),
								 glm::vec4(0, -sin(current_X), cos(current_X), 0),
								 glm::vec4(0,0,0,1) );

	rotate_trans = I * Rotation_z;
	rotate_trans = rotate_trans * Rotation_x;

}

void JointNode::reset_angle_amount(){
	current_X = 0;
	current_Y = 0;
}