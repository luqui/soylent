//----------------------------------------------------------------------------
// File : flo.cpp
//----------------------------------------------------------------------------
// Copyright 2003 Mark J. Harris and
//     The University of North Carolina at Chapel Hill
//----------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that 
// copyright notice and this permission notice appear in supporting 
// documentation.  Binaries may be compiled with this software without any 
// royalties or restrictions. 
//
// The author(s) and The University of North Carolina at Chapel Hill make no 
// representations about the suitability of this software for any purpose. 
// It is provided "as is" without express or implied warranty.
/**
* @file flo.cpp
* 
* Flo fluid simulation class implementation.
*/

#include <glh/glh_extensions.h>
#include "flo.h"
#include "Texture.h"
#include <gl/glut.h>
#include <assert.h>
#include "shared/pbuffer.h"
#include "shared/nv_png.h"
#include "shared/array_texture.h"
#include "shared/data_path.h"
#include "shared/load_cubemap.h"
#include "shared/geep/fluidbase.h"
#include <iostream>

using namespace glh;

//----------------------------------------------------------------------------
// Function     	: Flo::Flo
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::Flo()
* @brief Default Constructor
*/ 
Flo::Flo(int width, int height) 
:   _iWidth(width),
    _iHeight(height),
    _rTimestep(1),
    _iNumPoissonSteps(50),
    _rInkLongevity(0.9995f),
    _rViscosity(0),
    _bArbitraryBC(false),
    _bClearPressureEachStep(true),
    _bZCullOpt(false),
    _bComputeVorticity(true),
    _bApplyVCForce(true),
    _rVorticityConfinementScale(0.035f),
    _bImpulseToProcess(false),
    _bInkToAdd(false),
    _pVectorBuffer(0),
    _pScalarBuffer(0),
    _iDisplayTexture(0),
    _iBCTexture(0),
    _iBCDisplayTexture(0),
    _iBCDetailTexture(0),
    _bNormalMapDirty(true),
    _iNormalMap(0),
    _iCubeMap(0),
    _zeros(0),
    _iIndexBuffer(0),
    _iVertexBuffer(0),
    _iVBWidth(0),
    _iVBHeight(0),
    _bUpdateVectorField(false),
    _eVectorField(DISPLAY_VELOCITY)
{
    _pFluidImpl = new FluidBase(width, height, 1);

    _rInkColor[0] = _rInkColor[1] = _rInkColor[2] = 1;
	_rInkColor[0] = 0;
    memset(_iTextures, 0, TEXTURE_COUNT * sizeof(GLuint));

    _zeros = new float[_iWidth * _iHeight * 4];
    memset(_zeros, 0, _iWidth * _iHeight * 4 * sizeof(float));
}

//----------------------------------------------------------------------------
// Function     	: Flo::~Flo
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::~Flo()
* @brief Destructor
*/ 
Flo::~Flo()
{
	glDeleteTextures(TEXTURE_COUNT, _iTextures);
    delete _pFluidImpl;
    delete [] _zeros;
}

//----------------------------------------------------------------------------
// Function     	: Flo::Initialize
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::Initialize()
* @brief Initializes the Flo simulator.
* 
* Here we initialize the simulation.
*/ 
void Flo::Initialize(CGcontext context, bool bInitVectorViz)
{
    // create the offscreen buffer -- this has to be a float buffer because
    // we need float precision for the simulation.
    _pVectorBuffer = new PBuffer("rgba float=16f depth=0");
    _pScalarBuffer = new PBuffer("r float=32f depth=24");

    _pVectorBuffer->Initialize(_iWidth, _iHeight, false, true);
    _pScalarBuffer->Initialize(_iWidth, _iHeight, false, true);

    // Set the constant state for the pbuffer.
    _pVectorBuffer->Activate();
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        gluOrtho2D(0, 1, 0, 1);//*?* Should it be 0,1,0,1?
        glDisable(GL_DEPTH_TEST);
        glClearColor(0, 0, 0, 0);
    }
    _pVectorBuffer->Deactivate();

    _pScalarBuffer->Activate();
    { 
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, 1, 0, 1);//*?* Should it be 0,1,0,1?
        glClearDepth(1);
        glClearColor(0, 0, 0, 0);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    }
    _pScalarBuffer->Deactivate();

    // create texture objects.  All are float textures.
    int iTex = 0;
    glGenTextures(TEXTURE_COUNT, _iTextures);

    for (iTex = 0; iTex < TEXTURE_VECTOR_COUNT; ++iTex)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[iTex]);
        glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV,
                     _iWidth, _iHeight, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    for (iTex = TEXTURE_VECTOR_COUNT; iTex < TEXTURE_COUNT; ++iTex)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[iTex]);
        glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R16_NV, 
                     _iWidth, _iHeight, 0, GL_RGBA, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glGenTextures(1, &_iBCTexture);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iBCTexture);
    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8,
                 _iWidth, _iHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_NV, 
                    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _ClearTexture(_iBCTexture);

    glGenTextures(1, &_iBCDisplayTexture);
    glBindTexture(GL_TEXTURE_2D, _iBCDisplayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _iWidth, _iHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    _ClearTexture(_iBCDisplayTexture, GL_TEXTURE_2D);

    // create the display texture
    glGenTextures(1, &_iDisplayTexture);
    glBindTexture(GL_TEXTURE_2D, _iDisplayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _iWidth, _iHeight, 
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // create the normal map texture
    glGenTextures(1, &_iNormalMap);
    glBindTexture(GL_TEXTURE_2D, _iNormalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _iWidth, _iHeight, 
                 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // load cube map
    glGenTextures(1, &_iCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _iCubeMap);
    load_png_cubemap("cube_face_%s.png", true);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // load obstacle detail texture.
    array2<vec3ub> img;
    read_png_rgb("flo_bc.png", img);

    glGenTextures(1, &_iBCDetailTexture);
    glBindTexture(GL_TEXTURE_2D, _iBCDetailTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    make_rgb_texture(img, true);

    _pFluidImpl->Initialize(context);

    // Initialize the Streamops...
    _InitializeStreamOps(context, bInitVectorViz);

    if (bInitVectorViz)
    {
        // Initialize Vector Field Visualization
        glGenTextures(1, &_iVectorFieldTexture);
        glBindTexture(GL_TEXTURE_2D, _iVectorFieldTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI, _iWidth, _iHeight, 0, 
                    GL_RGBA, GL_FLOAT, 0);

        InitializeVertexBuffer(_iWidth / 4, _iHeight / 4);
    }

    // clear the textures
    Reset(true);
}

//----------------------------------------------------------------------------
// Function     	: Flo::_InitializeStreamOps
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::InitializeStreamOps()
* @brief Initializes the StreamOps used in the fluid simulation
* 
* Here we initialize the Stream operations, passing them parameters such
* as input textures, and fragment programs.
*/ 
void Flo::_InitializeStreamOps(CGcontext context, bool bInitVectorViz)
{
    data_path media;
    media.path.push_back(".");
	media.path.push_back("Media");

    std::string programsPath = media.get_path("programs/gpgpu_fluid/flo.cg");
    if (programsPath.empty())
    {
        printf("Error: Unable to locate Cg programs...\n");
    }

    programsPath += "/";

    std::string args = "-I" + programsPath;
    std::vector<const char*> fpargs;
    fpargs.clear();
    fpargs.push_back(args.c_str());
    fpargs.push_back(0);

    // Display: These ops are used to display scalar and vector fields with
    // and without bilinear interpolation.
	_displayColor.SetFPArgs(fpargs);
	_displayColor.InitializeFP(context, programsPath + "flo.cg", "displayColor");
	_displayColor.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayColor.SetStreamRect(0, 0, _iWidth, _iHeight);

	_displayColorBilerp.SetFPArgs(fpargs);
	_displayColorBilerp.InitializeFP(context, programsPath + "flo.cg", "displayColorBilerp");
	_displayColorBilerp.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayColorBilerp.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayVector.SetFPArgs(fpargs);
    _displayVector.InitializeFP(context, programsPath + "flo.cg", "displayVector");
    _displayVector.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayVector.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayWhiteVector.SetFPArgs(fpargs);
    _displayWhiteVector.InitializeFP(context, programsPath + "flo.cg", "displayWhiteVector");
    _displayWhiteVector.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayWhiteVector.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayVectorBilerp.SetFPArgs(fpargs);
    _displayVectorBilerp.InitializeFP(context, programsPath + "flo.cg", "displayVectorBilerp");
    _displayVectorBilerp.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayVectorBilerp.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayWhiteVectorBilerp.SetFPArgs(fpargs);
    _displayWhiteVectorBilerp.InitializeFP(context, programsPath + "flo.cg", "displayWhiteVectorBilerp");
    _displayWhiteVectorBilerp.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayWhiteVectorBilerp.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayScalar.SetFPArgs(fpargs);
    _displayScalar.InitializeFP(context, programsPath + "flo.cg", "displayScalar");
    _displayScalar.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayScalar.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayScalarBilerp.SetFPArgs(fpargs);
    _displayScalarBilerp.InitializeFP(context, programsPath + "flo.cg", "displayScalarBilerp");
    _displayScalarBilerp.SetTexCoordRect(0, 0, _iWidth, _iHeight);
	_displayScalarBilerp.SetStreamRect(0, 0, _iWidth, _iHeight);

    _genNormalMap.InitializeFP(context, programsPath + "/flo.cg", "genNormalMap");
    _genNormalMap.SetTextureParameter("ink", _iTextures[TEXTURE_DENSITY]);
    _genNormalMap.SetTexCoordRect(0, 0, _iWidth, _iHeight);
    _genNormalMap.SetStreamRect(0, 0, _iWidth, _iHeight);

    _displayMilkAndMetal.InitializeFP(context, programsPath + "/flo.cg", "displayMilkAndMetal");
    _displayMilkAndMetal.SetTexCoordRect(0, 0, 0, _iWidth, _iHeight);
    _displayMilkAndMetal.SetTexCoordRect(1, 0, 0, 1, 1);
	_displayMilkAndMetal.SetStreamRect(0, 0, _iWidth, _iHeight);

    if (bInitVectorViz)
    {
        _vectorFieldVP.InitializeVP(context, programsPath + "/flo.cg", "vectorFieldVP");
        _scalarFieldVP.InitializeVP(context, programsPath + "/flo.cg", "scalarFieldVP");
        _vectorFieldPointSizeVP.InitializeVP(context, programsPath + "/flo.cg", "vectorFieldPointSizeVP");
        _scalarFieldPointSizeVP.InitializeVP(context, programsPath + "/flo.cg", "scalarFieldPointSizeVP");
    }
}

//----------------------------------------------------------------------------
// Function     	: Flo::_ClearTexture
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::_ClearTexture()
* @brief Clears the texture specified by @a id to all zeros.
*/ 
void Flo::_ClearTexture(unsigned int id, GLenum target /* = GL_TEXTURE_RECTANGLE_NV*/)
{
    glBindTexture(target, id);
    glTexSubImage2D(target, 0, 0, 0, _iWidth, _iHeight, GL_RGBA, GL_FLOAT, _zeros);
}


//----------------------------------------------------------------------------
// Function     	: Flo::Shutdown
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::Shutdown()
* @brief Shuts down the Flo simulator.
*/ 
void Flo::Finalize()
{
    _pFluidImpl->Finalize();

    glDeleteTextures(TEXTURE_COUNT, _iTextures);
    glDeleteTextures(1, &_iDisplayTexture);
    glDeleteTextures(1, &_iNormalMap);
    glDeleteTextures(1, &_iCubeMap);
    glDeleteTextures(1, &_iVectorFieldTexture);

    delete _pVectorBuffer;
    _pVectorBuffer = 0;

    delete _pScalarBuffer;
    _pScalarBuffer = 0;

    _displayVector.ShutdownFP();
    _displayWhiteVector.ShutdownFP();
    _displayVectorBilerp.ShutdownFP();
    _displayWhiteVectorBilerp.ShutdownFP();
    _displayScalar.ShutdownFP();
    _displayScalarBilerp.ShutdownFP();

    _genNormalMap.ShutdownFP();
    _displayMilkAndMetal.ShutdownFP();

    glDeleteBuffersARB(1, &_iIndexBuffer);
    glDeleteBuffersARB(1, &_iVertexBuffer);
    glDeleteBuffersARB(1, &_iHeadTailBuffer);
}

//----------------------------------------------------------------------------
// Function     	: Flo::Update
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::Update(float timestep)
* @brief Runs a fluid simulation step. 
* 
* Update solves the incompressible Navier-Stokes equations for a single 
* time step.  It consists of four main steps:
*
* 1. Add Impulse
* 2. Advect
* 3. Apply Vorticity Confinement
* 4. Diffuse (if viscosity > 0)
* 5. Project (computes divergence-free velocity from divergent field).
*
* 1. Advect: pulls the velocity and Ink fields forward along the velocity
*            field.  This results in a divergent field, which must be 
*            corrected in step 4.
* 2. Add Impulse: simply add an impulse to the velocity (and optionally,Ink)
*    field where the user has clicked and dragged the mouse.
* 3. Apply Vorticity Confinement: computes the amount of vorticity in the 
*            flow, and applies a small impulse to account for vorticity lost 
*            to numerical dissipation.
* 4. Diffuse: viscosity causes the diffusion of velocity.  This step solves
*             a poisson problem: (I - nu*dt*Laplacian)u' = u.
* 5. Project: In this step, we correct the divergence of the velocity field
*             as follows.
*        a.  Compute the divergence of the velocity field, div(u)
*        b.  Solve the Poisson equation, Laplacian(p) = div(u), for p using 
*            Jacobi iteration.
*        c.  Now that we have p, compute the divergence free velocity:
*            u = gradient(p)
*/ 
void Flo::Update()
{
    // Activate rendering to the offscreen buffer
    _pVectorBuffer->Activate();

    //---------------
    // 1.  Advect
    //---------------
    // Advect velocity (velocity advects itself, resulting in a divergent
    // velocity field.  Later, correct this divergence).

    // Set the no-slip velocity...
    // This sets the scale to -1, so that v[0, j] = -v[1, j], so that at 
    // the boundary between them, the avg. velocity is zero. 
    _pFluidImpl->ApplyBoundaryConditions(_iTextures[TEXTURE_VELOCITY], -1);

    // Set the no-slip velocity on arbitrary interior boundaries if they are enabled
    if (_bArbitraryBC)
        _pFluidImpl->ComputeArbitraryVelocityBCs(_iTextures[TEXTURE_VELOCITY],
                                                 _iTextures[TEXTURE_VELOCITY_OFFSETS]);

    // Advect velocity.
    _pFluidImpl->Advect(_iTextures[TEXTURE_VELOCITY], 
                        _iTextures[TEXTURE_VELOCITY], 
                        _rTimestep, 1);
    
    // Set ink boundaries to zero
    _pFluidImpl->ApplyBoundaryConditions(_iTextures[TEXTURE_DENSITY], 0);

    // Advect "ink", a passive scalar carried by the flow.
    _pFluidImpl->Advect(_iTextures[TEXTURE_VELOCITY], 
                        _iTextures[TEXTURE_DENSITY], 
                        _rTimestep, _rInkLongevity);

    if (_bUpdateVectorField && _eVectorField == DISPLAY_INK) 
    {
        glBindTexture(GL_TEXTURE_2D, _iVectorFieldTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);
    }

    //---------------
    // 2. Add Impulse
    //---------------
    for (std::vector<Impulse>::iterator imp = _impulses.begin();
         imp != _impulses.end(); 
         imp++)
    {   
        _pFluidImpl->ApplyImpulse(_iTextures[TEXTURE_VELOCITY], 
                                  (*imp).strength, (*imp).position, (*imp).radius);

        if ((*imp).bAddInk)
        {
            _pFluidImpl->ApplyImpulse(_iTextures[TEXTURE_DENSITY], 
                                      (*imp).color, (*imp).position, (*imp).radius);
        }
    }
    _impulses.clear();

    //---------------
    // 3. Apply Vorticity Confinement
    //---------------
    if (_bComputeVorticity)
        _pFluidImpl->ComputeVorticity(_iTextures[TEXTURE_VELOCITY],
                                      _iTextures[TEXTURE_VORTICITY]);

    if (_bUpdateVectorField && _eVectorField == DISPLAY_VORTICITY) 
    {
        glBindTexture(GL_TEXTURE_2D, _iVectorFieldTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);
    }

    if (_bApplyVCForce)
        _pFluidImpl->ComputeVorticityConfinement(_iTextures[TEXTURE_VELOCITY],
                                                 _iTextures[TEXTURE_VORTICITY],
                                                 _rTimestep,
                                                 _rVorticityConfinementScale);
                
    //--------------- 
    // 3. Diffuse (if viscosity is > 0)
    //---------------
    // If this is a viscous fluid, solve the poisson problem for the viscous
    // diffusion
    if (_rViscosity > 0)
    {
        _pFluidImpl->ComputeViscousDiffusion(_iTextures[TEXTURE_VELOCITY],
                                             _rTimestep, _rViscosity, 
                                             _iNumPoissonSteps);
    }

    //---------------
    // 4. Project divergent velocity into divergence-free field
    //---------------
    _pScalarBuffer->Activate(_pVectorBuffer);

    // Use zcull when arbitrary BCs are in effect to save computation
    if (_bArbitraryBC)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    //---------------
    // 4a. compute divergence
    //---------------
    // Compute the divergence of the velocity field
    _pFluidImpl->ComputeDivergence(_iTextures[TEXTURE_DIVERGENCE], 
                                   _iTextures[TEXTURE_VELOCITY]);
    
    //---------------
    // 4b. Compute pressure disturbance
    //---------------
    
    if (_bZCullOpt)
    {
        _pFluidImpl->SetupPoissonPressureZCull(_iTextures[TEXTURE_PRESSURE]);
    }

    // Clear the pressure texture, to initialize the pressure disturbance to 
    // zero before iterating.  If this is disabled, the solution converges 
    // faster, but tends to have oscillations.
    if (_bClearPressureEachStep)
        _ClearTexture(_iTextures[TEXTURE_PRESSURE]);

    // Solve for the pressure disturbance caused by the divergence, by solving
    // the poisson problem Laplacian(p) = div(u)
    _pFluidImpl->SolvePoissonPressureEquation(_iTextures[TEXTURE_PRESSURE], 
                                              _iTextures[TEXTURE_DIVERGENCE],
                                              _iNumPoissonSteps, 
                                              _bArbitraryBC,
                                              _iTextures[TEXTURE_PRESSURE_OFFSETS]);

    if (_bUpdateVectorField && _eVectorField == DISPLAY_PRESSURE) 
    {
        glBindTexture(GL_TEXTURE_2D, _iVectorFieldTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);
    }

    _pVectorBuffer->Activate(_pScalarBuffer);

    // Set the no-slip velocity...
    // We do this here again because the advection step modified the velocity,
    // and we want to avoid instabilities at the boundaries.

    // This sets the scale to -1, so that v[0, j] = -v[1, j], so that at 
    // the boundary between them, the avg. velocity is zero.
    _pFluidImpl->ApplyBoundaryConditions(_iTextures[TEXTURE_VELOCITY], -1);

    if (_bArbitraryBC)
        _pFluidImpl->ComputeArbitraryVelocityBCs(_iTextures[TEXTURE_VELOCITY],
                                                 _iTextures[TEXTURE_VELOCITY_OFFSETS]);

    //---------------
    // 4c. Subtract gradient(p) from u
    //---------------
    // This gives us our final, divergence free velocity field. 
    _pFluidImpl->SubtractPressureGradient(_iTextures[TEXTURE_PRESSURE], 
                                          _iTextures[TEXTURE_VELOCITY]);

    if (_bUpdateVectorField && _eVectorField == DISPLAY_VELOCITY) 
    {
        glBindTexture(GL_TEXTURE_2D, _iVectorFieldTexture);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);
    }

    // End rendering to the offscreen buffer.
    _pVectorBuffer->Deactivate();

    _bNormalMapDirty = true;
}


//----------------------------------------------------------------------------
// Function     	: Flo::Reset
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::Reset()
* @brief Resets the simulation.
*/ 
void Flo::Reset(bool resetBC /* = false */)
{
    // create some zeros to clear textures with.
    float *zeros = new float[_iWidth * _iHeight * 4];
    memset(zeros, 0, _iWidth * _iHeight * 4 * sizeof(float));

    int iTex = 0;
    // clear all textures to zero.
    for (iTex = 0; iTex < TEXTURE_COUNT; ++iTex)
    {
        glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[iTex]);
        glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, _iWidth, _iHeight, 
                        GL_RGBA, GL_FLOAT, zeros);
    }

    if (resetBC)
    {
        ResetBoundary();
    }
	else {//repeated in ResetBoundary()
		_pVectorBuffer->Activate();
		{
			_pFluidImpl->UpdateBoundaryOffsetTables(_iTextures[TEXTURE_VELOCITY_OFFSETS],
													_iTextures[TEXTURE_PRESSURE_OFFSETS],
													_iBCTexture);
		}
		_pScalarBuffer->Activate(_pVectorBuffer);
		{
			_pFluidImpl->SetupObstacleZCull(_iBCTexture);
		}  
		_pScalarBuffer->Deactivate();
	}
    delete [] zeros;
}

void Flo::ResetBoundary() {
	// create some zeros to clear textures with.
    float *zeros = new float[_iWidth * _iHeight * 4];
    memset(zeros, 0, _iWidth * _iHeight * 4 * sizeof(float));

	glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iBCTexture);
    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, _iWidth, _iHeight, 
                    GL_RGBA, GL_FLOAT, zeros);
    glBindTexture(GL_TEXTURE_2D, _iBCDisplayTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _iWidth, _iWidth, 
                    GL_RGBA, GL_FLOAT, zeros);

	_pVectorBuffer->Activate();
	{
		_pFluidImpl->UpdateBoundaryOffsetTables(_iTextures[TEXTURE_VELOCITY_OFFSETS],
												_iTextures[TEXTURE_PRESSURE_OFFSETS],
												_iBCTexture);
	}
	_pScalarBuffer->Activate(_pVectorBuffer);
	{
		_pFluidImpl->SetupObstacleZCull(_iBCTexture);
	}  
	_pScalarBuffer->Deactivate();

	delete [] zeros;
}

void Flo::SetDensityMap(float* density_map) {
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[TEXTURE_DENSITY]);
	glGetTexImage(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, GL_FLOAT, density_map);
}

void Flo::ReplaceDensityMap(float* density_map) {
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[TEXTURE_DENSITY]);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, _iWidth, _iHeight, GL_RGBA, GL_FLOAT, density_map);
}

void Flo::SetVelocityMap(float* velocity_map) {
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[TEXTURE_VELOCITY]);
	glGetTexImage(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, GL_FLOAT, velocity_map);
}

void Flo::ReplaceVelocityMap(float* velocity_map) {
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iTextures[TEXTURE_VELOCITY]);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, _iWidth, _iHeight, GL_RGBA, GL_FLOAT, velocity_map);
}


//----------------------------------------------------------------------------
// Function     	: Flo::Display
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::Display()
* @brief Displays the simulation on a screen quad.
*/ 
void Flo::Display(DisplayMode mode, 
                  bool bilerp /* = false */, 
                  bool makeTex /*= false*/,
                  bool displayBoundary /* = false */,
                  bool whiteBackground /* = false */)
{
    FloDisplayOp *display;

    // If makeTex is true, then we render to a viewport the size of the 
    // simulation, rather than the whole window.  Then (see below), we copy the 
    // viewport to a texture. This is a regular (non-float) 2D texture, and we 
    // can enable bilinear filtering when rendering it.  This is faster than 
    // performing the bilinear filtering directly on the float texture in a 
    // fragment program, but the quality of the filtering is lower.

    // Also note that makeTex causes the scaled and biased fields, such as the 
    // velocity, pressure, and vorticity, to lose their scaling and biasing on 
    // display...
    if (makeTex)
    {
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, _iWidth, _iHeight);
    }

    static DisplayMode previous = DISPLAY_COUNT;

    if (DISPLAY_MILK_AND_METAL == mode)
    {
        if (_bNormalMapDirty)
            _GenerateNormalMap();
        _displayMilkAndMetal.SetTextureParameter("texture", _iTextures[TEXTURE_DENSITY]);
        _displayMilkAndMetal.SetTextureParameter("normalMap", _iNormalMap);
        _displayMilkAndMetal.SetTextureParameter("envMap", _iCubeMap);
        _displayMilkAndMetal.SetFragmentParameter4f("bias", 0, 0, 0, 0);
        _displayMilkAndMetal.SetFragmentParameter4f("scale", 1, 1, 1, 1);
        _displayMilkAndMetal.Compute();
    }
    else
    {
        if (mode != previous)
        {
            switch(mode) 
            {
            default:
            case DISPLAY_INK:
                if (whiteBackground)					
                    display = (bilerp) ? &_displayWhiteVectorBilerp : &_displayWhiteVector;
                else
					display = (bilerp) ? &_displayColorBilerp : &_displayColor;
                display->SetFragmentParameter4f("scale", COLOR_SCALE, COLOR_SCALE, COLOR_SCALE, COLOR_SCALE);
                display->SetTextureParameter("texture", _iTextures[TEXTURE_DENSITY]);
                break;
            case DISPLAY_VELOCITY:
                display = (bilerp) ? &_displayVectorBilerp : &_displayVector;
                display->SetFragmentParameter4f("bias", 0.5f, 0.5f, 0.5f, 0.5f);
                display->SetFragmentParameter4f("scale", 0.5f, 0.5f, 0.5f, 0.5f);
                display->SetTextureParameter("texture", _iTextures[TEXTURE_VELOCITY]);
                break;
            case DISPLAY_PRESSURE:
                display = (bilerp) ? &_displayScalarBilerp : &_displayScalar;
                display->SetFragmentParameter4f("bias", 0, 0, 0, 0);
                display->SetFragmentParameter4f("scale", 2, -1, -2, 1);
                display->SetTextureParameter("texture", _iTextures[TEXTURE_PRESSURE]);
                break;
            case DISPLAY_VORTICITY:
                display = (bilerp) ? &_displayScalarBilerp : &_displayScalar;
                display->SetFragmentParameter4f("bias", 0, 0, 0, 0);
                display->SetFragmentParameter4f("scale", 1, 1, -1, 1);
                display->SetTextureParameter("texture", _iTextures[TEXTURE_VORTICITY]);
                break;
            case DISPLAY_MILK_AND_METAL:
                break;
            }
        }
        display->Compute();
    }

    // See the comments about makeTex at the top of this method.
    if (makeTex && mode != DISPLAY_MILK_AND_METAL)
    {
        glBindTexture(GL_TEXTURE_2D, _iDisplayTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);

        glPopAttrib(); // GL_VIEWPORT_BIT

        glEnable(GL_TEXTURE_2D);

        // now display
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0, 0); glVertex2f(0, 0);
            glTexCoord2f(1, 0); glVertex2f(1, 0);
            glTexCoord2f(1, 1); glVertex2f(1, 1);
            glTexCoord2f(0, 1); glVertex2f(0, 1);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    if (displayBoundary && (mode == DISPLAY_INK || mode == DISPLAY_MILK_AND_METAL))
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, _iBCDisplayTexture);
		glActiveTextureARB(GL_TEXTURE1_ARB);
        glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D, _iBCDetailTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBegin(GL_QUADS);
        {
            glColor4f(.15, .15, .15, 1);
            glTexCoord2f(0, 0); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0); glVertex2f(0, 0);
            glTexCoord2f(1, 0); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 0); glVertex2f(_iWidth, 0);
            glTexCoord2f(1, 1); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1, 1); glVertex2f(_iWidth, _iHeight);
            glTexCoord2f(0, 1); glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 1); glVertex2f(0, _iHeight);
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glActiveTextureARB(GL_TEXTURE0_ARB);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
    }
}

//----------------------------------------------------------------------------
// Function     	: Flo::AddImpulse
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::AddImpulse(const float strength[3], const float position[3], float radius)
* @brief Sets the current interaction impulse.
*/ 
void Flo::AddImpulse(const float strength[3], 
                      const float position[2], 
                      float radius,
                      bool bAddInk /* = true */)
{
    // don't add impulses if outside the window!
    if (position[0] < 0 || position[0] > 1 || 
        position[1] < 0 || position[1] > 1)
        return;

    Impulse imp(strength, position, _rInkColor, radius, bAddInk);

    _impulses.push_back(imp);
}

void Flo::AddDensityImpulse(float color[3], const float position[2], float radius) {
	float oldColor[3];
	GetInkColor(oldColor);
	SetInkColor(color);
	float str[3] = {0,0,0};
	AddImpulse(str, position, radius, true);
	SetInkColor(oldColor);
}


//----------------------------------------------------------------------------
// Function     	: Flo::DrawBoundary
// Description	    : 
//----------------------------------------------------------------------------
/**
* @fn Flo::SetImpulse(const float strength[3], const float position[3], float radius)
* @brief Called when the user is drawing in arbitrary boundaries.
*/ 
void Flo::DrawBoundary(const float position[2], float radius, bool bErase /*= false*/, ::Texture* tex /*= NULL*/)
{
    // don't add boundaries if the mouse is outside the window!
    if (position[0] < 0 || position[0] > 1 || 
        position[1] < 0 || position[1] > 1)
        return;

    float xMin = position[0] - radius;
    float xMax = xMin + 2 * radius;
    float yMin = position[1] - radius;
    float yMax = yMin + 2 * radius;

    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(0, 0, _iWidth, _iHeight);

	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);

	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

	//draw the old boundary texture to the frame buffer
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iBCTexture);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
    glBegin(GL_QUADS);
    {
        glColor4f(1, 1, 1, 1);
        glTexCoord2f(      0,        0); glVertex2f(0, 0);
        glTexCoord2f(_iWidth,        0); glVertex2f(1, 0);
        glTexCoord2f(_iWidth, _iHeight); glVertex2f(1, 1);
        glTexCoord2f(      0, _iHeight); glVertex2f(0, 1);
    }
    glEnd();
    glDisable(GL_TEXTURE_RECTANGLE_NV);

	//add the new boundary to the frame buffer
	if (tex == NULL) {
		glBegin(GL_QUADS);
		{
			if (bErase)
				glColor4f(0, 0, 0, 0);
			else
				glColor4f(1, 1, 1, 1);
			glVertex3f(xMin, yMin, 0);
			glVertex3f(xMax, yMin, 0);
			glVertex3f(xMax, yMax, 0);
			glVertex3f(xMin, yMax, 0);
		}
		glEnd();
	}
	else {
		TextureBinding texb = tex->bind();
		glBegin(GL_QUADS);
		{
			if (bErase)
				glColor4f(0, 0, 0, 0);
			else
				glColor4f(1, 1, 1, 1);
			glTexCoord2f(0, tex->get_height());					glVertex3f(.1, .1, 0);
			glTexCoord2f(tex->get_width(), tex->get_height());	glVertex3f(.9, .1, 0);
			glTexCoord2f(tex->get_width(), 0);					glVertex3f(.9, .9, 0);
			glTexCoord2f(0, 0);									glVertex3f(.1, .9, 0);
		}
		glEnd();
	}

	//copy the frame buffer back into the boundary texture
    glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, _iWidth, _iHeight);

    glViewport(0, 0, _iWidth, _iHeight);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);

	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

	//draw the boundary texture back to the frame buffer (is this step necessary *?*)
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iBCTexture);
    glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBegin(GL_QUADS);
    {
        glColor4f(1, 1, 1, 1);
        glTexCoord2f(      0,        0); glVertex2f(0, 0);
        glTexCoord2f(_iWidth,        0); glVertex2f(1, 0);
        glTexCoord2f(_iWidth, _iHeight); glVertex2f(1, 1);
        glTexCoord2f(      0, _iHeight); glVertex2f(0, 1);
    }
    glEnd();
    glDisable(GL_TEXTURE_RECTANGLE_NV);

	//copy the boundary texture into the boundary display texture
    glBindTexture(GL_TEXTURE_2D, _iBCDisplayTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);

    glViewport(vp[0], vp[1], vp[2], vp[3]);

	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);

    _pVectorBuffer->Activate();
    {
        _pFluidImpl->UpdateBoundaryOffsetTables(_iTextures[TEXTURE_VELOCITY_OFFSETS],
                                                _iTextures[TEXTURE_PRESSURE_OFFSETS],
                                                _iBCTexture);
    }
    _pScalarBuffer->Activate(_pVectorBuffer);
    {
        _pFluidImpl->SetupObstacleZCull(_iBCTexture);
    }
    _pScalarBuffer->Deactivate();

	glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}


void Flo::_GenerateNormalMap()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, _iWidth, _iHeight);
    
    _genNormalMap.Compute();

    glBindTexture(GL_TEXTURE_2D, _iNormalMap);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _iWidth, _iHeight);

    glPopAttrib(); // GL_DEPTH_BUFFER_BIT
    glPopAttrib(); // GL_VIEWPORT_BIT
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    _bNormalMapDirty = false;
}

void Flo::InitializeVertexBuffer(unsigned int iColumns, unsigned int iRows)
{
    _iVBWidth = iColumns;
    _iVBHeight = iRows;

    glDeleteBuffersARB(1, &_iIndexBuffer);
    glDeleteBuffersARB(1, &_iVertexBuffer);
    glDeleteBuffersARB(1, &_iHeadTailBuffer);

    // create the static index buffer
    int numVertices = 2 * iRows * iColumns;
    GLuint *indices = new GLuint[numVertices];
    for (int i = 0; i < numVertices; i++)
    {
        indices[i] = i;
    }

    glGenBuffersARB(1, &_iIndexBuffer);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _iIndexBuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, numVertices * sizeof(GLuint), 
                    indices, GL_STATIC_DRAW_ARB);

    delete [] indices;

    // create the static vertex buffer
    GLfloat *vertices = new GLfloat[numVertices * 2];
    GLubyte *headTail = new GLubyte[numVertices];
    int index = 0;
    float xstep = 1.0f / iColumns;
    float ystep = 1.0f / iRows;
    float y = 0.5f / iRows;
    int   i_m = 0;
    for (unsigned int j = 0; j < iRows; ++j) 
    {
        float x = 0.5f / iColumns;
        for (unsigned int i = 0; i < iColumns; ++i) 
        {
            headTail[i_m++] = i_m % 2;
            headTail[i_m++] = i_m % 2;
            vertices[index++] = x;
            vertices[index++] = y;
            vertices[index++] = x;
            vertices[index++] = y;
            x += xstep;
        }
        y += ystep;
    }

    glGenBuffersARB(1, &_iVertexBuffer);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _iVertexBuffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVertices * 2 * sizeof(GLfloat), 
                    vertices, GL_STATIC_DRAW_ARB);

    glGenBuffersARB(1, &_iHeadTailBuffer);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _iHeadTailBuffer);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, numVertices * sizeof(GLubyte), 
                    headTail, GL_STATIC_DRAW_ARB);

    delete [] vertices;
    delete [] headTail;
}

void Flo::DisplayVectorField(VectorDisplayMode mode, 
                             int lineWidth, int pointSize, float vectorScale)
{
    unsigned int vertexStride = 0;
    unsigned int headTailStride = 0;
    unsigned int vertexOffset = 0;
    unsigned int headTailOffset = 0;
    unsigned int vertexMultiplier = 2;
    GLenum drawMode = GL_LINES;
    GenericCgGLVertexProgram *vp = &_vectorFieldVP;

    switch(mode) 
    {
    case VECTOR_DISPLAY_NONE:
    default:
        return;
    	break;
    case VECTOR_DISPLAY_QUIVER:
        break;
    case VECTOR_DISPLAY_STATIC_DOTS_SIZED:
        vp = (_eVectorField == DISPLAY_VELOCITY || _eVectorField == DISPLAY_INK)
            ? &_vectorFieldPointSizeVP : &_scalarFieldPointSizeVP;
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
        vp->SetVertexParameter1f("pointScale", pointSize);
        vertexStride = 4 * sizeof(GLfloat);
        headTailStride = 2 * sizeof(GLubyte);
        vertexMultiplier = 1;
        drawMode = GL_POINTS;
        break;
    case VECTOR_DISPLAY_STATIC_DOTS:
        vp = (_eVectorField == DISPLAY_VELOCITY || _eVectorField == DISPLAY_INK)
            ? &_vectorFieldVP : &_scalarFieldVP;
        vertexStride = 4 * sizeof(GLfloat);
        headTailStride = 2 * sizeof(GLubyte);
        vertexMultiplier = 1;
        drawMode = GL_POINTS;
        break;
    case VECTOR_DISPLAY_SPRING_DOTS_SIZED:
        vp = (_eVectorField == DISPLAY_VELOCITY || _eVectorField == DISPLAY_INK)
            ? &_vectorFieldPointSizeVP : &_scalarFieldPointSizeVP;
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
        vp->SetVertexParameter1f("pointScale", pointSize);
        vertexStride = 4 * sizeof(GLfloat);
        headTailStride = 2 * sizeof(GLubyte);
        vertexOffset = 2 * sizeof(GLfloat);
        headTailOffset = sizeof(GLubyte);
        vertexMultiplier = 1;
        drawMode = GL_POINTS;
        break;
    case VECTOR_DISPLAY_SPRING_DOTS:
        vp = (_eVectorField == DISPLAY_VELOCITY || _eVectorField == DISPLAY_INK)
            ? &_vectorFieldVP : &_scalarFieldVP;
        vertexStride = 4 * sizeof(GLfloat);
        headTailStride = 2 * sizeof(GLubyte);
        vertexOffset = 2 * sizeof(GLfloat);
        headTailOffset = sizeof(GLubyte);
        vertexMultiplier = 1;
        drawMode = GL_POINTS;
        break;
    }

    switch(_eVectorField) 
    {
    case DISPLAY_INK:
        vp->SetVertexParameter4f("colorBias", 0, 0, 0, 0);
        vp->SetVertexParameter4f("colorScale", 1, 1, 1, 1);
        break;
    case DISPLAY_VELOCITY:
        vp->SetVertexParameter4f("colorBias", 0.5f, 0.5f, 0.5f, 0.5f);
        vp->SetVertexParameter4f("colorScale", 0.5f, 0.5f, 0.5f, 0.5f);
        break;
    case DISPLAY_PRESSURE:
        vp->SetVertexParameter4f("colorBias", 0, 0, 0, 0);
        vp->SetVertexParameter4f("colorScale", 2, -1, -2, -1);
        break;
    case DISPLAY_VORTICITY:
        vp->SetVertexParameter4f("colorBias", 0, 0, 0, 0);
        vp->SetVertexParameter4f("colorScale", 1, 1, -1, 1);
        break;
    }

    glLineWidth(lineWidth);
    glPointSize(pointSize);
    vp->SetVertexParameter1f("vectorScale", vectorScale);
    vp->SetTextureParameter("texture", _iVectorFieldTexture);
    
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, _iIndexBuffer);
    
    vp->EnableClientState("pos");
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _iVertexBuffer);
    vp->SetParameterPointer("pos", 2, GL_FLOAT, vertexStride, 
                                       (GLvoid*)vertexOffset);

    vp->EnableClientState("headTail");
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _iHeadTailBuffer);
    vp->SetParameterPointer("headTail", 1, GL_UNSIGNED_BYTE, headTailStride, 
                                       (GLvoid*)headTailOffset);

    vp->SetState();
    glDrawElements(drawMode, vertexMultiplier * _iVBWidth * _iVBHeight, GL_UNSIGNED_INT, 0);
    vp->ResetState();

    glDisableClientState(GL_VERTEX_ARRAY);
    glLineWidth(1);
    glPointSize(1);
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
}