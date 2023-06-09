﻿#ifndef FMMTL_KERNELSKELETON_KERN
#define FMMTL_KERNELSKELETON_KERN
/** @file KernelSkeleton
 * @brief An example Kernel and Expansion implementation that explains the
 * required and optional methods and types in the Kernel and Expansion concepts.
 *
 * These classes will be used to evaluate the matrix-vector product
 * r_i = sum_j K(t_i, s_j) c_j
 * for all i where K is the Kernel defined by this class.
 *
 * MORE on Tree Operations.
 */

#include "fmmtl/Kernel.hpp"

// Use a library-defined Vector class that supports multiple architectures
#include "fmmtl/numeric/Vec.hpp"


/** @struct KernelSkeleton
 * @brief A short description of an implemented Kernel.
 *
 * A Kernel is a function:
 * K : (target_type, source_type) -> kernel_value_type
 * The types in the matrix-vector product are
 * result_type = kernel_value_type * charge_type
 *
 * The Kernel class need only define the operator() to compute the kernel,
 * and define the above types in the Kernel computation. (TODO: Not necessary)
 * However, some optimizations may be possible by implementing optional methods
 * detailed below.
 */
struct KernelSkeleton
    //: public Kernel<KernelSkeleton>   // This class is an FMMTL Kernel
{
  //! Return type of a kernel evaluation
  typedef double kernel_value_type;

  //! Source type
  typedef VecX<3,double> source_type;
  //! Charge type associated with each source
  //! The primitive type of the vector in the matvec
  // TODO: Accept anything algebraically compatable with kernel_value_type?
  typedef double charge_type;

  //! Target type
  typedef VecX<3,double> target_type;
  //! Result type associated with each target
  //! The product of the kernel_value_type and the charge_type
  // TODO: Infer from std::result_of<charge_type * kernel_value_type>::type?
  typedef double result_type;

  /** Kernel evaluation
   * K(t,s) where t is the target
   *          and s is the source.
   *
   * @param[in] t,s  The target and source to evaluate the kernel
   * @return         The Kernel value, K(t,s)
   */
  FMMTL_INLINE
  kernel_value_type operator()(const target_type& t,
                               const source_type& s) const {
    (void) t;
    (void) s;
    return kernel_value_type(0);
  }

#if 0
  /*******************************************************************/
  /************************* Optional ********************************/
  /*******************************************************************/

  /** Optional Kernel value source and target transposition
   * K(t,s) -> K(s,t)
   * Often, a kernel has a symmetry in s and t that can be computed faster than
   * by calling the evaluation operator. If this function is implemented, the
   * computation may use it to prevent uneccessary calls to the evaluation
   * operator and accelerate the S2T interactions.
   *
   * @param[in] kts  A kernel value that was returned from operator()(t,s)
   * @returns        The value of operator()(s,t)
   * @note           If this function is implemented, then it must
   *                 be the case that std::is_same<source_type,target_type>
   */
  FMMTL_INLINE
  kernel_value_type transpose(const kernel_value_type& kts) const {
    return kts;
  }

  /** Optional Kernel vectorized asymmetric S2T operation
   * r_i += sum_j K(t_i,s_j) * c_j
   *
   * @param[in] s_first,s_last  Iterator range to the sources
   * @param[in] c_first         Iterator to charges corresponding to sources
   * @param[in] t_first,t_last  Iterator range to the targets
   * @param[in] r_first         Iterator to result accumulators
   *                             corresponding to targets
   */
  template <typename SourceIter, typename ChargeIter,
            typename TargetIter, typename ResultIter>
  void S2T(SourceIter s_first, SourceIter s_last, ChargeIter c_first,
           TargetIter t_first, TargetIter t_last, ResultIter r_first) const {
    (void) s_first;
    (void) s_last;
    (void) c_first;
    (void) t_first;
    (void) t_last;
    (void) r_first;
  }

  /** Optional Kernel vectorized symmetric S2T operation
   * This can occur when s_i == t_i for all i.
   * rt_i += sum_j K(t_i, s_j) * cs_j
   * rs_j += sum_i K(s_j, t_i) * ct_i
   *
   * @param[in] s_first,s_last  Iterator range to the sources
   * @param[in] cs_first        Iterator to charges corresponding to sources
   * @param[in] t_first,t_last  Iterator range to the targets
   * @param[in] ct_first        Iterator to charges corresponding to targets
   * @param[in] rt_first        Iterator to result accumulators
   *                             corresponding to targets
   * @param[in] rs_first        Iterator to result accumulators
   *                             corresponding to sources
   */
  template <typename SourceIter, typename ChargeIter,
            typename TargetIter, typename ResultIter>
  void S2T(SourceIter s_first, SourceIter s_last, ChargeIter cs_first,
           TargetIter t_first, TargetIter t_last, ChargeIter ct_first,
           ResultIter rt_first, ResultIter rs_first) const {
    (void) s_first;
    (void) s_last;
    (void) cs_first;
    (void) t_first;
    (void) t_last;
    (void) ct_first;
    (void) rt_first;
    (void) rs_first;
  }
#endif  // By default, exclude optional methods
};

/** TODO: Remove this line using the Kernel<Kernel> inheritance? */
FMMTL_KERNEL_EXTRAS(KernelSkeleton);



/** Kernel Expansion Example
 *
 * A kernel expansion provides the primitive operations needed to create
 * fast methods for kernel-matrix operations.
 *
 * This class should usually be defined in its own ExpansionName.hpp file.
 * If defined in the .kern file, it should use C++03 to allow for
 * cross-compilation with older compilers.
 */
#include "fmmtl/Expansion.hpp"

/** @struct ExpansionSkeleton
 * @brief A set of methods that can be used in the KernelMatrix and extend
 * the functionality of a Kernel with
 *
 * The source_type and target_type must be castable to a point_type. FMMs and
 * Treecodes use spacial trees to partition the sources and targets so they must
 * define a dimension and point_type.
 *
 * multipole_type and local_type with initialization methods INITM and INITL
 * S2M operator
 * S2L operator (Adaptive FMM)
 * M2M operator
 * M2L operator (FMM)
 * M2T operator (Treecode)
 * L2L operator (FMM)
 * L2T operator (FMM)
 */
struct ExpansionSkeleton
    : public Expansion<KernelSkeleton, ExpansionSkeleton>
{
  /** @brief Spacial type to build the tree.
   * Restricted to fmmtl/numeric/Vec.hpp for now.
   * @note source_type and target_type must be convertible to point_type
   *       or S2P and T2P should be defined. These provide a spacial
   *       interpritation of the sources and targets for clustering.
   * @note Also defines the spacial dimension of the problem: [1-4] available.
   * TODO: Generalize? Use any type with op[]/size() or begin()/end()? */
  typedef Vec<3,double> point_type;

  //! Multipole expansion type
  typedef std::vector<double> multipole_type;
  //! Local expansion type
  typedef std::vector<double> local_type;

  //! Constructor
  ExpansionSkeleton() : Expansion() {}

  /** Initialize a multipole expansion to algebraic zero
   * using the size and level number of the box containing it
   * (Optional: If not implemented, default constructor for multipole_type used)
   *
   * @param[in] M        The multipole to be initialized
   * @param[in] extents  The dimensions of the box containing the expansion
   * @param[in] level    The level number of the box. 0: Root box
   */
  void init_multipole(multipole_type& M,
                      const point_type& extents, unsigned level) const {
    (void) M;
    (void) extents;
    (void) level;
  }
  /** Initialize a local expansion to algebraic zero
   * using the size and level number of the box containing it
   * (Optional: If not implemented, default constructor for local_type used)
   *
   * @param[in] L        The local expansion to be initialized
   * @param[in] extents  The dimensions of the box containing the expansion
   * @param[in] level    The level number of the box. 0: Root box
   */
  void init_local(local_type& L,
                  const point_type& extents, unsigned level) const {
    (void) L;
    (void) extents;
    (void) level;
  }

  /***************************************/
  /******** Multipole Operations *********/
  /***************************************/

  /** @note Not all of these operations are required. Implementing more of them
   * may allow the execution to choose more optimal paths.
   *
   * The operations form a graph:
   *      __                __
   * M2M /  \      M2L     /  \ L2L
   *     \->M------>------>L<-/
   *        ^\__        __/|
   *        |   \__  __/   |
   *        |      \/      v
   *    S2M ^   ___/\___   | L2T
   *        | _/S2L  M2T\_ |
   *        |/            \v
   *        S (src)        T (trg)
   *
   * and there must exist a path from S (src) to T (trg) in this graph.
   * Some paths may be more efficient than others,
   * both asymptotically and actually.
   */

  /** Kernel S2M operation
   * M += c * Op(s) where M is the multipole expansion,
   *                      s is the source,
   *                  and c is the charge.
   *
   * @param[in] source  The source to accumulate into the multipole expansion
   * @param[in] charge  The source's corresponding charge
   * @param[in] center  The center of the box containing the multipole expansion
   * @param[in,out] M   The multipole expansion to accumulate into
   */
  void S2M(const source_type& source, const charge_type& charge,
           const point_type& center, multipole_type& M) const {
    (void) source;
    (void) charge;
    (void) center;
    (void) M;
  }

  /** Kernel S2L operation
   * L += c * Op(s) where L is the local expansion,
   *                      s is the source,
   *                  and c is the charge.
   *
   * @param[in] source  The source to accumulate into the local expansion
   * @param[in] charge  The source's corresponding charge
   * @param[in] center  The center of the box containing the local expansion
   * @param[in,out] L   The local expansion to accumulate into
   */
  void S2L(const source_type& source, const charge_type& charge,
           const point_type& center, local_type& L) const {
    (void) source;
    (void) charge;
    (void) center;
    (void) L;
  }

  /** Kernel M2M operation
   * M_t += Op(M_s) where M_t is the target
   *                  and M_s is the source.
   *
   * @param[in]     source       The multipole source at the child level
   * @param[in,out] target       The multipole target to accumulate into
   * @param[in]     translation  The vector from source to target
   * @pre @a source includes the influence of all sources within its box
   */
  void M2M(const multipole_type& source,
                 multipole_type& target,
           const point_type& translation) const {
    (void) source;
    (void) target;
    (void) translation;
  }

  /** Kernel M2T operation
   * r += Op(M, t) where M is the multipole expansion,
   *                     t is the target,
   *                 and r is the result.
   *
   * @param[in]     M       The multpole expansion
   * @param[in]     center  The center of the box with the multipole expansion
   * @param[in]     target  The target at which to evaluate the multipole
   * @param[in,out] result  The corresponding result value to accumulate into
   * @pre @a M includes the influence of all sources within its box
   */
  void M2T(const multipole_type& M, const point_type& center,
           const target_type& target, result_type& result) const {
    (void) M;
    (void) center;
    (void) target;
    (void) result;
  }

  /** Kernel M2L operation
   * L += Op(M) where L is the local expansion
   *              and M is the multipole expansion
   *
   * @param[in]     source       The multpole expansion source
   * @param[in,out] target       The local expansion target to accumulate into
   * @param[in]     translation  The vector from source to target
   * @pre @a translation obeys the multipole-acceptance criteria
   * @pre @a source includes the influence of all sources within its box
   */
  void M2L(const multipole_type& source,
                 local_type& target,
           const point_type& translation) const {
    (void) source;
    (void) target;
    (void) translation;
  }

  /** Kernel L2L operation
   * L_t += Op(L_s) where L_t is the target
   *                  and L_s is the source.
   *
   * @param[in]     source       The local expansion source at the parent level
   * @param[in,out] target       The local expansion target to accumulate into
   * @param[in]     translation  The vector from source to target
   * @pre @a source includes the influence of all sources outside its box
   */
  void L2L(const local_type& source,
                 local_type& target,
           const point_type& translation) const {
    (void) source;
    (void) target;
    (void) translation;
  }

  /** Kernel L2T operation
   * r += Op(L, t) where L is the local expansion,
   *                     t is the target,
   *                 and r is the result.
   *
   * @param[in] L       The local expansion
   * @param[in] center  The center of the box with the local expansion
   * @param[in] target  The target of this L2T operation
   * @param[in] result  The corresponding result value to accumulate into
   * @pre @a L includes the influence of all sources outside its box
   */
  void L2T(const local_type& L, const point_type& center,
           const target_type& target, result_type& result) const {
    (void) L;
    (void) center;
    (void) target;
    (void) result;
  }


#if 0
  /*******************************************************************/
  /************************* Optional ********************************/
  /*******************************************************************/

  /** Optional source-to-point transformation.
   * If the source_type is not convertible to point_type or some other spatial
   * interpretation of the sources is desired, implement this method to transform
   * a source to a point appropriately.
   *
   * @param[in] source  The source data to be transformed
   * @returns A point_type representing the spacial position of the source data
   */
  point_type S2P(const source_type& source) const {
    (void) source;
    return point_type();
  }

  /** Optional target-to-point transformation.
   * If the target_type is not convertible to point_type or some other spatial
   * interpretation of the targets is desired, implement this method to transform
   * a target to a point appropriately.
   *
   * @param[in] target  The target data to be transformed
   * @returns A point_type representing the spacial position of the target data
   */
  point_type T2P(const target_type& target) const {
    (void) target;
    return point_type();
  }

  /** Optional Dynamic Multipole Acceptance Criteria
   * Some MACs depend on the data other than box descriptors. Extra data
   * regarding source or target distributions may be stored in multipole
   * and local expansions and used by this function as an interaction criteria.
   * WARNING: Implementing this function may prevent some optimizations.
   *
   * @param[in]  M  The multipole of the source box being considered
   * @param[in]  L  The local expansion of the target box being considered
   * @pre @a M includes the influence of all sources within its box
   * @pre @a L has been initialized with init_local or with default initialization
   */
  bool MAC(const multipole_type& M,
           const local_type& L) {
    (void) M;
    (void) L;
    return true;
  }

  /** Optional Kernel vectorized S2M operation
   * M = sum_j Op(s_j) * c_j where M is the multipole,
   *                               s_j are the sources,
   *                           and c_j are the charges.
   *
   * @param[in]     s_first,s_last  Iterator range to the sources
   * @param[in]     c_first         Iterator to charges corresponding to sources
   * @param[in]     center          The center of the box containing M
   * @param[in,out] M               The multipole expansion to accumulate into
   */
  template <typename SourceIter, typename ChargeIter>
  void S2M(SourceIter s_first, SourceIter s_last, ChargeIter c_first,
           const point_type& center, multipole_type& M) const {
    (void) s_first;
    (void) s_last;
    (void) c_first;
    (void) center;
    (void) M;
  }

  /** Optional Kernel vectorized S2L operation
   * L = sum_j Op(s_j) * c_j where L is the local expansion,
   *                               s_j are the sources,
   *                           and c_j are the charges.
   *
   * @param[in]     s_first,s_last  Iterator range to the sources
   * @param[in]     c_first         Iterator to charges corresponding to sources
   * @param[in]     center          The center of the box containing L
   * @param[in,out] L               The local expansion to accumulate into
   */
  template <typename SourceIter, typename ChargeIter>
  void S2L(SourceIter s_first, SourceIter s_last, ChargeIter c_first,
           const point_type& center, local_type& L) const {
    (void) s_first;
    (void) s_last;
    (void) c_first;
    (void) center;
    (void) L;
  }

  /** Optional Kernel vectorized M2T operation
   * r_i += Op(t_i, M) where M is the multipole
   *                         t_i are the targets
   *                     and r_i are the results.
   *
   * @param[in] M               The multpole expansion
   * @param[in] center          The center of the multipole's box
   * @param[in] t_first,t_last  Iterator range to the targets
   * @param[in] r_first         Iterator to the result accumulator
   * @pre M includes the influence of all sources within its box
   */
  template <typename TargetIter, typename ResultIter>
  void M2T(const multipole_type& M, const point_type& center,
           TargetIter t_first, TargetIter t_last,
           ResultIter r_first) const {
    (void) M;
    (void) center;
    (void) t_first;
    (void) t_last;
    (void) r_first;
  }

  /** Optional Kernel vectorized L2T operation
   * r_i += Op(t_i, L) where L is the local expansion
   *                         t_i are the targets
   *                     and r_i are the results.
   *
   * @param[in] L               The local expansion
   * @param[in] center          The center of the box with the local expansion
   * @param[in] t_first,t_last  Iterator range to the targets
   * @param[in] r_first         Iterator to the result accumulator
   * @pre @a L includes the influence of all sources outside its box
   */
  template <typename TargetIter, typename ResultIter>
  void L2T(const local_type& L, const point_type& center,
           TargetIter t_first, TargetIter t_last,
           ResultIter r_first) const {
    (void) L;
    (void) center;
    (void) t_first;
    (void) t_last;
    (void) r_first;
  }
#endif  // By default, exclude optional methods
};


// For simple examples, see UnitKernel.kern and ExpKernel.kern

#endif
