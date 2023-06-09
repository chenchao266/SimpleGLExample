﻿#ifndef FMMTL_STOKES_KERN
#define FMMTL_STOKES_KERN

#include "fmmtl/Kernel.hpp"

#include "fmmtl/numeric/Vec.hpp"

/** Stokeslet Green's function:
 *
 * In index notation:
 * S_ij(t,s) = delta_ij / |s-t| + (s-t)_i (s-t)_j / |s-t|^3
 * In matrix form:
 * S(t,s)    = I / |s-t| + (s-t) (s-t)^T / |s-t|^3
 */
struct Stokeslet
    //: public Kernel<Stokeslet> 
{
  typedef VecX<3,double>  source_type;
  typedef VecX<3,double>  charge_type;
  typedef VecX<3,double>  target_type;
  typedef VecX<3,double>  result_type;

  /** A compressed struct to represent the rank-1 3x3 stokeslet matrix */
  struct kernel_value_type {
      VecX<3,double> r;

    FMMTL_INLINE
    kernel_value_type(const Vec<3,double>& _r) : r(_r) {}

    FMMTL_INLINE
    result_type operator*(const charge_type& c) const {
      double invR2 = 1.0 / norm_2_sq(r);
      if (invR2 > 1e20) invR2 = 0;
      double invR = std::sqrt(invR2);
      double rcInvR3 = inner_prod(r,c) * invR * invR2;

      return result_type(invR*c[0] + rcInvR3*r[0],
                         invR*c[1] + rcInvR3*r[1],
                         invR*c[2] + rcInvR3*r[2]);
    }
  };

  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    return kernel_value_type(s-t);
  }

  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kernel_value_type(-kts.r);
  }
};
FMMTL_KERNEL_EXTRAS(Stokeslet);

#endif
