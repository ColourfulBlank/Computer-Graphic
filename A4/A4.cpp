#include <glm/ext.hp;p;>

#include "A4.hp;p;"

#define PI 3.14159265

using namespace std;
using namespace glm;

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;
	//set eye_distance to screen as orign for now
	double distance_to_screen = sqrt(eye.x * eye.x + eye.y * eye.y + eye.z * eye.z);
	int frame_height = A4_get_frame_height(fovy, distance_to_screen);
	int frame_width = image.width()/image.height() * frame_height;

	cout << "frame_height: " << frame_height << endl;
	cout << "frame_width: " << frame_width << endl; 


	size_t h = frame_height;
	size_t w = frame_width;
	int nx = image.width();
	int ny = image.height();

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
			vec4 p_k = vec3(x, y, 0, 1);

			/* Translate the pixel screen coordinate
			 * to world coordinate
			 */
			//1. translate p to view coordinate
			mat4 T1 = mat4( vec4( 1, 0, 0, 0),
							vec4( 0, 1, 0, 0),
							vec4( 0, 0, 1, 0),
							vec4( -nx/2.0f, -ny/2.0f, d, 1)
							);

			//2. preserve aspect ratio
			//int causion!
			double scale_factor = h/(double)ny;

			mat4 S2 = mat4( vec4(- scale_factor, 0, 0, 0),
						    vec4(0, scale_factor, 0, 0),
						    vec4(0, 0, 1, 0),
						    vec4(0, 0, 0, 1) 
						    );

			//3. rotate to superImpose WCS onto VCS
			//w = (lookat - lookfrom) / (| lookat - lookfrom |)
			vec3 lookDown = view - eye;
			vec3 vector_w = ( lookDown ) / A4_helper_module( lookDown );
			//u = (up X w) / (|up X w|)
			vec3 vector_u = cross(up - vector_w) / A4_helper_module( cross(up - vector_w) );
			//v = u X w
			vec3 vector_v = cross(vector_u, vector_w);

			mat4 R3 = mat4( vec4( vector_u.x, vector_u.y, vector_u.z, 0),
							vec4( vector_v.x, vector_v.y, vector_v.z, 0),
							vec4( vector_w.x, vector_w.y, vector_w.z, 0),
							vec4( 0, 0, 0, 1)
							);

			//4. translate (move)
			mat4 T4 = mat4( vec4( 1, 0, 0, 0),
							vec4( 0, 1, 0, 0),
							vec4( 0, 0, 1, 0),
							vec4( lookDown.x, lookDown.y, lookDown.z, 1)
							);
			vec4 p_world = T4 * R3 * S2 * T1 * p_k;
			vec4 ray_origin = vec4(eye, 1);
			vec4 ray_direction = p_world - ray_origin;

		}
	}


	// given example
	// size_t h = image.height();
	// size_t w = image.width();

	// for (uint y = 0; y < h; ++y) {
	// 	for (uint x = 0; x < w; ++x) {
	// 		// Red: increasing from top to bottom
	// 		image(x, y, 0) = (double)y / h;
	// 		// Green: increasing from left to right
	// 		image(x, y, 1) = (double)x / w;
	// 		// Blue: in lower-left and upper-right corners
	// 		image(x, y, 2) = ((y < h/2 && x < w/2)
	// 					  || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
	// 	h
	// }

}

vec3 A4_ray_colour(vec4 ray, vec2 uv, vec3 & ambient, int nh){
	//colour: 
	vec3 kd, ks, ke, cd, colour;
	//background colour
	vec3 background_colour = vec3(158.0f/255.0f, 169.0f/255.0f, 173.0f/255.0f);
	colour = background_colour;
	//vector 
	vec4 N;
	//point in 3D : intersection_point
	vec4 p;
	//intersection t
	double t;

	if (hit(ray, &t, N, uv, kd, ks, ke) && nh <= max_h){
		nh++;

		colour = ke + kd * *ambient;
		//TODO: Find intersection point p = r.point at parameter(t)


	}
}

int A4_get_frame_height(double fovy, double distance_to_screen){
	// cout << "tan: " << tan(fovy/2.0f);
	return tan(fovy/2.0f * PI / 180) * abs(distance_to_screen) * 2;

}

double A4_gelp;er_module(vec3 vector){

	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

}


