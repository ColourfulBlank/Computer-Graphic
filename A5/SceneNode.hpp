#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode,
    Ball
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_scale() const;
    const glm::mat4& get_rotation() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    void set_scale(const glm::mat4& m);
    void set_rotation(const glm::mat4& m);

    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);
// add method want it only be called in subclass?

    void set_transform_from_parent(glm::mat4 m);
    // void draw_parent();

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);


	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	std::string m_name;
    NodeType m_nodeType;
	unsigned int m_nodeId;

//add field
    glm::mat4 parent_trans;
    glm::mat4 parent_invtrans;
    glm::mat4 scale_trans;
    glm::mat4 scale_invtrans;
    glm::mat4 rotate_trans;
    glm::mat4 rotate_invtrans;
    
private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
