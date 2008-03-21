//----------------------------------------------------------------------------
// File : streamopCgGL.h
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
* @file StreamOpCgGL.h
* 
* StreamOp policy classes that use Cg and OpenGL.
*/
#ifndef __STREAMOPCGGL_H__
#define __STREAMOPCGGL_H__

//#include <gl/glew.h>
#include <cg/cggl.h>
#include <string>
#include <map>
#include <vector>

#pragma warning(disable:4786)

using namespace std;

namespace geep
{

    //----------------------------------------------------------------------------
    /**
    * @class GenericCgGLVertexProgram
    * @brief Manages vertex program state using Cg and OpenGL.
    * 
    * This class supports a single vertex program and its parameters. It is used
    * both as a vertex state policy class for geep::StreamOp, but also as a 
    * standalone wrapper for vertex programs to avoid proliferation of 
    * CGParameter variables.
    */
    class GenericCgGLVertexProgram
    {
    public:
        GenericCgGLVertexProgram() : 
          _vertexProgram(0), 
          _profile(CG_PROFILE_UNKNOWN) {}

        virtual ~GenericCgGLVertexProgram() {}

        void SetState();   // called by StreamOp::Compute()
        void ResetState(); // called by StreamOp::Compute()

        void SetVPArgs(std::vector<const char*> args);
        void InitializeVP(CGcontext context, string vpFilename, 
                          string entryPoint = "");
        void ShutdownVP();

        void SetVertexParameter1fv(string name, const float *v);
        void SetVertexParameter1f(string name, float x);
        void SetVertexParameter2fv(string name, const float *v);
        void SetVertexParameter2f(string name, float x, float y);
        void SetVertexParameter3fv(string name, const float *v);
        void SetVertexParameter3f(string name, float x, float y, float z);
        void SetVertexParameter4fv(string name, const float *v);
        void SetVertexParameter4f(string name, float x, float y, float z, float w);

        void SetTextureParameter(string name, GLuint texObj);   

        void SetStateMatrixParameter(string name, CGGLenum matrix, 
                                     CGGLenum transform);

        void SetParameterPointer(string name, GLint fsize, GLenum type, 
                                 GLsizei stride, const GLvoid *pointer);
        void EnableClientState(string name);
        void DisableClientState(string name);
    protected: // types
        // We maintain a map of CG parameters to texture objects, so that SetState()
        // and ResetState() can enable and disable them.  A std::map is used because
        // it doesn't allow duplicates.  Since inserting a duplicate key leaves the
        // map unchanged, we must first erase any pair with the parameter we are 
        // setting, so that we reflect the most recent call to SetTextureParameter.
        typedef map<CGparameter, GLuint> TexSet;

    protected: // data
        CGprogram _vertexProgram;
        CGprofile _profile;

        TexSet    _texParams;

        std::vector<const char*> _args;
    };

    //----------------------------------------------------------------------------
    /**
    * @class GenericCgGLFragmentProgram
    * @brief Manages fragment program state using Cg and OpenGL.
    * 
    * This class supports a single fragment program and its parameters. It is 
    * used both as a fragment state policy class for geep::StreamOp, but also 
    * as a standalone wrapper for fragment programs to avoid proliferation of 
    * CGParameter variables.
    */
    class GenericCgGLFragmentProgram
    {
    public: 
        GenericCgGLFragmentProgram() : 
          _fragmentProgram(0), 
          _profile(CG_PROFILE_UNKNOWN) {}

        virtual ~GenericCgGLFragmentProgram() {}

        void SetState();   // called by StreamOp::Compute()
        void ResetState(); // called by StreamOp::Compute()

        void SetFPArgs(std::vector<const char*> args);
        void InitializeFP(CGcontext context, string fpFilename,
                          string entryPoint = "");
        void ShutdownFP();

        void SetFragmentParameter1fv(string name, const float *v);
        void SetFragmentParameter1f(string name, float x);
        void SetFragmentParameter2fv(string name, const float *v);
        void SetFragmentParameter2f(string name, float x, float y);
        void SetFragmentParameter3fv(string name, const float *v);
        void SetFragmentParameter3f(string name, float x, float y, float z);
        void SetFragmentParameter4fv(string name, const float *v);
        void SetFragmentParameter4f(string name, float x, float y, float z, float w);

        void SetTextureParameter(string name, GLuint texObj);

    protected: // types
        // We maintain a map of CG parameters to texture objects, so that SetState()
        // and ResetState() can enable and disable them.  A std::map is used because
        // it doesn't allow duplicates.  Since inserting a duplicate key leaves the
        // map unchanged, we must first erase any pair with the parameter we are 
        // setting, so that we reflect the most recent call to SetTextureParameter.
        typedef map<CGparameter, GLuint> TexSet;

    protected: // data
        CGprogram _fragmentProgram;
        CGprofile _profile;

        TexSet    _texParams;

        std::vector<const char*> _args;
    };

}; // namespace geep
#endif //__STREAMOPCGGL_H__