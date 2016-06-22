#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;
	outer_x_max = 0;
	outer_x_min = 0;
	outer_y_max = 0;
	outer_y_min = 0;
	outer_z_max = 0;
	outer_z_min = 0;
	std::ifstream ifs( fname.c_str() );
	bool flag = false;
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			if (!flag){
				flag = true;
				outer_x_max = vx;
				outer_x_min = vx;
				outer_y_max = vy;
				outer_y_min = vy;
				outer_z_max = vz;
				outer_z_min = vz;
			}
			outer_x_max = glm::max(outer_x_max, vx);
			outer_x_min = glm::min(outer_x_min, vx);
			outer_y_max = glm::max(outer_y_max, vy);
			outer_y_min = glm::min(outer_y_min, vy);
			outer_z_max = glm::max(outer_z_max, vz);
			outer_z_min = glm::min(outer_z_min, vz);
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	p_type = PrimitiveType::Mesh;
}

glm::vec3 Mesh::get_vertices(int i){
	return m_vertices[i];
}
Triangle Mesh::get_faces(int i){
	return m_faces[i];
}

int Mesh::get_faces_size(){
	return m_faces.size();
}

glm::vec3 Mesh::get_center(){
	return glm::vec3(outer_x_min + (outer_x_max - outer_x_min)/2.0, outer_y_min + (outer_y_max - outer_y_min)/2.0, outer_z_min + (outer_z_max - outer_z_min)/2.0);
}
double Mesh::get_r(){
	return glm::max( glm::max((outer_x_max - outer_x_min), (outer_y_max - outer_y_min)), (outer_z_max - outer_z_min) ) / 2.0;
}
std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
