

#include "explosive.hpp"
Explosive::Explosive(
		const std::string & meshId,
		const std::string & name
)
	: GeometryNode(meshId, name, NodeType::Explosive)
{	
	m_nodeType = NodeType::Explosive;
	Picked_material = material;
	Picked_material.kd.x = 1.0f;
	Picked_material.kd.y = 1.0f;
	Picked_material.kd.z = 0.0f;
	Picked_material.ks.x = 1.0f;
	Picked_material.ks.y = 1.0f;
	Picked_material.ks.z = 0.0f;
}
glm::mat4x4 Explosive::fly(){
	double t = time_frame * 1;
	double change_distance_x = local_v * t;//vx_b * t + 0.5 * axb * t * t;
	double change_distance_y = local_g * t;//vy_b * t + 0.5 * ayb * t * t;
	double change_distance_z = local_w * t;
	glm::mat4x4 Translation;
	// std::cout << change_distance_x << std::endl;
	// double change_y = change_distance_y * cos(local_angle) + change_distance_x * cos(angle);
	// std::cout << change_y << std::endl;
	// if (time_frame < 100){
		Translation = glm::mat4x4( glm::vec4(1, 0, 0, 0), 
								   glm::vec4(0, 1, 0, 0), 
								   glm::vec4(0, 0, 1, 0), 
								   glm::vec4(change_distance_x , change_distance_y, change_distance_z, 1));
	// } else{
		// Translation = glm::mat4x4( glm::vec4(1, 0, 0, 0), 
		// 						   glm::vec4(0, 1, 0, 0), 
		// 						   glm::vec4(0, 0, 1, 0), 
		// 						   glm::vec4(0, 0, 0, 1));
	// }
	return Translation;
}
float Explosive::get_time_frame(){
	return time_frame;
}
void Explosive::init_fly(float arm_angle, float g, float v, float w, float Contact_time){
	time_frame = Contact_time;
	local_g = g * 0.00001;
	local_v = v * 0.00001;
	local_w = w * 0.00001;
	local_angle = arm_angle;
	time_to_contact = std::abs(local_v * 2.0 * cos(local_angle)/ local_g);
	// std::cout << time_to_contact << std::endl;
	// start_time = std::chrono::system_clock::now();

}
