

#include "Ball.hpp"
Ball::Ball(
		const std::string & meshId,
		const std::string & name
)
	: GeometryNode(meshId, name, NodeType::Ball),
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
glm::mat4x4 Ball::fly(){
	// std::chrono::time_point<std::chrono::system_clock> end_time = std::chrono::system_clock::now();
	// std::chrono::duration<double> ts = end_time - start_time;
	// std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(ts);
	// double t = ts.count();
	double t = time_frame * 0.1;
	double angle =  - local_angle + PI/2.0;
	double vx_a = local_v * sin(local_angle);
	double vy_a = local_v * cos(local_angle);
	double vy_a_now = vy_a + local_g * t;
	double vx_b = 0;
	double vy_b = sqrt(vx_a * vx_a + vy_a * vy_a);
	double axb = local_g * cos( angle );
	double ayb = local_g * sin( angle );
	double change_distance_x = vx_b * t + 0.5 * axb * t * t;
	double change_distance_y = vy_b * t + 0.5 * ayb * t * t;
	double change_distance_z = local_w * t;
	glm::mat4x4 Translation;
	double change_y = change_distance_y * cos(local_angle) + change_distance_x * cos(angle);
	// std::cout << change_distance_x << std::endl;
	if (change_y >= -0.32 * cos(local_angle)){
		Translation = glm::mat4x4( glm::vec4(1, 0, 0, 0), 
								   glm::vec4(0, 1, 0, 0), 
								   glm::vec4(0, 0, 1, 0), 
								   glm::vec4(change_distance_x , change_distance_y, change_distance_z, 1));
	} else {
		Translation = glm::mat4x4( glm::vec4(1, 0, 0, 0), 
								   glm::vec4(0, 1, 0, 0), 
								   glm::vec4(0, 0, 1, 0), 
								   glm::vec4(0, 0, 0, 1));
		ball_state = BallState::Contact;
		Contact_time = 0;
	}
	time_frame++;
	return Translation;
}

void Ball::init_fly(float arm_angle, float g, float v, float w){
	time_frame = 0;
	ball_state = BallState::Flying;
	local_g = g * 0.01;
	local_v = v * 0.01;
	local_w = w * 0.01;
	local_angle = arm_angle;
	time_to_contact = std::abs(local_v * 2.0 * cos(local_angle)/ local_g);
	// std::cout << time_to_contact << std::endl;
	// start_time = std::chrono::system_clock::now();

}
