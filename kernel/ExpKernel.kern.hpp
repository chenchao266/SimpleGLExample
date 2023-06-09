﻿#ifndef FMMTL_EXPKERNEL_KERN
#define FMMTL_EXPKERNEL_KERN
/** @file ExpKernel
 * @brief Implements the separable exponential kernel defined by
 * K(t,s) = exp(sum_i t_i - s_i)
 *
 * Note: Mostly for testing purposes.
 */

#include "fmmtl/Kernel.hpp"

// Use a library-defined Vector class that supports multiple architectures
#include "fmmtl/numeric/Vec.hpp"

struct ExpPotential
    //: public Kernel<ExpPotential>
{
  typedef VecX<3,double>  source_type;
  typedef double         charge_type;
  typedef VecX<3,double>  target_type;
  typedef double         result_type;
  typedef double         kernel_value_type;

  /** Kernel evaluation
   * K(t,s) = exp(sum_i t_i - s_i)
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    return exp(t[0] + t[1] + t[2]
               - s[0] - s[1] - s[2]);
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kernel_value_type(1.0) / kts;
  }
};
FMMTL_KERNEL_EXTRAS(ExpPotential);


#include "fmmtl/Expansion.hpp"

struct ExpExpansion
    : public Expansion<ExpPotential, ExpExpansion>
{
  //! Point type to use for the trees
  typedef VecX<3,double> point_type;

  //! Multipole expansion type
  typedef double multipole_type;
  //! Local expansion type
  typedef double local_type;

  /** Kernel S2M operation */
  void S2M(const source_type& source, const charge_type& charge,
           const point_type& center, multipole_type& M) const {
    M += exp(center[0] + center[1] + center[2]
             - source[0] - source[1] - source[2]) * charge;
  }

  /** Kernel S2L operation */
  void S2L(const source_type& source, const charge_type& charge,
           const point_type& center, local_type& L) const {
    L += exp(center[0] + center[1] + center[2]
             - source[0] - source[1] - source[2]) * charge;
  }

  /** Kernel M2M operator */
  void M2M(const multipole_type& source,
                 multipole_type& target,
           const point_type& translation) const {
    target += exp(translation[0] + translation[1] + translation[2]) * source;
  }

  /** Kernel M2L operation */
  void M2L(const multipole_type& source,
                 local_type& target,
           const point_type& translation) const {
    target += exp(translation[0] + translation[1] + translation[2]) * source;
  }

  /** Kernel M2T operation */
  void M2T(const multipole_type& M, const point_type& center,
           const target_type& target, result_type& result) const {
    result += exp(target[0] + target[1] + target[2]
                  - center[0] - center[1] - center[2]) * M;
  }

  /** Kernel L2L operator */
  void L2L(const local_type& source,
                 local_type& target,
           const point_type& translation) const {
    target += exp(translation[0] + translation[1] + translation[2]) * source;
  }

  /** Kernel L2T operation */
  void L2T(const local_type& L, const point_type& center,
           const target_type& target, result_type& result) const {
    result += exp(target[0] + target[1] + target[2]
                  - center[0] - center[1] - center[2]) * L;
  }
};

#endif
