//
//  main.cpp
//  sdlopengl
//
//  Created by McLeod, Aaron on 2015-02-13.
//  Copyright (c) 2015 McLeod, Aaron. All rights reserved.
//
#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #define GLEW_STATIC
    #include <GL/glew.h>
#elif _WIN32
    #include <windows.h>
    #include <GL/glew.h>
#elif _WIN64
    #include <windows.h>
    #include <GL/glew.h>
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifdef main
#undef main
#endif

#include <iostream>
#include <assert.h>
#include <vector>

void bindAttributes(GLuint& shaderProgram);
GLuint compileShaders(GLuint& vertexShader, GLuint& fragmentShader);
void setupElementBuffer(GLuint& ebo);
void setupVertices(GLuint& vbo);

void bindAttributes(GLuint& shaderProgram) {
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);


    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
}

GLuint compileShaders(GLuint& vertexShader, GLuint& fragmentShader) {
    const GLchar* vertex =
        "#version 150 core\n\
        in vec2 position;\n\
        in vec3 color;\n\
        in vec2 texcoord;\n\
        out vec3 Color;\n\
        out vec2 Texcoord;\n\
        void main() {\n\
            Color = color;\n\
            Texcoord = texcoord;\n\
            gl_Position = vec4(position, 0.0, 1.0);\n\
        }";

    const GLchar* fragment =
        "#version 150 core\n\
        in vec3 Color;\n\
        in vec2 Texcoord;\n\
        out vec4 outColor;\n\
        uniform sampler2D tex;\n\
        void main() {\n\
            outColor = texture(tex, Texcoord) * vec4(Color, 1.0);\n\
        }";

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex, NULL);
    glCompileShader(vertexShader);

    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cout << buffer << std::endl;
        assert(false);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cout << buffer << std::endl;
        assert(false);
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    return shaderProgram;
}

void setupElementBuffer(GLuint& ebo) {
    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

void setupVertices(GLuint& vbo) {
    GLfloat vertices[] = {
        //  Position   Color             Texcoords
        -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    SDL_Event windowEvent;

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint ebo;
    glGenBuffers(1, &ebo);

    setupVertices(vbo);
    setupElementBuffer(ebo);

    GLuint fragmentShader;
    GLuint vertexShader;

    GLuint shaderProgram = compileShaders(vertexShader, fragmentShader);
    bindAttributes(shaderProgram);

    GLuint tex;
    glGenTextures(1, &tex);

    SDL_Surface *image = IMG_Load("pic.png");

    if (!image) {
        assert(false);
    }


    int mode;
    if (image->format->BytesPerPixel == 3) { // RGB 24bit
        mode = GL_RGB;
    }
    else if (image->format->BytesPerPixel == 4) { // RGBA 32bit
        mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, mode, image->w, image->h, 0, mode, GL_UNSIGNED_BYTE, image->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    while (true) {
        if (SDL_PollEvent(&windowEvent)) {
            if (windowEvent.type == SDL_QUIT) {
                break;
            }
            else if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) {
                break;
            }
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);

    SDL_GL_DeleteContext(context);
    SDL_FreeSurface(image);

    SDL_Quit();
    return 0;
}