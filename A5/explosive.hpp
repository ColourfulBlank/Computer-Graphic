#pragma once
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <chrono>
#include <ctime>
#include <glm/glm.hpp>


// enum class BallState {
// 	Waiting,
// 	Flying,
// 	Contact
// };
class Explosive : public GeometryNode {
public:

	Explosive(
		const std::string & meshId,
		const std::string & name
	);
	float local_g, local_v, local_w, local_angle, time_to_contact;
	float time_frame;
	NodeType m_nodeType;
	void init_fly(float arm_angle, int i, float Contact_time);
	glm::mat4x4 fly();
	glm::mat4x4 rotation_mesh();
	float get_time_frame();
	glm::vec3 transforms[1000];
};
