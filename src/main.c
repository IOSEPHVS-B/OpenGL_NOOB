#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "image.h"

int screenWidth = 640;
int screenHeight = 640;
int windowHandle = 0;
unsigned int frameCount = 0;

GLuint 
    vertexShaderID,
    fragmentShaderID,
    programID,
    vaoID,
    vboID,
    vaoID2,
    vboID2,
    colorBufferID,
    texture,
    texCoordID,
    texCoordID2;

const GLchar* vertexShader =
{
    "#version 400 \n"\

    "layout(location=0) in vec4 in_Position; \n"\
    "layout(location=1) in vec2 in_TexCoords; \n"\
    
    "out vec2 ex_TexCoords; \n"\
    

    "void main(void) \n"\
    "{ \n"\
    "  gl_Position = in_Position; \n"\
    "  ex_TexCoords = in_TexCoords; \n"\
    "}\n"
};

const GLchar* fragmentShader =
{
    "#version 400 \n"\

    "in vec2 ex_TexCoords; \n"\
    "uniform vec4 out_Palette[4]; \n"\
    "out vec4 out_Color; \n"\

    "uniform sampler2D ourTexture; \n"\

    "void main(void) \n"\
    "{ \n"\
    "  out_Color = texture(ourTexture, ex_TexCoords); \n"\
    "  out_Color = out_Palette[ 3-int(out_Color.x * 3.99) ]; \n"\
    "}\n"
};


void initialize(int argc, char* argv[]);
void initWindow(int argc, char* artv[]);
void resizeFunction(int, int);
void renderFunction(void);
void timerFunction(int);
void idleFunction(void);
void cleanup(void);
void createVBO(void);
void destroyVBO(void);
void createShaders(void);
void destroyShaders(void);


int main(int argc, char* argv[])
{
    initialize(argc, argv);
    
    glutMainLoop();

    return 0;
}


void initialize(int argc, char *argv[])
{
    GLenum GlewInitResult;

    initWindow(argc, argv);

    GlewInitResult = glewInit();

    if (GlewInitResult != GLEW_OK)
    {
        fprintf(stderr, "ERROR: %s\n", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }
    
    fprintf(
        stdout,
        "INFO: OpenGL version: %s\n",
        glGetString(GL_VERSION)
    );

    createShaders();
    createVBO();

    glClearColor(127/255.0f, 205/255.0f, 76/255.0f, 1.0f);
}


void initWindow(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitContextVersion(4, 0);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

    glutInitWindowSize(screenWidth, screenHeight);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    windowHandle = glutCreateWindow("window!");

    if (windowHandle < 1)
    {
        fprintf(stderr, "Couldn't init window/n");
        exit(EXIT_FAILURE);
    }

    glutReshapeFunc(resizeFunction);
    glutDisplayFunc(renderFunction);
    glutIdleFunc(idleFunction);
    glutTimerFunc(0, timerFunction, 0);
    glutCloseFunc(cleanup);
}


void resizeFunction(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, screenWidth, screenHeight);
}


void renderFunction()
{
    ++frameCount;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    //Shaydah
    glUniform4fv(glGetUniformLocation(programID, "out_Palette"), 4, (GLfloat*) pal);

    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(vaoID2);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
    glutPostRedisplay();
}


void idleFunction()
{
    glutPostRedisplay();
}


void timerFunction(int value)
{
    if (value != 0)
    {
        char tempString[128];

        sprintf(tempString, "Clown: %d FPS @ %d x %d", frameCount*4, screenWidth, screenHeight);

        glutSetWindowTitle(tempString);
    }

    frameCount = 0;
    glutTimerFunc(250, timerFunction, 1);
}


void cleanup(void)
{
    destroyShaders();
    destroyVBO();
}


void createVBO(void)
{
    GLfloat vertices[] = {
    -0.8f,  0.8f, 0.0f, 1.0f,
     0.0f,  0.8f, 0.0f, 1.0f,
    -0.8f, -0.0f, 0.0f, 1.0f,
     0.0f, -0.0f, 0.0f, 1.0f
    };

    GLfloat texCoords[] = {
    0.0f,  0.0f,
    0.5f,  0.0f,
    0.0f, 0.5f,
    0.5f, 0.5f
    };

    GLfloat vertices2[] = {
     0.0f,  0.0f, 0.0f, 1.0f,
     0.8f,  0.0f, 0.0f, 1.0f,
     0.0f, -0.8f, 0.0f, 1.0f,
     0.8f, -0.8f, 0.0f, 1.0f
    };

    GLfloat texCoords2[] = {
    0.5f,  0.5f,
    1.0f,  0.5f,
    0.5f, 1.0f,
    1.0f, 1.0f
    };

    GLenum errorCheckValue = glGetError();

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &texCoordID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &vaoID2);
    glBindVertexArray(vaoID2);

    glGenBuffers(1, &vboID2);
    glBindBuffer(GL_ARRAY_BUFFER, vboID2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &texCoordID2);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordID2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords2), texCoords2, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);


    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 16, 16, 0, GL_RED, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    errorCheckValue = glGetError();
    if (errorCheckValue != GL_NO_ERROR)
    {
        fprintf(stderr, "VBO ERROR: %s\n", gluErrorString(errorCheckValue));
        exit(-1);
    }
}


void destroyVBO(void)
{
    GLenum errorCheckValue = glGetError();

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &texCoordID);
    glDeleteBuffers(1, &vboID);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vaoID);

    errorCheckValue = glGetError();
    if (errorCheckValue != GL_NO_ERROR)
    {
        fprintf(stderr, "ERROR: Could not destroy the VBO: %s \n", gluErrorString(errorCheckValue));
        exit(-1);
    }
}


void createShaders(void)
{
    GLenum errorCheckValue = glGetError();

    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderID);

    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderID);

    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    glUseProgram(programID);

    errorCheckValue = glGetError();
    if (errorCheckValue != GL_NO_ERROR)
    {
        fprintf(stderr, "ERROR: Could not create the shaders: %s \n", gluErrorString(errorCheckValue));
        exit(-1);
    }
}


void destroyShaders(void)
{
    GLenum ErrorCheckValue = glGetError();

    glUseProgram(0);

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    glDeleteProgram(programID);

    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
        stderr,
        "ERROR: Could not destroy the shaders: %s \n",
        gluErrorString(ErrorCheckValue)
        );

        exit(-1);
    }
}
