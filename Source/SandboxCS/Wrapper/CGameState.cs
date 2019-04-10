/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.10
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class CGameState : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal CGameState(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CGameState obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CGameState() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          testPINVOKE.delete_CGameState(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public virtual void Update(SWIGTYPE_p_f32 elapsedTime) {
    testPINVOKE.CGameState_Update(swigCPtr, SWIGTYPE_p_f32.getCPtr(elapsedTime));
    if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
  }

  public virtual bool Load() {
    bool ret = testPINVOKE.CGameState_Load(swigCPtr);
    return ret;
  }

  public virtual void End() {
    testPINVOKE.CGameState_End(swigCPtr);
  }

}