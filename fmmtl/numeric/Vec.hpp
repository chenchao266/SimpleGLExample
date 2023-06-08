#pragma once
/** @file VecX.hpp
 * @brief A small N-dimensional numerical vector type that works on CPU and GPU.
 *
 * TODO: Place in fmmtl namespace.
 */

#include <iostream>
#include <cmath>

#include "fmmtl/config.hpp"

#define for_i    for(std::size_t i = 0; i != N; ++i)
#define for_I(K) for(std::size_t i = K; i != N; ++i)

/** @class VecX
 * @brief Class representing ND points and vectors.
 */
template <unsigned N, typename T>
struct VecX {
  FMMTL_STATIC_ASSERT(N > 0, "VecX<N,T> needs N >= 1");

  T elem[N];
  enum {DEM = N};
  typedef T               value_type;
  typedef T&              reference;
  typedef const T&        const_reference;
  typedef T*              iterator;
  typedef const T*        const_iterator;
  typedef std::size_t     size_type;
  typedef std::ptrdiff_t  difference_type;

  // CONSTRUCTORS

  // TODO: require zero-initialization?
  FMMTL_INLINE VecX() {
    for_i elem[i] = T(0);
    //std::fill(this->begin(), this->end(), value_type());
  }
  // TODO: Force 0-initialization to get POD and trivial semantics?
  //FMMTL_INLINE VecX() = default;
  FMMTL_INLINE explicit VecX(const value_type& b) {
    for_i elem[i] = b;
    //std::fill(this->begin(), this->end(), b);
  }
  FMMTL_INLINE VecX(const value_type& b0,
                   const value_type& b1) {
    FMMTL_STATIC_ASSERT(N >= 2, "Too many arguments to VecX constructor");
    elem[0] = b0; elem[1] = b1;
    for_I(2) elem[i] = T(0);
    //std::fill(this->begin() + 2, this->end(), value_type());
  }
  FMMTL_INLINE VecX(const value_type& b0,
                   const value_type& b1,
                   const value_type& b2) {
    FMMTL_STATIC_ASSERT(N >= 3,  "Too many arguments to VecX constructor");
    elem[0] = b0; elem[1] = b1; elem[2] = b2;
    for_I(3) elem[i] = T(0);
    //std::fill(this->begin() + 3, this->end(), value_type());
  }
  FMMTL_INLINE VecX(const value_type& b0,
                   const value_type& b1,
                   const value_type& b2,
                   const value_type& b3) {
    FMMTL_STATIC_ASSERT(N >= 4,  "Too many arguments to VecX constructor");
    elem[0] = b0; elem[1] = b1; elem[2] = b2; elem[3] = b3;
    for_I(4) elem[i] = T(0);
    //std::fill(this->begin() + 4, this->end(), value_type());
  }
  template <typename U>
  FMMTL_INLINE explicit VecX(const VecX<N,U>& v) {
    for_i elem[i] = v[i];
    //std::copy(v.begin(), v.end(), this->begin());
  }
  /*
  template <typename Generator>
  FMMTL_INLINE explicit VecX(const Generator& gen) {
    for_i elem[i] = gen(i);
  }
  */
  template <typename U, typename OP>
  FMMTL_INLINE explicit VecX(const VecX<N,U>& v, OP op) {
    for_i elem[i] = op(v[i]);
  }
  template <typename U1, typename OP, typename U2>
  FMMTL_INLINE explicit VecX(const VecX<N,U1>& v1, const VecX<N,U2>& v2, OP op) {
    for_i elem[i] = op(v1[i], v2[i]);
  }
  template <typename U1, typename OP, typename U2>
  FMMTL_INLINE explicit VecX(const U1& v1, const VecX<N,U2>& v2, OP op) {
    for_i elem[i] = op(v1, v2[i]);
  }
  template <typename U1, typename OP, typename U2>
  FMMTL_INLINE explicit VecX(const VecX<N,U1>& v1, const U2& v2, OP op) {
    for_i elem[i] = op(v1[i], v2);
  }

  // COMPARATORS

  FMMTL_INLINE bool operator==(const VecX& b) const {
    for_i { if (!(elem[i] == b[i])) return false; }
    return true;
    //return std::equal(this->begin(), this->end(), b.begin());
  }
  FMMTL_INLINE bool operator!=(const VecX& b) const {
    return !(*this == b);
  }

  // MODIFIERS

  /** Add scalar @a b to this VecX */
  template <typename U>
  FMMTL_INLINE VecX& operator+=(const U& b) {
    for_i elem[i] += b;
    return *this;
  }
  /** Subtract scalar @a b from this VecX */
  template <typename U>
  FMMTL_INLINE VecX& operator-=(const U& b) {
    for_i elem[i] -= b;
    return *this;
  }
  /** Scale this VecX up by scalar @a b */
  template <typename U>
  FMMTL_INLINE VecX& operator*=(const U& b) {
    for_i elem[i] *= b;
    return *this;
  }
  /** Scale this VecX down by scalar @a b */
  template <typename U>
  FMMTL_INLINE VecX& operator/=(const U& b) {
    for_i elem[i] /= b;
    return *this;
  }
  /** Add VecX @a b to this VecX */
  template <typename U>
  FMMTL_INLINE VecX& operator+=(const VecX<N,U>& b) {
    for_i elem[i] += b[i];
    return *this;
  }
  /** Subtract VecX @a b from this VecX */
  template <typename U>
  FMMTL_INLINE VecX& operator-=(const VecX<N,U>& b) {
    for_i elem[i] -= b[i];
    return *this;
  }
  /** Scale this VecX up by factors in @a b */
  template <typename U>
  FMMTL_INLINE VecX& operator*=(const VecX<N,U>& b) {
    for_i elem[i] *= b[i];
    return *this;
  }
  /** Scale this VecX down by factors in @a b */
  template <typename U>
  FMMTL_INLINE VecX& operator/=(const VecX<N,U>& b) {
    for_i elem[i] /= b[i];
    return *this;
  }

  // ACCESSORS

  FMMTL_INLINE reference       operator[](size_type i)       { return elem[i]; }
  FMMTL_INLINE const_reference operator[](size_type i) const { return elem[i]; }

  FMMTL_INLINE T*       data()       { return elem; }
  FMMTL_INLINE const T* data() const { return elem; }

  FMMTL_INLINE reference       front()       { return elem[0]; }
  FMMTL_INLINE const_reference front() const { return elem[0]; }
  FMMTL_INLINE reference        back()       { return elem[N-1]; }
  FMMTL_INLINE const_reference  back() const { return elem[N-1]; }

  FMMTL_INLINE static size_type     size() { return N; }
  FMMTL_INLINE static size_type max_size() { return N; }
  FMMTL_INLINE static bool         empty() { return false; }

  // ITERATORS

  FMMTL_INLINE iterator        begin()       { return elem; }
  FMMTL_INLINE const_iterator  begin() const { return elem; }
  FMMTL_INLINE const_iterator cbegin() const { return elem; }

  FMMTL_INLINE iterator          end()       { return elem+N; }
  FMMTL_INLINE const_iterator    end() const { return elem+N; }
  FMMTL_INLINE const_iterator   cend() const { return elem+N; }
};

// OPERATORS

/** Write a VecX to an output stream */
template <unsigned N, typename T>
inline std::ostream& operator<<(std::ostream& s, const VecX<N,T>& a) {
  s << a[0];
  for (unsigned i = 1; i != a.size(); ++i) s << " " << a[i];
  return s;
}
/** Read a VecX from an input stream */
template <unsigned N, typename T>
inline std::istream& operator>>(std::istream& s, VecX<N,T>& a) {
  for_i s >> a[i];
  return s;
}

/** Compute cross product of two 3D Vecs */
template <typename T>
FMMTL_INLINE VecX<3,T> cross(const VecX<3,T>& a, const VecX<3,T>& b) {
  return VecX<3,T>(a[1]*b[2] - a[2]*b[1],
                  a[2]*b[0] - a[0]*b[2],
                  a[0]*b[1] - a[1]*b[0]);
}

// ARITHMETIC UNARY OPERATORS

/** Unary negation: Return -@a a */
template <unsigned N, typename T>
FMMTL_INLINE VecX<N,T> operator-(VecX<N,T> a) {
  for_i a[i] = -a[i];
  return a;
}
/** Unary plus: Return @a a. ("+a" should work if "-a" works.) */
template <unsigned N, typename T>
FMMTL_INLINE const VecX<N,T>& operator+(const VecX<N,T>& a) {
  return a;
}

// ARITHEMTIC BINARY OPERATORS

#if !defined(__CUDACC__)
/** This is not being compiled with CUDA, but with C++11 compatible compiler.
 * Type promotion with SFINAE selection can be accomplished with
 *   template <typename T, typename U>
 *   using prod_type = decltype(std::declval<T>() * std::declval<U>());
 * but we use a C++03 style for compatibility with the nvcc version below.
 */
#  define FMMTL_BINARY_PROMOTE_DECLARE(NAME, OP)                            \
  template <typename T, typename U,                                         \
            typename R = decltype(std::declval<T>() OP std::declval<U>())>  \
  struct NAME##_op {                                                        \
    typedef R type;                                                         \
    FMMTL_INLINE R operator()(const T& a, const U& b) const {               \
      return a OP b;                                                        \
    }                                                                       \
  }

#  define FMMTL_UNARY_PROMOTE_DECLARE(NAME, OP)                             \
  template <typename T,                                                     \
            typename R = decltype(OP(std::declval<T>()))>                   \
  struct NAME##_op {                                                        \
    typedef R type;                                                         \
    FMMTL_INLINE R operator()(const T& a) const {                           \
      return OP(a);                                                         \
    }                                                                       \
  }
#else
/** This is being compiled by CUDA, which does not have decltype.
 * Instead, a simple fix is to disallow type promotion and cross our fingers.
 * TODO: Improve.
 */
#  define FMMTL_BINARY_PROMOTE_DECLARE(NAME, OP)                            \
  template <typename T, typename U>                                         \
  struct NAME##_op {};                                                      \
  template <typename T>                                                     \
  struct NAME##_op<T,T> {                                                   \
    typedef T type;                                                         \
    FMMTL_INLINE T operator()(const T& a, const T& b) const {               \
      return a OP b;                                                        \
    }                                                                       \
  }

#  define FMMTL_UNARY_PROMOTE_DECLARE(NAME, OP)                             \
  template <typename T>                                                     \
  struct NAME##_op {                                                        \
    typedef T type;                                                         \
    FMMTL_INLINE T operator()(const T& a) const {                           \
      return OP(a);                                                         \
    }                                                                       \
  }
#endif

 

FMMTL_BINARY_PROMOTE_DECLARE(sum,+);
FMMTL_BINARY_PROMOTE_DECLARE(diff,-);
FMMTL_BINARY_PROMOTE_DECLARE(prod,*);
FMMTL_BINARY_PROMOTE_DECLARE(div,/);

 

// TODO: namespace VecX and all operators

template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename sum_op<T,U>::type>
operator+(const VecX<N,T>& a, const VecX<N,U>& b) {
  return VecX<N,typename sum_op<T,U>::type>(a, b, sum_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename sum_op<T,U>::type>
operator+(const VecX<N,T>& a, const U& b) {
  return VecX<N,typename sum_op<T,U>::type>(a, b, sum_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename sum_op<T,U>::type>
operator+(const T& a, const VecX<N,U>& b) {
  return VecX<N,typename sum_op<T,U>::type>(a, b, sum_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename diff_op<T,U>::type>
operator-(const VecX<N,T>& a, const VecX<N,U>& b) {
  return VecX<N,typename diff_op<T,U>::type>(a, b, diff_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename diff_op<T,U>::type>
operator-(const VecX<N,T>& a, const U& b) {
  return VecX<N,typename diff_op<T,U>::type>(a, b, diff_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename diff_op<T,U>::type>
operator-(const T& a, const VecX<N,U>& b) {
  return VecX<N,typename diff_op<T,U>::type>(a, b, diff_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename prod_op<T,U>::type>
operator*(const VecX<N,T>& a, const VecX<N,U>& b) {
  return VecX<N,typename prod_op<T,U>::type>(a, b, prod_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename prod_op<T,U>::type>
operator*(const VecX<N,T>& a, const U& b) {
  return VecX<N,typename prod_op<T,U>::type>(a, b, prod_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename prod_op<T,U>::type>
operator*(const T& a, const VecX<N,U>& b) {
  return VecX<N,typename prod_op<T,U>::type>(a, b, prod_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename div_op<T,U>::type>
operator/(const VecX<N,T>& a, const VecX<N,U>& b) {
  return VecX<N,typename div_op<T,U>::type>(a, b, div_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename div_op<T,U>::type>
operator/(const VecX<N,T>& a, const U& b) {
  return VecX<N,typename div_op<T,U>::type>(a, b, div_op<T,U>());
}
template <unsigned N, typename T, typename U>
FMMTL_INLINE VecX<N,typename div_op<T,U>::type>
operator/(const T& a, const VecX<N,U>& b) {
  return VecX<N,typename div_op<T,U>::type>(a, b, div_op<T,U>());
}

// ELEMENTWISE OPERATORS

 

FMMTL_UNARY_PROMOTE_DECLARE(abs, abs);
template <unsigned N, typename T>
FMMTL_INLINE VecX<N,typename abs_op<T>::type>
abs(const VecX<N,T>& a) {
  return VecX<N,typename abs_op<T>::type>(a, abs_op<T>());
}

FMMTL_UNARY_PROMOTE_DECLARE(sqrt, sqrt);
template <unsigned N, typename T>
FMMTL_INLINE VecX<N,typename sqrt_op<T>::type>
sqrt(const VecX<N,T>& a) {
  return VecX<N,typename sqrt_op<T>::type>(a, sqrt_op<T>());
}

FMMTL_UNARY_PROMOTE_DECLARE(conj, conj);
template <unsigned N, typename T>
FMMTL_INLINE VecX<N,typename conj_op<T>::type>
conj(const VecX<N,T>& a) {
  return VecX<N,typename conj_op<T>::type>(a, conj_op<T>());
}

FMMTL_UNARY_PROMOTE_DECLARE(real, real);
template <unsigned N, typename T>
FMMTL_INLINE VecX<N,typename real_op<T>::type>
real(const VecX<N,T>& a) {
  return VecX<N,typename real_op<T>::type>(a, real_op<T>());
}

FMMTL_UNARY_PROMOTE_DECLARE(imag, imag);
template <unsigned N, typename T>
FMMTL_INLINE VecX<N,typename imag_op<T>::type>
imag(const VecX<N,T>& a) {
  return VecX<N,typename imag_op<T>::type>(a, imag_op<T>());
}


 


// Compliance with std::
namespace std {

template <unsigned I, unsigned N, typename T>
typename VecX<N,T>::reference
get(VecX<N,T>& a) {
  FMMTL_STATIC_ASSERT(I < N, "I must be less than N.");
  return a[I];
}

template <unsigned I, unsigned N, typename T>
typename VecX<N,T>::const_reference
get(const VecX<N,T>& a) {
  FMMTL_STATIC_ASSERT(I < N, "I must be less than N.");
  return a[I];
}

template <unsigned N, typename T>
void
swap(VecX<N,T>& a, VecX<N,T>& b) {
  std::swap_ranges(a.begin(), a.end(), b.begin());
}

} // end namespace std


// META OPERATIONS

#include "fmmtl/meta/dimension.hpp"

 

//template <unsigned N, typename T>
//struct dimension<typename VecX<N,T> > {
//  const static std::size_t value = N;
//};

 


#undef for_i
#undef FMMTL_BINARY_PROMOTE_DECLARE
#undef FMMTL_UNARY_PROMOTE_DECLARE

#include "fmmtl/numeric/norm.hpp"
#include "fmmtl/numeric/random.hpp"
