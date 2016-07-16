#include "OpenGL.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace GLMath;

OpenGL gl;
static unsigned int tick = 0;
static float zoomLog = 0;
static float maxIterations = 128;
static float camSpeedSlow = 0.4;
static float camSpeed = 0.8;
unsigned int KEYS = 0;

mat4 viewMatrix = Identity();

inline void MoveCam()
{
	gl.Position += gl.Velocity * camSpeed * exp(zoomLog) * gl.DeltaTime();
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl.CalcFPS();

	MoveCam();
	viewMatrix = Translate(gl.Position) * Scale(exp(zoomLog) * gl.Width()/gl.Height(), exp(zoomLog), 1);
	GLuint viewMatrixId = glGetUniformLocation(gl.ProgramId(), "viewMatrix");
	GLuint maxItersId = glGetUniformLocation(gl.ProgramId(), "maxIterations");
	glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix.p[0][0]);
	glUniform1f(maxItersId, maxIterations);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

	glutSwapBuffers();
	glutPostRedisplay();

	if(tick % 1500 == 0)
	{
		cout << gl.FramesPerSecond() << endl;
		tick = 0;
	}
	tick++;
}

void Keyboard(unsigned char key, int x, int y)
{
	camSpeed = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) > 0 ? 0.4 : 0.8;
	switch (key)
	{
		case 'r': case 'R':
			gl.Wireframe();
			break;
		case 'f': case 'F':
			gl.Fullscreen();
			break;
		case 'w': case 'W':
			if((KEYS & 1) == 0)
			{
				gl.Velocity.y += 1;
				KEYS += 1;
			}
			break;
		case 'a': case 'A':
			if((KEYS & 2) == 0)
			{
				gl.Velocity.x -= 1;
				KEYS += 2;
			}
			break;
		case 's': case 'S':
			if((KEYS & 4) == 0)
			{
				gl.Velocity.y -= 1;
				KEYS += 4;
			}
			break;
		case 'd': case 'D':
			if((KEYS & 8) == 0)
			{
				gl.Velocity.x += 1;
				KEYS += 8;
			}
			break;
		case '=':
			maxIterations *= 1.25;
			break;
		case '-':
			maxIterations /= 1.25;
			break;
		case 27:
			glutDestroyWindow(gl.WindowId());
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void KeyboardUp(unsigned char key, int x, int y)
{
	camSpeed = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) > 0 ? 0.4 : 0.8;
	switch(key)
	{
		case 'w': case 'W':
			gl.Velocity.y = 0;
			KEYS -= 1;
			break;
		case 'a': case 'A':
			gl.Velocity.x = 0;
			KEYS -= 2;
			break;
		case 's': case 'S':
			gl.Velocity.y = 0;
			KEYS -= 4;
			break;
		case 'd': case 'D':
			gl.Velocity.x = 0;
			KEYS -= 8;
			break;
		case 'p': case 'P':
			gl.SaveScreenshot("awesome.bmp");
			break;
	}
	glutPostRedisplay();
}

void MouseWheel(int button, int state, int x, int y)
{
	if(button == 3 || button == 4)
	{
		if(state == GLUT_UP)
			return;

		float zoomSpeed = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) > 0 ? 0.01 : 0.1;
		// Scroll up
		if(button == 3)
			zoomLog -= zoomSpeed;
		else
			zoomLog += zoomSpeed;
	}
    glutPostRedisplay();
}

void Resize(int width, int height)
{
	glViewport(0, 0, width, height);
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	gl.GLInit(argc, argv, 800, 600, "Fractal Explorer");
	glutDisplayFunc(Display);
	glutIdleFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(MouseWheel);
	glutReshapeFunc(Resize);

	gl.StartFPS();
	glutMainLoop();
	return 0;
}
