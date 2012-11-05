#ifndef IGL_MOSEK_QUADPROG_H
#define IGL_MOSEK_QUADPROG_H
#include "../igl_inline.h"
#include <vector>
#include <map>
#include "mosek.h"


#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace igl
{
  struct MosekData
  {
    // Integer parameters
    std::map<MSKiparame,int> intparam;
    // Double parameters
    std::map<MSKdparame,double> douparam;
    // Default values
    MosekData();
  };
  // Solve a convex quadratic optimization problem with linear and constant
  // bounds, that is:
  //
  // Minimize: ½ * xT * Q⁰ * x + cT * x + cf
  //
  // Subject to: lc ≤ Ax ≤ uc
  //             lx ≤ x ≤ ux
  //
  // where we are trying to find the optimal vector of values x. 
  //
  // Note: Q⁰ must be symmetric and the ½ is a convention of MOSEK
  //
  // Note: Because of how MOSEK accepts different parts of the system, Q should
  // be stored in IJV (aka Coordinate) format and should only include entries in
  // the lower triangle. A should be stored in Column compressed (aka Harwell
  // Boeing) format. As described:
  // http://netlib.org/linalg/html_templates/node92.html
  // or
  // http://en.wikipedia.org/wiki/Sparse_matrix
  //   #Compressed_sparse_column_.28CSC_or_CCS.29
  // 
  //
  // Templates:
  //   Index  type for index variables
  //   Scalar  type for floating point variables (gets cast to double?)
  // Input:
  //   n  number of variables, i.e. size of x
  //   Qi  vector of qnnz row indices of non-zeros in LOWER TRIANGLE ONLY of Q⁰
  //   Qj  vector of qnnz column indices of non-zeros in LOWER TRIANGLE ONLY of 
  //     Q⁰
  //   Qv  vector of qnnz values of non-zeros in LOWER TRIANGLE ONLY of Q⁰, 
  //     such that:
  //     Q⁰(Qi[k],Qj[k]) = Qv[k] for k ∈ [0,Qnnz-1], where Qnnz is the number of
  //     non-zeros in Q⁰
  //   c   (optional) vector of n values of c, transpose of coefficient row vector
  //     of linear terms, EMPTY means c == 0
  //   cf  (optional) value of constant term in objective, 0 means cf == 0, so
  //     optional only in the sense that it is mandatory
  //   m  number of constraints, therefore also number of rows in linear
  //     constraint coefficient matrix A, and in linear constraint bound vectors 
  //     lc and uc
  //   Av  vector of non-zero values of A, in column compressed order
  //   Ari  vector of row indices corresponding to non-zero values of A,
  //   Acp  vector of indices into Ari and Av of the first entry for each column
  //     of A, size(Acp) = (# columns of A) + 1 = n + 1
  //   lc  vector of m linear constraint lower bounds
  //   uc  vector of m linear constraint upper bounds
  //   lx  vector of n constant lower bounds
  //   ux  vector of n constant upper bounds
  // Output:
  //   x  vector of size n to hold output of optimization
  // Return:
  //   true only if optimization was successful with no errors
  // 
  // Note: All indices are 0-based
  //
  template <typename Index, typename Scalar>
  IGL_INLINE bool mosek_quadprog(
    const Index n,
    /* mosek won't allow this to be const*/ std::vector<Index> & Qi,
    /* mosek won't allow this to be const*/ std::vector<Index> & Qj,
    /* mosek won't allow this to be const*/ std::vector<Scalar> & Qv,
    const std::vector<Scalar> & c,
    const Scalar cf,
    const Index m,
    /* mosek won't allow this to be const*/ std::vector<Scalar> & Av,
    /* mosek won't allow this to be const*/ std::vector<Index> & Ari,
    const std::vector<Index> & Acp,
    const std::vector<Scalar> & lc,
    const std::vector<Scalar> & uc,
    const std::vector<Scalar> & lx,
    const std::vector<Scalar> & ux,
    MosekData & mosek_data,
    std::vector<Scalar> & x);

  // Wrapper with Eigen elements
  //// Templates:
  ////   Scalar  Scalar type for sparse matrix  (e.g. double)
  ////   Derived  dervied type from matrix/vector (e.g. VectorXd)
  IGL_INLINE bool mosek_quadprog(
    const Eigen::SparseMatrix<double> & Q,
    const Eigen::VectorXd & c,
    const double cf,
    const Eigen::SparseMatrix<double> & A,
    const Eigen::VectorXd & lc,
    const Eigen::VectorXd & uc,
    const Eigen::VectorXd & lx,
    const Eigen::VectorXd & ux,
    MosekData & mosek_data,
    Eigen::VectorXd & x);
}

#ifdef IGL_HEADER_ONLY
#  include "mosek_quadprog.cpp"
#endif

#endif
