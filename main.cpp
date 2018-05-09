/* main.cpp
*/

#include "Boilerplate.h"
#include "City.h"

using namespace std;

// Window Parameters
#define DEFAULT_WINDOW_WIDTH 1920
#define DEFAULT_WINDOW_HEIGHT 1080

// Control Constants
#define MOUSE_SENSITIVITY 0.005
#define MOVEMENT_SPEED 0.1

// Call index for glNewList
#define STREETS 1
#define PAVEMENT 2

Camera cam;
Vec2 mouse;

Skybox sky;

bool renderWire = false;

// Create city as a vector of many Building objects
vector<Building> city = vector<Building>();

// Generate city by putting building into city vector
// Building objects created will have 
// BuldingType, buildingWidth, buildingDepth, coords and building id
void buildCity()
{
	GLint bID = 1010;
        
	for(int i = BUILD_WIDTH / 2; i < CITY_WIDTH; i += (BUILD_WIDTH))
	{
		i += STREET_WIDTH;
		for(int j = BUILD_DEPTH / 2; j < CITY_DEPTH; j += (BUILD_DEPTH))
		{
		    j += STREET_WIDTH;
						
			int choice = rand() % 100;
			int buildWidth = BUILD_WIDTH;
			int buildDepth = BUILD_DEPTH;
			if (rand() % 20 < 10)
			{
			    buildWidth = BUILD_WIDTH + STREET_WIDTH + rand() % 5;
			    buildDepth = BUILD_DEPTH + STREET_WIDTH + rand() % 5;
			}
			else if (rand() % 20 > 2){
			    buildWidth = BUILD_WIDTH + rand() % 5;
			    buildDepth = BUILD_DEPTH + rand() % 5;
			}
			
			if (choice < 5)
			{
				city.push_back(Building(BLOCKS, buildWidth, buildDepth, Trans4x4(i, j), bID));
			}
			else if (choice < 30)
			{
				city.push_back(Building(STATE, buildWidth, buildDepth, Trans4x4(i, j), bID));
			}
			else if (choice < 35)
			{
				city.push_back(Building(MODERN, buildWidth, buildDepth, Trans4x4(i, j), bID));
			}
			else if (choice < 45)
			{
				city.push_back(Building(NONE, buildWidth, buildDepth, Trans4x4(i, j), bID));
			}
		
			else if(choice < 70)
			{
				city.push_back(Building(STANDARD, buildWidth, buildDepth, Trans4x4(i, j), bID));
			}			
			else if(choice < 100)
			{
				city.push_back(Building(STACKED, buildWidth, buildDepth, Trans4x4(i, j), bID));
			}
			
			bID++;
		}
	}
}

// Draw generated city based on city vector
void drawCity()
{
	for(size_t i = 0; i < city.size(); i++)
	{
		city[i].draw();
	}
	glCallList(PAVEMENT);
//	if(cam.eye.z < 75 && cam.eye.z > 0)
//		glCallList(STREETS);

    // Draw skybox
	sky.draw();

}

void createPavement(int index)
{
    // Draw quadric shape with normal. 
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE ); // This defines the side that should be lit by light rays
	
	// Create display list
	// More convenient and efficient way to name & organise set of OpenGL commands
	glNewList( index, GL_COMPILE );

	glDisable( GL_LIGHTING );  // Use simple draw color.

	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex3f(-BUILD_WIDTH / 2, -BUILD_DEPTH / 2, -0.001);
	glVertex3f(-BUILD_WIDTH / 2, CITY_DEPTH + BUILD_DEPTH / 2, -0.001);
	glVertex3f(CITY_WIDTH + BUILD_WIDTH / 2, CITY_DEPTH + BUILD_DEPTH / 2, -0.001);
	glVertex3f(CITY_WIDTH + BUILD_WIDTH / 2, -BUILD_DEPTH / 2, -0.001);
	glEnd();

	glEnable(GL_LIGHTING);

	glEndList();
	gluDeleteQuadric(qobj);
}

void createStreets(int index)
{
    // Draw quadric shape with normal. 
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricNormals( qobj, GL_TRUE ); // This defines the side that should be lit by light rays
	
	// Create display list
	// More convenient and efficient way to name & organise set of OpenGL commands
	glNewList( index, GL_COMPILE );

	glDisable( GL_LIGHTING );  // Use simple draw color.

	glColor3f(0.9, 0.9, 0.2);
	bool on = false;
	for(float i = BUILD_WIDTH + STREET_WIDTH/2; i < CITY_WIDTH - BUILD_WIDTH; i += BUILD_WIDTH + STREET_WIDTH)
	{
		for(float j = 0.5; j < CITY_WIDTH - BUILD_WIDTH; j += BUILD_WIDTH + STREET_WIDTH)
		{
			for(float k = j; k < j + BUILD_WIDTH; k += (BUILD_WIDTH / 8))
			{
				if(!on)
				{
					glBegin(GL_POLYGON);
					glVertex3f(i + 0.05, k, 0);
					glVertex3f(i - 0.05, k, 0);
					on = !on;
				}
				else
				{
					glVertex3f(i - 0.05, k, 0);
					glVertex3f(i + 0.05, k, 0);
					glEnd();
					on = !on;
				}
			}
		}
	}
	on = false;
	for(float i = BUILD_WIDTH + STREET_WIDTH/2; i < CITY_WIDTH - BUILD_WIDTH; i += BUILD_WIDTH + STREET_WIDTH)
	{
		for(float j = 0.5; j < CITY_WIDTH - BUILD_WIDTH; j += BUILD_WIDTH + STREET_WIDTH)
		{
			for(float k = j; k < j + BUILD_WIDTH; k += (BUILD_WIDTH / 8))
			{
				if(!on)
				{
					glBegin(GL_POLYGON);
					glVertex3f(k, i - 0.05, 0);
					glVertex3f(k, i + 0.05, 0);
					on = !on;
				}
				else
				{
					glVertex3f(k, i + 0.05, 0);
					glVertex3f(k, i - 0.05, 0);
					glEnd();
					on = !on;
				}
			}
		}
	}

	glEnable(GL_LIGHTING);

	glEndList();
	gluDeleteQuadric(qobj);
}

//Does all the drawing 
void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // clear frame buffer

	// Set up the viewing frustum.  Zooming will be performed here by changing the size
	// of the XY extent of the frustum.
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -cam.halfWidth, cam.halfWidth, -cam.halfHeight, cam.halfHeight, cam.zNear, cam.zFar );

	
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	calculateCameraRotation(cam);
	gluLookAt(cam);

	glEnable( GL_LIGHTING );
	glEnable( GL_NORMALIZE );

	drawCity();

	glFlush();
	glutSwapBuffers();
}

// Mouse Listener - Function to tell OpenGL to call
// whenever a mouse button is pressed or released.
void mouse_button( int button, int state, int xPos, int yPos )
{
	glutPostRedisplay();
}

// Alters the scene based on mouse motion
// Depends on if anything is selected
void mouse_motion( int x, int y )
{
	double deltaX = x - mouse.x;
	double deltaY = y - mouse.y;
	
	// Camera Rotation
	rotateHorizontal(cam, MOUSE_SENSITIVITY * deltaY);
	rotateVertical(cam, MOUSE_SENSITIVITY * deltaX);
	
	mouse = Vec2(x, y);
	glutPostRedisplay();
}

// Keyboard Listener - Function to tell OpenGL to call
// whenever a key on keyboard is pressed or released.
void key_press( unsigned char key, int x, int y )
{
	double speed = MOVEMENT_SPEED;
	if(glutGetModifiers() == GLUT_ACTIVE_SHIFT)
	{
		speed *= 5;
	}
	switch(key)
	{
		case 'w':
		case 'W':
			moveForward(cam, speed*2);
			break;
		case 's':
		case 'S':
			moveForward(cam, -speed*2);
			break;
		case 'r':
		case 'R':
			renderWire = !renderWire;
			if(renderWire)
			{
				glClearColor( 0.90, 0.90, 0.90, 0.0 );
				glPolygonMode(GL_BACK, GL_LINE);
			}
			else
			{
				glClearColor(0.90, 0.90, 0.90, 0.0 );
				glPolygonMode(GL_BACK, GL_FILL);
			}
			break;
		case 'c':
		case 'C':
			glClearColor(0.90, 0.90, 0.90, 0.0 ); // clear window.
			// Rebuild city
			sky = Skybox(Vec3(-CITY_WIDTH/2, -CITY_WIDTH/2, -CITY_WIDTH/2), Vec3(3*CITY_WIDTH/2, 3*CITY_WIDTH/2, 3*CITY_WIDTH/2));
			buildCity();
			break;
		case 'q':
		case 'Q':
			exit(0);
	
	}

	glutPostRedisplay();
}

// Process "special" key-press events.  This includes the arrow keys, the
// "home" key, and the "PageUp" and "PageDown" keys.  Notice that the first
// argument is an "int" rather than an "unsighed char".
void special_key_press( int key, int x, int y ) 
{
    double speed = MOVEMENT_SPEED;
	if(glutGetModifiers() == GLUT_ACTIVE_SHIFT)
	{
		speed *= 5;
	}
	switch( key )
	{
	case up_arrow_key:
		moveForward(cam, speed*2);
		break;

	case down_arrow_key: 
			moveForward(cam, -speed*2);
    		break;

	case left_arrow_key:
		break;

	case right_arrow_key:
		break;
	}
	glutPostRedisplay();
}

// Just keep track of where the mouse was last
// Used for keyboard key stroke based deletion
void mouse_passive_motion( int x, int y )
{
	mouse = Vec2(x, y);
}

int main( int argc, char** argv )
{
	// Seed the random number generator, this only should happen once
	srand (time(NULL));

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );
	glutInitWindowPosition( 100, 100 );
	glutCreateWindow( "city" );
	glClearColor(0.90, 0.90, 0.90, 0.0 ); // clear window.

	glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
        glEnable (GL_BLEND);
        glCullFace (GL_FRONT);
        glEnable (GL_CULL_FACE);
        glEnable (GL_POLYGON_SMOOTH);

	glutDisplayFunc( display );
	glutMouseFunc( mouse_button );
	glutMotionFunc( mouse_motion );
	glutKeyboardFunc( key_press );
	glutSpecialFunc( special_key_press );
	glutPassiveMotionFunc( mouse_passive_motion );

	glEnable( GL_DEPTH_TEST ); // Turn on depth buffering.

	// Adjust light reflection/shininess and ambient based on material
	float mat_ambient[] = {0.02f, 0.02f, 0.02f,  1.0f};
    	float mat_diffuse[] = {0.10f, 0.10f, 0.10f, 1.0f};

	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT , mat_ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,mat_ambient );

	glMaterial( GL_AMBIENT  , Color(0.02,0.02,0.02) );
	glMaterial( GL_DIFFUSE  , Color(0.1,0.1,0.1) );

	sky = Skybox(Vec3(-CITY_WIDTH/2, -CITY_WIDTH/2, -CITY_WIDTH/2), Vec3(3*CITY_WIDTH/2, 3*CITY_WIDTH/2, 3*CITY_WIDTH/2));
	createPavement(PAVEMENT);
	createStreets(STREETS);
	buildCity();

	glutMainLoop();
	return 0;
}

