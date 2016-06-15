#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

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
A3::A3(const std::string & luaSceneFile)
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
	z_buffer_enable = false;
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
	rootRotation = I;
	joint_rotate_x = 0;
	joint_rotate_y = 0;
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
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
			getAssetFilePath("pin.obj")
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
void A3::processLuaSceneFile(const std::string & filename) {
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
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
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
void A3::enableVertexShaderInputSlots()
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
void A3::uploadVertexDataToVbos (
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

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
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
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}
// 

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
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
void A3::appLogic()
{

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
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
	float opacity(0.5f);

	ImGui::Begin("Application", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		if( ImGui::Button( "Reset Position (I)" ) ) {
			cout << "Reset Position" << endl;
			resetPosition();
		}	
		if( ImGui::Button( "Reset Orientation (O)" ) ) {
			cout << "Reset Orientation" << endl;
			resetOrientation();
		}
		if( ImGui::Button( "Reset Joints (N)" ) ) {
			cout << "Reset Joints" << endl;
			resetJoints();
		}
		if( ImGui::Button( "Reset All (A)" ) ) {
			cout << "Reset All" << endl;
			resetAll();
		}
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			cout << "Quit Application" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	ImGui::Begin("Edit", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		if( ImGui::Button( "Undo (U)" ) ) {
			cout << "Undo" << endl;
		}
		if( ImGui::Button( "Redo (R)" ) ) {
			cout << "Redo" << endl;
		}

	ImGui::End();
	ImGui::Begin("Options", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);

		
		ImGui::Checkbox( "Circle (C)", &circle_enable );
		ImGui::Checkbox( "Z-buffer (Z)", &z_buffer_enable );
		ImGui::Checkbox( "Backface culling (B)", &Backface_culling_enable );
		ImGui::Checkbox( "Frontface culling (F)", &Frontface_culling_enable );
		if( ImGui::RadioButton( "Position/Orientation (P)", &current_mode, 0 ) ) {
			cout << "Position/Orientation: " << current_mode << endl;
			deSelect();	
		}
		if( ImGui::RadioButton( "Joints (J)", &current_mode, 1 ) ) {
			cout << "Joints: " << current_mode << endl;	
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms( const ShaderProgram & shader,
								  const GeometryNode & node,
							      const glm::mat4 & viewMatrix) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView;
		if (node.m_nodeType == NodeType::GeometryNode){
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
		if (node.m_nodeType == NodeType::GeometryNode){
			location = shader.getUniformLocation("material.kd");
			vec3 kd;
			if (picked_Id[node.m_nodeId] == 1){
				// std::cout << Picked_material.kd.x << std::endl;
				kd = node.Picked_material.kd;	
			} else {
				kd = node.material.kd;
			}
			glUniform3fv(location, 1, value_ptr(kd));
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.ks");
			vec3 ks = node.material.ks;
			glUniform3fv(location, 1, value_ptr(ks));
			
			CHECK_GL_ERRORS;
			location = shader.getUniformLocation("material.shininess");
			glUniform1f(location, node.material.shininess);
			CHECK_GL_ERRORS;
			//
			// 
		}

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
 // int temp = 0;
void A3::draw() {
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

void A3::enableCulling(bool on){
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
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	renderSceneNode(root);

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

void A3::renderSceneNode(const SceneNode & root){
	
	totalNodes = root.totalSceneNodes();
	const GeometryNode * geometryNode = static_cast <const GeometryNode *>(& root);

	updateShaderUniforms(m_shader, *geometryNode, m_view);


	for (SceneNode * node : root.children){
		if (node->m_nodeType == NodeType::GeometryNode){
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.get_transform() * root.get_rotation() * root.get_scale());
			renderGeomeNode(*node);
		} else if (node->m_nodeType == NodeType::JointNode){
			((JointNode *)node)->JointNode::set_transform_from_parent(root.get_transform() * root.get_rotation() * root.get_scale());
			renderJointNode(*node);
		}
	}
}
void A3::renderGeomeNode(const SceneNode & root){
	

	const GeometryNode * geometryNode = static_cast <const GeometryNode *>(& root);
	// cout << geometryNode->m_name << endl;
	updateShaderUniforms(m_shader, *geometryNode, m_view);
	// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
	BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
	//-- Now render the mesh:

	m_shader.enable();
	// false colour setting
	GLint colour_location = m_shader.getUniformLocation("colour");
	// float r = (geometryNode->m_nodeId & 0x000 >> 0) / (float)totalNodes;// /255.0f;
	// float g = (geometryNode->m_nodeId  >> 1) / (float)totalNodes;// /255.0f;
	// float b = (geometryNode->m_nodeId  >> 2) / (float)totalNodes;// /255.0f;
	int r = (geometryNode->m_nodeId & 0x000000FF) >>  0;
	int g = (geometryNode->m_nodeId & 0x0000FF00) >>  8;
	int b = (geometryNode->m_nodeId & 0x00FF0000) >> 16;
	// cout << r << " " << g << " " << b << endl;
	glUniform4f(colour_location, r/255.0f, g/255.0f, b/255.0f, 1.0f);
	CHECK_GL_ERRORS;
	
	glDrawArrays( GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices );
	m_shader.disable();

	for (SceneNode * node : root.children){
		if (node->m_nodeType == NodeType::GeometryNode){
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.parent_trans * root.get_transform() * root.get_rotation() );
			renderGeomeNode(*node);
		} else {
			((JointNode *)node)->JointNode::set_transform_from_parent(root.parent_trans* root.get_transform() * root.get_rotation() );
			renderJointNode(*node);
		}
	}

}
void A3::renderJointNode(const SceneNode & root){
	const JointNode * jointNode = static_cast <const JointNode *>(& root);
	for (int i = 0; i < m_rootNode->totalSceneNodes(); i++){
		if (Joint_children[i] == root.m_nodeId){
			if (picked_Id[i] == 1){
				((JointNode * )jointNode)->rotate_x(joint_rotate_x);
				((JointNode * )jointNode)->rotate_y(joint_rotate_y);
				joint_rotate_x = 0;
				joint_rotate_y = 0;
			}
		}
	}
	for (SceneNode * node : root.children){

		if (node->m_nodeType == NodeType::GeometryNode){
			Joint_children[node->m_nodeId] = root.m_nodeId;
			const GeometryNode * geometryNode = static_cast <const GeometryNode *>(node);
			// if (geometryNode->m_name == "head") {
			// 	if (headNode != NULL){
			// 		headNode = (SceneNode *)&root;
			// 		headRotateTrans = root.get_transform();	
			// 	}
			// }
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.parent_trans * root.get_transform() * root.get_rotation() );
			renderGeomeNode(*node);
		} else if (node->m_nodeType == NodeType::JointNode){
			((GeometryNode *)node)->GeometryNode::set_transform_from_parent(root.parent_trans * root.get_transform() * root.get_rotation() );
			renderJointNode(*node);
		}
	}
}
//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	if (circle_enable == true){
		glBindVertexArray(m_vao_arcCircle);
		m_shader_arcCircle.enable();
			GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
			float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
			glm::mat4 M;
			if( aspect > 1.0 ) {
				M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
			} else {
				M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
			}
			// M = arcTrans * M;
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
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
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
			    float angleInView = std::min(1.0f,acos(dot(p, d))) * 0.1;
			    glm::vec3 a = p * d;
			    // a = glm::normalize(a);
			    glm::vec4 axisInWorldframe = glm::inverse(m_view) * vec4(a, 0);
			    m_rootNode->set_rotation(glm::rotate( m_rootNode->get_rotation(),
								   					glm::degrees(angleInView),
								   					{ axisInWorldframe.x, -axisInWorldframe.y, axisInWorldframe.z}));

			}if (current_mode == 1){
				joint_rotate_x = deltaX * PI;
				joint_rotate_y = deltaY * PI;

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
			if (current_mode == 1){
				//rotate head
				// rotateHead(deltaX);
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
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	mouseState[button] = actions;
	if (! ImGui::IsMouseHoveringAnyWindow()){
	if (mouseState[0] == 1){
		if (current_mode == 1){
			if (mouseState[1] == 0){
				pickingMode(1);

				glClear(GL_COLOR_BUFFER_BIT);
				glClear(GL_DEPTH_BUFFER_BIT);

				// glEnable( GL_DEPTH_TEST );
				if (z_buffer_enable){
					glEnable( GL_DEPTH_TEST );
				}
				renderSceneGraph(*m_rootNode);
				// glDisable( GL_DEPTH_TEST );
				if (z_buffer_enable){
					glDisable( GL_DEPTH_TEST );
				}
				unsigned char data[4];
				glReadPixels(last_xPos, m_windowHeight - last_yPos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
				
				picked_Id[lookingUpId(data)] = picked_Id[lookingUpId(data)] == 1 ? 0 : 1;
				cout << picked_colour[0] << " " << picked_colour[1] << " " << picked_Id[2] << endl;
				cout << lookingUpId(data) << endl;

				pickingMode(0);
			}
			
		}	
	}
	if (mouseState[0] == 0){
		if (current_mode == 1){

			// pickingMode(1);
			// glReadPixels(last_xPos, m_windowHeight - last_yPos, 1, 1, GL_RGB, GL_FLOAT, &picked_colour);
			// cout <<"RGB " << picked_colour[0] <<" "<< picked_colour[1] <<" "<< picked_colour[2] << endl;
			// pickingMode(0);
		}	
	}
	// Fill in with event handling code...
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
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
bool A3::keyInputEvent (
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
			cout << "c key pressed" << endl;
			z_buffer_enable = z_buffer_enable ? false : true;
			eventHandled = true;
		}
		if (key == GLFW_KEY_B ) {
			cout << "b key pressed" << endl;
			Backface_culling_enable = Backface_culling_enable ? false : true;
			eventHandled = true;
		}
		if (key == GLFW_KEY_F ) {
			cout << "f key pressed" << endl;
			Frontface_culling_enable = Frontface_culling_enable ? false : true;
			eventHandled = true;
		}
		if (key == GLFW_KEY_P ) {
			cout << "p key pressed" << endl;
			current_mode = 0;
			deSelect();
			eventHandled = true;
		}
		if (key == GLFW_KEY_J ) {
			cout << "j key pressed" << endl;
			current_mode = 1;

			eventHandled = true;
		}
		if (key == GLFW_KEY_U ) {
			cout << "u key pressed" << endl;
			undo();
			eventHandled = true;
		}
		if (key == GLFW_KEY_R ) {
			cout << "r key pressed" << endl;
			redo();
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}

//gui functions
//applications
void A3::resetPosition(){
	cout << initRootTrans << endl;
	m_rootNode->set_transform(initRootTrans);
}
void A3::resetOrientation(){
	rootRotation = I;

}
void A3::resetJoints(){

}
void A3::resetAll(){
	resetPosition();
	resetOrientation();
	resetJoints();
}
//edit
void A3::redo(){

}
void A3::undo(){

}

void A3::pickingMode(int trager){

	m_shader.enable();
	GLint joint = m_shader.getUniformLocation("joint");
	glUniform1i(joint, trager);
	CHECK_GL_ERRORS;
	m_shader.disable();
}

unsigned int A3::lookingUpId(unsigned char * data){
	int id = data[0] + 
			 data[1] * 256 +
			 data[2] * 256 * 256;
	if (id/m_rootNode->totalSceneNodes() > 1){
		return 0;
	}
	return id;
}

void A3::setTrans(vec3 translation, vec3 rotation){
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
// M = M * modelRotation_y;
// M = M * modelRotation_x;
// M = M * modelRotation_z; 
// cout << m_rootNode->get_transform() << endl;
// cout << Translation << endl;
// arcTrans =  arcTranslation * arcTrans;
m_rootNode->set_transform( m_rootNode->get_transform() * Rotation_z);
m_rootNode->set_transform(  m_rootNode->get_transform() * Rotation_y);
m_rootNode->set_transform(  m_rootNode->get_transform() * Rotation_x);
m_rootNode->set_transform( Translation * m_rootNode->get_transform());
}
// void A3::rotateHead(double amount_x, double amount_y){

// }
//sample code from https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
/**
 * Get a normalized vector from the center of the virtual ball O to a
 * point P on the virtual ball surface, such that P is aligned on
 * screen's (X,Y) coordinates.  If (X,Y) is too far away from the
 * sphere, return the nearest point on the virtual ball surface.
 */
glm::vec3 A3::get_arcball_vector(int x, int y) {

	// int width, height;
	// glfwGetWindowSize(m_window, &width, &height);
	// glm::vec3 P = glm::vec3(x/width * 2 - 1.0, y / height * 2 - 1.0, 0);
  glm::vec3 P = glm::vec3(x/m_framebufferWidth*2.0f - 1.0,
			  y/m_framebufferHeight*2.0f - 1.0,
			  0);
  P.y = m_framebufferHeight - P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1*1)
    P.z = sqrt(1*1 - OP_squared);  // Pythagore
  else
    P = glm::normalize(P);  // nearest point
  return P;
}
void A3::deSelect(){
	for (int i = 0; i < m_rootNode->totalSceneNodes(); i++){
		picked_Id[i] = 0;	
	}
	
}
// glm::vec3 mapPlaneToSphere(double x, double y){
//     z = -1 + 2 * x;
//     phi = 2 * PI * y;
//     theta = asin(z);
//     return vector3(cos(theta) * cos(phi), 
//                    cos(theta) * sin(phi), 
//                    z);
// }