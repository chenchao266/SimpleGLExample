/*
 * Copyright 1993-2013 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

#include <glad/gl.h>

#include <math.h>
#include <assert.h>
#include <stdio.h>

#include "render_particles.h"
//#include "shaders.h"

#ifndef M_PI
#define M_PI    3.1415926535897932384626433832795
#endif

ParticleRenderer::ParticleRenderer()
    : m_pos(0),
      m_numParticles(0),
      m_pointSize(1.0f),
      m_particleRernderingSize(0.125f * 0.5f),
      //m_program(0),
      m_vbo(0),
      m_colorVBO(0)
{
    _initGL();
}

ParticleRenderer::~ParticleRenderer()
{
    m_pos = 0;
}

void ParticleRenderer::setPositions(float *pos, int numParticles)
{
    m_pos = pos;
    m_numParticles = numParticles;
}

void ParticleRenderer::setVertexBuffer(unsigned int vbo, int numParticles)
{
    m_vbo = vbo;
    m_numParticles = numParticles;
}

void ParticleRenderer::_drawPoints()
{
    if (!m_vbo)
    {
        glBegin(GL_POINTS);
        {
            int k = 0;

            for (int i = 0; i < m_numParticles; ++i)
            {
                glVertex3fv(&m_pos[k]);
                k += 4;
            }
        }
        glEnd();
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexPointer(4, GL_FLOAT, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);

        if (m_colorVBO)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO);
            glColorPointer(4, GL_FLOAT, 0, 0);
            glEnableClientState(GL_COLOR_ARRAY);
        }

        glDrawArrays(GL_POINTS, 0, m_numParticles);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }
}

void ParticleRenderer::display(DisplayMode mode /* = PARTICLE_POINTS */)
{
    switch (mode)
    {
        case PARTICLE_POINTS:
            glColor3f(1, 1, 1);
            glPointSize(m_pointSize);
            _drawPoints();
            break;

        default:
        case PARTICLE_SPHERES:
            glEnable(GL_POINT_SPRITE);
            glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
            glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);

            m_program.use();
            m_program.setUniform(pointScale, m_window_h / tanf(m_fov*0.5f*(float)M_PI/180.0f));
            m_program.setUniform(pointRadius, m_particleRernderingSize);

            glColor3f(1, 1, 1);
            _drawPoints();

            m_program.unuse();
            glDisable(GL_POINT_SPRITE);
            break;
    }
}

//unsigned
//ParticleRenderer::_compileProgram(const char *vsource, const char *fsource)
//{
//    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//    glShaderSource(vertexShader, 1, &vsource, 0);
//    glShaderSource(fragmentShader, 1, &fsource, 0);
//
//    glCompileShader(vertexShader);
//    glCompileShader(fragmentShader);
//
//    GLuint program = glCreateProgram();
//
//    glAttachShader(program, vertexShader);
//    glAttachShader(program, fragmentShader);
//
//    glLinkProgram(program);
//
//    // check if program linked
//    GLint success = 0;
//    glGetProgramiv(program, GL_LINK_STATUS, &success);
//
//    if (!success)
//    {
//        char temp[256];
//        glGetProgramInfoLog(program, 256, 0, temp);
//        printf("Failed to link program:\n%s\n", temp);
//        glDeleteProgram(program);
//        program = 0;
//    }
//
//    return program;
//}

void ParticleRenderer::_initGL()
{
    m_program.create(vertexShader, spherePixelShader);
    pointScale = m_program.getLoc("pointScale"); //  
    pointRadius = m_program.getLoc("pointRadius"); //  
#if !defined(__APPLE__) && !defined(MACOSX)
    glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);
    glClampColor(GL_CLAMP_FRAGMENT_COLOR, GL_FALSE);
#endif
}



#define STRINGIFY(A) #A

// vertex shader
const char *vertexShader = STRINGIFY(
    uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
uniform float densityScale;
uniform float densityOffset;
void main()
{
    // calculate window-space point size
    vec3 posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));
    float dist = length(posEye);
    gl_PointSize = pointRadius * (pointScale / dist);

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);

    gl_FrontColor = gl_Color;
}
);

// pixel shader for rendering points as shaded spheres
const char *spherePixelShader = STRINGIFY(
    void main()
{
    const vec3 lightDir = vec3(0.577, 0.577, 0.577);

    // calculate normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(N.xy, N.xy);

    if (mag > 1.0) discard;   // kill pixels outside circle

    N.z = sqrt(1.0 - mag);

    // calculate lighting
    float diffuse = max(0.0, dot(lightDir, N));

    gl_FragColor = gl_Color * diffuse;
}
);
