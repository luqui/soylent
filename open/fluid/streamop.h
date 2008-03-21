//----------------------------------------------------------------------------
// File : streamop.h
//----------------------------------------------------------------------------
// Copyright 2003 Mark J. Harris and
// The University of North Carolina at Chapel Hill
//----------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that copyrigh
// notice and this permission notice appear in supporting documentation. 
// Binaries may be compiled with this software without any royalties or 
// restrictions. 
//
// The author(s) and The University of North Carolina at Chapel Hill make no 
// representations about the suitability of this software for any purpose. 
// It is provided "as is" without express or implied warranty.
/**
 * @file streamop.h
 * 
 * A generic abstraction for the stream computations commonly performed when 
 * a GPU is used for general purpose computation (GPGPU).
 */
#ifndef __STREAMOP_H__
#define __STREAMOP_H__

#ifdef WIN32
#pragma warning(disable: 4786)
#endif

// "geep" is the sound you make when you pronounce "GPGPU".
namespace geep
{

//----------------------------------------------------------------------------
/**
 * @class StreamOp
 * @brief A generic abstraction of a GPGPU stream computation.
 * 
 * A stream computation is a computation performed in parallel on a 2D lattice
 * or a single slice of a 2D lattice.  StreamOp serves as an abstraction of 
 * the typical idiom of such a computation.  It is a template (host) class, 
 * using policy-based class design [Alexandrescu 2001].  The template 
 * parameters specify policy classes that implement small pieces of a StreamOp's
 * functionality.  This way, by providing a set of simple policy 
 * implementations, a wide variety of stream ops can be created by mixing and 
 * matching policies.  This is very flexible.
 *
 * The policies that must be provided are: 
 *   * OutputPolicy -- sets up / shuts down any special "render target" 
 *       functionality needed by the StreamOp.
 *   * StatePolicy -- sets and unsets the graphics state needed for the StreamOp.
 *   * VertexPipePolicy -- Sets up / shuts down vertex programs.
 *   * FragmentPipePolicy -- Sets up / shuts down fragment programs.
 *   * ComputePolicy -- Performs the computation (usually via rendering).
 *   * UpdatePolicy -- performs any copies or other update functions after 
 *       the compution has been performed.
 *
 * The interfaces of the policies are not strictly defined.  They each have
 * one or two methods that are required for StreamOp to compile.  These are 
 * shown in the "Noop" policies given below.  A policy may (and often will) 
 * have a richer interface than just the methods given in the Noops below.
 * See streamopGL.h or streamopCGGl.h for examples.
 */
template 
<
  class OutputPolicy,
  class StatePolicy,
  class VertexPipePolicy,
  class FragmentPipePolicy,
  class ComputePolicy,
  class UpdatePolicy
>
class StreamOp : public OutputPolicy,
                 public StatePolicy,
                 public VertexPipePolicy,
                 public FragmentPipePolicy,
                 public ComputePolicy,
                 public UpdatePolicy
{
public:
  StreamOp()  {}
  ~StreamOp() {}

  // The only method of the StreamOp host class is Compute(), which 
  // uses the inherited policy methods to perform the slab computation.
  // Note that this also defines the interfaces that the policy classes 
  // must implement.
  void Compute()
  {
    // Activate the output slab, if necessary
    ActivateOutput();

    // Set the necessary state for the stream operation
    StatePolicy::SetState();
    VertexPipePolicy::SetState();
    FragmentPipePolicy::SetState();
   
    SetViewport();

    // Perform the stream operation
    ComputePolicy::Compute();

    // Put the results of the operation into the output stream.
    UpdateOutputStream();
    
    // Reset state
    FragmentPipePolicy::ResetState();
    VertexPipePolicy::ResetState();
    StatePolicy::ResetState();

    // Deactivate the output stream, if necessary
    DeactivateOutput();
  }
};

//----------------------------------------------------------------------------
// "Noop" policies -- these define interfaces for the various SlabOp policies.
//----------------------------------------------------------------------------

struct NoopOutputPolicy
{
protected:
  void ActivateOutput()   {}
  void DeactivateOutput() {}
};

struct NoopStatePolicy
{
protected:
  void SetState()   {}
  void ResetState() {}
};

struct NoopVertexPipePolicy
{
protected:
  void SetState()   {}
  void ResetState() {}
};

struct NoopFragmentPipePolicy
{
protected:
  void SetState()   {}
  void ResetState() {}
};

struct NoopComputePolicy
{
protected:
  void Compute() {}
};

struct NoopUpdatePolicy
{
protected:
  void SetViewport() {}
  void UpdateOutputStream() {}
};

// An example (trivial and useless) StreamOp definition.
typedef StreamOp
< 
  NoopOutputPolicy, 
  NoopStatePolicy,
  NoopVertexPipePolicy,
  NoopFragmentPipePolicy,
  NoopComputePolicy,
  NoopUpdatePolicy 
>
StreamNop;

};

#endif //__STREAMOP_H__