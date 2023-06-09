﻿#ifndef FMMTL_GAUSSIAN_KERN
#define FMMTL_GAUSSIAN_KERN
/** @file Gaussian
 * @brief Implements the Gaussian kernel defined by
 * K(t,s) = exp(-||t - s||^2 / h^2)
 *
 */

#include "fmmtl/Kernel.hpp"

// Use a library-defined Vector class that supports multiple architectures
#include "fmmtl/numeric/Vec.hpp"

template <std::size_t D>
struct Gaussian
    //: public Kernel<Gaussian<D> >
{
  typedef VecX<D,double>  source_type;
  typedef double         charge_type;
  typedef VecX<D,double>  target_type;
  typedef double         result_type;
  typedef double         kernel_value_type;

  double inv_h_;
  double inv_h_sq_;

  FMMTL_INLINE
  Gaussian(double h = 1)
      : inv_h_(1/h), inv_h_sq_(inv_h_*inv_h_) {
  }

  /** Kernel evaluation */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    return exp(-inv_h_sq_ * norm_2_sq(t-s));
  }
  /** Fast transpose */
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kts;
  }
};
// XXX: Need to fix the build system...
FMMTL_KERNEL_EXTRAS(Gaussian<1>);
FMMTL_KERNEL_EXTRAS(Gaussian<2>);
FMMTL_KERNEL_EXTRAS(Gaussian<3>);
FMMTL_KERNEL_EXTRAS(Gaussian<4>);

#endif
