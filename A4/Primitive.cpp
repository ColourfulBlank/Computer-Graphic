#include "Primitive.hpp"
Primitive::Primitive(){
	p_type = PrimitiveType::Primitive;
}

Primitive::~Primitive(){
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
NonhierBox::NonhierBox(const glm::vec3& pos, double size)
  : m_pos(pos), m_size(size)
{
	p_type = PrimitiveType::NonhierBox;	
}
NonhierBox::~NonhierBox()
{
}
