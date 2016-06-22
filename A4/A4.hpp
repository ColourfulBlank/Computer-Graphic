#pragma once
#include <glm/glm.hpp>
#include <climits>
#include <vector>
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "polyroots.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "PhongMaterial.hpp"
#include "Mesh.hpp"




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
);

glm::vec3 ray_colour(SceneNode * root, glm::vec4 ray, glm::vec2 uv, const glm::vec3 & ambient, glm::vec4 look_from, const std::list<Light *> & lights, int nh, int counter_y, int height);

int get_frame_height(double fovy, double distance_to_screen);

glm::vec3 directLight(SceneNode * root, glm::vec4 p, glm::vec4 N, glm::vec2 uv, const Light * light);

glm::vec4 ggReflection(glm::vec4 ray, glm::vec4 N);

bool hit(SceneNode * root, glm::vec4 ray, glm::vec4 look_from, double * t, glm::vec4 * N, glm::vec2 uv, glm::vec3 * kd, glm::vec3 * ks, glm::vec3 * ke, double * shininess, bool s);

glm::vec4 get_intersection_point(glm::vec4 point_of_origin, glm::vec4 direction, double t);

double helper_module(glm::vec3 vector);

bool mesh_hit_check(glm::vec4 p0, glm::vec4 p1, glm::vec4 p2, glm::vec4 a, glm::vec4 b_a, double * t);
