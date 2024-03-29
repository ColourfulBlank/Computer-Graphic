#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual ~PhongMaterial();
  glm::vec3 getKd();
  glm::vec3 getKs();
  glm::vec3 getKe();
  double getShininess();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_ke;

  double m_shininess;
};
