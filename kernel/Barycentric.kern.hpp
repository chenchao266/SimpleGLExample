﻿#ifndef FMMTL_BARYCENTRIC_KERN
#define FMMTL_BARYCENTRIC_KERN

#include "fmmtl/Kernel.hpp"

#include "fmmtl/numeric/Vec.hpp"

struct Barycentric
    //: public Kernel<Barycentric> 
{
  typedef VecX<1,double>  source_type;
  typedef double         charge_type;
  typedef VecX<1,double>  target_type;
  typedef double         result_type;
  typedef double         kernel_value_type;

  /** Kernel evaluation
   * K(t,s) =  1 / R  where |R| > 0
   *           0      else
   * where R = s-t
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    return (t[0] == s[0] ? 0 : 1.0 / (t[0] - s[0]));
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return -kts;
  }
};
FMMTL_KERNEL_EXTRAS(Barycentric);

#endif
