
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include <fstream>
#include <iostream>


#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "glut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glslprogram.h"

#include "Easing.h"
#include "GUI.h"
#include "GUIBox.h"
#include "GUIElement.h"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"


struct Voxel
{
    glm::vec3 position;         // current render position
    glm::vec3 startPosition;    // animation starting position
    glm::vec3 finalPosition;    // animation final position
    glm::vec3 color;
    float scale;
    float startScale;
    float finalScale;
    int startTime;              // program time
    int id;

    Voxel() {}
    Voxel(glm::vec3 position, glm::vec3 color)
    {
        this->position = position;
        this->startPosition = position;
        this->finalPosition = position;
        this->color = color;
        this->scale = 1.;
        this->startScale = 1.;
        this->finalScale = 1.;
        this->startTime = -1;
    }
};


// title of the window:

const char *WINDOWTITLE = { "Voxel Editor - Joshua Cox" };

// the escape key:

#define ESCAPE		0x1b
#define SPACE       0x20

// initial window size:

const int INIT_WINDOW_WIDTH = { 900 };
const int INIT_WINDOW_HEIGHT = { 900 };

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };

// minimum allowable scale factor:

const float MINSCALE = { 0.05f };

// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };
float Time;
float S0 = 0.;
float T0 = 0.;
float Ds = 0.25f;
float Dt = 0.25f;
float Size = 0.5f;

const int MS_IN_THE_ANIMATION_CYCLE = 1000;

// which projection:

enum Projections
{
    ORTHO,
    PERSP
};


// which button:

enum ButtonVals
{
    RESET,
    QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };

// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
bool	Frozen;
bool    GridOn;
GLuint  SphereList;
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
bool	ShadowMapOn;
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees

float	LightX =  -5.;
float	LightY =  25.;
float	LightZ =  20.;

GLuint	DepthFramebuffer;
GLuint	DepthTexture;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLSLProgram*	GetDepth;
GLSLProgram*	RenderWithShadows;
GLSLProgram*	DisplayShadowMap;

GUI* Gui;

// voxel variables

Voxel cursor;
glm::vec3 cursorSize;
unsigned int selectedColor;
std::vector<Voxel> voxels;
std::vector<glm::vec3> colors{
    { 1.0, 1.0, 1.0},
    { 0.8, 0.8, 0.8},
    { 0.6, 0.6, 0.6},
    { 0.4, 0.4, 0.4},
    { 0.2, 0.2, 0.2},
    { 0.05, 0.05, 0.05},
    { 0.9, 0.1, 0.1},
    { 0.6, 0.1, 0.1},
    { 0.3, 0.1, 0.1},
    { 0.1, 0.9, 0.1},
    { 0.1, 0.6, 0.1},
    { 0.1, 0.3, 0.1},
    { 0.1, 0.1, 0.9},
    { 0.1, 0.1, 0.6},
    { 0.1, 0.1, 0.3},
    { 0.9, 0.9, 0.1},
    { 0.6, 0.6, 0.1},
    { 0.1, 0.9, 0.9},
    { 0.34, 0.54, 0.17},
    { 0.57, 0.45, 0.17},
    { 0.43, 0.29, 0.16},
};
std::vector<std::vector<Voxel>> savedVoxels;

// function prototypes:

void	Animate( );
void	Display( );
void	DisplayOneScene(GLSLProgram * );
void    DisplayText( );
void    DisplayVoxels( GLSLProgram* );
void	DoAxesMenu( int );
void	DoDisplayMenu(int);
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void    DrawGrid(float gridSize, size_t gridLines);
void    InitGui( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void    MouseMove( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );
void	OsuSphere(float radius, int slices, int stacks);
void	Cross(float v1[3], float v2[3], float vout[3]);
void	DrawPoint(struct point *p);
float	Unit(float vin[3], float vout[3]);

void    AddVoxelsAtCursor();
void    AddVoxel(glm::vec3, glm::vec3);
void    AnimateVoxel(Voxel& voxel);
void    ChangeCursor(glm::vec3);
void    ClearVoxels();
void    EraseVoxels();
void    InitVoxels();
void    MoveVoxel(Voxel& voxel, glm::vec3 position);
void    LoadVoxels(int);
void    PrintVoxels();
void    RemoveVoxel(glm::vec3);
bool    VoxelPresent(glm::vec3 position);


// main program:

int
main( int argc, char *argv[ ] )
{
    glutInit( &argc, argv );
    InitGraphics( );
    InitGui( );
    InitVoxels( );
    InitLists( );
    Reset( );
    InitMenus( );
    glutSetWindow( MainWindow );
    glutMainLoop( );
    // this is here to make the compiler happy:
    return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
// this is typically where animation parameters are set
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
    int ms = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
    ms %= MS_IN_THE_ANIMATION_CYCLE;
    Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;        // [ 0., 1. )

    // loop through all scene voxels
    for (Voxel &voxel : voxels)
    {
        AnimateVoxel(voxel);
    }
    AnimateVoxel(cursor);

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}

// draw the complete scene:

void
Display( )
{
    glutSetWindow( MainWindow );

    //first pass, render from light's perspective, store depth of scene in texture
    glBindFramebuffer(GL_FRAMEBUFFER, DepthFramebuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    glDisable(GL_NORMALIZE);

    // these matrices are the equivalent of projection and view matrices
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.f, 100.f);
    glm::vec3 lightPos(LightX, LightY, LightZ);
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.));

    //this matrix is the transformation matrix that the vertex shader will use instead of glViewProjectionMatrix:
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    GetDepth->Use();
    GetDepth->SetUniformVariable((char*)"uLightSpaceMatrix", lightSpaceMatrix);
    glm::vec3 color = glm::vec3(0., 1., 1.);
    GetDepth->SetUniformVariable((char*)"uColor", color);
    DisplayOneScene(GetDepth);
    GetDepth->Use(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // second pass:
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy;			// minimum dimension
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);

    if (ShadowMapOn)
    {
        DisplayShadowMap->Use();
        DisplayShadowMap->SetUniformVariable((char*)"uShadowMap", 0 );

        glm::mat4 model = glm::mat4(1.f);
        DisplayShadowMap->SetUniformVariable((char*)"uModel", model);

        glm::vec3 eye = glm::vec3(0., 0., 1.);
        glm::vec3 look = glm::vec3(0., 0., 0.);
        glm::vec3 up = glm::vec3(0., 1., 0.);
        glm::mat4 view = glm::lookAt(eye, look, up);
        DisplayShadowMap->SetUniformVariable((char*)"uView", view);

        glm::mat4 proj = glm::ortho(-0.6f, 0.6f,  -0.6f, 0.6f, .1f, 100.f);
        DisplayShadowMap->SetUniformVariable((char*)"uProj", proj);

        glBegin(GL_QUADS);
        glTexCoord2f(0., 0.);
        glVertex3f(-1., -1., 0.);
        glTexCoord2f(1., 0.);
        glVertex3f( 1., -1., 0.);
        glTexCoord2f(1., 1.);
        glVertex3f( 1.,  1., 0.);
        glTexCoord2f(0., 1.);
        glVertex3f(-1., 1., 0.);
        glEnd();

        DisplayShadowMap->Use(0);
    }
    else
    {
        RenderWithShadows->Use();
        RenderWithShadows->SetUniformVariable((char*)"uShadowMap", 0 );
        RenderWithShadows->SetUniformVariable((char*)"uLightX", LightX);
        RenderWithShadows->SetUniformVariable((char*)"uLightY", LightY);
        RenderWithShadows->SetUniformVariable((char*)"uLightZ", LightZ);
        RenderWithShadows->SetUniformVariable((char*)"uLightSpaceMatrix", lightSpaceMatrix);

        glm::vec3 eye = glm::vec3(0., 0., 8.);
        glm::vec3 look = glm::vec3(0., 0., 0.);
        glm::vec3 up = glm::vec3(0., 1., 0.);
        glm::mat4 view = glm::lookAt(eye, look, up);

        if (Scale < MINSCALE)
            Scale = MINSCALE;
        glm::vec3 scale = glm::vec3(Scale, Scale, Scale);
        view = glm::scale(view, scale);

        glm::vec3 xaxis = glm::vec3(1., 0., 0.);
        glm::vec3 yaxis = glm::vec3(0., 1., 0.);
        view = glm::rotate(view, glm::radians(Yrot), yaxis);
        view = glm::rotate(view, glm::radians(Xrot), xaxis);
        RenderWithShadows->SetUniformVariable((char*)"uView", view);

        glm::mat4 proj = glm::perspective(glm::radians(75.f), 1.f, .1f, 1000.f);
        RenderWithShadows->SetUniformVariable((char*)"uProj", proj);
        DisplayOneScene(RenderWithShadows);
        RenderWithShadows->Use(0);

        // possibly draw the axes:
        if (AxesOn != 0)
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90., 1., .1, 1000.);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);
            glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
            glRotatef(Yrot, 0., 1., 0.);
            glRotatef(Xrot, 1., 0., 0.);
            glColor3f(1., 1., 1.);
            glCallList(AxesList);
        }

    }

    // display gui
    Gui->Display();

    glutSwapBuffers( );
    glFlush( );
}


void
DisplayOneScene(GLSLProgram * prog )
{
    if (GridOn)
    {
        // render grid
        float gridSize = 100.;
        size_t gridLines = 100;
        float y_offset = -0.5;

        float spacing = gridSize / (gridLines - 1);
        float offset = gridSize / 2.;

        glm::mat4 model = glm::mat4(1.f);
        prog->SetUniformVariable((char*)"uModel", model);
        glm::vec3 color = glm::vec3(.2, .2, .2);
        prog->SetUniformVariable((char*)"uColor", color);

        for (size_t i = 0; i < gridLines; i++)
        {
            GLfloat pos = offset - i * spacing;

            glBegin(GL_LINES);
            glVertex3f(pos, y_offset, offset);
            glVertex3f(pos, y_offset, -offset);

            glVertex3f(offset, y_offset, pos);
            glVertex3f(-offset, y_offset, pos);
            glEnd();
        }
    }
    
    DisplayVoxels(prog);

    prog->Use(0);
}


void DisplayVoxels(GLSLProgram* prog)
{
    // render cursor
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, cursor.position);
    prog->SetUniformVariable((char*)"uModel", model);
    glm::vec3 color = glm::vec3(0.1, 1.0, 0.1);
    prog->SetUniformVariable((char*)"uColor", color);

    // render extended cursor
    for (int i = 0; i < cursorSize.x; ++i)
    {
        float xOffset = cursor.position.x + i;
        for (int j = 0; j < cursorSize.y; ++j)
        {
            float yOffset = cursor.position.y + j;
            for (int k = 0; k < cursorSize.z; ++k)
            {
                float zOffset = cursor.position.z + k;
                glm::mat4 cursorModel = glm::mat4(1.f);
                cursorModel = glm::translate(cursorModel, { xOffset, yOffset, zOffset });
                prog->SetUniformVariable((char*)"uModel", cursorModel);
                glutWireCube(1.);
            }
        }
    }

    // render voxels
    for (auto voxel : voxels)
    {
        model = glm::mat4(1.f);
        model = glm::translate(model, voxel.position);
        glm::vec3 scaleVector = voxel.scale * glm::vec3(1., 1., 1.);
        model = glm::scale(model, scaleVector);
        prog->SetUniformVariable((char*)"uModel", model);
        color = glm::vec3(1., 0., 0.);
        prog->SetUniformVariable((char*)"uColor", voxel.color);
        glutSolidCube(1.);
    }
}


struct point {
    float x, y, z;		// coordinates
    float nx, ny, nz;	// surface normal
    float s, t;		// texture coords
};



int			NumLngs, NumLats;
struct point* Pts;
struct point* PtsPointer(int lat, int lng)
{
    if (lat < 0)	lat += (NumLats - 1);
    if (lng < 0)	lng += (NumLngs - 1);
    if (lat > NumLats - 1)	lat -= (NumLats - 1);
    if (lng > NumLngs - 1)	lng -= (NumLngs - 1);
    return &Pts[NumLngs * lat + lng];
}

void
DrawPoint(struct point* p)
{
    glNormal3f(p->nx, p->ny, p->nz);
    glTexCoord2f(p->s, p->t);
    glVertex3f(p->x, p->y, p->z);
}


void
OsuSphere(float radius, int slices, int stacks)
{
    struct point top, bot;		// top, bottom points
    struct point* p;

    // set the globals:

    NumLngs = slices;
    NumLats = stacks;

    if (NumLngs < 3)
        NumLngs = 3;

    if (NumLats < 3)
        NumLats = 3;


    // allocate the point data structure:

    Pts = new struct point[NumLngs * NumLats];

    // fill the Pts structure:

    for (int ilat = 0; ilat < NumLats; ilat++)
    {
        float lat = (float)( -M_PI / 2. + M_PI * (float)ilat / (float)(NumLats - 1) );
        float xz = cos(lat);
        float y = sin(lat);
        for (int ilng = 0; ilng < NumLngs; ilng++)
        {
            float lng = (float)( -M_PI + 2. * M_PI * (float)ilng / (float)(NumLngs - 1) );
            float x = xz * cos(lng);
            float z = -xz * sin(lng);
            p = PtsPointer(ilat, ilng);
            p->x = radius * x;
            p->y = radius * y;
            p->z = radius * z;
            p->nx = x;
            p->ny = y;
            p->nz = z;
            p->s = (float) ( (lng + M_PI)        / (2.f*M_PI) );
            p->t = (float) ( (lat + (M_PI/2.f) ) / M_PI );
        }
    }

    top.x = 0.;		top.y = radius;	top.z = 0.;
    top.nx = 0.;		top.ny = 1.;		top.nz = 0.;
    top.s = 0.;		top.t = 1.;

    bot.x = 0.;		bot.y = -radius;	bot.z = 0.;
    bot.nx = 0.;		bot.ny = -1.;		bot.nz = 0.;
    bot.s = 0.;		bot.t = 0.;

    // connect the north pole to the latitude NumLats-2:

    glBegin(GL_QUADS);
    for (int ilng = 0; ilng < NumLngs - 1; ilng++)
    {
        p = PtsPointer(NumLats - 1, ilng);
        DrawPoint(p);

        p = PtsPointer(NumLats - 2, ilng);
        DrawPoint(p);

        p = PtsPointer(NumLats - 2, ilng + 1);
        DrawPoint(p);

        p = PtsPointer(NumLats - 1, ilng + 1);
        DrawPoint(p);
    }
    glEnd();

    // connect the south pole to the latitude 1:

    glBegin(GL_QUADS);
    for (int ilng = 0; ilng < NumLngs - 1; ilng++)
    {
        p = PtsPointer(0, ilng);
        DrawPoint(p);

        p = PtsPointer(0, ilng + 1);
        DrawPoint(p);

        p = PtsPointer(1, ilng + 1);
        DrawPoint(p);

        p = PtsPointer(1, ilng);
        DrawPoint(p);
    }
    glEnd();

    // connect the other 4-sided polygons:

    glBegin(GL_QUADS);
    for (int ilat = 2; ilat < NumLats - 1; ilat++)
    {
        for (int ilng = 0; ilng < NumLngs - 1; ilng++)
        {
            p = PtsPointer(ilat - 1, ilng);
            DrawPoint(p);

            p = PtsPointer(ilat - 1, ilng + 1);
            DrawPoint(p);

            p = PtsPointer(ilat, ilng + 1);
            DrawPoint(p);

            p = PtsPointer(ilat, ilng);
            DrawPoint(p);
        }
    }
    glEnd();

    delete[ ] Pts;
    Pts = NULL;
}


void
DoAxesMenu( int id )
{
    AxesOn = id;

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


void
DoDisplayMenu(int id)
{
    ShadowMapOn = ( id != 0 );

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


void
DoDebugMenu( int id )
{
    DebugOn = id;

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
    switch( id )
    {
        case RESET:
            Reset( );
            break;

        case QUIT:
            // gracefully close out the graphics:
            // gracefully close the graphics window:
            // gracefully exit the program:
            glutSetWindow( MainWindow );
            glFinish( );
            glutDestroyWindow( MainWindow );
            exit( 0 );
            break;

        default:
            fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
    }

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
    WhichProjection = id;

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
    glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

    char c;			// one character to print
    for( ; ( c = *s ) != '\0'; s++ )
    {
        glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
    }
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
    glPushMatrix( );
        glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
        float sf = ht / ( 119.05f + 33.33f );
        glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
        char c;			// one character to print
        for( ; ( c = *s ) != '\0'; s++ )
        {
            glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
        }
    glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
    // get # of milliseconds since the start of the program:

    int ms = glutGet( GLUT_ELAPSED_TIME );

    // convert it to seconds:

    return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
    glutSetWindow( MainWindow );

    int axesmenu = glutCreateMenu( DoAxesMenu );
    glutAddMenuEntry( "Off",  0 );
    glutAddMenuEntry( "On",   1 );

    int displaymenu = glutCreateMenu(DoDisplayMenu);
    glutAddMenuEntry("3D Scene", 0);
    glutAddMenuEntry("2D Shadow Map", 1);

    int debugmenu = glutCreateMenu( DoDebugMenu );
    glutAddMenuEntry( "Off",  0 );
    glutAddMenuEntry( "On",   1 );

    int mainmenu = glutCreateMenu( DoMainMenu );
    glutAddSubMenu(   "Axes",          axesmenu);
    glutAddSubMenu(   "Display",       displaymenu);
    glutAddMenuEntry( "Reset",         RESET );
    glutAddSubMenu(   "Debug",         debugmenu);
    glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

    glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT );
    MainWindow = glutCreateWindow( WINDOWTITLE );
    glutSetWindowTitle( WINDOWTITLE );

    glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );
    glutSetWindow( MainWindow );
    glutDisplayFunc( Display );
    glutReshapeFunc( Resize );
    glutKeyboardFunc( Keyboard );
    glutMouseFunc( MouseButton );
    glutMotionFunc( MouseMotion );
    glutPassiveMotionFunc( MouseMove );
    glutVisibilityFunc( Visibility );
    glutIdleFunc( Animate );

    // init glew (a window must be open to do this):

    GLenum err = glewInit( );
    if( err != GLEW_OK )
    {
        fprintf( stderr, "glewInit Error\n" );
    }
    else
    {
        fprintf(stderr, "GLEW initialized OK\n");
        fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    }

    GetDepth = new GLSLProgram();
    bool valid = GetDepth->Create((char*)"GetDepth.vert", (char*)"GetDepth.frag");
    if (!valid)
    {
        fprintf(stderr, "GetDepth Shader cannot be created!\n");		//DoMainMenu(QUIT);
    }
    else
    {
        fprintf(stderr, "GetDepth Shader created successfully.\n");
    }
    GetDepth->SetVerbose(false);
    
    RenderWithShadows = new GLSLProgram();
    valid = RenderWithShadows->Create((char*)"RenderWithShadows.vert", (char*)"RenderWithShadows.frag");
    if (!valid)
    {
        fprintf(stderr, "RenderWithShadows Shader cannot be created!\n");
    }
    else
    {
        fprintf(stderr, "RenderWithShadows Shader created successfully.\n");
    }
    RenderWithShadows->SetVerbose(false);

    DisplayShadowMap = new GLSLProgram();
    valid = DisplayShadowMap->Create((char*)"DisplayShadowMap.vert", (char*)"DisplayShadowMap.frag");
    if (!valid)
    {
        fprintf(stderr, "DisplayShadowMap Shader cannot be created!\n");
    }
    else
    {
        fprintf(stderr, "DisplayShadowMap Shader created successfully.\n");
    }
    DisplayShadowMap->SetVerbose(false);

    // create a framebuffer object and a depth texture object:
    glGenFramebuffers(1, &DepthFramebuffer);
    glGenTextures(1, &DepthTexture);

    //Create a texture that will be the framebuffer's depth buffer
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //Attach texture to framebuffer as depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, DepthFramebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);

    // force opengl to accept a framebuffer that doesn't have a color buffer in it:
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*
 *  Create floor grid display list
 */
void
DrawGrid(float gridSize, size_t gridLines)
{
    // render grid
    float y_offset = -1.;

    float spacing = gridSize / (gridLines - 1);
    float offset = gridSize / 2.;

    for (size_t i = 0; i < gridLines; i++)
    {
        GLfloat pos = offset - i * spacing;
        GLfloat delta = (float)i / (float)gridLines;

        glBegin(GL_LINES);
        glVertex3f(pos, y_offset, offset);
        glVertex3f(pos, y_offset, -offset);

        glVertex3f(offset, y_offset, pos);
        glVertex3f(-offset, y_offset, pos);
        glEnd();
    }
}


void
InitGui()
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // create gui
    Gui = new GUI(windowWidth, windowHeight);

    // info box

    auto* infoBox = new GUIBox(83, 82, 15, 12);
    infoBox->CenterChildren();
    Gui->AddChild(infoBox);

    auto* wasdTitle = new GUINode(0, 9, 12, 3);
    wasdTitle->SetText("Move Cursor:");
    infoBox->AddChild(wasdTitle);

    auto* wasdInfo = new GUINode(0, 6, 12, 3);
    wasdInfo->SetText("W A S D");
    infoBox->AddChild(wasdInfo);

    auto* qeTitle = new GUINode(0, 3, 12, 3);
    qeTitle->SetText("Raise/Lower:");
    infoBox->AddChild(qeTitle);

    auto* qeInfo = new GUINode(0, 0, 12, 3);
    qeInfo->SetText("Q E");
    infoBox->AddChild(qeInfo);

    // controls box

    auto* box = new GUIBox(83, 25, 15, 55);
    box->CenterChildren();
    Gui->AddChild(box);

    // color controls

    auto* colorTitle = new GUINode(0, 50, 15, 5);
    colorTitle->SetText("Selected color");
    box->AddChild(colorTitle);

    auto* colorBlock = new GUINode(0, 36, 12, 12);
    colorBlock->backgroundColor = colors[selectedColor];
    box->AddChild(colorBlock);

    auto* nextClrBtn = new GUIElement(0, 31, 12, 4);
    nextClrBtn->SetText("Next color");
    nextClrBtn->SetCallback([colorBlock]()
    {
        selectedColor = ++selectedColor == colors.size() ? 0 : selectedColor;
        colorBlock->backgroundColor = colors[selectedColor];
    });
    box->AddChild(nextClrBtn);

    auto* lastClrBtn = new GUIElement(0, 26, 12, 4);
    lastClrBtn->SetText("Prev. color");
    lastClrBtn->SetCallback([colorBlock]()
        {
            selectedColor = --selectedColor > colors.size() - 1 ? colors.size() - 1 : selectedColor;
            colorBlock->backgroundColor = colors[selectedColor];
        });
    box->AddChild(lastClrBtn);

    // cursor control buttons

    auto* cursorTitle = new GUINode(0, 21, 12, 4);
    cursorTitle->SetText("Change cursor");
    box->AddChild(cursorTitle);

    auto* cursorBtns = new GUIBox(0, 9, 12, 12);
    box->AddChild(cursorBtns);

    // x control

    auto* xBtnDown = new GUIElement(0, 8, 4, 4);
    xBtnDown->SetText("<");
    xBtnDown->SetCallback([]() { ChangeCursor({ -1, 0, 0 }); });
    cursorBtns->AddChild(xBtnDown);

    auto* cursorX = new GUINode(4, 8, 4, 4);
    cursorX->SetText("X");
    cursorBtns->AddChild(cursorX);

    auto* xBtnUp = new GUIElement(8, 8, 4, 4);
    xBtnUp->SetText(">");
    xBtnUp->SetCallback([]() { ChangeCursor({ 1, 0, 0 }); });
    cursorBtns->AddChild(xBtnUp);

    // y control

    auto* yBtnDown = new GUIElement(0, 4, 4, 4);
    yBtnDown->SetText("<");
    yBtnDown->SetCallback([]() { ChangeCursor({ 0, -1, 0 }); });
    cursorBtns->AddChild(yBtnDown);

    auto* cursorY = new GUINode(4, 4, 4, 4);
    cursorY->SetText("Y");
    cursorBtns->AddChild(cursorY);

    auto* yBtnUp = new GUIElement(8, 4, 4, 4);
    yBtnUp->SetText(">");
    yBtnUp->SetCallback([]() { ChangeCursor({ 0, 1, 0 }); });
    cursorBtns->AddChild(yBtnUp);

    // z control

    auto* zBtnDown = new GUIElement(0, 0, 4, 4);
    zBtnDown->SetText("<");
    zBtnDown->SetCallback([]() { ChangeCursor({ 0, 0, -1 }); });
    cursorBtns->AddChild(zBtnDown);

    auto* cursorZ = new GUINode(4, 0, 4, 4);
    cursorZ->SetText("Z");
    cursorBtns->AddChild(cursorZ);

    auto* zBtnUp = new GUIElement(8, 0, 4, 4);
    zBtnUp->SetText(">");
    zBtnUp->SetCallback([]() { ChangeCursor({ 0, 0, 1 }); });
    cursorBtns->AddChild(zBtnUp);

    auto* drawBtn = new GUIElement(0, 4, 14, 4);
    drawBtn->SetText("Draw [space]");
    drawBtn->SetCallback([]() { AddVoxelsAtCursor();  });
    box->AddChild(drawBtn);

    auto* eraseBtn = new GUIElement(0, 0, 14, 4);
    eraseBtn->SetText("Remove [r]");
    eraseBtn->SetCallback([]() { EraseVoxels();  });
    box->AddChild(eraseBtn);

    // demo box

    auto* demoBox = new GUIBox(83, 10, 15, 10);
    demoBox->CenterChildren();
    Gui->AddChild(demoBox);

    auto* demoTitle = new GUINode(0, 6, 12, 4);
    demoTitle->SetText("Demos");
    demoBox->AddChild(demoTitle);

    auto* demoBtn = new GUIElement(0, 2, 12, 4);
    demoBtn->SetText("Demo 1");
    demoBtn->SetCallback([]() { LoadVoxels(0); });
    demoBox->AddChild(demoBtn);
}


/*
 * Load voxels from saved file
 */
void
InitVoxels()
{
    std::vector<Voxel> demoVoxels = {};

    std::string line;
    std::ifstream infile("saved_voxels.txt");
    if (infile.is_open())
    {
        while (getline(infile, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> tokens{
                std::istream_iterator<std::string>{iss},
                std::istream_iterator<std::string>{} };

            std::vector<float> attrs;
            for (auto token : tokens)
            {
                attrs.push_back(stof(token));
            }

            glm::vec3 pos = { attrs[0], attrs[1], attrs[2] };
            glm::vec3 color = { attrs[3], attrs[4], attrs[5] };
            demoVoxels.push_back({ pos, color });
        }
        infile.close();
    }

    savedVoxels.push_back(demoVoxels);
}


float
Dot(float v1[3], float v2[3])
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


void
Cross(float v1[3], float v2[3], float vout[3])
{
    float tmp[3];
    tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
    tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
    tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
    vout[0] = tmp[0];
    vout[1] = tmp[1];
    vout[2] = tmp[2];
}


float
Unit(float vin[3], float vout[3])
{
    float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
    if (dist > 0.0)
    {
        dist = sqrt(dist);
        vout[0] = vin[0] / dist;
        vout[1] = vin[1] / dist;
        vout[2] = vin[2] / dist;
    }
    else
    {
        vout[0] = vin[0];
        vout[1] = vin[1];
        vout[2] = vin[2];
    }
    return dist;
}


/*
 * Adds new voxel at the cursor's position
 */
void AddVoxelsAtCursor()
{
    for (int i = 0; i < cursorSize.x; ++i)
    {
        for (int j = 0; j < cursorSize.y; ++j)
        {
            for (int k = 0; k < cursorSize.z; ++k)
            {
                glm::vec3 offset{ i, j, k };
                glm::vec3 voxelPosition = cursor.finalPosition + offset;

                // try to remove any voxel at the current location
                RemoveVoxel(voxelPosition);

                AddVoxel(voxelPosition, colors[selectedColor]);
            }
        }
    }
}


/*
 * Adds new voxel at the position
 */
void AddVoxel(glm::vec3 position, glm::vec3 color)
{
    // try to remove any voxel at the current location
    RemoveVoxel(position);

    Voxel voxel = { position, color };
    voxel.id = voxels.size();
    voxel.scale = .5f;
    voxel.startScale = .5f;
    voxel.finalScale = 1.f;
    voxel.startTime = glutGet(GLUT_ELAPSED_TIME);
    voxels.emplace_back(voxel);
    
}


/*
 * Removes any voxel under the cursor
 */
void RemoveVoxel(glm::vec3 position)
{
    voxels.erase(std::remove_if(voxels.begin(), voxels.end(), [position](Voxel v)
        {
            return glm::length(position - v.position) < .01f;
        }), voxels.end());
}


/*
 * Check if a voxel is present in a position
 */
bool
VoxelPresent(glm::vec3 position)
{
    return glm::length(position - cursor.finalPosition) < .01f;
}


void
AnimateVoxel(Voxel& voxel)
{
    if (voxel.startTime < 0) return;
    
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    const float animationLength = 300.; // ms
    
    const float deltaTime = currentTime - voxel.startTime;
    float t = deltaTime > animationLength ? 1 : deltaTime / animationLength;
    if (t >= 1)
    {
        voxel.startTime = -1;
        voxel.position = voxel.finalPosition;
        voxel.scale = voxel.finalScale;
    }
    else
    {
        const float ease = easeOutBack(t);
        voxel.position = glm::lerp(voxel.startPosition, voxel.finalPosition, ease);
        voxel.scale = glm::lerp(voxel.startScale, voxel.finalScale, ease);
    } 
}


void
MoveVoxel(Voxel& voxel, glm::vec3 position)
{
    voxel.finalPosition += position;
    voxel.startPosition = voxel.position;
    voxel.startTime = glutGet(GLUT_ELAPSED_TIME);
}


void
LoadVoxels(int index)
{
    // verify valid index
    if (savedVoxels.size() <= index) return;

    for (auto voxel : savedVoxels[index])
    {
        AddVoxel(voxel.position, voxel.color);

    }
}


void
PrintVoxels()
{
    for (auto voxel : voxels)
    {
        fprintf(stdout, "%.1f %.1f %.1f %.2f %.2f %.2f\n", voxel.position.x, voxel.position.y, voxel.position.z, voxel.color.r, voxel.color.g, voxel.color.b);
    }
}


/*
 * Erase all voxels under the cursor
 */
void
EraseVoxels()
{
    for (int i = 0; i < cursorSize.x; ++i)
    {
        for (int j = 0; j < cursorSize.y; ++j)
        {
            for (int k = 0; k < cursorSize.z; ++k)
            {
                glm::vec3 offset{ i, j, k };
                glm::vec3 voxelPosition = cursor.finalPosition + offset;

                // try to remove any voxel at the current location
                RemoveVoxel(voxelPosition);
            }
        }
    }
}


void
ChangeCursor(const glm::vec3 size)
{
    cursorSize += size;

    cursorSize.x = cursorSize.x < 1 ? 1 : cursorSize.x;
    cursorSize.y = cursorSize.y < 1 ? 1 : cursorSize.y;
    cursorSize.z = cursorSize.z < 1 ? 1 : cursorSize.z;
}


void ClearVoxels()
{
    voxels.clear();
}


// initialize the display lists that will not change:display
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
    // create the axes:

    AxesList = glGenLists( 1 );
    glNewList( AxesList, GL_COMPILE );
        glLineWidth( AXES_WIDTH );
            Axes( 10. );
        glLineWidth( 1. );
    glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
    if( DebugOn != 0 )
        fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

    switch( c )
    {
        case '1':
        case '2':
        case '3':
            LoadVoxels(c - '0' - 1);
            break;
        case 'a':
            MoveVoxel(cursor, { -1., 0., 0. });
            break;
        case 'c':
            ClearVoxels();
            break;
        case 'd':
            MoveVoxel(cursor, { 1., 0., 0. });
            break;

        case 'e':
            MoveVoxel(cursor, { 0., 1., 0. });
            break;

        case 'f':
        case 'F':
            Frozen = !Frozen;
            if( Frozen )
                glutIdleFunc(NULL);
            else
            glutIdleFunc(Animate);
            break;
        case 'g':
            GridOn = !GridOn;
            break;
        case 'i':
            ChangeCursor({ 1., 0., 0. });
            break;
        case 'I':
            ChangeCursor({ -1., 0., 0. });
            break;
        case 'm':
        case 'M':
            ShadowMapOn = !ShadowMapOn;
            break;
        case 'o':
            ChangeCursor({ 0., 1., 0. });
            break;
        case 'O':
            ChangeCursor({ 0., -1., 0. });
            break;
        case 'p':
            ChangeCursor({ 0., 0., 1. });
            break;
        case 'P':
            ChangeCursor({ 0., 0., -1. });
            break;
        case 'q':
            MoveVoxel(cursor, { 0., -1., 0. });
            break;

        case 'r':
            EraseVoxels();
            break;
        case 's':
            MoveVoxel(cursor, { 0., 0., 1. });
            break;

        case 'w':
            MoveVoxel(cursor, { 0., 0., -1. });
            break;
        case '/':
            PrintVoxels();
            break;
        case ESCAPE:
            DoMainMenu(QUIT);	// will not return here
            break;				// happy compiler

        case SPACE:
            AddVoxelsAtCursor();
            break;

        default:
            fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
    }

    // bubble event to gui
    Gui->OnKeyboard(c, x, y);

    // force a call to Display( ):

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
    int b = 0;			// LEFT, MIDDLE, or RIGHT

    if( DebugOn != 0 )
        fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

    
    // get the proper button bit mask:

    switch( button )
    {
        case GLUT_LEFT_BUTTON:
            b = LEFT;		break;

        case GLUT_MIDDLE_BUTTON:
            b = MIDDLE;		break;

        case GLUT_RIGHT_BUTTON:
            b = RIGHT;		break;

        default:
            b = 0;
            fprintf( stderr, "Unknown mouse button: %d\n", button );
    }


    // button down sets the bit, up clears the bit:

    if( state == GLUT_DOWN )
    {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b;		// set the proper bit
    }
    else
    {
        ActiveButton &= ~b;		// clear the proper bit
    }

    // bubble event to gui
    Gui->OnMouseButton(ActiveButton);
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
    if( DebugOn != 0 )
        fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


    int dx = x - Xmouse;		// change in mouse coords
    int dy = y - Ymouse;

    if( ( ActiveButton & LEFT ) != 0 )
    {
        Xrot += ( ANGFACT*dy );
        Yrot += ( ANGFACT*dx );
    }


    if( ( ActiveButton & MIDDLE ) != 0 )
    {
        Scale += SCLFACT * (float) ( dx - dy );

        // keep object from turning inside-out or disappearing:

        if( Scale < MINSCALE )
            Scale = MINSCALE;
    }

    Xmouse = x;			// new current position
    Ymouse = y;

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


void MouseMove(int x, int y)
{
    // bubble event to gui
    Gui->OnMouseMove(x, y);
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
    ActiveButton = 0;
    AxesOn = 0;
    DebugOn = 0;
    Frozen = false;
    GridOn = true;
    Scale  = 0.50f;
    ShadowMapOn = false;
    WhichProjection = PERSP;
    Xrot = Yrot = 0.;

    cursor.position = { 0., 0., 0. };
    cursor.startPosition = cursor.position;
    cursor.finalPosition = cursor.position;
    cursor.startTime = -1;
    cursor.id = -1;
    selectedColor = 0;
    cursorSize = { 1, 1, 1 };
}


// called when user resizes the window:

void
Resize( int width, int height )
{
    if( DebugOn != 0 )
        fprintf( stderr, "ReSize: %d, %d\n", width, height );

    // don't really need to do anything since window size is
    // checked each time in Display( ):

    glutSetWindow( MainWindow );
    glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
    if( DebugOn != 0 )
        fprintf( stderr, "Visibility: %d\n", state );

    if( state == GLUT_VISIBLE )
    {
        glutSetWindow( MainWindow );
        glutPostRedisplay( );
    }
    else
    {
        // could optimize by keeping track of the fact
        // that the window is not visible and avoid
        // animating or redrawing it ...
    }
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
        0.f, 1.f, 0.f, 1.f
          };

static float xy[ ] = {
        -.5f, .5f, .5f, -.5f
          };

static int xorder[ ] = {
        1, 2, -3, 4
        };

static float yx[ ] = {
        0.f, 0.f, -.5f, .5f
          };

static float yy[ ] = {
        0.f, .6f, 1.f, 1.f
          };

static int yorder[ ] = {
        1, 2, 3, -2, 4
        };

static float zx[ ] = {
        1.f, 0.f, 1.f, 0.f, .25f, .75f
          };

static float zy[ ] = {
        .5f, .5f, -.5f, -.5f, 0.f, 0.f
          };

static int zorder[ ] = {
        1, 2, 3, 4, -5, 6
        };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
    glBegin( GL_LINE_STRIP );
        glVertex3f( length, 0., 0. );
        glVertex3f( 0., 0., 0. );
        glVertex3f( 0., length, 0. );
    glEnd( );
    glBegin( GL_LINE_STRIP );
        glVertex3f( 0., 0., 0. );
        glVertex3f( 0., 0., length );
    glEnd( );

    float fact = LENFRAC * length;
    float base = BASEFRAC * length;

    glBegin( GL_LINE_STRIP );
        for( int i = 0; i < 4; i++ )
        {
            int j = xorder[i];
            if( j < 0 )
            {
                
                glEnd( );
                glBegin( GL_LINE_STRIP );
                j = -j;
            }
            j--;
            glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
        }
    glEnd( );

    glBegin( GL_LINE_STRIP );
        for( int i = 0; i < 5; i++ )
        {
            int j = yorder[i];
            if( j < 0 )
            {
                
                glEnd( );
                glBegin( GL_LINE_STRIP );
                j = -j;
            }
            j--;
            glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
        }
    glEnd( );

    glBegin( GL_LINE_STRIP );
        for( int i = 0; i < 6; i++ )
        {
            int j = zorder[i];
            if( j < 0 )
            {
                
                glEnd( );
                glBegin( GL_LINE_STRIP );
                j = -j;
            }
            j--;
            glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
        }
    glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
    // guarantee valid input:

    float h = hsv[0] / 60.f;
    while( h >= 6. )	h -= 6.;
    while( h <  0. ) 	h += 6.;

    float s = hsv[1];
    if( s < 0. )
        s = 0.;
    if( s > 1. )
        s = 1.;

    float v = hsv[2];
    if( v < 0. )
        v = 0.;
    if( v > 1. )
        v = 1.;

    // if sat==0, then is a gray:

    if( s == 0.0 )
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }

    // get an rgb from the hue itself:
    
    float i = floor( h );
    float f = h - i;
    float p = v * ( 1.f - s );
    float q = v * ( 1.f - s*f );
    float t = v * ( 1.f - ( s * (1.f-f) ) );

    float r = 0.;
    float g = 0.;
    float b = 0.;

    switch( (int) i )
    {
        case 0:
            r = v;	g = t;	b = p;
            break;
    
        case 1:
            r = q;	g = v;	b = p;
            break;
    
        case 2:
            r = p;	g = v;	b = t;
            break;
    
        case 3:
            r = p;	g = q;	b = v;
            break;
    
        case 4:
            r = t;	g = p;	b = v;
            break;
    
        case 5:
            r = v;	g = p;	b = q;
            break;
    }


    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}
