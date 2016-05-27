#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

int current_mode = 0;
const float PI = 3.14159265f;

float view_delta_x = 0;
float view_delta_y = 0;
float view_delta_z = 0;
float model_delta_x = 0;
float model_delta_y = 0;
float model_delta_z = 0;
float sx = 1;
float sy = 1;
float sz = 1;
float view_theta_z = 0;
float view_theta_x = 0;
float view_theta_y = 0;
float model_theta_z = 0;
float model_theta_x = 0;
float model_theta_y = 0;

mat4x4 I = mat4x4(vec4(1, 0, 0, 0),
				  vec4(0, 1, 0, 0),
				  vec4(0, 0, 1, 0),
				  vec4(0, 0, 0, 1));

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.reserve(kMaxVertices);
	colours.reserve(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f)), delta_right(0), old_x_position(0), old_y_position(0)
{	
	//set cube 
	cube_structure = new vec4 * [12];
	for (int i = 0; i < 12; ++i){
		cube_structure[i] = new vec4[2];
	}
	cube_structure[0][0] = vec4(-0.25f, -0.25f, -0.25f, 1.0f);
	cube_structure[0][1] = vec4(-0.25f, -0.25f, 0.25f, 1.0f);
	cube_structure[1][0] = vec4(-0.25f, -0.25f, -0.25f, 1.0f);
	cube_structure[1][1] = vec4(-0.25f, 0.25f, -0.25f, 1.0f);
	cube_structure[2][0] = vec4(-0.25f, -0.25f, -0.25f, 1.0f);
	cube_structure[2][1] = vec4(0.25f, -0.25f, -0.25f, 1.0f);
	cube_structure[3][0] = vec4(0.25f, 0.25f, 0.25f, 1.0f);
	cube_structure[3][1] = vec4(-0.25f, 0.25f, 0.25f, 1.0f);
	cube_structure[4][0] = vec4(0.25f, 0.25f, 0.25f, 1.0f);
	cube_structure[4][1] = vec4(0.25f, -0.25f, 0.25f, 1.0f);
	cube_structure[5][0] = vec4(0.25f, 0.25f, 0.25f, 1.0f);
	cube_structure[5][1] = vec4(0.25f, 0.25f, -0.25f, 1.0f);
	cube_structure[6][0] = vec4(-0.25f, 0.25f, -0.25f, 1.0f);
	cube_structure[6][1] = vec4(-0.25f, 0.25f, 0.25f, 1.0f);
	cube_structure[7][0] = vec4(-0.25f, 0.25f, -0.25f, 1.0f);
	cube_structure[7][1] = vec4(0.25f, 0.25f, -0.25f, 1.0f);
	cube_structure[8][0] = vec4(0.25f, -0.25f, -0.25f, 1.0f);
	cube_structure[8][1] = vec4(0.25f, 0.25f, -0.25f, 1.0f);
	cube_structure[9][0] = vec4(0.25f, -0.25f, -0.25f, 1.0f);
	cube_structure[9][1] = vec4(0.25f, -0.25f, 0.25f, 1.0f);
	cube_structure[10][0] = vec4(-0.25f, -0.25f, 0.25f, 1.0f);
	cube_structure[10][1] = vec4(-0.25f, 0.25f, 0.25f, 1.0f);
	cube_structure[11][0] = vec4(-0.25f, -0.25f, 0.25f, 1.0f);
	cube_structure[11][1] = vec4(0.25f, -0.25f, 0.25f, 1.0f);
	
	//cood
	x_axis[0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	x_axis[1] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	y_axis[0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	y_axis[1] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	z_axis[0] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	z_axis[1] = vec4(0.0f, 0.0f, 1.0f, 1.0f);


	M = I;
	V = I;
	P = I;

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{
	//set cube 
	
	for (int i = 0; i < 12; ++i){
		delete [] cube_structure[i];
	}
	delete [] cube_structure;

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);


	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
 

}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();
	// set up transformed data
	vec4 changed_cube_structure[12][2];
	vec4 changed_x[2] = { x_axis[0], x_axis[1] };
	vec4 changed_y[2] = { y_axis[0], y_axis[1] };
	vec4 changed_z[2] = { z_axis[0], z_axis[1] };
	//init Transformations


	mat4x4 viewTranslation = mat4x4( vec4(1, 0, 0, 0), 
									 vec4(0, 1, 0, 0), 
									 vec4(0, 0, 1, 0), 
									 vec4(view_delta_x, view_delta_y, view_delta_z, 1));

	mat4x4 modelTranslation = mat4x4( vec4(1, 0, 0, 0), 
									  vec4(0, 1, 0, 0), 
									  vec4(0, 0, 1, 0), 
									  vec4(model_delta_x, model_delta_y, model_delta_z, 1));

	mat4x4 modelScale = mat4x4( vec4(sx, 0, 0, 0),
							    vec4(0, sy, 0, 0),
							    vec4(0, 0, sz, 0),
							    vec4(0, 0, 0, 1) );

	mat4x4 viewRotation_z = mat4x4 (vec4(cos(view_theta_z), sin(view_theta_z),0 ,0),
									vec4(-sin(view_theta_z), cos(view_theta_z), 0, 0),
									vec4(0,0,1,0),
									vec4(0,0,0,1) );
	mat4x4 modelRotation_z = mat4x4(vec4(cos(model_theta_z), sin(model_theta_z),0 ,0),
									vec4(-sin(model_theta_z), cos(model_theta_z), 0, 0),
									vec4(0,0,1,0),
									vec4(0,0,0,1) );

	mat4x4 viewRotation_x = mat4x4 (vec4(1, 0, 0, 0),
									vec4(0, cos(view_theta_x), sin(view_theta_x), 0),
									vec4(0, -sin(view_theta_x), cos(view_theta_x), 0),
									vec4(0,0,0,1) );
	mat4x4 modelRotation_x = mat4x4 (vec4(1, 0, 0, 0),
									vec4(0, cos(model_theta_x), sin(model_theta_x), 0),
									vec4(0, -sin(model_theta_x), cos(model_theta_x), 0),
									vec4(0,0,0,1) );

	mat4x4 viewRotation_y = mat4x4 (vec4(cos(view_theta_y), 0, -sin(view_theta_y), 0),
									vec4(0, 1, 0, 0),
									vec4(sin(view_theta_y), 0, cos(view_theta_y), 0),
									vec4(0,0,0,1) );
	mat4x4 modelRotation_y = mat4x4 (vec4(cos(model_theta_y), 0, -sin(model_theta_y), 0),
									vec4(0, 1, 0, 0),
									vec4(sin(model_theta_y), 0, cos(model_theta_y), 0),
									vec4(0,0,0,1) );
	
	// M = modelScale * M; 
	
	// M = modelRotation_z * M;
	// M = modelRotation_x * M;
	// M = modelRotation_y * M;

	// M = modelTranslation * M;
	// // V = viewScale * V;
	// V = V * viewTranslation;
	// V = viewRotation_z * V;
	// V = viewRotation_x * V;
	// V = viewRotation_y * V;
	M = M * modelTranslation;
	M = M * modelRotation_y;
	M = M * modelRotation_x;
	M = M * modelRotation_z;
	M = M * modelScale; 
	// V = viewScale * V;
	V = V * viewTranslation;
	V = V * viewRotation_y;
	V = V * viewRotation_x;
	V = V * viewRotation_z;
	
	
	// cout <<"before "<< V << endl;
	
	// cout <<"after "<< V << endl;
	// cout << changed_x[0] << endl;
	// cout << viewTranslation * changed_x[0] * V  << endl;
	// cout << V * changed_x[1] * viewTranslation << endl;

	for (int i = 0; i < 12; ++i){
		// changed_cube_structure[i][0] = cube_structure[i][0] * M * V * P; 
		// changed_cube_structure[i][1] = cube_structure[i][1] * M * V * P; 
		changed_cube_structure[i][0] = P * V * M * cube_structure[i][0]; 
		changed_cube_structure[i][1] = P * V * M * cube_structure[i][1]; 
	}
	

	//draw
	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	drawLine( vec2(P * V * changed_x[0]), 
			vec2(P * V * changed_x[1] ) );
	setLineColour(vec3(0.0f, 1.0f, 0.0f));
	drawLine( vec2(P * V * changed_y[0] ), 
			vec2(P * V * changed_y[1] ) );
	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	drawLine( vec2(P * V * changed_z[0] ), 
			vec2(P * V * changed_z[1] ) );

	setLineColour(vec3(0.2f, 1.0f, 1.0f));
	for (int i = 0; i < 12; ++i){
		drawLine( vec2(changed_cube_structure[i][0]), 
			vec2(changed_cube_structure[i][1]) );
	}
	//reset 
	view_theta_x = 0;
	view_theta_y = 0;
	view_theta_z = 0;
	model_theta_x = 0;
	model_theta_y = 0;
	model_theta_z = 0;
	view_delta_x = 0;
	view_delta_y = 0;
	view_delta_z = 0;
	model_delta_x = 0;
	model_delta_y = 0;
	model_delta_z = 0;

	sx = 1;
	sz = 1;
	sy = 1;

}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		//quit button short cut Q
		if( ImGui::Button( "Quit Application" ) ) {
			cout << "Goodbye!" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		//reset button short cut R
		if( ImGui::Button( "Reset Application" ) ) {
			cout << "Application reseted" << endl;
			M = I;
			V = I;
			P = I;
			current_mode = 0;

		}
	
		if( ImGui::RadioButton( "Rotate View", &current_mode, 0 ) ) {
			cout << "Rotate View" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Translate View", &current_mode, 1 ) ) {
			cout << "Translate View" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Perspective", &current_mode, 2 ) ) {
			cout << "Perspective" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Rotate Model", &current_mode, 3 ) ) {
			cout << "Rotate Model" << current_mode << endl;	
		}

		if( ImGui::RadioButton( "Translate Model", &current_mode, 4 ) ) {
			cout << "Translate Model" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Scale Model", &current_mode, 5 ) ) {
			cout << "Scale Model" << current_mode << endl;	
		}

		if( ImGui::RadioButton( "Viewport", &current_mode, 6 ) ) {
			cout << "Viewport" << current_mode << endl;
		}





		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (ImGui::GetIO().MouseDown[0] 
			|| ImGui::GetIO().MouseDown[1] 
			|| ImGui::GetIO().MouseDown[2]
		){ 
			//some mouse(s) dragging
			cout << "some mouse(s) dragging: "<< ImGui::GetIO().MouseDown[0] + ImGui::GetIO().MouseDown[1] + ImGui::GetIO().MouseDown[2] << endl;
			
			if (current_mode == 0){
				//rotate view
				delta_right = 5 * (xPos - old_x_position) *  PI / m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					view_theta_x = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					view_theta_z = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					view_theta_y = delta_right;
				}
			} else if (current_mode == 1){
				//translate view
				delta_right = 5 * (xPos - old_x_position) / m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					view_delta_x = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					view_delta_z = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					view_delta_y = delta_right;
				}
			} else if (current_mode == 2){
				//Perspective
				// delta_right += (xPos - old_x_position) *  PI / m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					// view_theta_x = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					// view_theta_z = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					// view_theta_y = delta_right;
				}
			} else if (current_mode == 3){
				//Rotate Model
				delta_right = 5 * (xPos - old_x_position) *  PI / m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					model_theta_x = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					model_theta_z = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					model_theta_y = delta_right;
				}
			} else if (current_mode == 4){
				//Translate Model
				delta_right = 5 * (xPos - old_x_position) / m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					model_delta_x += delta_right;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					model_delta_z += delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					model_delta_y += delta_right;
				}
			} else if (current_mode == 5){
				//Scale Model
				delta_right = (xPos - old_x_position) /m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					sx += delta_right;
					cout << sx << endl;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					sz += delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					sy += delta_right;
				}
			} else if (current_mode == 6){
				//ViewPort
				delta_right = 5 * (xPos - old_x_position) *  PI / m_windowWidth;//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					model_theta_x = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					model_theta_z = delta_right;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					model_theta_y = delta_right;
				}
			}
			
		}	
	}
	old_x_position = xPos;
	old_y_position = yPos;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
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
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q ) {
			cout << "Q Keyboard pressed" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}

	}

	// Fill in with event handling code...

	return eventHandled;
}


