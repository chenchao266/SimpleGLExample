﻿#ifndef FMMTL_UNITKERNEL_KERN
#define FMMTL_UNITKERNEL_KERN
/** @file UnitKernel
 * @brief Implements the unit kernel defined by
 * K(t,s) = 1  if t != s
 * K(t,s) = 0  if t == s
 *
 * Note: Mostly for testing purposes.
 */

#include "fmmtl/Kernel.hpp"
// Use a library-defined Vector class that supports multiple architectures
#include "fmmtl/numeric/Vec.hpp"

struct UnitPotential
    //: public Kernel<UnitPotential>
{
  typedef VecX<3,double>  source_type;
  typedef double         charge_type;
  typedef VecX<3,double>  target_type;
  typedef double         result_type;
  typedef unsigned       kernel_value_type;

  /** Kernel evaluation
   * K(t,s) =  1  if s != t,
   *           0  else
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    return kernel_value_type(!(s == t));
  }
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kts;
  }
};
FMMTL_KERNEL_EXTRAS(UnitPotential);


#include "fmmtl/Expansion.hpp"

struct UnitExpansion
    : public Expansion<UnitPotential, UnitExpansion>
{
  //! Point type to use for the trees
  typedef VecX<3,double> point_type;

  //! Multipole expansion type
  typedef double multipole_type;
  //! Local expansion type
  typedef double local_type;

  /** Kernel S2M operation */
  void S2M(const point_type&, const charge_type& charge,
           const point_type&, multipole_type& M) const {
    M += charge;
  }

  /** Kernel S2L operation */
  void S2L(const source_type&, const charge_type& charge,
           const point_type&, local_type& L) const {
    L += charge;
  }

  /** Kernel M2M operator */
  void M2M(const multipole_type& source,
                 multipole_type& target,
           const point_type&) const {
    target += source;
  }

  /** Kernel M2L operation */
  void M2L(const multipole_type& source,
                 local_type& target,
           const point_type&) const {
    target += source;
  }

  /** Kernel M2T operation */
  void M2T(const multipole_type& M, const point_type&,
           const target_type&, result_type& result) const {
    result += M;
  }

  /** Kernel L2L operator */
  void L2L(const local_type& source,
                 local_type& target,
           const point_type&) const {
    target += source;
  }

  /** Kernel L2T operation */
  void L2T(const local_type& L, const point_type&,
           const target_type&, result_type& result) const {
    result += L;
  }
};

#endif
