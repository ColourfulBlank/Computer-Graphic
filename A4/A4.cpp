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
	distance_to_screen = distance_to_screen == 0 ? 800.0 : distance_to_screen;
	int frame_height = get_frame_height(fovy, distance_to_screen);
	int frame_width = image.width()/image.height() * frame_height;

	size_t h = frame_height;
	size_t w = frame_width;
	int nx = image.width();
	int ny = image.height();
	int progress = 0;
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

			vec3 colour = ray_colour(root, ray_direction, vec2(0,0), ambient, ray_origin, lights, 0, y, ny);
			// cout << colour.x << " " << colour.y << " " << colour.z << endl;
			image(x, y, 0) = colour.x;
			image(x, y, 1) = colour.y;
			image(x, y, 2) = colour.z;
		}
		progress = (double)y/(double)ny * 100;
		cout << "Processed: " << progress << '%' << endl;
	}
}

vec3 ray_colour(SceneNode * root, vec4 ray, vec2 uv, const vec3 & ambient, vec4 look_from, const std::list<Light *> & lights, int nh, int counter_y, int height){
	
	//colour: 
	vec3 kd, ks, ke, cd, colour = vec3(0,0,0);
	//background colour
	vec3 background_colour_black = vec3(0.0f, 0.0f, 0.0f);
	vec3 background_colour_white = vec3(1.0f, 1.0f, 1.0f);
	//shininess
	double shininess;
	//vector 
	vec4 N;
	//point in 3D : intersection_point
	vec4 p;
	//intersection t
	double t;
	//
	int max_h = 1;

	if (hit(root, ray, look_from, &t, &N, uv, &kd, &ks, &ke, &shininess, true) && nh <= max_h){
		nh++;
		ke = kd;//in ambient lighting
		colour = kd * ambient;
		//Find intersection point p = r.point at parameter(t)
		p = get_intersection_point(look_from, ray, t);

		// cout <<"p " << p.x <<" "<< p.y<<" " << p.z << endl;		
			// colour = colour + kd * directLight(root, p, N, uv, lights);
			vec3 diffuse_colour = vec3(0,0,0);
			vec3 specular_colour = vec3(0,0,0);
			for(const Light * light : lights) {
				double hit_point = 0.0;
				vec3 light_colour = vec3(1,1,1);
				vec4 light_position = vec4(light->position, 1);
				vec4 light_ray = light_position - p;
				vec4 reflected_ray = ggReflection(light_ray, N);
				if (!hit(root, light_ray, p+light_ray*0.01, &hit_point, NULL, vec2(0,0), NULL, NULL, NULL, NULL, false)){
					if ( kd != vec3(0,0,0) ){ //diffuse surface
						diffuse_colour += kd * glm::max(dot(normalize(light_ray), N), 0.0f) * light->colour;
					}
					if ( ks != vec3(0,0,0) ){ //specular
						specular_colour = specular_colour + ks * pow(glm::max(dot(normalize(reflected_ray), normalize(-ray)), 0.0f), shininess) * light->colour;
					}
						
				}
			}
			colour += diffuse_colour;
			colour += specular_colour;
			if ( ks != vec3(0,0,0) ){ //specular
				vec4 reflected_ray_eye = ggReflection(ray, N);
				colour += 0.15 * ray_colour(root, normalize(reflected_ray_eye), uv, ambient, p+reflected_ray_eye*0.01, lights, nh,  counter_y, height);
			}
			
	} else {

		colour = background_colour_black + vec3(0, 0, (double)counter_y/(double)height);
		
	}
	colour = vec3(glm::min(colour.x, 1.0f), glm::min(colour.y, 1.0f), glm::min(colour.z, 1.0f));
	return colour;
}
vec3 directLight(SceneNode * root, vec4 p, vec4 N, vec2 uv, const Light * light){
	
		vec3 light_colour = vec3(1,1,1);
		
	return light_colour;
}

vec4 ggReflection(vec4 ray, vec4 N){
	//note theta may be zero
	return 2.0 * dot(normalize(ray), N) * N - normalize(ray);
}

bool hit(SceneNode * root, vec4 ray, vec4 look_from, double * t, vec4 * N, vec2 uv, vec3 * kd, vec3 * ks, vec3 * ke, double * shininess, bool s){
	bool flag = false;
	double root_number = 0.0;

	
	for (SceneNode * node : root->children){
		if (node->m_nodeType == NodeType::GeometryNode) {
			GeometryNode * geometryNode = static_cast<GeometryNode *> (node);
			double roots[2];
			int number_of_roots = 0;
			//NonhierSphere
			if (geometryNode->m_primitive->get_primitiveType() == PrimitiveType::NonhierSphere){
				NonhierSphere * nonhierSphere = static_cast<NonhierSphere *> (geometryNode->m_primitive);
				vec4 sphere_center = nonhierSphere->get_pos();
				double sphere_radius = nonhierSphere->get_radius();
				double A = dot(vec3(ray), vec3(ray));
				double B = dot(vec3(ray), vec3(look_from - sphere_center)) * 2.0f;
				double C = dot(vec3(look_from - sphere_center), vec3(look_from - sphere_center)) - sphere_radius * sphere_radius;
				number_of_roots = quadraticRoots(A, B, C, roots);
				
				if (number_of_roots > 0){
					double new_root = 0.0;
					if (number_of_roots == 2){
						if (roots[0] > 0 && roots[1] > 0){
							new_root = roots[0] < roots[1] ? roots[0] : roots[1];
							if (!flag){
								root_number = new_root;
							}
							flag = true;
						} else if (roots[0] > 0){
							new_root = roots[0];
							if (!flag){
								root_number = new_root;
							}
							flag = true;	
						}else if (roots[1] > 0){
							new_root = roots[1];
							if (!flag){
								root_number = new_root;
							}
							flag = true;
						}
					} else {
						if (roots[0] > 0){
							new_root = roots[0];
							if (!flag){
								root_number = new_root;
							}
							flag = true;	
						}
					}
					if (flag == true && new_root <= root_number){
						root_number = new_root;
						if (s){
							*N = normalize(get_intersection_point(look_from, ray, root_number) - sphere_center);
							*kd = ((PhongMaterial *)geometryNode->m_material)->getKd();
							*ks = ((PhongMaterial *)geometryNode->m_material)->getKs();
							*shininess = ((PhongMaterial *)geometryNode->m_material)->getShininess();
						}
					}
				}
			}
			// NonhierBox
			else if (geometryNode->m_primitive->get_primitiveType() == PrimitiveType::NonhierBox){
				NonhierBox * nonhierBox = static_cast<NonhierBox *> (geometryNode->m_primitive);
				vec4 box_corner = nonhierBox->get_pos();
				double box_radius = nonhierBox->get_size();
				vec4 box_points[36] = {
									 box_corner + vec4(box_radius,box_radius,box_radius,0), box_corner + vec4(0,0,box_radius,0), box_corner + vec4(box_radius,0,box_radius,0),
									 box_corner + vec4(box_radius,box_radius,box_radius,0), box_corner + vec4(0,box_radius,box_radius,0), box_corner + vec4(0,0,box_radius,0),

									 box_corner + vec4(box_radius,box_radius,box_radius,0), box_corner + vec4(box_radius,0,0,0), box_corner + vec4(box_radius,box_radius,0,0),
									 box_corner + vec4(box_radius,box_radius,box_radius,0), box_corner + vec4(box_radius,0,box_radius,0), box_corner + vec4(box_radius,0,0,0),

									 box_corner + vec4(box_radius,box_radius,box_radius,0), box_corner + vec4(box_radius,box_radius,0,0), box_corner + vec4(0,box_radius,0,0),
									 box_corner + vec4(box_radius,box_radius,box_radius,0), box_corner + vec4(0,box_radius,0,0), box_corner + vec4(0,box_radius,box_radius,0),

									 box_corner,   box_corner + vec4(box_radius,0,0,0), box_corner + vec4(box_radius,0,box_radius,0),
									 box_corner,  box_corner + vec4(box_radius,0,box_radius,0),  box_corner + vec4(0,0,box_radius,0),

									 box_corner,  box_corner + vec4(box_radius,box_radius,0,0), box_corner + vec4(0,box_radius,0,0),
									 box_corner, box_corner + vec4(box_radius,0,0,0), box_corner + vec4(box_radius,box_radius,0,0), 

									 box_corner, box_corner + vec4(0,box_radius,box_radius,0), box_corner + vec4(0,box_radius,0,0), 
									 box_corner, box_corner + vec4(0,0,box_radius,0), box_corner + vec4(0,box_radius,box_radius,0), 
									};
				vec4 box_normal[6] = {
					vec4(0,0,1,0),
					vec4(1,0,0,0),
					vec4(0,1,0,0),
					vec4(0,-1,0,0),
					vec4(0,0,-1,0),
					vec4(-1,0,0,0),
				};
				for (int i = 0; i < 36; i+=3){
					double hit_point = 0.0;
					if( mesh_hit_check(box_points[i], box_points[i+1], box_points[i+2], look_from, ray, &hit_point) ){
						if (flag == false){
							root_number = hit_point;
							flag = true;
						}
						if (flag == true && hit_point <= root_number){
							root_number = hit_point;
							if (s){
								*N = box_normal[i/6];
								*kd = ((PhongMaterial *)geometryNode->m_material)->getKd();
								*ks = ((PhongMaterial *)geometryNode->m_material)->getKs();
								*shininess = ((PhongMaterial *)geometryNode->m_material)->getShininess();
							}
						}
						flag = true;
					}
				}
				
			} 
			//mesh 
			else if (geometryNode->m_primitive->get_primitiveType() == PrimitiveType::Mesh){
				Mesh * mesh = static_cast<Mesh *> (geometryNode->m_primitive);

				vec4 sphere_center = vec4(mesh->get_center(), 1);
				double sphere_radius = mesh->get_r();
				double A = dot(vec3(ray), vec3(ray));
				double B = dot(vec3(ray), vec3(look_from - sphere_center)) * 2.0f;
				double C = dot(vec3(look_from - sphere_center), vec3(look_from - sphere_center)) - sphere_radius * sphere_radius;
				number_of_roots = quadraticRoots(A, B, C, roots);
				if (number_of_roots > 0){
					int number_of_faces = mesh->get_faces_size();
					for (int i = 0; i < number_of_faces; i++){
						Triangle triangle = mesh->get_faces(i);
						vec4 p0 = vec4(mesh->get_vertices(triangle.v1) , 1);
						vec4 p1 = vec4(mesh->get_vertices(triangle.v2) , 1);
						vec4 p2 = vec4(mesh->get_vertices(triangle.v3) , 1);
						double hit_point = 0.0;
						if( mesh_hit_check(p0, p1, p2, look_from, ray, &hit_point) ){
							if (flag == false){
								root_number = hit_point;
								flag = true;
							}
							if (flag == true && hit_point <= root_number){
								root_number = hit_point;
								if (s){
									*N = normalize(vec4(cross(vec3(p1 - p0), vec3(p2 - p0)), 0));
									*kd = ((PhongMaterial *)geometryNode->m_material)->getKd();
									*ks = ((PhongMaterial *)geometryNode->m_material)->getKs();
									*shininess = ((PhongMaterial *)geometryNode->m_material)->getShininess();
								}
							}
							flag = true;
						}
					}
				}
			}
		}
	}
	*t = root_number;
	return flag;	
	
}
// bool hit_recersive()
bool mesh_hit_check(vec4 p0, vec4 p1, vec4 p2, vec4 a, vec4 b_a, double * t){
	//
	double R1 = a.x - p0.x;
	double R2 = a.y - p0.y;
	double R3 = a.z - p0.z;

	double X1 = p1.x - p0.x;
	double X2 = p2.x - p0.x;
	double X3 = -b_a.x;

	double Y1 = p1.y - p0.y;
	double Y2 = p2.y - p0.y;
	double Y3 = -b_a.y;

	double Z1 = p1.z - p0.z;
	double Z2 = p2.z - p0.z;
	double Z3 = -b_a.z;

	double D = determinant(mat3(vec3 (X1, Y1, Z1),
								vec3 (X2, Y2, Z2),
								vec3 (X3, Y3, Z3)
								));
	if ( abs(D) == 0.0 ){
		return false;
	}
	double D1 = determinant(mat3(vec3 (R1, R2, R3),
								 vec3 (X2, Y2, Z2),
								 vec3 (X3, Y3, Z3)
								));
	double D2 = determinant(mat3(vec3 (X1, Y1, Z1),
								 vec3 (R1, R2, R3),
								 vec3 (X3, Y3, Z3)
								));
	double D3 = determinant(mat3(vec3 (X1, Y1, Z1),
								 vec3 (X2, Y2, Z2),
								 vec3 (R1, R2, R3)
								));
	*t = (double)D3/(double)D;
	double beta = (double)D2/(double)D;
	double gama = (double)D1/(double)D;

	if (* t > 0.0 && beta > 0.0 && gama > 0.0 && (beta + gama < 1.0 || abs(beta + gama - 1.0) < 0.0001)) {
		return true;
	}
	return false;
}

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


