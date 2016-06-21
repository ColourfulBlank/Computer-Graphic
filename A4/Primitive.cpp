#include "Primitive.hpp"
Primitive::Primitive(){
	p_type = PrimitiveType::Primitive;
}

Primitive::~Primitive(){
}

PrimitiveType Primitive::get_primitiveType(){
	return p_type;
}

Sphere::Sphere(){
	p_type = PrimitiveType::Sphere;	
}

Sphere::~Sphere(){

}

Cube::Cube(){
	p_type = PrimitiveType::Cube;	
}
Cube::~Cube()
{
}
NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius)
  : m_pos(pos), m_radius(radius)
{
	p_type = PrimitiveType::NonhierSphere;	 
}
NonhierSphere::~NonhierSphere()
{
}
glm::vec4 NonhierSphere::get_pos(){
	return glm::vec4(m_pos, 1);
}
double NonhierSphere::get_radius(){
	return m_radius;
}
NonhierBox::NonhierBox(const glm::vec3& pos, double size)
  : m_pos(pos), m_size(size)
{
	p_type = PrimitiveType::NonhierBox;	
}
NonhierBox::~NonhierBox()
{
}
glm::vec4 NonhierBox::get_pos(){
	return glm::vec4(m_pos, 1);
}
double NonhierBox::get_size(){
	return m_size;
}