#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

int current_mode = 3;
const float PI = 3.14159265f;
int draw_new_view_port = 0;


mat4x4 I = mat4x4(vec4(1, 0, 0, 0),
				  vec4(0, 1, 0, 0),
				  vec4(0, 0, 1, 0),
				  vec4(0, 0, 0, 1));

vec2 viewPort_startPoint;
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
	//init
	view_delta_x = 0;
	view_delta_y = 0;
	view_delta_z = 0;
	model_delta_x = 0;
	model_delta_y = 0;
	model_delta_z = 0;
	sx = 1;
	sy = 1;
	sz = 1;
	view_theta_z = 0;
	view_theta_x = 0;
	view_theta_y = 0;
	model_theta_z = 0;
	model_theta_x = 0;
	model_theta_y = 0;
	theta = PI/6;
	aspect = 1;
	far = 10;
	near = -1;


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
	V = inverse(mat4x4( vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, -1, 0), vec4(0, 0, 5, 1)));
	P = I;

	//view port
	viewPort_top[0] = vec2(-0.95,0.95);
	viewPort_top[1] = vec2(0.95, 0.95);
	viewPort_bot[0] = vec2(-0.95,-0.95);
	viewPort_bot[1] = vec2(0.95,-0.95);
	viewPort_left[0] = vec2(-0.95,0.95);
	viewPort_left[1] = vec2(-0.95, -0.95);
	viewPort_right[0] = vec2(0.95, 0.95);
	viewPort_right[1] = vec2(0.95, -0.95);
	m_width = 768;
	m_height = 689;
	m_width_aspect = (viewPort_top[1].x + 1) * m_width / 2 - (viewPort_top[0].x + 1) * m_width / 2;
	m_height_aspect = (viewPort_top[1].y + 1) * m_height / 2 - (viewPort_bot[1].y + 1) * m_height / 2;

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
	for (int i = 0; i < 12; ++i){ 
		changed_cube_structure[i][0] = cube_structure[i][0];
		changed_cube_structure[i][1] = cube_structure[i][1];
	}
	vec4 changed_x[2] = { x_axis[0], x_axis[1] };
	vec4 changed_y[2] = { y_axis[0], y_axis[1] };
	vec4 changed_z[2] = { z_axis[0], z_axis[1] };

	// aspect = m_width_aspect/m_height_aspect;
	aspect = m_width/m_height;
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

	mat4x4 viewRotation_x = mat4x4 ( vec4(1, 0, 0, 0),
									 vec4(0, cos(view_theta_x), sin(view_theta_x), 0),
									 vec4(0, -sin(view_theta_x), cos(view_theta_x), 0),
									 vec4(0,0,0,1) );
	mat4x4 modelRotation_x = mat4x4 (vec4(1, 0, 0, 0),
									 vec4(0, cos(model_theta_x), sin(model_theta_x), 0),
									 vec4(0, -sin(model_theta_x), cos(model_theta_x), 0),
									 vec4(0,0,0,1) );

	mat4x4 viewRotation_y = mat4x4 ( vec4(cos(view_theta_y), 0, -sin(view_theta_y), 0),
									 vec4(0, 1, 0, 0),
									 vec4(sin(view_theta_y), 0, cos(view_theta_y), 0),
									 vec4(0,0,0,1) );
	mat4x4 modelRotation_y = mat4x4 ( vec4(cos(model_theta_y), 0, -sin(model_theta_y), 0),
									  vec4(0, 1, 0, 0),
									  vec4(sin(model_theta_y), 0, cos(model_theta_y), 0),
									  vec4(0,0,0,1) );
	
	mat4x4 projection_z_plus = mat4x4 ( vec4((1/tan(theta/2.0f))/aspect, 0, 0, 0),
								   		vec4(0, (1/tan(theta/2.0f)), 0, 0),
								   		vec4(0, 0, 1, 1),
								   		vec4(0, 0, 1, 0)
								 );

	
	P = projection_z_plus;

	M = M * modelScale;
	
	M = M * modelRotation_y;
	M = M * modelRotation_x;
	M = M * modelRotation_z;
	M = M * modelTranslation; 

	V = inverse(viewRotation_y) * V;
	V = inverse(viewRotation_x) * V;
	V = inverse(viewRotation_z) * V;
	V = inverse(viewTranslation) * V;
	
	// setLineColour(vec3(0.0f, 0.0f, 1.0f));
	// draw_points_view(changed_x[0], changed_x[1]);

	// setLineColour(vec3(0.0f, 1.0f, 0.0f));
	// draw_points_view(changed_y[0], changed_y[1]);

	// setLineColour(vec3(1.0f, 0.0f, 0.0f));
	// draw_points_view(changed_z[0], changed_z[1]);
	
	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	draw_points(changed_x[0], changed_x[1]);

	setLineColour(vec3(0.0f, 1.0f, 0.0f));
	draw_points(changed_y[0], changed_y[1]);

	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	draw_points(changed_z[0], changed_z[1]);


	setLineColour(vec3(0.2f, 1.0f, 1.0f));
	for (int i = 0; i < 12; ++i){
		draw_points(changed_cube_structure[i][0], changed_cube_structure[i][1]);
	}

	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	drawLine( viewPort_top[0], viewPort_top[1]);
	drawLine( viewPort_bot[0], viewPort_bot[1]);
	drawLine( viewPort_left[0], viewPort_left[1]);
	drawLine( viewPort_right[0], viewPort_right[1]);
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
void A2::draw_points(glm::vec4 i, glm::vec4 e){
	vec4 point[2];
	point[0] = V * M * i;
	point[1] = V * M * e;
	if (near_cliping(point) && far_cliping(point)) {
		point[0] = P * point[0];
		point[1] = P * point[1];
		point[0].x = point[0].x/point[0].z;
		point[0].y = point[0].y/point[0].z;
		point[1].x = point[1].x/point[1].z;
		point[1].y = point[1].y/point[1].z;
		point[0].x = viewPort_reDraw_x(point[0].x);
		point[0].y = viewPort_reDraw_y(point[0].y);
		point[1].x = viewPort_reDraw_x(point[1].x);
		point[1].y = viewPort_reDraw_y(point[1].y);
		viewPort_cliping(vec2(point[0]), vec2(point[1]));
	}
}
void A2::draw_points_view(glm::vec4 i, glm::vec4 e){
	vec4 point[2];
	point[0] = V * i;
	point[1] = V * e;
	if (near_cliping(point) && far_cliping(point)) {
		point[0] = P * point[0];
		point[1] = P * point[1];
		point[0].x = point[0].x/point[0].z;
		point[0].y = point[0].y/point[0].z;
		point[1].x = point[1].x/point[1].z;
		point[1].y = point[1].y/point[1].z;
		point[0].x = viewPort_reDraw_x(point[0].x);
		point[0].y = viewPort_reDraw_y(point[0].y);
		point[1].x = viewPort_reDraw_x(point[1].x);
		point[1].y = viewPort_reDraw_y(point[1].y);
		viewPort_cliping(vec2(point[0]), vec2(point[1]));
	}
}
bool A2::near_cliping(glm::vec4 * p){

	vec4 i = p[0];
	vec4 e = p[1];

	vec4 line[2] = {
		vec4(0, 0, near, 0), vec4(0, 0, 1, 1)
	};

	vec4 dif_IP = (i - line[0]);
	vec4 dif_EP = (e - line[0]);
	float wecI = dif_IP.x * line[1].x + dif_IP.y * line[1].y + dif_IP.z * line[1].z;
	float wecE = dif_EP.x * line[1].x + dif_EP.y * line[1].y + dif_EP.z * line[1].z;

	if (wecI < 0 && wecE < 0) return false;
	if (wecI >= 0 && wecE >= 0) return true;
	float t = wecI/(wecI - wecE);
	if ( wecI < 0 ) {
		i = i + t * (e - i);
	} else {
		e = i + t * (e - i);
	}
	p[0] = i;
	p[1] = e;
	return true;

}
bool A2::far_cliping(glm::vec4 * p){
	
	vec4 i = p[0];
	vec4 e = p[1];

	vec4 line[2] = {
		vec4(0, 0, far, 0), vec4(0, 0, -1, 1)
	};

	
	// for (int j = 0; j < 4; ++j){
	vec4 dif_IP = (i - line[0]);
	vec4 dif_EP = (e - line[0]);
	float wecI = dif_IP.x * line[1].x + dif_IP.y * line[1].y + dif_IP.z * line[1].z;
	float wecE = dif_EP.x * line[1].x + dif_EP.y * line[1].y + dif_EP.z * line[1].z;

	if (wecI < 0 && wecE < 0) return false;
	if (wecI >= 0 && wecE >= 0) return true;
	float t = wecI/(wecI - wecE);
	if ( wecI < 0 ) {
		i = i + t * (e - i);
	} else {
		e = i + t * (e - i);
	}
	p[0] = i;
	p[1] = e;
	return true;

}

float A2::viewPort_reDraw_x(float x){
	int Xwl = -1;
	return (x - Xwl) * abs(m_width_aspect/m_width) + viewPort_bot[0].x;	
	// return x;
	
}
float A2::viewPort_reDraw_y(float y){
	int Ywl = -1;
	return (y - Ywl) * abs(m_height_aspect/m_height) + viewPort_bot[0].y;
	// return y;
}

void A2::viewPort_cliping(vec2 i, vec2 e){

	vec2 lineSegments[4][2] = {
		{ viewPort_top[1], vec2(0, -1)},
		{ viewPort_top[1], vec2(-1, 0)},
		{ viewPort_bot[0], vec2(0, 1)},
		{ viewPort_bot[0], vec2(1, 0)}
	};
	for (int j = 0; j < 4; ++j){
		vec2 dif_IP = (i - lineSegments[j][0]);
		vec2 dif_EP = (e - lineSegments[j][0]);
		float wecI = dif_IP.x * lineSegments[j][1].x + dif_IP.y * lineSegments[j][1].y;
		float wecE = dif_EP.x * lineSegments[j][1].x + dif_EP.y * lineSegments[j][1].y;

		if (wecI < 0 && wecE < 0) return;
		if (wecI >= 0 && wecE >= 0) continue;
		float t = wecI/(wecI - wecE);
		if ( wecI < 0 ) {
			i = i + t * (e - i);
		} else {
			e = i + t * (e - i);
		}
	}

	drawLine(i, e);
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
		// Create Button, and check if it was clicked:
		//quit button short cut Q
		if( ImGui::Button( "Quit Application" ) ) {
			cout << "Goodbye!" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		//reset button
		if( ImGui::Button( "Reset Application" ) ) {
			cout << "Application reseted" << endl;
			M = I;
			V = inverse(mat4x4( vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, -1, 0), vec4(0, 0, 5, 1)));
			P = I;
			current_mode = 3;
			viewPort_top[0] = vec2(-0.95,0.95);
			viewPort_top[1] = vec2(0.95, 0.95);
			viewPort_bot[0] = vec2(-0.95,-0.95);
			viewPort_bot[1] = vec2(0.95,-0.95);
			viewPort_left[0] = vec2(-0.95,0.95);
			viewPort_left[1] = vec2(-0.95, -0.95);
			viewPort_right[0] = vec2(0.95, 0.95);
			viewPort_right[1] = vec2(0.95, -0.95);
			m_width = 768;
			m_height = 689;
			m_width_aspect = (viewPort_top[1].x + 1) * m_width / 2 - (viewPort_top[0].x + 1) * m_width / 2;
			m_height_aspect = (viewPort_top[1].y + 1) * m_height / 2 - (viewPort_bot[1].y + 1) * m_height / 2;
			theta = PI/6;
			aspect = 1;
			far = 10;
			near = -1;

		}
		if( ImGui::RadioButton( "Rotate Model (R)", &current_mode, 3 ) ) {
			cout << "Rotate Model" << current_mode << endl;	
		}

		if( ImGui::RadioButton( "Translate Model(T)", &current_mode, 4 ) ) {
			cout << "Translate Model" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Scale Model (S)", &current_mode, 5 ) ) {
			cout << "Scale Model" << current_mode << endl;	
		}
		if( ImGui::RadioButton( "Rotate View (O)", &current_mode, 0 ) ) {
			cout << "Rotate View" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Translate View (N)", &current_mode, 1 ) ) {
			cout << "Translate View" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Perspective (P)", &current_mode, 2 ) ) {
			cout << "Perspective" << current_mode << endl;
		}

		if( ImGui::RadioButton( "Viewport (V)", &current_mode, 6 ) ) {
			cout << "Viewport" << current_mode << endl;
		}


		ImGui::Text( "Near: %0.001f", near );
		ImGui::Text( "Far: %0.001f", far );
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
			// cout << "some mouse(s) dragging: "<< ImGui::GetIO().MouseDown[0] + ImGui::GetIO().MouseDown[1] + ImGui::GetIO().MouseDown[2] << endl;
			
			if (current_mode == 0){
				//rotate view
				delta_right = (xPos - old_x_position) *  PI / m_framebufferWidth;//xPos difference		
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
				delta_right = 5 * (xPos - old_x_position) / m_framebufferWidth;//xPos difference		
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
				delta_right = (xPos - old_x_position);//xPos difference		
				if ( ImGui::GetIO().MouseDown[0] ){
					// view_theta_x = delta_right;
					theta += delta_right * PI / m_framebufferWidth;
					// cout << "theta: "<< theta << endl;
					if (theta <= PI/36) theta = PI/36;
					if (theta >= 8*PI/9) theta = 8*PI/9;
				}
				if ( ImGui::GetIO().MouseDown[1] ){
					far += 5 * delta_right / m_framebufferWidth;
					// cout << "far: " << far << endl;
				}
				if ( ImGui::GetIO().MouseDown[2] ){
					near += 5 * delta_right / m_framebufferWidth;
					// cout << "near: " << near << endl;
				}
			} else if (current_mode == 3){
				//Rotate Model
				delta_right = 2 * (xPos - old_x_position) *  PI / m_framebufferWidth;//xPos difference		
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
				delta_right = 5 * (xPos - old_x_position) / m_framebufferWidth;//xPos difference		
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
				delta_right = 3 * (xPos - old_x_position) /m_framebufferWidth;//xPos difference		
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
				float xPos_w = 2*xPos/m_width - 1;
				float yPos_w = -(2*yPos/m_height -1);
				if ( ImGui::GetIO().MouseDown[0] ){
					// cout << m_windowWidth << " " << m_windowHeight << endl;
					// cout << "X: " << xPos <<" Y: "<<yPos<<endl;
					// cout << "X: " << xPos_w <<" Y: "<<yPos_w<<endl;
					if (draw_new_view_port == 0){
						draw_new_view_port = 1;
						viewPort_startPoint = vec2(xPos_w, yPos_w);
						viewPort_top[0] = vec2(xPos_w, yPos_w);
						viewPort_top[1] = vec2(xPos_w, yPos_w);
						viewPort_bot[0] = vec2(xPos_w, yPos_w);
						viewPort_bot[1] = vec2(xPos_w, yPos_w);
						viewPort_left[0] = vec2(xPos_w, yPos_w);
						viewPort_left[1] = vec2(xPos_w, yPos_w);
						viewPort_right[0] = vec2(xPos_w, yPos_w); 
						viewPort_right[1] = vec2(xPos_w, yPos_w);
					} else {
						//left top -> right bottom
						if (xPos_w >= viewPort_startPoint.x && yPos_w <= viewPort_startPoint.y){
							viewPort_top[0].x = viewPort_startPoint.x;
							viewPort_top[0].y = viewPort_startPoint.y;
							viewPort_top[1].x = xPos_w;
							viewPort_top[1].y = viewPort_startPoint.y;
							viewPort_bot[0].x = viewPort_startPoint.x;
							viewPort_bot[0].y = yPos_w;
							viewPort_bot[1].x = xPos_w;
							viewPort_bot[1].y = yPos_w;
							viewPort_left[0].x = viewPort_startPoint.x;
							viewPort_left[0].y = viewPort_startPoint.y;
							viewPort_left[1].x = viewPort_startPoint.x;
							viewPort_left[1].y = yPos_w;
							viewPort_right[0].x = xPos_w;
							viewPort_right[0].y = viewPort_startPoint.y;
							viewPort_right[1].x = xPos_w;
							viewPort_right[1].y = yPos_w;
						} else if (xPos_w >= viewPort_startPoint.x && yPos_w > viewPort_startPoint.y){
							viewPort_top[0].x = viewPort_startPoint.x;
							viewPort_top[0].y = yPos_w;
							viewPort_top[1].x = xPos_w;
							viewPort_top[1].y = yPos_w;
							viewPort_bot[0].x = viewPort_startPoint.x;
							viewPort_bot[0].y = viewPort_startPoint.y;
							viewPort_bot[1].x = xPos_w;
							viewPort_bot[1].y = viewPort_startPoint.y;
							viewPort_left[0].x = viewPort_startPoint.x;
							viewPort_left[0].y = viewPort_startPoint.y;
							viewPort_left[1].x = viewPort_startPoint.x;
							viewPort_left[1].y = yPos_w;
							viewPort_right[0].x = xPos_w;
							viewPort_right[0].y = viewPort_startPoint.y;
							viewPort_right[1].x = xPos_w;
							viewPort_right[1].y = yPos_w;
						} else if (xPos_w < viewPort_startPoint.x && yPos_w <= viewPort_startPoint.y){
							viewPort_top[0].x = xPos_w;
							viewPort_top[0].y = viewPort_startPoint.y;
							viewPort_top[1].x = viewPort_startPoint.x;
							viewPort_top[1].y = viewPort_startPoint.y;
							viewPort_bot[0].x = xPos_w;
							viewPort_bot[0].y = yPos_w;
							viewPort_bot[1].x = viewPort_startPoint.x;
							viewPort_bot[1].y = yPos_w;
							viewPort_left[0].x = xPos_w;
							viewPort_left[0].y = viewPort_startPoint.y;
							viewPort_left[1].x = xPos_w;
							viewPort_left[1].y = yPos_w;
							viewPort_right[0].x = viewPort_startPoint.x;
							viewPort_right[0].y = viewPort_startPoint.y;
							viewPort_right[1].x = viewPort_startPoint.x;
							viewPort_right[1].y = yPos_w;

						} else if (xPos_w < viewPort_startPoint.x && yPos_w > viewPort_startPoint.y){
							viewPort_top[0].x = xPos_w;
							viewPort_top[0].y = yPos_w;
							viewPort_top[1].x = viewPort_startPoint.x;
							viewPort_top[1].y = yPos_w;
							viewPort_bot[0].x = xPos_w;
							viewPort_bot[0].y = viewPort_startPoint.y;
							viewPort_bot[1].x = viewPort_startPoint.x;
							viewPort_bot[1].y = viewPort_startPoint.y;
							viewPort_left[0].x = xPos_w;
							viewPort_left[0].y = yPos_w;
							viewPort_left[1].x = xPos_w;
							viewPort_left[1].y = viewPort_startPoint.y;
							viewPort_right[0].x = viewPort_startPoint.x;
							viewPort_right[0].t = viewPort_startPoint.y;
							viewPort_right[1].x = viewPort_startPoint.x;
							viewPort_right[1].y = yPos_w;
						}

						m_width_aspect = (viewPort_top[1].x + 1) * m_width / 2 - (viewPort_top[0].x + 1) * m_width / 2;
						m_height_aspect = (viewPort_top[1].y + 1) * m_height / 2 - (viewPort_bot[1].y + 1) * m_height / 2;
					}
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
	if (!ImGui::GetIO().MouseDown[0]){
		draw_new_view_port = 0;
	}
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
	cout << "width: " << width << " height: " << height << endl;
	m_width = width;
	m_height = height;

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
		if ( key == GLFW_KEY_Q ) {
			cout << "Q Keyboard pressed" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if ( key == GLFW_KEY_O ) {
			cout << "O Keyboard pressed" << endl;
			current_mode = 0;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_N ) {
			cout << "N Keyboard pressed" << endl;
			current_mode = 1;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_P ) {
			cout << "P Keyboard pressed" << endl;
			current_mode = 2;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_R ) {
			cout << "R Keyboard pressed" << endl;
			current_mode = 3;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_T ) {
			cout << "T Keyboard pressed" << endl;
			current_mode = 4;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_S ) {
			cout << "S Keyboard pressed" << endl;
			current_mode = 5;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_V ) {
			cout << "V Keyboard pressed" << endl;
			current_mode = 6;
			eventHandled = true;
		}
		
	}

	// Fill in with event handling code...

	return eventHandled;
}


