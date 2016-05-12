#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <math.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

static const size_t DIM = 16;
const float PI = 3.14159265f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ), projection_distance( 45.0f ), degrees(0.0f), old_x_position( 0 ), old_y_position( 0 ), mouse_state( 0 ), cursor_x( 0 ), cursor_y( 0 )
{
	// colour[0] = 0.0f;
	// colour[1] = 0.0f;
	// colour[2] = 0.0f;
	for (int i = 0; i < 27; i++){
		current_col_array[i] = 0.0f;
	}
	for (int i = 0; i < 18; i++){
		Number_of_Block[i] = new int[18];
		Colour_id[i] = new int[18];
		for (int j = 0; j < 18; j++){
			Number_of_Block[i][j] = 0;
			Colour_id[i][j] = 0;
		}
	}

}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	for (int i = 0; i < 18; i++){
		delete [] Number_of_Block[i];
		delete [] Colour_id[i];
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );


	initGrid();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );
	proj = glm::perspective( 
		glm::radians( projection_distance ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {

		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1; 
		ct += 6;


		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );// need to change to Dynamic?


	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initCube(){

	size_t sz = 3 * 3 * 2 * 6;
	float *verts = new float[ sz ];

	//xz
	verts[0] = 0.0f;
	verts[1] = 0.0f;
	verts[2] = 0.0f;
	verts[3] = -1.0f;
	verts[4] = 0.0f;
	verts[5] = 0.0f;
	verts[6] = 0.0f;
	verts[7] = 0.0f;
	verts[8] = -1.0f;

	verts[9] = 0.0f;
	verts[10] = 0.0f;
	verts[11] = -1.0f;
	verts[12] = -1.0f;
	verts[13] = 0.0f;
	verts[14] = -1.0f;
	verts[15] = -1.0f;
	verts[16] = 0.0f;
	verts[17] = 0.0f;
	//xy
	verts[18] = 0.0f;
	verts[19] = 0.0f;
	verts[20] = 0.0f;
	verts[21] = 0.0f;
	verts[22] = 1.0f;
	verts[23] = 0.0f;
	verts[24] = -1.0f;
	verts[25] = 0.0f;
	verts[26] = 0.0f;

	verts[27] = 0.0f;
	verts[28] = 1.0f;
	verts[29] = 0.0f;
	verts[30] = -1.0f;
	verts[31] = 1.0f;
	verts[32] = 0.0f;
	verts[33] = -1.0f;
	verts[34] = 0.0f;
	verts[35] = 0.0f;
	// //yz
	verts[36] = 0.0f;
	verts[37] = 1.0f;
	verts[38] = 0.0f;
	verts[39] = 0.0f;
	verts[40] = 0.0f;
	verts[41] = 0.0f;
	verts[42] = 0.0f;
	verts[43] = 1.0f;
	verts[44] = -1.0f;

	verts[45] = 0.0f;
	verts[46] = 1.0f;
	verts[47] = -1.0f;
	verts[48] = 0.0f;
	verts[49] = 0.0f;
	verts[50] = 0.0f;
	verts[51] = 0.0f;
	verts[52] = 0.0f;
	verts[53] = -1.0f;
	//rest three
	//xz
	verts[54] = 0.0f;
	verts[55] = 1.0f;
	verts[56] = 0.0f;
	verts[57] = 0.0f;
	verts[58] = 1.0f;
	verts[59] = -1.0f;
	verts[60] = -1.0f;
	verts[61] = 1.0f;
	verts[62] = 0.0f;

	verts[63] = 0.0f;
	verts[64] = 1.0f;
	verts[65] = -1.0f;
	verts[66] = -1.0f;
	verts[67] = 1.0f;
	verts[68] = -1.0f;
	verts[69] = -1.0f;
	verts[70] = 1.0f;
	verts[71] = 0.0f;
	// xy
	verts[72] = 0.0f;
	verts[73] = 1.0f;
	verts[74] = -1.0f;
	verts[75] = -1.0f;
	verts[76] = 0.0f;
	verts[77] = -1.0f;
	verts[78] = 0.0f;
	verts[79] = 0.0f;
	verts[80] = -1.0f;

	verts[81] = 0.0f;
	verts[82] = 1.0f;
	verts[83] = -1.0f;
	verts[84] = -1.0f;
	verts[85] = 1.0f;
	verts[86] = -1.0f;
	verts[87] = -1.0f;
	verts[88] = 0.0f;
	verts[89] = -1.0f;
	// yz
	verts[90] = -1.0f;
	verts[91] = 1.0f;
	verts[92] = -1.0f;
	verts[93] = -1.0f;
	verts[94] = 1.0f;
	verts[95] = 0.0f;
	verts[96] = -1.0f;
	verts[97] = 0.0f;
	verts[98] = 0.0f;

	verts[99] = -1.0f;
	verts[100] = 0.0f;
	verts[101] = 0.0f;
	verts[102] = -1.0f;
	verts[103] = 0.0f;
	verts[104] = -1.0f;
	verts[105] = -1.0f;
	verts[106] = 1.0f;
	verts[107] = -1.0f;


	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_cube_vao );
	glBindVertexArray( m_cube_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_cube_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_DYNAMIC_DRAW );// need to change to Dynamic?


	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		ImGui::SameLine();
		if( ImGui::Button( "Reset Application" ) ) {
			current_col = 0; 
			for (int i = 0; i < 27; i++){
				current_col_array[i] = 0.0f;
			}
			projection_distance = 45.0f;
			degrees = 0.0f;
			old_x_position = 0;
			old_y_position = 0;
			mouse_state = 0;
		}
		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.
		for (int i = 0; i < 9; i++){
			ImGui::PushID( i );
			ImGui::ColorEdit3( "##Colour", &current_col_array[i*3] );
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
				// Select this colour.
				std::cout << i << " colour picked" << std::endl;
			}
			ImGui::PopID();
		}
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}


		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	vec3 y_axis(0.0f,1.0f,0.0f);
	W = glm::rotate( W, degrees, y_axis);

	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	proj = glm::perspective( 
			glm::radians( projection_distance ),
			float( m_framebufferWidth ) / float( m_framebufferHeight ),
			1.0f, 1000.0f );
	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the cubes
		initCube();
		glBindVertexArray( m_cube_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Highlight the active square.


	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);
	cout << "In some windows" << endl;
	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {

		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if (ImGui::IsMouseDragging()){
			degrees += (xPos - old_x_position) *  PI / m_windowWidth;//xPos difference
		}
			
	}
	old_x_position = xPos;
	old_y_position = yPos;
	// cout << "xPos: " << xPos << " yPos: " << yPos << endl;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		cout << "Mouse Is Not Hovering Any Window" <<endl;
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
	cout << "mouse scrolled: y: " << yOffSet << " x:" << xOffSet << endl;
	projection_distance -= yOffSet;
	
	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		if (key == GLFW_KEY_EQUAL) {
			cout << "+ key pressed" << endl;
			projection_distance -= 1.0f;
			
			eventHandled = true;
		}
		if (key == GLFW_KEY_MINUS) {
			cout << "- key pressed" << endl;
			projection_distance += 1.0f;
			
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q ) {
			cout << "q key pressed" << endl;
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_R ) {
			cout << "r key pressed" << endl;
			current_col = 0; 
			for (int i = 0; i < 27; i++){
				current_col_array[i] = 0.0f;
			}
			projection_distance = 45.0f;
			degrees = 0.0f;
			old_x_position = 0;
			old_y_position = 0;
			mouse_state = 0;
			cursor_x = 0;
			cursor_y = 0;
			eventHandled = true;

		}
		if ( key == GLFW_KEY_SPACE ){
			cout << "space key pressed" << endl;
			Number_of_Block[cursor_x][cursor_y]++;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_DELETE ){
			cout << "delete key pressed" << endl;
			if ( Number_of_Block[cursor_x][cursor_y] != 0 ){
				Number_of_Block[cursor_x][cursor_y]--;
			}
			eventHandled = true;
		}
	}

	return eventHandled;
}
