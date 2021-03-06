/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.10
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class ANIMATION_MAPPING : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal ANIMATION_MAPPING(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(ANIMATION_MAPPING obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~ANIMATION_MAPPING() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          testPINVOKE.delete_ANIMATION_MAPPING(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public SWIGTYPE_p_String MeshName {
    set {
      testPINVOKE.ANIMATION_MAPPING_MeshName_set(swigCPtr, SWIGTYPE_p_String.getCPtr(value));
      if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
    } 
    get {
      SWIGTYPE_p_String ret = new SWIGTYPE_p_String(testPINVOKE.ANIMATION_MAPPING_MeshName_get(swigCPtr), true);
      if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
      return ret;
    } 
  }

  public SWIGTYPE_p_VectorT_Core__ANIMATION_MAPPING__SOUND_ANIMATION_t Animations {
    set {
      testPINVOKE.ANIMATION_MAPPING_Animations_set(swigCPtr, SWIGTYPE_p_VectorT_Core__ANIMATION_MAPPING__SOUND_ANIMATION_t.getCPtr(value));
    } 
    get {
      IntPtr cPtr = testPINVOKE.ANIMATION_MAPPING_Animations_get(swigCPtr);
      SWIGTYPE_p_VectorT_Core__ANIMATION_MAPPING__SOUND_ANIMATION_t ret = (cPtr == IntPtr.Zero) ? null : new SWIGTYPE_p_VectorT_Core__ANIMATION_MAPPING__SOUND_ANIMATION_t(cPtr, false);
      return ret;
    } 
  }

  public ANIMATION_MAPPING() : this(testPINVOKE.new_ANIMATION_MAPPING(), true) {
  }

}
