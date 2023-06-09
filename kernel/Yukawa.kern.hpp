﻿#ifndef FMMTL_YUKAWA_KERN
#define FMMTL_YUKAWA_KERN

#include "fmmtl/Kernel.hpp"

#include "fmmtl/numeric/Vec.hpp"

struct YukawaPotential
    //: public Kernel<YukawaPotential>
{
  typedef VecX<3,double>  source_type;
  typedef double         charge_type;
  typedef VecX<3,double>  target_type;
  typedef double         result_type;
  typedef double         kernel_value_type;

  double kappa;

  FMMTL_INLINE
  YukawaPotential(double _kappa = 1) : kappa(_kappa) {}

  /** Kernel evaluation
   * K(t,s) =  exp(-kR)/R  if R >= 1e-10
   *           0           else
   * where k = kappa, R = |s-t|_2
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    double R = norm_2(s - t);              //   R
    double invR  = 1.0 / R;                //   1.0 / R
    if (R < 1e-10) { R = invR = 0; }       //   Exclude self interaction
    return exp(-kappa*R) * invR;           //   Potential
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kts;
  }
};
FMMTL_KERNEL_EXTRAS(YukawaPotential);

struct YukawaKernel
    //: public Kernel<YukawaKernel> 
{
  typedef VecX<3,double>  source_type;
  typedef double         charge_type;
  typedef VecX<3,double>  target_type;
  typedef VecX<4,double>  result_type;
  typedef VecX<4,double>  kernel_value_type;

  double kappa;

  FMMTL_INLINE
  YukawaKernel(double _kappa = 1) : kappa(_kappa) {}

  /** Kernel evaluation
   * K(t,s) =  {exp(-kR)/R, (s-t)(kR+1)exp(-kR)/R^3}  if R >= 1e-10
   *           {0,0,0,0}                              else
   * where k = kappa, R = |s-t|_2
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    VecX<3,double> dist = s - t;            //   VecXtor from target to source
    double R2 = norm_2_sq(dist);              //   R^2
    double R  = std::sqrt(R2);             //   R
    double invR  = 1.0/R;                  //   1.0 / R
    double invR2 = 1.0/R2;                 //   1.0 / R^2
    if (R2 < 1e-20) { invR = invR2 = 0; }; //   Exclude self interaction
    double pot = exp(-kappa*R) * invR;     //   Potential
    dist *= pot * (kappa*R + 1) * invR2;   //   Force
    return kernel_value_type(pot, dist[0], dist[1], dist[2]);
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kernel_value_type(kts[0], -kts[1], -kts[2], -kts[3]);
  }
};
FMMTL_KERNEL_EXTRAS(YukawaKernel);

#endif
