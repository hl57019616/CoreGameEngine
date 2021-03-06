/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.10
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


using System;
using System.Runtime.InteropServices;

public class CProjectile : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal CProjectile(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(CProjectile obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~CProjectile() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if (swigCPtr.Handle != IntPtr.Zero) {
        if (swigCMemOwn) {
          swigCMemOwn = false;
          testPINVOKE.delete_CProjectile(swigCPtr);
        }
        swigCPtr = new HandleRef(null, IntPtr.Zero);
      }
      GC.SuppressFinalize(this);
    }
  }

  public CProjectile(CProjectileProfile Profile, CWeapon Weapon, bool IsVisible, bool UsePhysics) : this(testPINVOKE.new_CProjectile__SWIG_0(CProjectileProfile.getCPtr(Profile), CWeapon.getCPtr(Weapon), IsVisible, UsePhysics), true) {
    if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
  }

  public CProjectile(CProjectileProfile Profile, CWeapon Weapon, bool IsVisible) : this(testPINVOKE.new_CProjectile__SWIG_1(CProjectileProfile.getCPtr(Profile), CWeapon.getCPtr(Weapon), IsVisible), true) {
    if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
  }

  public CProjectile(CProjectileProfile Profile, CWeapon Weapon) : this(testPINVOKE.new_CProjectile__SWIG_2(CProjectileProfile.getCPtr(Profile), CWeapon.getCPtr(Weapon)), true) {
    if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
  }

  public bool Shoot(CPlayer Player, SWIGTYPE_p_Vector3 Direction) {
    bool ret = testPINVOKE.CProjectile_Shoot(swigCPtr, CPlayer.getCPtr(Player), SWIGTYPE_p_Vector3.getCPtr(Direction));
    if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
    return ret;
  }

  public void Update(SWIGTYPE_p_f32 elapsedTime) {
    testPINVOKE.CProjectile_Update(swigCPtr, SWIGTYPE_p_f32.getCPtr(elapsedTime));
    if (testPINVOKE.SWIGPendingException.Pending) throw testPINVOKE.SWIGPendingException.Retrieve();
  }

  public SWIGTYPE_p_f32 GetDamage() {
    SWIGTYPE_p_f32 ret = new SWIGTYPE_p_f32(testPINVOKE.CProjectile_GetDamage(swigCPtr), true);
    return ret;
  }

  public CPlayer GetOwner() {
    IntPtr cPtr = testPINVOKE.CProjectile_GetOwner(swigCPtr);
    CPlayer ret = (cPtr == IntPtr.Zero) ? null : new CPlayer(cPtr, false);
    return ret;
  }

  public CWeapon GetWeapon() {
    IntPtr cPtr = testPINVOKE.CProjectile_GetWeapon(swigCPtr);
    CWeapon ret = (cPtr == IntPtr.Zero) ? null : new CWeapon(cPtr, false);
    return ret;
  }

  public SWIGTYPE_p_Vector3 GetPosition() {
    IntPtr cPtr = testPINVOKE.CProjectile_GetPosition(swigCPtr);
    SWIGTYPE_p_Vector3 ret = (cPtr == IntPtr.Zero) ? null : new SWIGTYPE_p_Vector3(cPtr, false);
    return ret;
  }

  public SWIGTYPE_p_Vector3 GetDirection() {
    IntPtr cPtr = testPINVOKE.CProjectile_GetDirection(swigCPtr);
    SWIGTYPE_p_Vector3 ret = (cPtr == IntPtr.Zero) ? null : new SWIGTYPE_p_Vector3(cPtr, false);
    return ret;
  }

  public bool isAlive {
    set {
      testPINVOKE.CProjectile_isAlive_set(swigCPtr, value);
    } 
    get {
      bool ret = testPINVOKE.CProjectile_isAlive_get(swigCPtr);
      return ret;
    } 
  }

}
