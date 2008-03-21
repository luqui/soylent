//----------------------------------------------------------------------------
// FILE    : fluidbase.cpp
// CREATED : 19 September 2004
// AUTHOR  : Mark J. Harris
// 
// COMMENTS: 
// 
// REVISION: 
// 
//----------------------------------------------------------------------------

#include "fluidbase.h"
#include "shared/data_path.h"
#include <assert.h>

FluidBase::FluidBase(int width, int height, float gridScale)
:   _iWidth(width),
    _iHeight(height),
    _dx(gridScale),
    _iVelocityOffsetTexture(0),
    _iPressureOffsetTexture(0)
{
}


FluidBase::~FluidBase()
{
}


void FluidBase::Initialize(CGcontext context)
{
    _CreateOffsetTextures();

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

    // Compute the minimum and maximum vertex and texture coordinates for 
    // rendering the Stream interiors (sans boundaries).
    float xMin  = 1.0f / (float)_iWidth;         // one pixel from left
    float xMax  = 1.0f - 1.0f / (float)_iWidth;  // one pixel from right
    float yMin  = 1.0f / (float)_iHeight;        // on pixel from bottom
    float yMax  = 1.0f - 1.0f / (float)_iHeight; // one pixel from top

    float sMin  = 1.0f;                          // the same, this time in 
    float sMax  = (float)_iWidth - 1.0f;         // texture coordinates
    float tMin  = 1.0f;
    float tMax  = (float)_iHeight - 1.0f;
//*?* I think this is where at least one change goes.
	/*xMin = sMin;
	xMax = sMax;
	yMin = tMin;
	yMax = tMax;*/

    // Add Impulse.  This renders a Gaussian "splat" into the specified texture,
    // with the specified position, radius, and color.
    _addImpulse.InitializeFP(context, programsPath + "splat.cg", "splat");
    _addImpulse.SetFragmentParameter2f("windowDims", 
                                       (float)_iWidth, (float)_iHeight);
    _addImpulse.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _addImpulse.SetStreamRect(xMin, yMin, xMax, yMax);

    // Advection:  This advects a field by the moving velocity field.  This is 
    // used to advect both velocity and scalar values, such as mass.  The result
    // of applying this to the velocity field is a moving but divergent velocity
    // field.  The next few steps correct that divergence to give a divergence-
    // free velocity field.
    _advect.SetFPArgs(fpargs);
    _advect.InitializeFP(context, programsPath + "flo.cg", "advect");
    _advect.SetFragmentParameter1f("rdx", 1.0f / _dx);
    _advect.SetFragmentParameter1f("dissipation", 1);
    _advect.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _advect.SetStreamRect(xMin, yMin, xMax, yMax);

    // Divergence of velocity: This computes how divergent the velocity field is
    // (how much in/out flow there is at every point).  Used as input to the 
    // Poisson solver, below.
    _divergence.SetFPArgs(fpargs);
    _divergence.InitializeFP(context, programsPath + "flo.cg", "divergence");
    _divergence.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _divergence.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _divergence.SetStreamRect(xMin, yMin, xMax, yMax);

    // Poisson-pressure solver: By running this Jacobi Relaxation solver for 
    // multiple iterations, this solves for the pressure disturbance in the 
    // fluid given the divergence of the velocity.
    _poissonSolver.SetFPArgs(fpargs);
    _poissonSolver.InitializeFP(context, programsPath + "flo.cg", "jacobi");
    _poissonSolver.SetFragmentParameter1f("alpha", -_dx * _dx);
    _poissonSolver.SetFragmentParameter1f("rBeta", 0.25f);
    _poissonSolver.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _poissonSolver.SetStreamRect(xMin, yMin, xMax, yMax);

    // Poisson solver for viscous diffusion: By running this Jacobi Relaxation 
    // solver for multiple iterations, this solves for the velocity diffusion in 
    // a viscous fluid.
    _poissonSolverVec2.SetFPArgs(fpargs);
    _poissonSolverVec2.InitializeFP(context, programsPath + "flo.cg", "jacobiVec2");
    _poissonSolverVec2.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _poissonSolverVec2.SetStreamRect(xMin, yMin, xMax, yMax);

    // Subtract Gradient.  After solving for the pressure disturbance, this 
    // subtracts the pressure gradient from the divergent velocity field to 
    // give a divergence-free field.
    _subtractGradient.SetFPArgs(fpargs);
    _subtractGradient.InitializeFP(context, programsPath + "flo.cg", "gradient");
    _subtractGradient.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _subtractGradient.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _subtractGradient.SetStreamRect(xMin, yMin, xMax, yMax);

    // vorticity computation.
    _vorticity.SetFPArgs(fpargs);
    _vorticity.InitializeFP(context, programsPath + "flo.cg", "vorticity");
    _vorticity.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _vorticity.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _vorticity.SetStreamRect(xMin, yMin, xMax, yMax);

    // vorticity confinement force computation.
    _vorticityForce.SetFPArgs(fpargs);
    _vorticityForce.InitializeFP(context, programsPath + "flo.cg", "vortForce");
    _vorticityForce.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _vorticityForce.SetTexCoordRect(0, sMin, tMin, sMax, tMax);
    _vorticityForce.SetStreamRect(xMin, yMin, xMax, yMax);

    // This applies pure neumann boundary conditions (see floPoissonBC.cg) to 
    // the pressure field once per iteration of the poisson-pressure jacobi 
    // solver.  Also no-slip BCs to velocity once per time step.
    _boundaries.SetFPArgs(fpargs);
    _boundaries.InitializeFP(context, programsPath + "flo.cg", "boundary");
    _boundaries.SetTexCoordRect(0, 0, (float)_iWidth, (float)_iHeight);
    _boundaries.SetTexResolution(_iWidth, _iHeight);
    _boundaries.SetFragmentParameter1f("scale", 0);

    _updateOffsets.SetFPArgs(fpargs);
    _updateOffsets.InitializeFP(context, programsPath + "flo.cg", "updateOffsets");
    _updateOffsets.SetTexCoordRect(0, 0, 0, (float)_iWidth, (float)_iHeight);
    _updateOffsets.SetStreamRect(xMin, yMin, xMax, yMax);

    // setup the arbitrary boundary operations.
    _arbitraryVelocityBC.SetFPArgs(fpargs);
    _arbitraryVelocityBC.InitializeFP(context, programsPath + "flo.cg", 
                                      "arbitraryVelocityBoundary");
    _arbitraryVelocityBC.SetTexCoordRect(0, 0, 0, (float)_iWidth, (float)_iHeight);
    _arbitraryVelocityBC.SetStreamRect(xMin, yMin, xMax, yMax);

    _arbitraryPressureBC.SetFPArgs(fpargs);
    _arbitraryPressureBC.InitializeFP(context, programsPath + "flo.cg", 
                                      "arbitraryPressureBoundary");
    _arbitraryPressureBC.SetTexCoordRect(0, 0, 0, (float)_iWidth, (float)_iHeight);
    _arbitraryPressureBC.SetStreamRect(xMin, yMin, xMax, yMax);

    _zcullSetupBoundaries.SetFPArgs(fpargs);
    _zcullSetupBoundaries.InitializeFP(context, programsPath + "flo.cg", "zCullSetupBoundaries");
    _zcullSetupBoundaries.SetTexCoordRect(0, 0, (float)_iWidth, (float)_iHeight);
    _zcullSetupBoundaries.SetStreamRect(xMin, yMin, xMax, yMax, 1);

    _zcullSetupPressure.SetFPArgs(fpargs);
    _zcullSetupPressure.InitializeFP(context, programsPath + "flo.cg", "zCullSetupPressure");
    _zcullSetupPressure.SetTexCoordRect(0, 0, (float)_iWidth, (float)_iHeight);
    _zcullSetupPressure.SetStreamRect(xMin, yMin, xMax, yMax, 1);
}

void FluidBase::Finalize()
{
    _advect.ShutdownFP();
    _divergence.ShutdownFP();
    _poissonSolver.ShutdownFP();
    _poissonSolverVec2.ShutdownFP();
    _subtractGradient.ShutdownFP();
    _vorticity.ShutdownFP();
    _vorticityForce.ShutdownFP();
    _boundaries.ShutdownFP();
    _arbitraryPressureBC.ShutdownFP();
    _arbitraryVelocityBC.ShutdownFP();
    _zcullSetupBoundaries.ShutdownFP();
    _zcullSetupPressure.ShutdownFP();

    glDeleteTextures(1, &_iPressureOffsetTexture);
    glDeleteTextures(1, &_iVelocityOffsetTexture);
}

void FluidBase::SetGridScale(float dx)
{
    _dx = dx;
    _advect.SetFragmentParameter1f("rdx", 1.0f / _dx);
    _divergence.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _subtractGradient.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _vorticity.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _vorticityForce.SetFragmentParameter1f("halfrdx", 0.5f / _dx);
    _poissonSolver.SetFragmentParameter1f("alpha", -_dx * _dx);
}

void FluidBase::EnableTextureUpdates(bool bEnable)
{
    _addImpulse.SetEnableCopy(bEnable);
    _advect.SetEnableCopy(bEnable);
    _divergence.SetEnableCopy(bEnable);
    _poissonSolver.SetEnableCopy(bEnable);
    _poissonSolverVec2.SetEnableCopy(bEnable);
    _subtractGradient.SetEnableCopy(bEnable);
    _vorticity.SetEnableCopy(bEnable);
    _vorticityForce.SetEnableCopy(bEnable);
    _updateOffsets.SetEnableCopy(bEnable);
    _arbitraryVelocityBC.SetEnableCopy(bEnable);
    _arbitraryPressureBC.SetEnableCopy(bEnable);
}


void FluidBase::ApplyBoundaryConditions(unsigned int bufferID, float neighborScale)
{
    static unsigned int lastID    = 0;
    static float        lastScale = 0;

    if(lastID != bufferID)
    {
        _boundaries.SetTextureParameter("x", bufferID);
        lastID = bufferID;
    }

    if (lastScale != neighborScale)
    {
        _boundaries.SetFragmentParameter1f("scale", neighborScale); 
        lastScale = neighborScale;
    }

    _boundaries.Compute();    
}


void FluidBase::Advect(unsigned int velocityID, unsigned int advectedID, 
                       float timestep, float dissipation)
{
    static float        lastTimestep    = 0;
    static float        lastDissipation = 1;
    static unsigned int lastVelocity    = 0;
    static unsigned int lastAdvected    = 0;

    if (lastTimestep != timestep)
    {
        _advect.SetFragmentParameter1f("timestep", timestep);
        lastTimestep = timestep;
    }

    if (lastDissipation != dissipation)
    {   
        _advect.SetFragmentParameter1f("dissipation", dissipation);
        lastDissipation = dissipation;
    }

    if (lastAdvected != advectedID)
    {
        _advect.SetOutputTexture(advectedID, _iWidth, _iHeight);
        _advect.SetTextureParameter("x", advectedID);
        lastAdvected = advectedID;
    }

    if (lastVelocity != velocityID)
    {
        _advect.SetTextureParameter("u", velocityID);
        lastVelocity = velocityID;
    }

    _advect.Compute();
}


void FluidBase::ApplyImpulse(unsigned int bufferID, 
                             float color[3],
                             float position[2],
                             float radius)
{
    _addImpulse.SetFragmentParameter3fv("color", color);
    _addImpulse.SetFragmentParameter2fv("position", position);
    _addImpulse.SetFragmentParameter1f("radius", radius);
    _addImpulse.SetOutputTexture(bufferID, _iWidth, _iHeight);
    _addImpulse.SetTextureParameter("base", bufferID);   
    _addImpulse.Compute();
}

void FluidBase::ComputeVorticity(unsigned int velocityID, 
                                 unsigned int vorticityID)
{
    static unsigned int lastVelocity  = 0;
    static unsigned int lastVorticity = 0;

    if (lastVelocity != velocityID)
    {
        _vorticity.SetTextureParameter("u", velocityID);
        lastVelocity = velocityID;
    }

    if (lastVorticity != vorticityID)
    {
        _vorticity.SetOutputTexture(vorticityID, _iWidth, _iHeight);
        lastVorticity = vorticityID;
    }

    _vorticity.Compute();
}

void FluidBase::ComputeVorticityConfinement(unsigned int velocityID, 
                                            unsigned int vorticityID, 
                                            float timestep,
                                            float scale)
{
    static unsigned int lastVelocity  = 0;
    static unsigned int lastVorticity = 0;
    static float        lastScale     = 0;
    static float        lastTimestep  = 0;

    if (lastTimestep != timestep)
    {
        _vorticityForce.SetFragmentParameter1f("timestep", timestep);
        lastTimestep = timestep;
    }

    if (lastVelocity != velocityID)
    {
        _vorticityForce.SetTextureParameter("u", velocityID);
        _vorticityForce.SetOutputTexture(velocityID, _iWidth, _iHeight);
        lastVelocity = velocityID;
    }

    if (lastVorticity != vorticityID)
    {
        _vorticityForce.SetTextureParameter("vort", vorticityID);
        lastVorticity = vorticityID;
    }

    if (lastScale != scale)
    {
        _vorticityForce.SetFragmentParameter2f("dxscale", 
                                               scale * _dx, scale * _dx);
        lastScale = scale;
    }

    ApplyBoundaryConditions(velocityID, -1);
    _vorticityForce.Compute();
}


void FluidBase::ComputeDivergence(unsigned int divergenceID, unsigned int velocityID)
{
    static unsigned int lastDivergence = 0;
    static unsigned int lastVelocity   = 0;

    if (lastDivergence != divergenceID)
    {
        _divergence.SetOutputTexture(divergenceID, _iWidth, _iHeight);
        lastDivergence = divergenceID;
    }

    if (lastVelocity != velocityID)
    {
        _divergence.SetTextureParameter("w", velocityID);
        lastVelocity = velocityID;
    }
    _divergence.Compute();
}


void FluidBase::ComputeViscousDiffusion(unsigned int velocityID, float timestep,
                                        float viscosity, int numIterations)
{
    static unsigned int lastVelocity = 0;
    static float        lastTimestep = 0;
    static float        lastViscosity = 0;
    static float        lastDX = 0;
    
    if (lastVelocity != velocityID)
    {
        _poissonSolverVec2.SetOutputTexture(velocityID, _iWidth, _iHeight);
        _poissonSolverVec2.SetTextureParameter("x", velocityID);
        lastVelocity = velocityID;
    }

    if (lastViscosity != viscosity || 
        lastTimestep != timestep ||
        lastDX != _dx)
    {
        float alpha = _dx * _dx / (viscosity * timestep);
        _poissonSolverVec2.SetFragmentParameter1f("alpha", alpha);
        _poissonSolverVec2.SetFragmentParameter1f("rBeta", 1.0f / (4.0f + alpha));

        lastViscosity = viscosity;
        lastTimestep = timestep;
        lastDX = _dx;
    }

    for (int i = 0; i < numIterations; ++i)
        _poissonSolverVec2.Compute();
}


void FluidBase::SolvePoissonPressureEquation(unsigned int pressureID, 
                                             unsigned int divergenceID,
                                             int numIterations, 
                                             bool obstacles,
                                             unsigned int pressureOffsetID /* = 0 */)
{
    static unsigned int lastPressure = 0;
    static unsigned int lastDivergence = 0;
    static unsigned int lastOffset = 0;
    
    if (lastPressure != pressureID)
    {
        _poissonSolver.SetOutputTexture(pressureID, _iWidth, _iHeight);
        _poissonSolver.SetTextureParameter("x", pressureID);
        _arbitraryPressureBC.SetTextureParameter("p", pressureID);        
        _arbitraryPressureBC.SetOutputTexture(pressureID, _iWidth, _iHeight);
        lastPressure = pressureID;
    }

    if (lastDivergence != divergenceID)
    {
        _poissonSolver.SetTextureParameter("b", divergenceID);
        lastDivergence = divergenceID;
    }

    if (obstacles && lastOffset != pressureOffsetID)
    {
        _arbitraryPressureBC.SetTextureParameter("offsets", pressureOffsetID);
        lastOffset = pressureOffsetID;
    }

    float step = 1.0f / (float)numIterations;
    float z = 1 - step;

    for (int i = 0; i < numIterations; ++i)
    {
        // Apply pure neumann boundary conditions
        ApplyBoundaryConditions(pressureID, 1);
        
        // Apply pure neumann boundary conditions to arbitrary 
        // interior boundaries if they are enabled.
        if (obstacles)
        {
            _arbitraryPressureBC.Compute();
        }

        // for the zcull optimization
        _poissonSolver.SetStreamRect(1.0f / (float)_iWidth, 
                                      1.0f / (float)_iHeight, 
                                      1 - 1.0f / (float)_iWidth, 
                                      1 - 1.0f / (float)_iHeight,
                                      z);
		//*?* this might be something we want to change
		//_poissonSolver.SetStreamRect(1, 1, _iWidth - 1, _iHeight - 1, z);
        z -= step;
        
        _poissonSolver.Compute(); // perform one Jacobi iteration      
    }
}


void FluidBase::SubtractPressureGradient(unsigned int pressureID, 
                                         unsigned int velocityID)
{
    static unsigned int lastPressure = 0;
    static unsigned int lastVelocity = 0;

    if (lastPressure != pressureID)
    {
        _subtractGradient.SetTextureParameter("p", pressureID);
        lastPressure = pressureID;
    }

    if (lastVelocity != velocityID)
    {
        _subtractGradient.SetOutputTexture(velocityID, _iWidth, _iHeight);
        _subtractGradient.SetTextureParameter("w", velocityID);
        lastVelocity = velocityID;
    }

    _subtractGradient.Compute();
}


void FluidBase::UpdateBoundaryOffsetTables(unsigned int velocityOffsetID, 
                                           unsigned int pressureOffsetID,
                                           unsigned int boundariesID)
{
    _updateOffsets.SetTextureParameter("b", boundariesID);
    _updateOffsets.SetTextureParameter("offsetTable", _iVelocityOffsetTexture);
    _updateOffsets.SetOutputTexture(velocityOffsetID, _iWidth, _iHeight);
    _updateOffsets.Compute();

    _updateOffsets.SetTextureParameter("offsetTable", _iPressureOffsetTexture);
    _updateOffsets.SetOutputTexture(pressureOffsetID, _iWidth, _iHeight);
    _updateOffsets.Compute();
}


void FluidBase::ComputeArbitraryVelocityBCs(unsigned int velocityID,
                                            unsigned int offsetsID)
{
    static unsigned int lastVelocity = 0;
    static unsigned int lastOffsets = 0;

    if (lastVelocity != velocityID)
    {
        _arbitraryVelocityBC.SetTextureParameter("u", velocityID);
        _arbitraryVelocityBC.SetOutputTexture(velocityID, _iWidth, _iHeight);
        lastVelocity = velocityID;
    }

    if (lastOffsets != offsetsID)
    {
        _arbitraryVelocityBC.SetTextureParameter("offsets", offsetsID);
        lastOffsets = offsetsID;
    }

    _arbitraryVelocityBC.Compute();
}


void FluidBase::SetupPoissonPressureZCull(unsigned int pressureID)
{
    static unsigned int lastPressure = 0;

    if (lastPressure != pressureID)
    {
        _zcullSetupPressure.SetTextureParameter("p", pressureID);
        lastPressure = pressureID;
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearColor(0, 1, 0, 0);
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    _zcullSetupPressure.Compute();

    glDepthMask(GL_FALSE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}


void FluidBase::SetupObstacleZCull(unsigned int boundariesID)
{
    static unsigned int lastBoundaries = 0;

    if (lastBoundaries != boundariesID)
    {
        _zcullSetupBoundaries.SetTextureParameter("b", boundariesID);
        lastBoundaries = boundariesID;
    }

    // This code runs a pass over the buffer, writing only depth.  
    // The shader checks all neighboring cells to see if they are boundary
    // cells.  If they are not, it discards the pixel -- thus the depth
    // buffer is unmodified except in "landlocked" cells.  In landlocked
    // cells, the depth buffer is written to a value equal to the near
    // plane depth. This means that if we enable depth test later when
    // we are rendering to this buffer, early z-cull will cause landlocked
    // cells to not be processed.  This gives a nice performance boost
    // when there are large obstacles in the flow.
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glClearColor(0, 0, 0, 0);
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    _zcullSetupBoundaries.Compute();

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}


void FluidBase::_CreateOffsetTextures()
{
    float velocityData[17*4] = 
    {      
        0,  0,  1,  1,   // Free or no neighboring boundaries (Error)
        0,  0,  0,  0,   // Unused
        0,  0,  0,  0,   // Unused
        0,  0,  0,  0,   // Unused
        -1, -1, -1, -1,   // Southwest 
        0,  0,  0,  0,   // Unused
        -1,  1, -1, -1,   // Northwest
        0,  0,  0,  0,   // Unused
        0,  0,  0,  0,   // Unused
        -1,  0, -1, -1,   // West
        1, -1, -1, -1,   // Southeast
        0, -1, -1, -1,   // South
        1,  1, -1, -1,   // Northeast
        0,  1, -1, -1,   // North
        0,  0,  0,  0,   // Unused
        1,  0, -1, -1,   // East (a boundary to the east)
        0,  0,  0,  0    // Unused
    };

    glGenTextures(1, &_iVelocityOffsetTexture);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iVelocityOffsetTexture);

    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV, 17, 1,
        0, GL_RGBA, GL_FLOAT, velocityData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    float pressureData[17*4] = 
    {    
        // This cell is a boundary cell (the inverse of above!)
        0,  0,  0,  0,   // Free or no neighboring boundaries
        0,  0,  0,  0,   // unused
        0,  0,  0,  0,   // unused
        0,  0,  0,  0,   // unused
        -1,  0,  0, -1,   // Southwest 
        0,  0,  0,  0,   // unused
        -1,  0,  0,  1,   // Northwest
        0,  0,  0,  0,   // Unused
        0,  0,  0,  0,   // Unused
        -1,  0, -1,  0,   // West
        0, -1,  1,  0,   // Southeast
        0, -1,  0, -1,   // South
        0,  1,  1,  0,   // Northeast
        0,  1,  0,  1,   // North
        0,  0,  0,  0,   // Unused
        1,  0,  1,  0,   // East (a boundary to the east)
        0,  0,  0,  0   // Unused
    };

    glGenTextures(1, &_iPressureOffsetTexture);
    glBindTexture(GL_TEXTURE_RECTANGLE_NV, _iPressureOffsetTexture);

    glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_RGBA16_NV, 17, 1,
        0, GL_RGBA, GL_FLOAT, pressureData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}