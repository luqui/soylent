//----------------------------------------------------------------------------
// File : flo.h
//----------------------------------------------------------------------------
// Copyright 2003 Mark J. Harris and
// The University of North Carolina at Chapel Hill
//----------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that copyright 
// notice and this permission notice appear in supporting documentation. 
// Binaries may be compiled with this software without any royalties or 
// restrictions. 
//
// The author(s) and The University of North Carolina at Chapel Hill make no 
// representations about the suitability of this software for any purpose. 
// It is provided "as is" without express or implied warranty.
/**
* @file flo.h
* 
* Flo fluid simulation class definition.
*/
#ifndef __FLO_H__
#define __FLO_H__

#include "streamop.h"
#include "streamopGL.h"
#include "streamopCgGL.h"
#include "fluidbase.h"
#include <vector>

using namespace geep;
using namespace std;

// forward decl
class PBuffer;
class Texture;

//----------------------------------------------------------------------------
/**
* @class Flo
* @brief A gpu implementation of fluid simulation.
*/
class Flo
{
public: // enumerations

    // Names for the textures used in the simulation
    enum Texture
    {
        TEXTURE_VELOCITY,
        TEXTURE_VELOCITY_OFFSETS,
        TEXTURE_PRESSURE_OFFSETS,
        TEXTURE_DENSITY,
        TEXTURE_VORTICITY,
        TEXTURE_VECTOR_COUNT,
        TEXTURE_DIVERGENCE = TEXTURE_VECTOR_COUNT,
        TEXTURE_PRESSURE,    
        TEXTURE_COUNT
    };

    enum Texture8Bit
    {
        TEXTURE_BOUNDARIES,
        TEXTURE_8BIT_COUNT
    };

    // Names for the display modes
    enum DisplayMode
    {
        DISPLAY_INK,
        DISPLAY_VELOCITY,
        DISPLAY_PRESSURE,
        DISPLAY_VORTICITY,
        DISPLAY_MILK_AND_METAL,
        DISPLAY_COUNT
    };

    enum VectorDisplayMode
    {
        VECTOR_DISPLAY_NONE,
        VECTOR_DISPLAY_QUIVER,
        VECTOR_DISPLAY_STATIC_DOTS,
        VECTOR_DISPLAY_STATIC_DOTS_SIZED,
        VECTOR_DISPLAY_SPRING_DOTS,
        VECTOR_DISPLAY_SPRING_DOTS_SIZED,
        VECTOR_DISPLAY_COUNT
    };

public:
    Flo(int width, int height);
    ~Flo();

	

    void  Initialize(CGcontext context, bool bInitVectorViz);
    void  Finalize();

    void  Update();   // updates the simulation by one time step
    void  Reset(bool resetBC = false);    // resets the state of the simulation
	void  ResetBoundary();

	void SetDensityMap(float* density_map);
	void ReplaceDensityMap(float* density_map);
	void SetVelocityMap(float* velocity_map);
	void ReplaceVelocityMap(float* velocity_map);

    // Renders the simulation
    void  Display(DisplayMode mode, bool bilerp = false, bool makeTex = false,
                  bool displayBoundary = false, bool whiteBackground = false);

    void  DisplayVectorField(VectorDisplayMode mode,
                             int lineWidth, int pointSize, float vectorScale);

    // Used to draw an impulse into the fluid with the mouse
    void  AddImpulse(const float strength[3], const float position[2], 
                     float radius, bool bAddInk = true);
	
	void  AddDensityImpulse(float color[3], const float position[2], float radius);

	void  DrawBoundary(const float position[2], float radius, bool bErase = false, ::Texture* tex = NULL);

    // setters / getters / enablers / queries
    void  SetTimeStep(float t)          { _rTimestep = t;    }
    float GetTimeStep() const           { return _rTimestep; }

    void  SetGridScale(float dx)        { _pFluidImpl->SetGridScale(dx);      }
    float GetGridScale() const          { return _pFluidImpl->GetGridScale(); }

    void  SetNumPoissonSteps(int steps) { _iNumPoissonSteps = steps; }
    int   GetNumPoissonSteps() const    { return _iNumPoissonSteps;  }

    void  SetViscosity(float viscosity) { _rViscosity = viscosity;   }
    float GetViscosity() const          { return _rViscosity;        }

    void  SetInkLongevity(float longev) { _rInkLongevity = longev; }

    void  EnableArbitraryBC(bool bEnable) { _bArbitraryBC = bEnable; }
    bool  IsArbitraryBCEnabled() const    { return _bArbitraryBC;    }

    void  EnablePressureClear(bool bEnable) { _bClearPressureEachStep = bEnable; }
    bool  IsPressureClearEnabled() const    { return _bClearPressureEachStep; }

    void  EnableZCullOpt(bool bEnable) { _bZCullOpt = bEnable; }
    bool  IsZCullOptEnabled() const    { return _bZCullOpt;    }

    void  EnableTextureUpdates(bool bEnable) { _pFluidImpl->EnableTextureUpdates(bEnable); }
    bool  IsTextureUpdateEnabled() const { return _pFluidImpl->IsTextureUpdateEnabled(); }

    void  EnableVectorFieldUpdates(bool bEnable) { _bUpdateVectorField = bEnable; }
    bool  IsVectorFieldUpdateEnabled() const { return _bUpdateVectorField; }
  
    void  EnableVorticityComputation(bool bEnable) { _bComputeVorticity = bEnable; }
    bool  IsVorticityComputationEnabled() const    { return _bComputeVorticity; }

    void  EnableVCForce(bool bEnable) { _bApplyVCForce = bEnable; }
    bool  IsVCForceEnabled() const    { return _bApplyVCForce; }

    void  SetVorticityConfinementScale(float scale) { _rVorticityConfinementScale = scale; }
    float GetVorticityConfinementScale() const      { return _rVorticityConfinementScale;  }

    void  SetInkColor(float color[3])  
    { _rInkColor[0] = color[0]; _rInkColor[1] = color[1]; _rInkColor[2] = color[2]; }
    void  GetInkColor(float color[3]) const         
    { color[0] = _rInkColor[0]; color[1] = _rInkColor[1]; color[2] = _rInkColor[2]; }

    void  InitializeVertexBuffer(unsigned int iColumns, unsigned int iRows);
    void  SetVectorField(DisplayMode field) { _eVectorField = field; }

protected: // types

    // The display Slabop.  This simply displays a float texture to the screen,
    // using a specified fragment program.
    typedef StreamOp < NoopOutputPolicy, NoopStatePolicy, 
                       NoopVertexPipePolicy, GenericCgGLFragmentProgram,
                       SingleTextureGLComputePolicy, NoopUpdatePolicy >
    FloDisplayOp; 

    typedef StreamOp < NoopOutputPolicy, NoopStatePolicy, 
                       NoopVertexPipePolicy, GenericCgGLFragmentProgram,
                       MultiTextureGLComputePolicy, NoopUpdatePolicy >
    FloMultiTexDisplayOp; 

    struct Impulse 
    {
        Impulse(const float str[3], const float pos[2], 
            float col[3], float rad, bool bInk)
        {
            strength[0] = str[0]; strength[1] = str[1]; strength[2] = str[2];
            position[0] = pos[0]; position[1] = pos[1];
            color[0] = col[0]; color[1] = col[1]; color[2] = col[2];
            radius = rad; bAddInk = bInk;
        }
        float strength[3];
        float position[2];
        float color[3];
        float radius;
        bool  bAddInk;
    };

protected: // methods
    void _InitializeStreamOps(CGcontext context, bool bInitVectorViz);
    void _ClearTexture(unsigned int id, GLenum target = GL_TEXTURE_RECTANGLE_NV);

    void _GenerateNormalMap();

protected: // data
    // constants
    FluidBase       *_pFluidImpl;

    int             _iWidth;
    int             _iHeight;

    float           _rTimestep;
    int             _iNumPoissonSteps; // number of steps used in the jacobi solver

    float           _rInkColor[3];
    float           _rInkLongevity;   // how long the "ink" lasts

    float           _rViscosity;

    bool            _bArbitraryBC;

    bool            _bClearPressureEachStep;
    bool            _bZCullOpt;
    
    bool            _bImpulseToProcess;
    bool            _bInkToAdd;

    bool            _bComputeVorticity;
    bool            _bApplyVCForce;
    float           _rVorticityConfinementScale;

    vector<Impulse> _impulses;

    // Display operations
	FloDisplayOp	_displayColor;
	FloDisplayOp	_displayColorBilerp;
    FloDisplayOp    _displayScalar;
    FloDisplayOp    _displayVector;
    FloDisplayOp    _displayWhiteVector; // white background
    FloDisplayOp    _displayScalarBilerp;
    FloDisplayOp    _displayVectorBilerp;
    FloDisplayOp    _displayWhiteVectorBilerp;

    FloDisplayOp    _genNormalMap;  
    FloMultiTexDisplayOp _displayMilkAndMetal;

    GenericCgGLVertexProgram _vectorFieldVP;
    GenericCgGLVertexProgram _scalarFieldVP;
    GenericCgGLVertexProgram _vectorFieldPointSizeVP;
    GenericCgGLVertexProgram _scalarFieldPointSizeVP;

    PBuffer         *_pVectorBuffer;
    PBuffer         *_pScalarBuffer;

    // Textures
    unsigned int    _iTextures[TEXTURE_COUNT];

    unsigned int    _iDisplayTexture;
    unsigned int    _iBCTexture;
    unsigned int    _iBCDisplayTexture;
    unsigned int    _iBCDetailTexture;

    bool            _bNormalMapDirty;
    unsigned int    _iNormalMap;    
    unsigned int    _iCubeMap;

    float           *_zeros; // used for clearing textures

    unsigned int    _iIndexBuffer;
    unsigned int    _iVertexBuffer;
    unsigned int    _iHeadTailBuffer;
    unsigned int    _iVectorFieldTexture;
    unsigned int    _iVBWidth;
    unsigned int    _iVBHeight;
    bool            _bUpdateVectorField;
    DisplayMode     _eVectorField;

private:
    Flo(); // private default constructor prevents default construction.
};

#endif //__FLO_H__
