#include <glm/ext.hpp>

#include "A4.hpp"

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
	int frame_height = get_frame_height(fovy, distance_to_screen);
	int frame_width = image.width()/image.height() * frame_height;

	cout << "frame_height: " << frame_height << endl;
	cout << "frame_width: " << frame_width << endl; 


	size_t h = frame_height;
	size_t w = frame_width;
	int nx = image.width();
	int ny = image.height();

	for (uint y = 0; y < ny; ++y) {
		for (uint x = 0; x < nx; ++x) {
			vec4 p_k = vec4(x, y, 0, 1);

			/* Translate the pixel screen coordinate
			 * to world coordinate
			 */
			//1. translate p to view coordinate
			mat4 T1 = mat4( vec4( 1, 0, 0, 0),
							vec4( 0, 1, 0, 0),
							vec4( 0, 0, 1, 0),
							vec4( -nx/2.0f, -ny/2.0f, distance_to_screen, 1)
							);

			//2. preserve aspect ratio
			//int causion!
			double scale_factor = (double)h/(double)ny;

			mat4 S2 = mat4( vec4(- scale_factor, 0, 0, 0),
						    vec4(0, scale_factor, 0, 0),
						    vec4(0, 0, 1, 0),
						    vec4(0, 0, 0, 1) 
						    );

			//3. rotate to superImpose WCS onto VCS
			//w = (lookat - look_from) / (| lookat - look_from |)
			vec3 lookDown = view - eye;
			vec3 vector_w = ( lookDown ) / helper_module( lookDown );
			//u = (up X w) / (|up X w|)
			vec3 vector_u = cross(up,vector_w) / helper_module( cross(up, vector_w) );
			//v = u X w
			vec3 vector_v = cross(vector_u, vector_w);
			//TODO: may wrong
			mat4 R3 = mat4( vec4( vector_u.x, vector_u.y, vector_u.z, 0),
							vec4( vector_v.x, vector_v.y, vector_v.z, 0),
							vec4( vector_w.x, vector_w.y, vector_w.z, 0),
							vec4( 0, 0, 0, 1)
							);

			//4. translate (move)
			mat4 T4 = mat4( vec4( 1, 0, 0, 0),
							vec4( 0, 1, 0, 0),
							vec4( 0, 0, 1, 0),
							vec4( eye.x, eye.y, eye.z, 1)
							);
			vec4 p_world = T4 * R3 * S2 * T1 * p_k;
			// cout << p_k.x << " " << p_k.y<< " "  << p_k.z<<endl;
			// cout << p_world.x << " " << p_world.y<< " "  << p_world.z<<endl;
			vec4 ray_origin = vec4(eye, 1);
			vec4 ray_direction = p_world - ray_origin;
			// cout << ray_direction << endl;
			// cout << ray_direction.x << " " << ray_direction.y<< " "  << ray_direction.z<<endl;

			vec3 colour = ray_colour(root, ray_direction, vec2(0,0), ambient, ray_origin, lights, 0);
			// cout << colour.x << " " << colour.y << " " << colour.z << endl;
			image(x, y, 0) = colour.x;
			image(x, y, 1) = colour.y;
			image(x, y, 2) = colour.z;
		}
	}
}

vec3 ray_colour(SceneNode * root, vec4 ray, vec2 uv, const vec3 & ambient, vec4 look_from, const std::list<Light *> & lights, int nh){
	
	//colour: 
	vec3 kd, ks, ke, cd, colour = vec3(0,0,0);
	//background colour
	vec3 background_colour = vec3(158.0f/255.0f, 169.0f/255.0f, 173.0f/255.0f);
	//shininess
	double shininess;
	//vector 
	vec4 N;
	//point in 3D : intersection_point
	vec4 p;
	//intersection t
	double t;
	//
	int max_h = 10;

	if (hit(root, ray, look_from, &t, &N, uv, &kd, &ks, &ke, &shininess) && nh <= max_h){
		
		nh++;
		ke = kd;//in ambient lighting
		colour = kd * ambient;
		//Find intersection point p = r.point at parameter(t)
		p = get_intersection_point(look_from, ray, t);

		if ( kd != vec3(0,0,0) ){ //diffuse surface
			colour = colour + kd * directLight(root, p, N, uv, lights);
		}

		if ( ks != vec3(0,0,0) ){ //specular
			vec4 reflected_ray = ggReflection(ray, N);
			for(const Light * light : lights) {
				// colour = colour + ks * pow(dot(reflected_ray, normalize(ray)), shininess) * light->colour; 
			}
			// + ray_colour(root, reflected_ray, uv, ambient, look_from, lights, nh);//pow(dot(reflected_ray, ray), shininess));// * ray_colour(root, reflected_ray, uv, ambient, look_from, lights, nh);
		}

	} else {
		colour = background_colour;
	}
	colour = vec3(glm::min(colour.x, 1.0f), glm::min(colour.y, 1.0f), glm::min(colour.z, 1.0f));
	return colour;
}
vec3 directLight(SceneNode * root, vec4 p, vec4 N, vec2 uv, const std::list<Light *> & lights){
	vec3 colour = vec3(0, 0, 0);
	for(const Light * light : lights) {
		vec4 light_position = vec4(light->position, 1);
		vec4 ray = light_position - p;
		// if (hit(root, ray, p, NULL, NULL, vec2(0,0), NULL, NULL, NULL, NULL) == false){
			colour += dot(normalize(ray), N) * light->colour;
		// } else {
			// colour += dot(normalize(ray), N) * vec3(0,0,0);
		// }
	}
	return colour;
}

vec4 ggReflection(vec4 ray, vec4 N){
	//note theta may be zero
	return normalize(2.0f * dot(ray, N) * N - ray);
}

bool hit(SceneNode * root, vec4 ray, vec4 look_from, double * t, vec4 * N, vec2 uv, vec3 * kd, vec3 * ks, vec3 * ke, double * shininess){
	bool flag = false;
	for (SceneNode * node : root->children){
		if (node->m_nodeType != NodeType::GeometryNode) continue;
		GeometryNode * geometryNode = static_cast<GeometryNode *> (node);
		if (geometryNode->m_primitive->get_primitiveType() == PrimitiveType::NonhierSphere){
			NonhierSphere * nonhierSphere = static_cast<NonhierSphere *> (geometryNode->m_primitive);
			vec4 sphere_center = nonhierSphere->get_pos();
			double sphere_radius = nonhierSphere->get_radius();
			double A = dot(vec3(ray), vec3(ray));
			double B = dot(vec3(ray), vec3(look_from - sphere_center)) * 2.0f;
			double C = dot(vec3(look_from - sphere_center), vec3(look_from - sphere_center)) - sphere_radius * sphere_radius;
			double roots[2];
			int number_of_roots = quadraticRoots(A, B, C, roots);

			if (number_of_roots > 0)
			{
				if (t != NULL){
					double new_root;
					if (number_of_roots == 2){
						if (roots[0] * roots[1] < 0){
							new_root = roots[0] > 0 ? roots[0] : roots[1];
						} else if (roots[0] > 0 && roots[1] > 0){
							new_root = roots[0] < roots[1] ? roots[0] : roots[1];
						}
					} else {
						new_root = roots[0];
					}
					if (!flag){
						*t = new_root;
					}
					if (new_root <= *t){
						*t = new_root;
						*N = normalize(get_intersection_point(look_from, ray, *t) - sphere_center);
						*kd = ((PhongMaterial *)geometryNode->m_material)->getKd();
						*ks = ((PhongMaterial *)geometryNode->m_material)->getKs();
						// *ke = ((PhongMaterial *)geometryNode->m_material)->getKe();
						*shininess = ((PhongMaterial *)geometryNode->m_material)->getShininess();
					}
				}
				flag = true;
			}
		}
	}
	return flag;
}
// bool hit_recersive()
vec4 get_intersection_point(vec4 point_of_origin, vec4 direction, double t){
	//This function used to get intercetion point on the ray by t
	return point_of_origin + t * direction;
}

int get_frame_height(double fovy, double distance_to_screen){
	// cout << "tan: " << tan(fovy/2.0f);
	return tan(fovy/2.0f * PI / 180.0f) * abs(distance_to_screen) * 2.0f;

}

double helper_module(vec3 vector){

	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

}


