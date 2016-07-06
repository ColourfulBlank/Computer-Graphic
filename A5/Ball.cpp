

#include "Ball.hpp"
Ball::Ball(
		const std::string & meshId,
		const std::string & name
)
	// : m_name(name),
	  // meshId(meshId),
	:  GeometryNode(meshId, name, NodeType::Ball),
	  ball_state(BallState::Waiting)
{	
	m_nodeType = NodeType::Ball;
	Picked_material = material;
	Picked_material.kd.x = 1.0f;
	Picked_material.kd.y = 1.0f;
	Picked_material.kd.z = 0.0f;
	Picked_material.ks.x = 1.0f;
	Picked_material.ks.y = 1.0f;
	Picked_material.ks.z = 0.0f;
}
glm::mat4x4 Ball::fly(float arm_angle){
	std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
	std::chrono::duration<double> ts = end_time - start_time;
	// std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(ts);
	double t = ts.count();
	std::cout << t << std::endl;
	double angle = PI/2.0 - arm_angle;
	double vx_a = local_v * sin(angle);
	double vy_a = local_v * cos(angle);
	double vx_b = vx_a * sin(angle) + vy_a * cos(angle);//sqrt(vx_a * vx_a + vy_a * vy_a);;
	double vy_b =  - vx_a * cos(angle) + vy_a * sin(angle);//sqrt(vx_a * vx_a + vy_a * vy_a);;
	double axb = local_g * sin(angle);
	double ayb = local_g * cos(angle);
	double change_distance_x = vx_b * t + 0.5 * axb * t * t;
	double change_distance_y = vy_b * t + 0.5 * ayb * t * t;
	glm::mat4x4 Translation = glm::mat4x4( glm::vec4(1, 0, 0, 0), 
						 glm::vec4(0, 1, 0, 0), 
						 glm::vec4(0, 0, 1, 0), 
						 glm::vec4( change_distance_y * 0.001, change_distance_x * 0.001, 0, 1));
	std::cout << "here" << std::endl;
	return Translation;
}

void Ball::init_fly(float g, float v){
	start_time = std::chrono::system_clock::now();
	ball_state = BallState::Flying;
	local_g = g;
	local_v = v;
}
void Ball::update_g(float g){
	local_g = g;
}