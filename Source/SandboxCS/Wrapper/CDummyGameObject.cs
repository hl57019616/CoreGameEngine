/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.10
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class CDummyGameObject : CGameObject {
  private HandleRef swigCPtr;

  internal CDummyGameObject(IntPtr cPtr, bool cMemoryOwn) : base(testPINVOKE.CDummyGameObject_SWIGUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CDummyGameObject obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CDummyGameObject() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          testPINVOKE.delete_CDummyGameObject(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public CDummyGameObject() : this(testPINVOKE.new_CDummyGameObject__SWIG_0(), true) {
  }

  public CDummyGameObject(string Name, SWIGTYPE_p_SceneNode n, SWIGTYPE_p_Ogre__Entity e) : this(testPINVOKE.new_CDummyGameObject__SWIG_1(Name, SWIGTYPE_p_SceneNode.getCPtr(n), SWIGTYPE_p_Ogre__Entity.getCPtr(e)), true) {
  }

}
