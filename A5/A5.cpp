#include "A5.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "Ball.hpp"
#include "explosive.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <ctime>
#include <cstdlib>



using namespace glm;
using namespace irrklang;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

int totalNodes;

unsigned int * picked_Id;
unsigned int * Joint_children;
mat4x4 I = mat4x4(vec4(1, 0, 0, 0),
				  vec4(0, 1, 0, 0),
				  vec4(0, 0, 1, 0),
				  vec4(0, 0, 0, 1));

//----------------------------------------------------------------------------------------
// Constructor
A5::A5(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{
	circle_enable = false;
	z_buffer_enable = true;
	Backface_culling_enable = false; 
	Frontface_culling_enable = false;
	picking = false;
	current_mode = 0;
	mouseState[0] = 0;
	mouseState[1] = 0;
	mouseState[2] = 0;
	totalNodes = 1;
	arcBall_xPos = m_framebufferWidth/2.0f;
	arcBall_yPos = m_framebufferHeight/2.0f;
	joint_rotate_x = 0;
	joint_rotate_y = 0;
	fire = 0;
	arm_angle = 0;
	v = 10.0f;
	w = 0.0f;
	g = -9.8f;
	arm_angle_in_degree = arm_angle / PI * 180.0f;
	soundEngine = irrklang::createIrrKlangDevice();

}

//----------------------------------------------------------------------------------------
// Destructor
A5::~A5()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A5::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj"),
			getAssetFilePath("cylender.obj"),
			getAssetFilePath("pin.obj"),
			getAssetFilePath("mesh.obj"),
			getAssetFilePath("plane.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A5::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
	initRootTrans = m_rootNode->get_transform();
	totalNodes = m_rootNode->totalSceneNodes();
	picked_Id = new unsigned int[totalNodes];
	Joint_children = new unsigned int[totalNodes];
	for (int i = 0; i < totalNodes; ++i){
		picked_Id[i] = 0;
	}
	for (int i = 0; i < totalNodes; ++i){
		Joint_children[i] = 0;
	}
	std::map<int, glm::vec2> Joint_Record;
	getNumberOfJointNode(*m_rootNode, & Joint_Record);
	undo_stack.push_back(Joint_Record);
}

int A5::getNumberOfJointNode(const SceneNode & root, std::map<int, glm::vec2> * ptr_Joint_Record){
	int ret = 0;
	if (root.m_nodeType == NodeType::JointNode){
		ret++;
		(*ptr_Joint_Record)[root.m_nodeId] = vec2(0,0);
	}
	for (SceneNode * node : root.children){
		ret += getNumberOfJointNode(*node, ptr_Joint_Record);
	}
	return ret;
}
//----------------------------------------------------------------------------------------
void A5::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A5::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A5::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2 * CIRCLE_PTS * sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A5::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A5::initPerspectiveMatrix()
{

	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	float aspect = ((float)w) / h;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A5::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 5.0f, 30.0f), vec3(0.0f, 0.0f, -1.0f),
	// m_view = glm::lookAt(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}
// 

//----------------------------------------------------------------------------------------
void A5::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A5::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.9f);
	ImGui::Begin("A5", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
    	if( ImGui::Button( "Reset All (A)" ) ) {
    		cout << "Reset All" << endl;
    		resetAll();
    	}
    	if( ImGui::Button( "Quit Application (Q)" ) ) {
    		cout << "Quit Application" << endl;
    		glfwSetWindowShouldClose(m_window, GL_TRUE);
    	}
		ImGui::Checkbox( "Circle (C)", &circle_enable );
		ImGui::Checkbox( "Z-buffer (Z)", &z_buffer_enable );
		ImGui::SliderFloat("init v", &v, 0, 100, "%.0f");
		ImGui::SliderFloat("init w", &w, -100, 100, "%.0f");
		ImGui::SliderFloat("init g", &g, -100, 100, "%.0f");
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms( const ShaderProgram & shader,
								  const GeometryNode & node,
							      const glm::mat4 & viewMatrix ) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView;
		if (node.m_nodeType == NodeType::GeometryNode || node.m_nodeType == NodeType::Ball || node.m_nodeType == NodeType::Explosive){
			modelView = viewMatrix * node.parent_trans * node.trans * node.rotate_trans * node.scale_trans;
		} else {
			modelView = viewMatrix * node.trans  * node.rotate_trans * node.scale_trans;
		}
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		if (node.m_nodeType == NodeType::GeometryNode || node.m_nodeType == NodeType::Ball || node.m_nodeType == NodeType::Explosive){
			location = shader.getUniformLocation("material.kd");
			vec3 kd;
			if (picked_Id[node.m_nodeId] == 1){
				kd = node.Picked_material.kd;	
			} else {
				kd = node.material.kd;
			}
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks;
			if (picked_Id[node.m_nodeId] == 1){
				ks = node.Picked_material.ks;	
			} else {
				ks = node.material.ks;
			}
			glUniform3fv(location, 1, value_ptr(ks)); 
			
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, node.material.shininess);
			CHECK_GL_ERRORS;
			
		}

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
 // int temp = 0;
void A5::draw() {
	if (z_buffer_enable){
		glEnable( GL_DEPTH_TEST );
	}
	enableCulling(true);

	renderSceneGraph(*m_rootNode);

	glCullFace(GL_FRONT);
	if (z_buffer_enable){
		glDisable( GL_DEPTH_TEST );
	}
	
	renderArcCircle();

}

void A5::enableCulling(bool on){
	if (Backface_culling_enable || Frontface_culling_enable){
		glEnable(GL_CULL_FACE);
		if (Backface_culling_enable && Frontface_culling_enable){
			glCullFace(GL_FRONT_AND_BACK);
		} else if (Backface_culling_enable){
			glCullFace(GL_BACK);
		} else if (Frontface_culling_enable){
			glCullFace(GL_FRONT);
		}
	} else {
		glDisable(GL_CULL_FACE);
	}
}
//----------------------------------------------------------------------------------------
void A5::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	renderSceneNode(root);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A5::renderSceneNode(const SceneNode & root){
	
	totalNodes = root.totalSceneNodes();
	const GeometryNode * geometryNode = static_cast <const GeometryNode *>(& root);

	updateShaderUniforms(m_shader, *geometryNode, m_view);


	for (SceneNode * node : root.children){
		if (node->m_nodeType == NodeType::GeometryNode){
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.parent_trans * root.get_transform() * root.get_rotation() );
			renderGeomeNode(*node);
		} else if (node->m_nodeType == NodeType::Ball){
			 renderBall(*node, root.parent_trans * root.get_transform() * root.get_rotation());
		} else {
			((JointNode *)node)->JointNode::set_transform_from_parent(root.parent_trans* root.get_transform() * root.get_rotation() );
			renderJointNode(*node);
		}
	}
	joint_rotate_x = 0;
	joint_rotate_y = 0;
}
void A5::renderGeomeNode(const SceneNode & root){
	

	const GeometryNode * geometryNode = static_cast <const GeometryNode *>(& root);
	updateShaderUniforms(m_shader, *geometryNode, m_view);
	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

	//-- Now render the mesh:
	m_shader.enable();
	glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
	m_shader.disable();

	for (SceneNode * node : root.children){
		if (node->m_nodeType == NodeType::GeometryNode){
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.parent_trans * root.get_transform() * root.get_rotation() );
			renderGeomeNode(*node);
		} else if (node->m_nodeType == NodeType::Ball){
			 renderBall(*node, root.parent_trans * root.get_transform() * root.get_rotation());
		} else {
			((JointNode *)node)->JointNode::set_transform_from_parent(root.parent_trans* root.get_transform() * root.get_rotation() );
			renderJointNode(*node);
		}
	}
}

void A5::renderBall(const SceneNode & root, mat4x4 trans){
	const GeometryNode * geometryNode = static_cast <const GeometryNode *>(& root);
	const Ball * ball = static_cast <const Ball *>(& root);

	if (((Ball *) ball)->ball_state == BallState::Waiting){
		((GeometryNode *)geometryNode)->GeometryNode::set_transform_from_parent( trans );
	} 

	if (fire > 0 && ball->ball_state == BallState::Waiting){
		((Ball *)ball)->init_fly(arm_angle, g, v, w);
		fire = 0;
	}
	
	if (ball->ball_state == BallState::Flying){
		mat4x4 Translation = ((Ball *)ball)->fly();
		((Ball *)ball)->GeometryNode::set_transform_from_parent(root.parent_trans * Translation);	
	}
	if (ball->ball_state == BallState::Contact){
		if (((Ball *)ball)->Contact_time == 0){
			std::string explosion_music = "Assets/explosion.mp3";
			soundEngine->play2D(explosion_music.c_str(), false);
		}
		if (((Ball *)ball)->Contact_time < 50){
			for (SceneNode * node : root.children){
				renderExplosive(*node, root.parent_trans * root.get_transform() * root.get_rotation(), ((Ball *)ball)->Contact_time);
			}
			((Ball *)ball)->Contact_time++;
		} else {
			((Ball *)ball)->Contact_time = 0;
			((Ball *)ball)->ball_state = BallState::Waiting;	
		}

		
	}

	if (ball->ball_state == BallState::Flying){
		updateShaderUniforms(m_shader, *geometryNode, m_view);
		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
		m_shader.disable();
	}
}

void A5::renderExplosive(const SceneNode & root, mat4x4 trans, float Contact_time){
	const GeometryNode * geometryNode = static_cast <const GeometryNode *>(& root);
	const Explosive * explosive = static_cast <const Explosive *>(& root);
	((GeometryNode *)geometryNode)->GeometryNode::set_transform_from_parent( trans );
	int particalCount = 500;
	// std::srand(std::time(0));
	// cout << Contact_time << endl;
	for (int i = 0; i < particalCount; i++){
		 int random_a = std::rand() % 720 - 360;
		 // std::cout << random_g <<" "<< random_v <<" "<< random_w <<" "<< endl;
		 // cout << i << endl;
		((Explosive *)explosive)->init_fly(random_a, i, Contact_time);
		
		mat4x4 Translation = ((Explosive *)explosive)->fly();
		mat4x4 Rotation = ((Explosive *)explosive)->rotation_mesh();
		((Explosive *)explosive)->SceneNode::set_rotation(Rotation);
		((Explosive *)explosive)->GeometryNode::set_transform_from_parent(root.parent_trans * Translation );	

		updateShaderUniforms(m_shader, *geometryNode, m_view);
		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
		m_shader.disable();
	}
}

void A5::renderJointNode(const SceneNode & root){
	const JointNode * jointNode = static_cast <const JointNode *>(& root);
	for (int i = 0; i < m_rootNode->totalSceneNodes(); i++){
		((JointNode * )jointNode)->rotate_x(joint_rotate_x);
		((JointNode * )jointNode)->rotate_y(joint_rotate_y);
		arm_angle = ((JointNode * )jointNode)->getCurrentX();
	}
	for (SceneNode * node : root.children){
		if (node->m_nodeType == NodeType::GeometryNode){
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.parent_trans * root.get_transform() * root.get_rotation() );
			renderGeomeNode(*node);
		} else if (node->m_nodeType == NodeType::Ball){
			renderBall(*node, root.parent_trans * root.get_transform() * root.get_rotation());
		} else {
			((JointNode *)node)->JointNode::set_transform_from_parent(root.parent_trans* root.get_transform() * root.get_rotation() );
			renderJointNode(*node);
		}
	}
}
//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A5::renderArcCircle() {
	if (circle_enable == true){
		glBindVertexArray(m_vao_arcCircle);
		m_shader_arcCircle.enable();
			GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
			int w, h;
			glfwGetWindowSize(m_window, &w, &h);
			float aspect = float(w)/float(h);
			glm::mat4 M;
			if( aspect > 1.0 ) {
				M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
			} else {
				M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
			}
			glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
			glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
		m_shader_arcCircle.disable();

		glBindVertexArray(0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A5::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	double deltaX; 
	double deltaY; 
	double deltaZ;
	deltaX = (xPos - last_xPos) / m_windowWidth;
	deltaY = (yPos - last_yPos) / m_windowHeight;
	deltaZ = deltaY;
	// Fill in with event handling code...
	if ( !ImGui::IsMouseHoveringAnyWindow() ){
		if (mouseState[1] == 1){ //right click

			if (current_mode == 0){

			    glm::vec3 p = get_arcball_vector(last_xPos, last_yPos);
			    glm::vec3 d = get_arcball_vector(xPos, yPos);
			    float angleAtView = -acos(std::min(1.0f, dot(p, d))) * 0.1f;
			    glm::vec3 a = cross(p, d);
			    glm::vec4 axisAtWorld = glm::inverse(m_view) * vec4(a.x, a.y, a.z, 0.0f);
			    m_rootNode->set_rotation(glm::rotate( m_rootNode->get_rotation(),
								   					glm::degrees(angleAtView),
								   					{ axisAtWorld.x, -axisAtWorld.y, axisAtWorld.z}));
			}
		}
		if (mouseState[0] == 1){ // left click
			if (current_mode == 0){
				setTrans(vec3(deltaX, -deltaY, 0), vec3(0,0,0));
			}
		}

		if (mouseState[2] == 1){ //middle
			if (current_mode == 0){
				setTrans(vec3(0, 0, deltaZ),vec3(0,0,0));
			} 
		}
	}
	last_xPos = xPos;
	last_yPos = yPos;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A5::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	mouseState[button] = actions;

	if (! ImGui::IsMouseHoveringAnyWindow()){
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
	if (mouseState[0] == 1){
		
	}

	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q ) {
			cout << "q key pressed" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_C ) {
			cout << "c key pressed" << endl;
			circle_enable = circle_enable ? false : true;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Z ) {
			cout << "z key pressed" << endl;
			z_buffer_enable = z_buffer_enable ? false : true;
			eventHandled = true;
		}
		if (key == GLFW_KEY_A ) {
			cout << "a key pressed" << endl;
			resetAll();
			eventHandled = true;
		}
		if (key == GLFW_KEY_RIGHT ){
			joint_rotate_y = -0.01 * PI * 2;
			eventHandled = true;
		} 
		if (key == GLFW_KEY_LEFT ){
			joint_rotate_y = 0.01 * PI * 2;
			eventHandled = true;
		} 
		if (key == GLFW_KEY_UP ){
			joint_rotate_x = -0.001 * PI*2;
			eventHandled = true;
		} 
		if (key == GLFW_KEY_DOWN ){
			joint_rotate_x = 0.001 * PI*2;
			eventHandled = true;
		}
		if (key == GLFW_KEY_SPACE ){
			fire = 1;
			eventHandled = true;
		} 

	}
	// Fill in with event handling code...

	return eventHandled;
}

//gui functions
//applications
void A5::resetPosition(){
	m_rootNode->set_transform(initRootTrans);
}
void A5::resetOrientation(){
	m_rootNode->set_rotation(I);

}
void A5::resetJoints(){
	resetJoints_re(*m_rootNode);
	reset_stacks();
}
void A5::resetJoints_re(const SceneNode & root){
	if (root.m_nodeType == NodeType::JointNode){
		const JointNode * jointNode = static_cast <const JointNode *>(& root);
		((JointNode *) jointNode)->reset_angle_amount();
		((JointNode *) jointNode)->upDateRotation();
	}
	for (SceneNode * node : root.children){
		resetJoints_re(*node);
	}
}

void A5::resetAll(){
	resetPosition();
	resetOrientation();
	resetJoints();
}
//edit
void A5::redo(){
	if(!redo_stack.empty()){
		std::map<int, glm::vec2> poped_stack = redo_stack.back();
		undo_stack.push_back(poped_stack);
		redo_stack.pop_back();
		updateTree(*m_rootNode, & poped_stack);
	}
}
void A5::undo(){
	if (undo_stack.size() == 1){
		// resetJoints();
		std::map<int, glm::vec2> poped_stack = undo_stack.back();
		updateTree(*m_rootNode, & poped_stack);
	} else {
		std::map<int, glm::vec2> poped_stack = undo_stack.back();
		redo_stack.push_back(poped_stack);
		undo_stack.pop_back();
		poped_stack = undo_stack.back();
		updateTree(*m_rootNode, & poped_stack);
	}
}
void A5::updateTree(const SceneNode & root, std::map<int, glm::vec2> * ptr_Joint_Record){
	if (root.m_nodeType == NodeType::JointNode){
		const JointNode * jointNode = static_cast <const JointNode *>(& root);
		((JointNode *)jointNode)->upDate_x((*ptr_Joint_Record)[root.m_nodeId].x);
		((JointNode *)jointNode)->upDate_y((*ptr_Joint_Record)[root.m_nodeId].y);
		((JointNode *)jointNode)->upDateRotation();
	}
	for (SceneNode * node : root.children){
		updateTree(*node, ptr_Joint_Record);
	}
}
void A5::pickingMode(int trager){

	m_shader.enable();
	GLint joint = m_shader.getUniformLocation("joint");
	glUniform1i(joint, trager);
	CHECK_GL_ERRORS;
	m_shader.disable();
}

unsigned int A5::lookingUpId(unsigned char * data){
	int id = data[0] + 
			 data[1] * 256 +
			 data[2] * 256 * 256;
	if (id/m_rootNode->totalSceneNodes() > 1){
		return 0;
	}
	return id;
}

void A5::setTrans(vec3 translation, vec3 rotation){
mat4x4 Translation = mat4x4( vec4(1, 0, 0, 0), 
							 vec4(0, 1, 0, 0), 
							 vec4(0, 0, 1, 0), 
							 vec4(translation.x, translation.y, translation.z, 1));
mat4x4 arcTranslation = mat4x4( vec4(1, 0, 0, 0), 
							 vec4(0, 1, 0, 0), 
							 vec4(0, 0, 1, 0), 
							 vec4(translation.x, translation.y, 0, 1));
mat4x4 Rotation_z = mat4x4 (vec4(cos(rotation.z), sin(rotation.z),0 ,0),
							vec4(-sin(rotation.z), cos(rotation.z), 0, 0),
							vec4(0,0,1,0),
							vec4(0,0,0,1) );
mat4x4 Rotation_y = mat4x4 ( vec4(cos(rotation.y), 0, -sin(rotation.y), 0),
							  vec4(0, 1, 0, 0),
							  vec4(sin(rotation.y), 0, cos(rotation.y), 0),
							  vec4(0,0,0,1) );
mat4x4 Rotation_x = mat4x4 (vec4(1, 0, 0, 0),
							 vec4(0, cos(rotation.x), sin(rotation.x), 0),
							 vec4(0, -sin(rotation.x), cos(rotation.x), 0),
							 vec4(0,0,0,1) );

m_rootNode->set_transform( m_rootNode->get_transform() * Rotation_z);
m_rootNode->set_transform(  m_rootNode->get_transform() * Rotation_y);
m_rootNode->set_transform(  m_rootNode->get_transform() * Rotation_x);
m_rootNode->set_transform( Translation * m_rootNode->get_transform());
}

//sample code from https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
/**
 * Get a normalized vector from the center of the virtual ball O to a
 * point P on the virtual ball surface, such that P is aligned on
 * screen's (X,Y) coordinates.  If (X,Y) is too far away from the
 * sphere, return the nearest point on the virtual ball surface.
 */
glm::vec3 A5::get_arcball_vector(float x, float y) {
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	glm::vec3 P = glm::vec3(x/w * 2 - 1.0, y / h * 2 - 1.0, 0);
	  
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1*1)
	    P.z = sqrt(1*1 - OP_squared);  // Pythagore
	else
	    P = glm::normalize(P);  // nearest point
	return P;
}
void A5::deSelect(){
	for (int i = 0; i < m_rootNode->totalSceneNodes(); i++){
		picked_Id[i] = 0;	
	}
	
}

void A5::add_to_stack_undo(const SceneNode & root, std::map<int, glm::vec2> * ptr_Joint_Record){
	if (root.m_nodeType == NodeType::JointNode){
		const JointNode * jointNode = static_cast <const JointNode *>(& root);
		(*ptr_Joint_Record)[root.m_nodeId] = vec2(((JointNode *)jointNode)->current_X, ((JointNode *)jointNode)->current_Y);
	}
	for (SceneNode * node : root.children){
		add_to_stack_undo(*node, ptr_Joint_Record);
	}
}

void A5::reset_stacks(){
	std::map<int, glm::vec2> poped_stack = undo_stack.front();
	undo_stack.clear();
	undo_stack.push_back(poped_stack);
	redo_stack.clear();
}
// void A5::add_to_stack_redo(const SceneNode & root){

// }