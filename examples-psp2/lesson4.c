#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <zbuffer.h>

#include <psp2/kernel/processmgr.h>
#include <psp2shell.h>
#include <vita2d.h>

#define SCR_W 960
#define SCR_H 544

#define printf psp2shell_print

typedef struct {
	ZBuffer *frameBuffer;
	vita2d_texture *texture;
	void *data;
	int w, h;
	unsigned int pitch;
	int	mode;
} GLScreen;

GLScreen *screen;

void gluPerspective( GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar ) {
		
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;
	fH = tan( fovY / 360 * pi ) * zNear;
	fW = fH * aspect;
	glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}

void glInitVita() {
	
	screen = (GLScreen*) malloc(sizeof(*screen));
	memset(screen, 0, sizeof(GLScreen));
	
	vita2d_init();
	vita2d_set_clear_color(RGBA8(0xFF, 0xFF, 0xFF, 0xFF));
	
	screen->texture = vita2d_create_empty_texture_format(SCR_W, SCR_H, SCE_GXM_TEXTURE_FORMAT_R5G6B5);
	screen->data = vita2d_texture_get_datap(screen->texture);
	screen->w = SCR_W;
	screen->h = SCR_H;
	screen->pitch = screen->w * 2;
	screen->mode = ZB_MODE_5R6G5B;

    screen->frameBuffer = ZB_open( screen->w, screen->h, screen->mode, 0, 0, 0, 0);
    glInit( screen->frameBuffer );
}

void glSwapVita() {
	
	ZB_copyFrameBuffer(screen->frameBuffer, screen->data, screen->pitch);
	vita2d_start_drawing();
	vita2d_draw_texture(screen->texture, 0, 0);
	vita2d_end_drawing();
	vita2d_swap_buffers();
}

/* rotation angle for the triangle. */
float rtri = 0.0f;

/* rotation angle for the quadrilateral. */
float rquad = 0.0f;

/* A general OpenGL initialization function.  Sets all of the initial parameters. */
void InitGL(int Width, int Height)	        // We call this right after our OpenGL window is created.
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
  //glDepthFunc(GL_LESS);				// The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
}

/* The function called when our window is resized (which shouldn't happen, because we're fullscreen) */
void ReSizeGLScene(int Width, int Height)
{
  if (Height==0)				// Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

  glViewport(0, 0, Width, Height);		// Reset The Current Viewport And Perspective Transformation

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
  glMatrixMode(GL_MODELVIEW);
}

/* The main drawing function. */
void DrawGLScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
  glLoadIdentity();				// Reset The View

  glTranslatef(-1.5f,0.0f,-6.0f);		// Move Left 1.5 Units And Into The Screen 6.0
	
  glRotatef(rtri,0.0f,1.0f,0.0f);		// Rotate The Triangle On The Y axis 
  // draw a triangle (in smooth coloring mode)
  glBegin(GL_POLYGON);				// start drawing a polygon
  glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
  glVertex3f( 0.0f, 1.0f, 0.0f);		// Top
  glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
  glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
  glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
  glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left	
  glEnd();					// we're done with the polygon (smooth color interpolation)

  glLoadIdentity();				// make sure we're no longer rotated.
  glTranslatef(1.5f,0.0f,-6.0f);		// Move Right 3 Units, and back into the screen 6.0
	
  glRotatef(rquad,1.0f,0.0f,0.0f);		// Rotate The Quad On The X axis 
  // draw a square (quadrilateral)
  glColor3f(0.5f,0.5f,1.0f);			// set color to a blue shade.
  glBegin(GL_QUADS);				// start drawing a polygon (4 sided)
  glVertex3f(-1.0f, 1.0f, 0.0f);		// Top Left
  glVertex3f( 1.0f, 1.0f, 0.0f);		// Top Right
  glVertex3f( 1.0f,-1.0f, 0.0f);		// Bottom Right
  glVertex3f(-1.0f,-1.0f, 0.0f);		// Bottom Left	
  glEnd();					// done with the polygon

  rtri+=15.0f;					// Increase The Rotation Variable For The Triangle
  rquad-=15.0f;					// Decrease The Rotation Variable For The Quad 

  // swap the buffers to display, since double buffering is used.
  glSwapVita();
  
}

int main(int argc, char **argv) {
	
	psp2shell_init(3333);
	
	glInitVita();

    InitGL(SCR_W, SCR_H);
    
    while(1) {
		DrawGLScene();
	}
    
    // cleanup:
    ZB_close(screen->frameBuffer);
    psp2shell_exit();
    sceKernelExitProcess(0);
    
    return 0;
}
