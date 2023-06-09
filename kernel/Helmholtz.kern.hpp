﻿#ifndef FMMTL_HELMHOLTZ_KERN
#define FMMTL_HELMHOLTZ_KERN

#include "fmmtl/Kernel.hpp"

#include "fmmtl/numeric/Vec.hpp"
#include "fmmtl/numeric/Complex.hpp"

struct HelmholtzPotential
    //: public Kernel<HelmholtzPotential> 
{
  typedef complex<double> complex_t;

  typedef Vec<3,double>  source_type;
  typedef complex_t        charge_type;
  typedef Vec<3,double>  target_type;
  typedef complex_t        result_type;
  typedef complex_t        kernel_value_type;

  double kappa;

  FMMTL_INLINE
  HelmholtzPotential(double _kappa = 1) : kappa(_kappa) {}

  /** Kernel evaluation
   * K(t,s) =  exp(ikR)/R  if R >= 1e-10
   *           0           else
   * where k = kappa, R = |s-t|_2
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    double R = norm_2(s - t);              //   R
    double invR = 1.0 / R;                 //   1.0 / R
    if (R < 1e-10) invR = 0;               //   Exclude self interaction
    R *= kappa;                            //   R <- kappa*R
    return complex_t(cos(R), sin(R)) * invR; //   Potential
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kts;
  }
};
FMMTL_KERNEL_EXTRAS(HelmholtzPotential);


struct HelmholtzKernel
    //: public Kernel<HelmholtzKernel>
{
  typedef complex<double> complex_t;

  typedef Vec<3,double>  source_type;
  typedef complex_t        charge_type;
  typedef Vec<3,double>  target_type;
  typedef Vec<4, complex_t> result_type;
  typedef Vec<4, complex_t> kernel_value_type;

  double kappa;

  FMMTL_INLINE
  HelmholtzKernel(double _kappa = 1) : kappa(_kappa) {}

  /** Kernel evaluation
   * K(t,s) =  {exp(ikR)/R, (s-t)(1-ikR)exp(ikR)/R^3}  if R >= 1e-10
   *           {0,0,0,0}                                else
   * where k = kappa, R = |s-t|_2
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    Vec<3,double> dist = s - t;                   //   Vector from target to source
    double R2 = norm_2_sq(dist);                  //   R^2
    double R  = std::sqrt(R2);                    //   R
    double invR  = 1.0/R;                         //   1.0 / R
    double invR2 = 1.0/R2;                        //   1.0 / R^2
    if (R2 < 1e-20) invR = invR2 = 0;             //   Exclude self interaction
    R *= kappa;                                   //   R <- kappa*R
    complex_t pot = complex_t(cos(R), sin(R)) * invR; //   Potential
    complex_t a = pot * complex_t(1,-R) * invR2;      //   Force
    return kernel_value_type(pot, a*dist[0], a*dist[1], a*dist[2]);
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kernel_value_type(kts[0], -kts[1], -kts[2], -kts[3]);
  }
};
FMMTL_KERNEL_EXTRAS(HelmholtzKernel);

#endif
