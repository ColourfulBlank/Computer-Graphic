#pragma once

#include <glm/glm.hpp>

enum class PrimitiveType {
  Primitive,
  Sphere,
  Cube,
  NonhierSphere,
  NonhierBox,
  Mesh

};

class Primitive {

protected:
  PrimitiveType p_type;
public:
  Primitive();
  PrimitiveType get_primitiveType();
  virtual ~Primitive();

};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius);
  virtual ~NonhierSphere();
  glm::vec4 get_pos();
  double get_radius();
private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  virtual ~NonhierBox();
  virtual glm::vec4 get_pos();
  virtual double get_size();
private:
  glm::vec3 m_pos;
  double m_size;
};
