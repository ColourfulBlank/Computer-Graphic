#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  glm::vec3 get_vertices(int i);
  Triangle get_faces(int i);
  int get_faces_size();
  glm::vec3 get_center();
  double get_r();
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<Triangle> m_faces;
	double outer_x_max;
	double outer_x_min;
	double outer_y_max;
	double outer_y_min;
	double outer_z_max;
	double outer_z_min;
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
