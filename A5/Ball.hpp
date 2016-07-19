#pragma once
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <chrono>
#include <ctime>
#include <glm/glm.hpp>


enum class BallState {
	Waiting,
	Flying,
	Contact
};
class Ball : public GeometryNode {
public:

	Ball(
		const std::string & meshId,
		const std::string & name
	);
	float local_g, local_v, local_w, local_angle, time_to_contact;
	float time_frame, Contact_time;
	NodeType m_nodeType;
	BallState ball_state;
	std::chrono::time_point<std::chrono::system_clock> start_time;
	void init_fly(float arm_angle, float g, float v, float w);
	glm::mat4x4 fly();
};
