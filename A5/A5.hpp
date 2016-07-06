#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>

#include <map>
#include <vector>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A5 : public CS488Window {
public:
	A5(const std::string & luaSceneFile);
	virtual ~A5();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderArcCircle();
	//triverse the tree 
	void renderSceneNode(const SceneNode & root);
	void renderGeomeNode(const SceneNode & root);
	void renderBall(const SceneNode & root, glm::mat4x4 trans);
	void renderJointNode(const SceneNode & root);
	//gui functions
	//applications
	void resetPosition();
	void resetOrientation();
	void resetJoints();
	void resetJoints_re(const SceneNode & root);
	void resetAll();
	//edit
	void redo();
	void undo();
	int getNumberOfJointNode(const SceneNode & root, std::map<int, glm::vec2> * ptr_Joint_Record);
	void add_to_stack_undo(const SceneNode & root, std::map<int, glm::vec2> * ptr_Joint_Record);
	void updateTree(const SceneNode & root, std::map<int, glm::vec2> * ptr_Joint_Record);
	void reset_stacks();
	// void add_to_stack_redo(const SceneNode & root);
	//options
	void pickingMode(int trager);
	void setTrans(glm::vec3 translation, glm::vec3 rotation);
	glm::vec3 get_arcball_vector(float x, float y);
	void enableCulling(bool on);
	// void rotateHead(double amount_x, double amount_y);
	//helper
	unsigned int lookingUpId(unsigned char * data);
	void deSelect();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	//added information
	bool circle_enable;
	bool z_buffer_enable;
	bool Backface_culling_enable;
	bool Frontface_culling_enable;
	bool picking;
	int fire;


	int current_mode;
	glm::mat4 initRootTrans;
	// glm::mat4 arcTrans;
	int mouseState[3];
	double last_xPos;
	double last_yPos;
	double picking_xPos;
	double picking_yPos;
	double arcBall_xPos;
	double arcBall_yPos;
	double joint_rotate_x;
	double joint_rotate_y;
	float arm_angle;
	float v;
	float g;
	float arm_angle_in_degree;

	float picked_colour[3];
	// SceneNode * headNode;
	// int headNode_Id;
	glm::mat4 headRotateTrans;
	glm::mat4 rootRotation;
	std::vector<std::map<int, glm::vec2> > undo_stack;
	// std::vector<std::map<int, float> > undo_stack_y;
	std::vector<std::map<int, glm::vec2> > redo_stack;
	// std::vector<std::map<int, float> > redo_stack_y;
	
	// std::map<unsigned int, glm::vec3> colour_ids;
};
