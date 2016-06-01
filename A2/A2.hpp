#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);
	void viewPort_cliping(glm::vec2 i, glm::vec2 e);
	bool near_cliping(glm::vec4 * p);
	bool far_cliping(glm::vec4 * p);
	float viewPort_reDraw_x(float x);
	float viewPort_reDraw_y(float y);
	void draw_points(glm::vec4 i, glm::vec4 e);
	void draw_points_view(glm::vec4 i, glm::vec4 e);
	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;
	glm::vec4 **cube_structure;
	glm::vec4 x_axis[2]; 
	glm::vec4 y_axis[2]; 
	glm::vec4 z_axis[2];
	glm::vec2 viewPort_top[2];
	glm::vec2 viewPort_bot[2];
	glm::vec2 viewPort_left[2];
	glm::vec2 viewPort_right[2];
	
	glm::mat4x4 M;
	glm::mat4x4 V;
	glm::mat4x4 P;

	float delta_right;
	float old_x_position;
	float old_y_position; 

	float view_delta_x;
	float view_delta_y;
	float view_delta_z;
	float model_delta_x;
	float model_delta_y;
	float model_delta_z;
	float sx;
	float sy;
	float sz;
	float view_theta_z;
	float view_theta_x;
	float view_theta_y;
	float model_theta_z;
	float model_theta_x;
	float model_theta_y;
	float theta;
	float aspect;
	float far;
	float near;
	float m_width;
	float m_height;
	float m_width_aspect;
	float m_height_aspect;

};
