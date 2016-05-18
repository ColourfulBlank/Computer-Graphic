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
int fly_dest_x = 0;
int fly_dest_z = 0;
int shift = 0;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ), projection_distance( 45.0f ), degrees_xz(0.0f), old_x_position( 0 ), old_y_position( 0 ), mouse_state( 0 ), cursor_z( 0 ), cursor_x( 0 ), total_blocks( 1 ), copy_info_number_blocks( 1 ), copy_info_colour_number ( current_col ), fly_x(0.0f), fly_z(0.0f)
{
	for (int i = 0; i < 36; i++){
		current_col_array[i] = 1.0f;
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
	// GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
 //  	GLfloat mat_shininess[] = { 50.0 };	
	// GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
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
void A1::initPointer(){
	size_t sz = 3 * 3 * 5;

	float *verts = new float[ sz ];

	float base_pointer[3 * 3 * 5] = {
		-0.5f, 0.0f, -0.5f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, -1.0f,

		-0.5f, 0.0f, -0.5f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, -1.0f,

		-0.5f, 0.0f, -0.5f,
		-1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		-0.5f, 0.0f, -0.5f,
		0.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f
	};


	int blocks_so_far = 0;
	for ( int i = 0; i < 45; i+=3 ){
		verts[blocks_so_far ] = base_pointer[i] + cursor_x;
		verts[blocks_so_far + 1] = base_pointer[i+1] + Number_of_Block[cursor_z][cursor_x] + 1;
		verts[blocks_so_far + 2] = base_pointer[i+2] + cursor_z;
		blocks_so_far += 3;
	}
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_pointer_vao );
	glBindVertexArray( m_pointer_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_pointer_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_pointer_vbo );
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
void A1::initCube(int colour_number){
	total_blocks = 0;
	for (int i = 0; i < 18; i++){
		for (int j = 0; j < 18; j ++ ){
			if (Colour_id[i][j] == colour_number){
				total_blocks += Number_of_Block[i][j];
			}
		}
	}
	size_t sz = 3 * 3 * 2 * 6 * total_blocks;
	if (sz > 0){
	float *verts = new float[ sz ];
	float base_verts[3 * 3 * 2 * 6];
	//xz
	base_verts[0] = 0.0f;
	base_verts[1] = 0.0f;
	base_verts[2] = 0.0f;
	base_verts[3] = -1.0f;
	base_verts[4] = 0.0f;
	base_verts[5] = 0.0f;
	base_verts[6] = 0.0f;
	base_verts[7] = 0.0f;
	base_verts[8] = -1.0f;

	base_verts[9] = 0.0f;
	base_verts[10] = 0.0f;
	base_verts[11] = -1.0f;
	base_verts[12] = -1.0f;
	base_verts[13] = 0.0f;
	base_verts[14] = -1.0f;
	base_verts[15] = -1.0f;
	base_verts[16] = 0.0f;
	base_verts[17] = 0.0f;
	//xy
	base_verts[18] = 0.0f;
	base_verts[19] = 0.0f;
	base_verts[20] = 0.0f;
	base_verts[21] = 0.0f;
	base_verts[22] = 1.0f;
	base_verts[23] = 0.0f;
	base_verts[24] = -1.0f;
	base_verts[25] = 0.0f;
	base_verts[26] = 0.0f;

	base_verts[27] = 0.0f;
	base_verts[28] = 1.0f;
	base_verts[29] = 0.0f;
	base_verts[30] = -1.0f;
	base_verts[31] = 1.0f;
	base_verts[32] = 0.0f;
	base_verts[33] = -1.0f;
	base_verts[34] = 0.0f;
	base_verts[35] = 0.0f;
	// //yz
	base_verts[36] = 0.0f;
	base_verts[37] = 1.0f;
	base_verts[38] = 0.0f;
	base_verts[39] = 0.0f;
	base_verts[40] = 0.0f;
	base_verts[41] = 0.0f;
	base_verts[42] = 0.0f;
	base_verts[43] = 1.0f;
	base_verts[44] = -1.0f;

	base_verts[45] = 0.0f;
	base_verts[46] = 1.0f;
	base_verts[47] = -1.0f;
	base_verts[48] = 0.0f;
	base_verts[49] = 0.0f;
	base_verts[50] = 0.0f;
	base_verts[51] = 0.0f;
	base_verts[52] = 0.0f;
	base_verts[53] = -1.0f;
	//rest three
	//xz
	base_verts[54] = 0.0f;
	base_verts[55] = 1.0f;
	base_verts[56] = 0.0f;
	base_verts[57] = 0.0f;
	base_verts[58] = 1.0f;
	base_verts[59] = -1.0f;
	base_verts[60] = -1.0f;
	base_verts[61] = 1.0f;
	base_verts[62] = 0.0f;

	base_verts[63] = 0.0f;
	base_verts[64] = 1.0f;
	base_verts[65] = -1.0f;
	base_verts[66] = -1.0f;
	base_verts[67] = 1.0f;
	base_verts[68] = -1.0f;
	base_verts[69] = -1.0f;
	base_verts[70] = 1.0f;
	base_verts[71] = 0.0f;
	// xy
	base_verts[72] = 0.0f;
	base_verts[73] = 1.0f;
	base_verts[74] = -1.0f;
	base_verts[75] = -1.0f;
	base_verts[76] = 0.0f;
	base_verts[77] = -1.0f;
	base_verts[78] = 0.0f;
	base_verts[79] = 0.0f;
	base_verts[80] = -1.0f;

	base_verts[81] = 0.0f;
	base_verts[82] = 1.0f;
	base_verts[83] = -1.0f;
	base_verts[84] = -1.0f;
	base_verts[85] = 1.0f;
	base_verts[86] = -1.0f;
	base_verts[87] = -1.0f;
	base_verts[88] = 0.0f;
	base_verts[89] = -1.0f;
	// yz
	base_verts[90] = -1.0f;
	base_verts[91] = 1.0f;
	base_verts[92] = -1.0f;
	base_verts[93] = -1.0f;
	base_verts[94] = 1.0f;
	base_verts[95] = 0.0f;
	base_verts[96] = -1.0f;
	base_verts[97] = 0.0f;
	base_verts[98] = 0.0f;

	base_verts[99] = -1.0f;
	base_verts[100] = 0.0f;
	base_verts[101] = 0.0f;
	base_verts[102] = -1.0f;
	base_verts[103] = 0.0f;
	base_verts[104] = -1.0f;
	base_verts[105] = -1.0f;
	base_verts[106] = 1.0f;
	base_verts[107] = -1.0f;

	

	int blocks_so_far = 0;
	for ( int z = 0; z < 18; ++z ){
		for ( int x = 0; x < 18; ++x ){
			if (Colour_id[z][x] == colour_number){
				for ( int y = 0; y < Number_of_Block[z][x]; ++y ){
					for (int l = 0; l < 108; l += 3){
						verts[blocks_so_far ] = base_verts[l] + x;
						verts[blocks_so_far + 1] = base_verts[l+1] + y;
						verts[blocks_so_far + 2] = base_verts[l+2] + z;
						blocks_so_far += 3;
					}
					
				}
			}
		}
	}
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_cube_vao );
	glBindVertexArray( m_cube_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_cube_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_cube_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );// need to change to Dynamic?


	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	//add a colour buffer
	sz = 3 * 12 * total_blocks;
	float colour_verts[sz];
	for (int i = 0; i < sz; i++){
		colour_verts[i] = 0.0f;
	}
	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;
}
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
		
		if( ImGui::Button( "Reset Application" ) ) {
			current_col = 0; 
			for (int i = 0; i < 36; i++){
				current_col_array[i] = 1.0f;
			}
			for (int i = 0; i < 18; i++){
				for (int j = 0; j < 18; j++){
					Number_of_Block[i][j] = 0;
					Colour_id[i][j] = 0;
				}
			}
			projection_distance = 45.0f;
			degrees_xz = 0.0f;
			old_x_position = 0;
			old_y_position = 0;
			fly_z = -fly_dest_z;
			fly_x = -fly_dest_x;
			fly_dest_x = 0;
			fly_dest_z = 0;
			mouse_state = 0;
			cursor_z = 0;
			cursor_x = 0;
		}
		ImGui::SameLine();
		if( ImGui::Button( "Reset Camera" ) ) {
			projection_distance = 45.0f;
			degrees_xz = 0.0f;
			old_x_position = 0;
			old_y_position = 0;
			fly_z = -fly_dest_z;
			fly_x = -fly_dest_x;
			fly_dest_x = 0;
			fly_dest_z = 0;
			mouse_state = 0;
		}
		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.
		for (int i = 0; i < 12; i++){
			ImGui::PushID( i );
			ImGui::ColorEdit3( "##Colour", &current_col_array[i*3] );
			ImGui::SameLine();
			if( ImGui::RadioButton( "##Col", &current_col, i ) ) {
				// Select this colour.
				std::cout << current_col << " colour picked" << std::endl;
				copy_info_colour_number = current_col;
			}
			ImGui::PopID();
		}
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		// if( ImGui::Button( "Test Window" ) ) {
		// 	showTestWindow = !showTestWindow;
		// }


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
	W = glm::rotate( W, degrees_xz, y_axis);

	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	view = glm::translate( view, vec3(fly_x, 0, fly_z));
	fly_x = 0;
	fly_z = 0;
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
		for (int i = 0; i < 12; i++){
			initCube(i);
			if (total_blocks > 0){
				glBindVertexArray( m_cube_vao );
				glUniform3f( col_uni, current_col_array[i*3], current_col_array[i*3 + 1], current_col_array[i*3 + 2] );
				glDrawArrays(GL_TRIANGLES, 0, 36 * total_blocks);
			}
		}

		// Highlight the active square.
		initPointer();
		glBindVertexArray( m_pointer_vao );
		glUniform3f( col_uni, current_col_array[current_col*3], current_col_array[current_col * 3 + 1], current_col_array[current_col * 3 + 2] );
		glDrawArrays(GL_TRIANGLES, 0, 15);


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
		if (ImGui::GetIO().MouseDown[1]){
			cout << "aooo" << endl;
		// if ( ImGui::IsMouseDragging() ){
			cout << "dragging" << endl;
					
			degrees_xz += (xPos - old_x_position) *  PI / m_windowWidth;//xPos difference		
			// }
		
			
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
			for (int i = 0; i < 18; i++){
				for (int j = 0; j < 18; j++){
					Number_of_Block[i][j] = 0;
					Colour_id[i][j] = 0;
				}
			}
			for (int i = 0; i < 36; i++){
				current_col_array[i] = 1.0f;
			}
			projection_distance = 45.0f;
			degrees_xz = 0.0f;
			old_x_position = 0;
			old_y_position = 0;
			mouse_state = 0;
			cursor_z = 0;
			cursor_x = 0;
			fly_z = -fly_dest_z;
			fly_x = -fly_dest_x;
			fly_dest_x = 0;
			fly_dest_z = 0;
			eventHandled = true;

		}
		if ( key == GLFW_KEY_SPACE ){
			cout << "space key pressed" << endl;
			Number_of_Block[cursor_z][cursor_x] += 1;
			Colour_id[cursor_z][cursor_x] = copy_info_colour_number;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_BACKSPACE ){
			cout << "BACKSPACE key pressed" << endl;
			if ( Number_of_Block[cursor_z][cursor_x] > 0 ){
				Number_of_Block[cursor_z][cursor_x]--;
				cout << Number_of_Block[cursor_z][cursor_x] << endl;
			}
			eventHandled = true;
		}
		if ( key == GLFW_KEY_RIGHT ){
			cout << "-> key pressed" << endl;
			if (cursor_x < 17){
				cursor_x++;
				if (shift == 1){
					Number_of_Block[cursor_z][cursor_x] = copy_info_number_blocks;
					Colour_id[cursor_z][cursor_x] = copy_info_colour_number;
					cout << "z: " << cursor_z << " x: " << cursor_x <<" num: " << Number_of_Block[cursor_z][cursor_x] <<  endl;
				}
			}
			eventHandled = true;
		}
		if ( key == GLFW_KEY_LEFT ){
			cout << "<- key pressed" << endl;
			if (cursor_x > 0){
				cursor_x--;
				if (shift == 1){
					Number_of_Block[cursor_z][cursor_x] = copy_info_number_blocks;
					Colour_id[cursor_z][cursor_x] = copy_info_colour_number;
					cout << "z: " << cursor_z << " x: " << cursor_x <<" num: " << Number_of_Block[cursor_z][cursor_x] <<  endl;
				}
			}
			
			eventHandled = true;
		}
		if ( key == GLFW_KEY_UP ){
			cout << "^ key pressed" << endl;
			if (cursor_z > 0){
				cursor_z--;
				if (shift == 1){
					Number_of_Block[cursor_z][cursor_x] = copy_info_number_blocks;
					Colour_id[cursor_z][cursor_x] = copy_info_colour_number;
					cout << "z: " << cursor_z << " x: " << cursor_x <<" num: " << Number_of_Block[cursor_z][cursor_x] <<  endl;
				}
			}
			
			eventHandled = true;
		}
		if ( key == GLFW_KEY_DOWN ){
			cout << "\\/ key pressed" << endl;
			if (cursor_z < 17){
				cursor_z++;
				if (shift == 1){
					Number_of_Block[cursor_z][cursor_x] = copy_info_number_blocks;
					Colour_id[cursor_z][cursor_x] = copy_info_colour_number;
					cout << "z: " << cursor_z << " x: " << cursor_x <<" num: " << Number_of_Block[cursor_z][cursor_x] <<  endl;
				}
			}
			
			eventHandled = true;
		}
		if ( key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT ){
			cout << "shift key pressed" << endl;
			shift = 1;
			copy_info_number_blocks = Number_of_Block[cursor_z][cursor_x];
			cout << copy_info_number_blocks << endl;
			copy_info_colour_number = Colour_id[cursor_z][cursor_x];

			eventHandled = true;
		}
		if ( key == GLFW_KEY_W ){
			cout << "w key pressed" << endl;
			fly_z--;
			fly_dest_z--;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_S ){
			cout << "s key pressed" << endl;
			fly_z++;
			fly_dest_z++;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_A ){
			cout << "a key pressed" << endl;
			fly_x--;
			fly_dest_x--;
			eventHandled = true;
		}
		if ( key == GLFW_KEY_D ){
			cout << "d key pressed" << endl;
			fly_x++;
			fly_dest_x++;
			eventHandled = true;
		}
		for (int i = 0; i < 18; i++){
			for (int j = 0; j < 18; j++){
				cout << Number_of_Block[i][j]<<" ";
			}
			cout << endl;
		}


	}
	if (action == GLFW_RELEASE){
		if ( key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT ){
			shift = 0;
			copy_info_number_blocks = 1;
			copy_info_colour_number = current_col;
		}
	}
	


	return eventHandled;
}