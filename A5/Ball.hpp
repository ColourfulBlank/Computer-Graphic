#pragma once
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <chrono>
#include <ctime>
#include <glm/glm.hpp>


enum class BallState {
	Waiting,
	Flying
};
class Ball : public GeometryNode {
public:
	Ball(
		const std::string & meshId,
		const std::string & name
	);
	float local_g, local_v;
	NodeType m_nodeType;
	BallState ball_state;
	std::chrono::time_point<std::chrono::system_clock> start_time;
	void init_fly(float g, float v);
	glm::mat4x4 fly(float arm_angle);
	void update_g(float g);
};
