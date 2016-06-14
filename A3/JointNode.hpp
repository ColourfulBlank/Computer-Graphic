#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
	JointNode(const std::string & name);
	virtual ~JointNode();
	double current_X;
	double current_Y;
	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);
	void rotate_x(double amount);
	void rotate_y(double amount);

	void set_transform_from_parent(glm::mat4 m);
	void upDateRotation();
	struct JointRange {
		double min, init, max;
	};

	glm::mat4 get_joint_transform(int on) const;

	JointRange m_joint_x, m_joint_y;

	glm::mat4 rotateTrans;
};
