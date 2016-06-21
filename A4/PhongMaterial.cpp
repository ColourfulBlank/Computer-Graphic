#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{
	m_ke = glm::vec3(0,0,0);
}
glm::vec3 PhongMaterial::getKd(){
	return m_kd;
}
glm::vec3 PhongMaterial::getKs(){
	return m_ks;
}
glm::vec3 PhongMaterial::getKe(){
	return m_ke;
}
double PhongMaterial::getShininess(){
	return m_shininess;
}

PhongMaterial::~PhongMaterial()
{}
