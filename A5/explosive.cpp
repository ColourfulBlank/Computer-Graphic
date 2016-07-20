

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
	// transforms = new glm::vec3[1000];
	// std::srand(std::time(0));
	for (int i = 0 ; i < 1000; i++){
		int random_x = (std::rand() % 100) / 10.0;
		int random_y = (std::rand() % 100 - 45) / 10.0;
		int random_z = (std::rand() % 100 - 45) / 10.0;
		// std::cout << random_x << " " << random_y <<" "<< random_z << std::endl;
		transforms[i] = glm::vec3( random_x, random_y, random_z);
	}
}
glm::mat4x4 Explosive::fly(){
	double t = time_frame * 0.1;
	double change_distance_x = local_v * t;//vx_b * t + 0.5 * axb * t * t;
	double change_distance_y = local_g * t;//vy_b * t + 0.5 * ayb * t * t;
	double change_distance_z = local_w * t;
	// double change_distance_x = local_v * t;//vx_b * t + 0.5 * axb * t * t;
	// double change_distance_y = local_g * t;//vy_b * t + 0.5 * ayb * t * t;
	// double change_distance_z = local_w * t;
	glm::mat4x4 Translation;

	// std::cout << change_distance_x << std::endl;
	// double change_y = change_distance_y * cos(local_angle) + change_distance_x * cos(angle);
	// std::cout <<  << std::endl;
	// if (time_frame < 100){
	Translation = glm::mat4x4( glm::vec4(1, 0, 0, 0), 
							   glm::vec4(0, 1, 0, 0), 
							   glm::vec4(0, 0, 1, 0), 
							   glm::vec4(change_distance_x , change_distance_y, change_distance_z, 1));

	return Translation;
}
glm::mat4x4 Explosive::rotation_mesh(){

	double t = time_frame * 10.0;
	double change_distance_x = local_v * t;//vx_b * t + 0.5 * axb * t * t;
	double change_distance_y = local_g * t;//vy_b * t + 0.5 * ayb * t * t;
	double change_distance_z = local_w * t;

	glm::mat4x4 Rotation_x;
	glm::mat4x4 Rotation_y;
	glm::mat4x4 Rotation_z;

	Rotation_x = glm::mat4x4 ( glm::vec4(1, 0, 0, 0),
									 glm::vec4(0, cos(change_distance_x), sin(change_distance_x), 0),
									 glm::vec4(0, -sin(change_distance_x), cos(change_distance_x), 0),
									 glm::vec4(0,0,0,1) );

	Rotation_y = glm::mat4x4 ( glm::vec4(cos(change_distance_y), 0, -sin(change_distance_y), 0),
									 glm::vec4(0, 1, 0, 0),
									 glm::vec4(sin(change_distance_y), 0, cos(change_distance_y), 0),
									 glm::vec4(0,0,0,1) );

	Rotation_z = glm::mat4x4 (glm::vec4(cos(change_distance_z), sin(change_distance_z),0 ,0),
									glm::vec4(-sin(change_distance_z), cos(change_distance_z), 0, 0),
									glm::vec4(0,0,1,0),
									glm::vec4(0,0,0,1) );
	return Rotation_z * Rotation_y * Rotation_x;
}

float Explosive::get_time_frame(){
	return time_frame;
}
void Explosive::init_fly(float arm_angle, int i, float Contact_time){
	time_frame = Contact_time;
	local_g = transforms[i].x * 0.05;
	local_v = transforms[i].y * 0.1;
	local_w = transforms[i].z * 0.1;
	local_angle = arm_angle;
	time_to_contact = std::abs(local_v * 2.0 * cos(local_angle)/ local_g);
	// std::cout << time_to_contact << std::endl;
	// start_time = std::chrono::system_clock::now();

}
