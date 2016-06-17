#pragma once

#include <glm/glm.hpp>

enum class PrimitiveType {
  Primitive,
  Sphere,
  Cube,
  NonhierSphere,
  NonhierBox

};

class Primitive {

protected:
  PrimitiveType p_type;
public:
  Primitive();
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

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
