﻿/*
 * Copyright 1993-2013 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

#ifndef __RENDER_PARTICLES__
#define __RENDER_PARTICLES__

#include "OpenGLWindow/LoadShader.h"

class ParticleRenderer
{
    public:
        ParticleRenderer();
        ~ParticleRenderer();

        void setPositions(float *pos, int numParticles);
        void setVertexBuffer(unsigned int vbo, int numParticles);
        void setColorBuffer(unsigned int vbo)
        {
            m_colorVBO = vbo;
        }

        enum DisplayMode
        {
            PARTICLE_POINTS,
            PARTICLE_SPHERES,
            PARTICLE_NUM_MODES
        };

        void display(DisplayMode mode = PARTICLE_POINTS);
        void displayGrid();
        void setParticleRadius(float r)
        {
            m_particleRadius = r;
        }
        void setPointSize(float size)
        {
            m_pointSize = size;
        }
        void setParticleRenderingSize(float r)
        {
            m_particleRernderingSize = r;
        }
        void setFOV(float fov)
        {
            m_fov = fov;
        }
        void setWindowSize(int w, int h)
        {
            m_window_w = w;
            m_window_h = h;
        }

    protected: // methods
        void _initGL();
        void _drawPoints();
        //GLuint _compileProgram(const char *vsource, const char *fsource);

    protected: // data
        float *m_pos;
        int m_numParticles;

        float m_pointSize;
        float m_particleRadius;
        float m_particleRernderingSize;
        float m_fov;
        int m_window_w, m_window_h;

        ProgramBase m_program;
        int pointScale;
        int pointRadius;
        unsigned m_vbo;
        unsigned m_colorVBO;
};

extern const char *vertexShader;
extern const char *spherePixelShader;

#endif //__ RENDER_PARTICLES__
