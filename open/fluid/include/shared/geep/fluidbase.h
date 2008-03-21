//----------------------------------------------------------------------------
// FILE    : fluidbase.h
// CREATED : 19 September 2004
// AUTHOR  : Mark J. Harris
// 
// COMMENTS: This class contains the machinery of the fluid simulation.  Very
// little state: just the stream computations necessary to implement a GPU 
// fluid simulation.  The texture data are decoupled so these operations can
// be applied to simulations that have multiple independent fields, for example.
// 
// REVISION: 
// 
//----------------------------------------------------------------------------
#ifndef __FLUIDBASE_H__
#define __FLUIDBASE_H__

#include <shared/geep/streamop.h>
#include <shared/geep/streamopGL.h>
#include <shared/geep/streamopCgGL.h>
#include <vector>

using namespace geep;

class FluidBase
{
protected: // types

    // These "StreamOp" (Stream Operation) types are the workhorses of the 
    // simulation.  They abstract the computation through a generic interface.
    // This uses the "geep" library.

    // The standard StreamOp: just performs a computation by binding a fragment
    // program and rendering a viewport-filling quad.
    typedef StreamOp < NoopOutputPolicy, NoopStatePolicy,
        NoopVertexPipePolicy, GenericCgGLFragmentProgram, 
        MultiTextureGLComputePolicy, CopyTexGLUpdatePolicy >
    FluidStreamOp;  

    // The boundary condition slabop.  Performs a computation only at boundaries
    // (one pixel wide) by binding a fragment program and rendering 4 
    // 1-pixel-wide quads at the edges of the viewport.
    typedef StreamOp < NoopOutputPolicy, NoopStatePolicy, 
        NoopVertexPipePolicy, GenericCgGLFragmentProgram,
        BoundaryGLComputePolicy, NoopUpdatePolicy >
    FluidBCOp;

    // The display Slabop.  This simply displays a float texture to the screen,
    // using a specified fragment program.
    typedef StreamOp < NoopOutputPolicy, NoopStatePolicy, 
        NoopVertexPipePolicy, GenericCgGLFragmentProgram,
        SingleTextureGLComputePolicy, NoopUpdatePolicy >
    FluidDisplayOp; 

    typedef StreamOp < NoopOutputPolicy, NoopStatePolicy, 
        NoopVertexPipePolicy, GenericCgGLFragmentProgram,
        MultiTextureGLComputePolicy, NoopUpdatePolicy >
    FluidMultiTexDisplayOp; 

public:
    FluidBase(int width, int height, float gridScale);
    virtual ~FluidBase();

    void Initialize(CGcontext context);
    void Finalize();

    void  SetGridScale(float dx);
    float GetGridScale() const          { return _dx; }

    // temporary
    void  EnableTextureUpdates(bool bEnable);
    bool  IsTextureUpdateEnabled() const     { return _bEnableTextureUpdates; }

    // Simulation modules    
    void ApplyBoundaryConditions(unsigned int bufferID, float neighborScale);
    
    void Advect(unsigned int velocityID, 
                unsigned int advectedID, 
                float timestep,
                float dissipation);
    
    void ApplyImpulse(unsigned int bufferID, float color[3], 
                      float position[2], float radius);
    
    void ComputeVorticity(unsigned int velocityID, 
                          unsigned int vorticityID);

    void ComputeVorticityConfinement(unsigned int velocityID, 
                                     unsigned int vorticityID,
                                     float timestep,
                                     float scale);

    void ComputeDivergence(unsigned int divergenceID, unsigned int velocityID);
    
    void ComputeViscousDiffusion(unsigned int velocityID, float timestep, 
                                 float viscosity, int numIterations);

    void SolvePoissonPressureEquation(unsigned int pressureID,
                                      unsigned int divergenceID,
                                      int numIterations,
                                      bool obstacles,
                                      unsigned int pressureOffsetID = 0);

    void SubtractPressureGradient(unsigned int pressureID, 
                                  unsigned int velocityID);

    void UpdateBoundaryOffsetTables(unsigned int velocityOffsetID,
                                    unsigned int pressureOffsetID,
                                    unsigned int boundariesID);

    void ComputeArbitraryVelocityBCs(unsigned int velocityID, 
                                     unsigned int offsetsID);   

    void SetupPoissonPressureZCull(unsigned int pressureID);
    void SetupObstacleZCull(unsigned int boundariesID);

protected: // methods
    void _CreateOffsetTextures();

protected: // data
    int             _iWidth;           // grid dimensions
    int             _iHeight;

    float           _dx;               // grid scale (assumes square cells)

    // lookup table textures
    unsigned int    _iVelocityOffsetTexture;
    unsigned int    _iPressureOffsetTexture;
    
    // temporary
    bool            _bEnableTextureUpdates; // for benchmarking the cost of copies

    // Simulation operations
    FluidStreamOp   _addImpulse;
    FluidStreamOp   _advect;
    FluidStreamOp   _divergence;
    FluidStreamOp   _poissonSolver;
    FluidStreamOp   _poissonSolverVec2;
    FluidStreamOp   _subtractGradient;
    FluidStreamOp   _vorticity;
    FluidStreamOp   _vorticityForce;  

    FluidStreamOp   _updateOffsets;
    FluidStreamOp   _arbitraryVelocityBC;
    FluidStreamOp   _arbitraryPressureBC;
    FluidDisplayOp  _zcullSetupBoundaries;
    FluidDisplayOp  _zcullSetupPressure;

    FluidBCOp       _boundaries;    
};

#endif //__FLUIDBASE_H__