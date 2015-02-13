//
//  main.cpp
//  sdlopengl
//
//  Created by McLeod, Aaron on 2015-02-13.
//  Copyright (c) 2015 McLeod, Aaron. All rights reserved.
//

#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <OpenGL/OpenGL.h>
#include <iostream>
#include <assert.h>

void bindAttributes(GLuint& shaderProgram);
GLuint compileShaders();
void setupGL(GLuint& vbo);

void bindAttributes(GLuint& shaderProgram) {
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
}

GLuint compileShaders() {
    const GLchar* vertex = "#version 150\n\
    in vec2 position;\n\
    void main() {\n\
      gl_Position = vec4(position, 0.0, 1.0);\n\
    }";
    
    const GLchar* fragment = "#version 150\n\
    out vec4 outColor;\n\
    void main() {\n\
      outColor = vec4(1.0, 1.0, 1.0, 1.0);\n\
    }";
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
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
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    
    if (status != GL_TRUE) {
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


void setupGL(GLuint& vbo) {
    float vertices[] = {
        0.0f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main(int argc, const char * argv[]) {
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
    
    setupGL(vbo);
    GLuint shaderProgram = compileShaders();
    bindAttributes(shaderProgram);
    
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
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        SDL_GL_SwapWindow(window);
    }
    
    
    SDL_GL_DeleteContext(context);
    
    SDL_Quit();
    return 0;
}
