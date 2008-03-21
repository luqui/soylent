//----------------------------------------------------------------------------
// File : streamopCgGL.cpp
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
* @file streamopCgGL.cpp
* 
* StreamOp policy class implementations that use Cg and GL.
*/

#pragma warning(disable:4786)

#include "streamopCgGL.h"
#include <assert.h>

namespace geep
{
    //----------------------------------------------------------------------------
    // Function         : GenericCgGLVertexProgram::SetVPArgs
    // Description	    : 
    //----------------------------------------------------------------------------
    /**
    * @fn GenericCgGLVertexProgram::SetVPArgs(std::vector<std::string> args);
    * @brief Initializes the vertex program.
    */ 
    void GenericCgGLVertexProgram::SetVPArgs(std::vector<const char*> args)
    {
        _args.clear();
        _args.insert(_args.begin(), args.begin(), args.end());       
    }

	//----------------------------------------------------------------------------
	// Function         : GenericCgGLVertexProgram::Initialize
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::Initialize(CGcontext context, string fpFilename)
	* @brief Initializes the vertex program.
	*/ 
	void GenericCgGLVertexProgram::InitializeVP(CGcontext context, 
												   string fpFilename,
												   string entryPoint /* = "" */)
	{
		ShutdownVP();

		// TODO: support user-selected profiles?
		_profile = cgGLGetLatestProfile(CG_GL_VERTEX);
		assert(_profile != CG_PROFILE_UNKNOWN);

		// Load and initialize the Reaction-Diffusion Vertex program
		_vertexProgram = cgCreateProgramFromFile(context, CG_SOURCE, 
												 fpFilename.c_str(),
												 _profile, 
												 entryPoint.empty() ? 
													NULL : entryPoint.c_str(),
												 _args.empty() ? NULL : &_args[0]);

		if(_vertexProgram != NULL)
		{
			cgGLLoadProgram(_vertexProgram);
		}
	}


	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::Shutdown
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::Shutdown()
	* @brief Shuts down the vertex program.
	*/ 
	void GenericCgGLVertexProgram::ShutdownVP()
	{
		if (_vertexProgram)
		{
			cgDestroyProgram(_vertexProgram);
			_vertexProgram = 0;
		}
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter1fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter1fv(string name, const float *v)
	* @brief Passes an array of 1 value to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter1fv(string name, 
															const float *v)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter1fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter1f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter1f(string name, float x)
	* @brief Passes the value to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter1f(string name, float x)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter1f(p, x);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter2fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter2fv(string name, const float *v)
	* @brief Passes an array of 2 values to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter2fv(string name, 
															const float *v)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter2fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter2f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter2f(string name, float x, float y)
	* @brief Passes the 2 values to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter2f(string name, 
														   float x, float y)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter2f(p, x, y);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter3fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter3fv(string name, const float *v)
	* @brief Passes an array of 3 values to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter3fv(string name, 
		                                                    const float *v)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter3fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter3f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter3f(string name, float x, float y, float z)
	* @brief Passes the 3 values to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter3f(string name, 
		                                                   float x, float y, 
		                                                   float z)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter3f(p, x, y, z);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter4fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter4fv(string name, const float *v)
	* @brief Passes an array of 4 values to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter4fv(string name, 
		                                                    const float *v)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter4fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetVertexParameter4f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetVertexParameter4f(string name, float x, float y, float z, float w)
	* @brief Passes the 4 values to the given named Cg parameter.
	*/ 
	void GenericCgGLVertexProgram::SetVertexParameter4f(string name, 
		                                                   float x, float y, 
		                                                   float z, float w)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetParameter4f(p, x, y, z, w);
	}

    //----------------------------------------------------------------------------
    // Function     	: GenericCgGLVertexProgram::SetTextureParameter
    // Description	    : 
    //----------------------------------------------------------------------------
    /**
    * @fn GenericCgGLFragmentProgram::SetTextureParameter(string name, GLuint texObj)
    * @brief Sets the named Cg texture parameter to the specified texture object ID.
    */ 
    void GenericCgGLVertexProgram::SetTextureParameter(string name, 
        GLuint texObj)
    {
        CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
        assert(p);
        // first erase any existing copy of this parameter, since maps don't allow 
        // duplicates (and attempts to insert them leave the map unchanged).
        _texParams.erase(p);
        _texParams.insert(TexSet::value_type(p, texObj));
    }

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLVertexProgram::SetStateMatrixParameter
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLVertexProgram::SetStateMatrixParameter(string name, GLenum matrix, GLenum transform)
	* @brief Sets the specified parameter to the specified transformed GL state matrix.
	*/ 
	void GenericCgGLVertexProgram::SetStateMatrixParameter(string name, 
		                                                      CGGLenum matrix, 
		                                                      CGGLenum transform)
	{
		CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
		assert(p);
		cgGLSetStateMatrixParameter(p, matrix, transform);
	}

	//----------------------------------------------------------------------------
	// Function     	  : FloAddForcesVertexProgram::SetState
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn FloAddForcesVertexProgram::SetState()
	* @brief Sets Vertex program state.
	*/ 
	void GenericCgGLVertexProgram::SetState()
	{
		cgGLBindProgram(_vertexProgram);
		cgGLEnableProfile(_profile);

        for (TexSet::iterator iter = _texParams.begin(); 
             iter != _texParams.end(); 
             iter++)
        {
            CGparameter p = iter->first;
            GLuint i = iter->second;
            cgGLSetTextureParameter(iter->first, iter->second);
            cgGLEnableTextureParameter(iter->first);  
        }
	}

	//----------------------------------------------------------------------------
	// Function     	: FloAddForcesVertexProgram::ResetState
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn FloAddForcesVertexProgram::ResetState()
	* @brief Restores Vertex program state.
	*/ 
	void GenericCgGLVertexProgram::ResetState()
	{
        for (TexSet::iterator iter = _texParams.begin(); 
            iter != _texParams.end(); 
            iter++)
        {
            cgGLDisableTextureParameter(iter->first);
        }

		cgGLDisableProfile(_profile);
	}

    //----------------------------------------------------------------------------
    // Function     	: GenericCgGLFragmentProgram::SetParameterPointer
    // Description	    : 
    //----------------------------------------------------------------------------
    /**
    * @fn GenericCgGLFragmentProgram::SetParameterPointer(string name, GLint fsize, GLenum type, GLsizei stride, const GLvoid *pointer))
    * @brief Sets a vertex array pointer associated with the named vertex program parameter.
    */ 
    void GenericCgGLVertexProgram::SetParameterPointer(string name, 
                                                       GLint fsize,
                                                       GLenum type,
                                                       GLsizei stride,
                                                       const GLvoid *pointer)
    {
        CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
        assert(p);
        cgGLSetParameterPointer(p, fsize, type, stride, pointer);
    }

    //----------------------------------------------------------------------------
    // Function     	: GenericCgGLFragmentProgram::EnableClientState
    // Description	    : 
    //----------------------------------------------------------------------------
    /**
    * @fn GenericCgGLFragmentProgram::EnableClientState(string name)
    * @brief Enables a vertex array pointer associated with the named vertex program parameter.
    */ 
    void GenericCgGLVertexProgram::EnableClientState(string name)
    {
        CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
        assert(p);
        cgGLEnableClientState(p);
    }

    //----------------------------------------------------------------------------
    // Function     	: GenericCgGLFragmentProgram::DisableClientState
    // Description	    : 
    //----------------------------------------------------------------------------
    /**
    * @fn GenericCgGLFragmentProgram::DisableClientState(string name)
    * @brief Disables a vertex array pointer associated with the named vertex program parameter.
    */ 
    void GenericCgGLVertexProgram::DisableClientState(string name)
    {
        CGparameter p = cgGetNamedParameter(_vertexProgram, name.c_str());
        assert(p);
        cgGLDisableClientState(p);
    }

    //----------------------------------------------------------------------------
    // Function     	: GenericCgGLFragmentProgram::SetFPArgs
    // Description	    : 
    //----------------------------------------------------------------------------
    /**
    * @fn GenericCgGLFragmentProgram::SetFPArgs(std::vector<const char*> args)
    * @brief Initializes the fragment program.
    */ 
    void GenericCgGLFragmentProgram::SetFPArgs(std::vector<const char*> args)
    {
        _args.clear();
        _args.insert(_args.begin(), args.begin(), args.end());
    }

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::Initialize
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::Initialize(CGcontext context, string fpFilename)
	* @brief Initializes the fragment program.
	*/ 
	void GenericCgGLFragmentProgram::InitializeFP(CGcontext context, 
		                                             string fpFilename,
		                                             string entryPoint)
	{
		ShutdownFP();

		// TODO: support user-selected profiles?
		_profile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		assert(_profile != CG_PROFILE_UNKNOWN);

		// Load and initialize the Reaction-Diffusion fragment program
		_fragmentProgram = cgCreateProgramFromFile(context, CG_SOURCE, 
		     	                                   fpFilename.c_str(),
			                                       _profile,
			                                       entryPoint.empty() ? 
                                                       NULL : entryPoint.c_str(),
                                                   _args.empty() ? NULL : &(_args[0]));

		if(_fragmentProgram != NULL)
		{
			cgGLLoadProgram(_fragmentProgram);
		}
	}


	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::Shutdown
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::Shutdown()
	* @brief Shuts down the fragment program.
	*/ 
	void GenericCgGLFragmentProgram::ShutdownFP()
	{
		if (_fragmentProgram)
		{
			cgDestroyProgram(_fragmentProgram);
			_fragmentProgram = NULL;
		}
		_texParams.clear();
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter1fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter1fv(string name, const float *v)
	* @brief Passes an array of 1 value to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter1fv(string name, 
		                                                        const float *v)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter1fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter1f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter1f(string name, float x)
	* @brief Passes the value to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter1f(string name, 
                                                               float x)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter1f(p, x);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter2fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter2fv(string name, const float *v)
	* @brief Passes an array of 2 values to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter2fv(string name, 
		                                                        const float *v)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter2fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter2f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter2f(string name, float x, float y)
	* @brief Passes the 2 values to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter2f(string name, 
		                                                       float x, float y)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter2f(p, x, y);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter3fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter3fv(string name, const float *v)
	* @brief Passes an array of 3 values to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter3fv(string name, 
		                                                        const float *v)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter3fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter3f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter3f(string name, float x, float y, float z)
	* @brief Passes the 3 values to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter3f(string name, 
		                                                       float x, float y, 
		                                                       float z)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter3f(p, x, y, z);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter4fv
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter4fv(string name, const float *v)
	* @brief Passes an array of 4 values to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter4fv(string name, 
		                                                        const float *v)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter4fv(p, v);
	}

	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetFragmentParameter4f
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetFragmentParameter4f(string name, float x, float y, float z, float w)
	* @brief Passes the 4 values to the given named Cg parameter.
	*/ 
	void GenericCgGLFragmentProgram::SetFragmentParameter4f(string name, 
															   float x, float y, 
															   float z, float w)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		cgGLSetParameter4f(p, x, y, z, w);
	}


	//----------------------------------------------------------------------------
	// Function     	: GenericCgGLFragmentProgram::SetTextureParameter
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn GenericCgGLFragmentProgram::SetTextureParameter(string name, GLuint texObj)
	* @brief Sets the named Cg texture parameter to the specified texture object ID.
	*/ 
	void GenericCgGLFragmentProgram::SetTextureParameter(string name, 
													        GLuint texObj)
	{
		CGparameter p = cgGetNamedParameter(_fragmentProgram, name.c_str());
		assert(p);
		// first erase any existing copy of this parameter, since maps don't allow 
		// duplicates (and attempts to insert them leave the map unchanged).
		_texParams.erase(p);
		_texParams.insert(TexSet::value_type(p, texObj));
	}

	//----------------------------------------------------------------------------
	// Function     	: FloAddForcesFragmentProgram::SetState
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn FloAddForcesFragmentProgram::SetState()
	* @brief Sets fragment program state.
	*/ 
	void GenericCgGLFragmentProgram::SetState()
	{
		cgGLBindProgram(_fragmentProgram);

		cgGLEnableProfile(_profile);

		for (TexSet::iterator iter = _texParams.begin(); 
			 iter != _texParams.end(); 
			 iter++)
		{
			CGparameter p = iter->first;
			GLuint i = iter->second;
			cgGLSetTextureParameter(iter->first, iter->second);
			cgGLEnableTextureParameter(iter->first);  
		}
	}

	//----------------------------------------------------------------------------
	// Function     	: FloAddForcesFragmentProgram::ResetState
	// Description	    : 
	//----------------------------------------------------------------------------
	/**
	* @fn FloAddForcesFragmentProgram::ResetState()
	* @brief Restores fragment program state.
	*/ 
	void GenericCgGLFragmentProgram::ResetState()
	{
		for (TexSet::iterator iter = _texParams.begin(); 
			 iter != _texParams.end(); 
			 iter++)
		{
			cgGLDisableTextureParameter(iter->first);
		}

		cgGLDisableProfile(_profile);
	}

}; // namespace geep