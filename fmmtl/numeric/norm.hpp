﻿#pragma once
/** @file norm.hpp
 * @brief Define linear algebraic operators for common types:
 *    inner_prod     //< Inner product
 *    dot            //< Alias of inner product
 *    norm_1         //< One-norm
 *    norm_2         //< Two-norm
 *    norm_2_sq      //< Squared two-norm -- avoid the sqrt
 *    norm_inf       //< Infinity-norm
 * C++03 compatible
 */


// TODO: Place in fmmtl namespace

#include <cmath>
#include <complex>

#include "fmmtl/config.hpp"
#include "fmmtl/numeric/Vec.hpp"
#include "fmmtl/numeric/Complex.hpp"

template <typename T>
struct norm_type {
  typedef T type;
};

template <typename T>
struct norm_type<complex<T> > {
  typedef typename norm_type<T>::type type;
};

template <typename T>
struct norm_type<std::complex<T> > {
  typedef typename norm_type<T>::type type;
};

template <std::size_t N, typename T>
struct norm_type<VecX<N,T> > {
  typedef typename norm_type<T>::type type;
};

///////////
/// POD ///
///////////

/** Compute the inner product of two doubles */
FMMTL_INLINE
double
inner_prod(double a, double b) {
  return a*b;
}
FMMTL_INLINE
double
dot(double a, double b) {
  return inner_prod(a,b);
}
/** Compute the inner product of two floats */
FMMTL_INLINE
float
inner_prod(float a, float b) {
  return a*b;
}
FMMTL_INLINE
float
dot(float a, float b) {
  return inner_prod(a,b);
}

/** Compute the squared L2 norm of a double */
FMMTL_INLINE
double
norm_2_sq(double a) {
  return a*a;
}
/** Compute the squared L2 norm of a float */
FMMTL_INLINE
float
norm_2_sq(float a) {
  return a*a;
}
/** Compute the L2 norm of a double */
FMMTL_INLINE
double
norm_2(double a) {
  return std::abs(a);
}
/** Compute the L2 norm of a float */
FMMTL_INLINE
float
norm_2(float a) {
  return std::abs(a);
}
/** Compute the L1 norm of a double */
FMMTL_INLINE
double
norm_1(double a) {
  return std::abs(a);
}
/** Compute the L1 norm of a float */
FMMTL_INLINE
float
norm_1(float a) {
  return std::abs(a);
}
/** Compute the L-infinity norm of a double */
FMMTL_INLINE
double
norm_inf(double a) {
  return std::abs(a);
}
/** Compute the L-infinity norm of a float */
FMMTL_INLINE
float
norm_inf(float a) {
  return std::abs(a);
}

/////////////////////////
/// complex<T> ///
/////////////////////////

/** Compute the inner product of two complex numbers */
template <typename T>
FMMTL_INLINE
typename norm_type<complex<T> >::type
inner_prod(const complex<T>& a, const complex<T>& b) {
  return inner_prod(a.real(),b.real()) + inner_prod(a.imag(),b.imag());
}
template <typename T>
FMMTL_INLINE
typename norm_type<complex<T> >::type
dot(const complex<T>& a, const complex<T>& b) {
  return inner_prod(a,b);
}

/** Compute the squared L2 norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<complex<T> >::type
norm_2_sq(const complex<T>& a) {
  return norm_2_sq(a.real()) + norm_2_sq(a.imag());
}
/** Compute the L2 norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<complex<T> >::type
norm_2(const complex<T>& a) {
  
  return std::sqrt(norm_2_sq(a));
}
/** Compute the L1 norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<complex<T> >::type
norm_1(const complex<T>& a) {
  return norm_1(a.real()) + norm_1(a.imag());
}
/** Compute the L-infinity norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<complex<T> >::type
norm_inf(const complex<T>& a) {
 
  return std::max(norm_inf(a.real()), norm_inf(a.imag()));
}

///////////////////////
/// std::complex<T> ///
///////////////////////

/** Compute the inner product of two complex numbers */
template <typename T>
FMMTL_INLINE
typename norm_type<std::complex<T> >::type
inner_prod(const std::complex<T>& a, const std::complex<T>& b) {
  return inner_prod(a.real(),b.real()) + inner_prod(a.imag(),b.imag());
}
template <typename T>
FMMTL_INLINE
typename norm_type<std::complex<T> >::type
dot(const std::complex<T>& a, const std::complex<T>& b) {
  return inner_prod(a,b);
}

/** Compute the squared L2 norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<std::complex<T> >::type
norm_2_sq(const std::complex<T>& a) {
  return norm_2_sq(a.real()) + norm_2_sq(a.imag());
}
/** Compute the L2 norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<std::complex<T> >::type
norm_2(const std::complex<T>& a) {
 
  return std::sqrt(norm_2_sq(a));
}
/** Compute the L1 norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<std::complex<T> >::type
norm_1(const std::complex<T>& a) {
  return norm_1(a.real()) + norm_1(a.imag());
}
/** Compute the L-infinity norm of a complex */
template <typename T>
FMMTL_INLINE
typename norm_type<std::complex<T> >::type
norm_inf(const std::complex<T>& a) {
 
  return std::max(norm_inf(a.real()), norm_inf(a.imag()));
}

////////////////
/// VecX<N,T> ///
////////////////

/** Compute the inner product of two Vecs */
template <std::size_t N, typename T>
FMMTL_INLINE
typename norm_type<VecX<N,T> >::type
inner_prod(const VecX<N,T>& a, const VecX<N,T>& b) {
  typename norm_type<VecX<N,T> >::type v = inner_prod(a[0],b[0]);
  for (std::size_t i = 1; i != N; ++i)
    v += inner_prod(a[i],b[i]);
  return v;
}
template <std::size_t N, typename T>
FMMTL_INLINE
typename norm_type<VecX<N,T> >::type
dot(const VecX<N,T>& a, const VecX<N,T>& b) {
  return inner_prod(a,b);
}

/** Compute the squared L2 norm of a VecX */
template <std::size_t N, typename T>
FMMTL_INLINE
typename norm_type<VecX<N,T> >::type
norm_2_sq(const VecX<N,T>& a) {
  typename norm_type<VecX<N,T> >::type v = norm_2_sq(a[0]);
  for (std::size_t i = 1; i != N; ++i)
    v += norm_2_sq(a[i]);
  return v;
}
/** Compute the L2 norm of a VecX */
template <std::size_t N, typename T>
FMMTL_INLINE
typename norm_type<VecX<N,T> >::type
norm_2(const VecX<N,T>& a) {
 
  return std::sqrt(norm_2_sq(a));
}
/** Optimization of L2 norm for N==1 */
template <typename T>
FMMTL_INLINE
typename norm_type<VecX<1,T> >::type
norm_2(const VecX<1,T>& a) {
  return norm_2(a[0]);
}
/** Compute the L1 norm of a VecX */
template <std::size_t N, typename T>
FMMTL_INLINE
typename norm_type<VecX<N,T> >::type
norm_1(const VecX<N,T>& a) {
  typename norm_type<VecX<N,T> >::type v = norm_1(a[0]);
  for (std::size_t i = 1; i != N; ++i)
    v += norm_1(a[i]);
  return v;
}
/** Compute the L-infinity norm of a VecX */
template <std::size_t N, typename T>
FMMTL_INLINE
typename norm_type<VecX<N,T> >::type
norm_inf(const VecX<N,T>& a) {
 
  typename norm_type<VecX<N,T> >::type v = norm_inf(a[0]);
  for (std::size_t i = 1; i != N; ++i)
    v = std::max(v, norm_inf(a[i]));
  return v;
}
