#pragma once
#include <vector>
#include <set>
#include "geometry.h"
#include "SparseVector.h"
template<typename T>
class SparseMatrix
{
	T det(int i, std::vector<bool>& use) const;
public:

    SparseMatrix() 
    {
        numRows_ = 0;
        numCols_ = 0;
    }
    explicit SparseMatrix(int rows, int cols)
        :rows_(rows, SparseVector<T>(cols)), numRows_(rows), numCols_(cols)
    {
    }
 
	//use default op=, copy ctr, dtr

	T Determinant() const;
	SparseMatrix Transpose() const;
	SparseMatrix MultiplyTranspose() const;
	SparseMatrix FastMultiplyTranspose() const;

	int Rows() const { return numRows_; }
	int Cols() const { return numCols_; }

	SparseVector<T>& operator[](int index)
	{
		return rows_[index];
	}

	const SparseVector<T>& operator[](int index) const
	{
		return rows_[index];
	}
 
	SparseMatrix& operator += (const SparseMatrix& rhs);
	SparseMatrix& operator -= (const SparseMatrix& rhs);
	SparseMatrix& operator *= (const SparseMatrix& rhs);
	SparseMatrix& operator*=(double rhs);
	SparseMatrix& operator/=(double rhs);
	void Multiply(const vec<-1, T>& rhs, vec<-1, T>& result) const;
	bool operator==(const SparseMatrix& rhs) const;
	bool operator!=(const SparseMatrix& rhs) const;
	void MakeSparse();
	double Sparsity() const;
	void clear();
private:
	int numRows_;
	int numCols_;
	std::vector<SparseVector<T> > rows_;
};

  
template<typename T>
T SparseMatrix<T>::det(int i, std::vector<bool>& use) const
{
    if (i == numCols_)
        return 1;
    double sum = 0;
    int neg = 1;
    for (int k = 0; k < numCols_; k++)
    {
        if (!use[k])
            continue;
        use[k] = false;
        sum += neg * rows_[i][k] * det(i + 1, use);
        use[k] = true;
        neg *= -1;
    }
    return sum;
}

//this is really, really slow
template<typename T>
T SparseMatrix<T>::Determinant() const
{
    std::vector<bool> use(numCols_, true);
    return det(0, use);
}
template<typename T>
SparseMatrix<T> SparseMatrix<T>::Transpose() const
{
    typename SparseVector<T>::const_iterator iter;
    SparseMatrix<T> res(numCols_, numRows_);
    for (int i = 0; i < numRows_; i++)
    {
        iter.reset((*this)[i]);
        while (iter)
        {
            double value = *iter;
            if (value != 0.0)
            {
                int j = iter.getIndex();

                //all insertions occur at the back
                res[j].addback(SparseVector<T>::SparseNode(i, value));
            }
            iter++;
        }
    }
    return res;
}

//use the fact that A * Atrans is symmetric, and only involves row-row dot products
template<typename T>
SparseMatrix<T> SparseMatrix<T>::MultiplyTranspose() const
{
    SparseMatrix<T> ret(numRows_, numRows_);
    //first phase computes A[i][j] for j >= i
    //uses n * n/2 * m operations
    for (int i = 0; i < numRows_; i++)
    {
        for (int j = i; j < numRows_; j++)
        {
            double val = InnerProduct(rows_[i], rows_[j]);
            if (val != 0)
            {
                //we're always going to be inserting at the back
                ret.rows_[i].addback(SparseVector<T>::SparseNode(j, val));
            }
        }
    }
    //second phase sets A[j][i] = A[i][j], j >i
    //uses n * m operations
    for (int i = numRows_ - 1; i >= 0; i--)
    {
        SparseVector<T>::const_iterator iter(ret[i]);
        iter.seek(i + 1);
        while (iter)
        {
            double val = *iter;
            if (val != 0)
            {
                int j = iter.getIndex();
                //this works because we build the lower triangular part backwards
                //thus, ret[j][i] will always be the first value in ret[j]
                ret[j].addfront(SparseVector<T>::SparseNode(i, val));
            }
            iter++;
        }
    }
    return ret;
}

//use the fact that A * Atrans is symmetric, and only involves row-row dot products
template<typename T>
SparseMatrix<T> SparseMatrix<T>::FastMultiplyTranspose() const
{
    SparseMatrix<T> ret(numRows_, numRows_);
    std::vector<std::list<int> > coverage(numCols_);
    for (int i = 0; i < numRows_; i++)
    {
        typename SparseVector<T>::const_iterator it(rows_[i]);
        while (it)
        {
            coverage[it.getIndex()].push_back(i);
            it++;
        }
    }
    std::set<std::pair<int, int> > computed;
    //do pairwise collisions

    //first phase computes A[i][j] for j >= i
    //uses n * m * m operations(assuming n * m non-sparse elements of ret)
    for (unsigned int i = 0; i < coverage.size(); i++)
    {
        for (std::list<int>::iterator it = coverage[i].begin(); it != coverage[i].end(); ++it)
        {
            for (std::list<int>::iterator it2 = it; it2 != coverage[i].end(); ++it2)
            {
                if (computed.find(std::make_pair(*it, *it2)) == computed.end())
                {
                    double product = (rows_[*it]).InnerProduct(rows_[*it2]);
                    if (product != 0)
                    {
                        ret[*it][*it2] = product;
                    }

                    computed.insert(std::make_pair(*it, *it2));
                }
            }
        }
    }

    //second phase sets A[j][i] = A[i][j], j >i
    //uses n * m operations
    for (int i = numRows_ - 1; i >= 0; i--)
    {
        typename SparseVector<T>::const_iterator iter(ret[i]);
        iter.seek(i + 1);
        while (iter)
        {
            double val = *iter;
            if (val != 0)
            {
                int j = iter.getIndex();
                //this works because we build the lower triangular part backwards
                //thus, ret[j][i] will always be the first value in ret[j]
                ret[j].addfront(SparseVector<T>::SparseNode(i, val));
            }
            iter++;
        }
    }
    return ret;
}
template<typename T>
SparseMatrix<T>& SparseMatrix<T>::operator += (const SparseMatrix<T>& rhs)
{
    for (int i = 0; i < numRows_; i++)
    {
        rows_[i] += rhs.rows_[i];
    }
    return *this;
}
template<typename T>
SparseMatrix<T>& SparseMatrix<T>::operator -= (const SparseMatrix<T>& rhs)
{
    for (int i = 0; i < numRows_; i++)
    {
        rows_[i] -= rhs.rows_[i];
    }
    return *this;
}
template<typename T>
SparseMatrix<T>& SparseMatrix<T>::operator *= (const SparseMatrix<T>& rhs)
{
    SparseMatrix<T> temp(*this);
    SparseMatrix<T> rhsTrans = rhs.Transpose();
    rows_ = std::vector<SparseVector<T> >(numRows_, SparseVector<T>(rhs.numCols_));
    numCols_ = rhs.numCols_;
    for (int i = 0; i < numRows_; i++)
    {
        for (int j = 0; j < rhs.numCols_; j++)
        {
            double sum = InnerProduct(temp[i], rhsTrans[j]);
            if (sum != 0)
                rows_[i][j] = sum;
        }
    }
    return *this;
}
template<typename T>
SparseMatrix<T>& SparseMatrix<T>::operator*=(double rhs)
{
    for (int i = 0; i < numRows_; i++)
    {
        rows_[i] *= rhs;
    }
    return *this;
}
template<typename T>
SparseMatrix<T>& SparseMatrix<T>::operator/=(double rhs)
{
    for (int i = 0; i < numRows_; i++)
    {
        rows_[i] /= rhs;
    }
    return *this;
}
template<typename T>
bool SparseMatrix<T>::operator==(const SparseMatrix<T>& rhs) const
{
    for (int i = 0; i < numRows_; i++)
    {
        if (rows_[i] != rhs.rows_[i])
            return false;
    }
    return true;
}
template<typename T>
bool SparseMatrix<T>::operator!=(const SparseMatrix<T>& rhs) const
{
    return !(operator==(rhs));
}
template<typename T>
void SparseMatrix<T>::MakeSparse()
{
    for (unsigned int i = 0; i < rows_.size(); i++)
    {
        rows_[i].makeSparse();
    }
}
template<typename T>
double SparseMatrix<T>::Sparsity() const
{
    double total = 0.0;
    for (unsigned int i = 0; i < rows_.size(); i++)
    {
        total += rows_[i].sparsity();
    }
    return total / numRows_;
}
template<typename T>
void SparseMatrix<T>::clear()
{
    for (std::vector<SparseVector<T> >::iterator i = rows_.begin(); i != rows_.end(); ++i)
        i->clear();
}
template<typename T>
SparseMatrix<T> operator+(const SparseMatrix<T>& a, const SparseMatrix<T>& b)
{
    return SparseMatrix<T>(a) += b;
}

template<typename T>
SparseMatrix<T> operator-(const SparseMatrix<T>& a, const SparseMatrix<T>& b)
{
    return SparseMatrix<T>(a) -= b;
}

template<typename T>
SparseMatrix<T> operator*(const SparseMatrix<T>& a, const SparseMatrix<T>& b)
{
    return SparseMatrix<T>(a) *= b;
}

template<typename T>
SparseMatrix<T> operator*(const SparseMatrix<T>& a, double b)
{
    return SparseMatrix<T>(a) *= b;
}

template<typename T>
SparseMatrix<T> operator*(double a, const SparseMatrix<T>& b)
{
    return SparseMatrix<T>(b) *= a;
}

template<typename T>
SparseVector<T> operator*(const SparseMatrix<T>& a, const SparseVector<T>& b)
{
    SparseVector<T> result(a.Rows());
    for (int i = 0; i < a.numRows_; i++)
    {
        double product = InnerProduct(a[i], b);
        if (product != 0)
        {
            //always add to back
            result.nodes_.push_back(SparseVector<T>::SparseNode(i, product));
        }
    }
    return result;
}

template<typename T>
vec<-1, T> operator*(const SparseMatrix<T>& a, const vec<-1, T>& b)
{
    vec<-1, T> result(a.Rows());
    a.Multiply(b, result);
    return result;
}
template<typename T>
void SparseMatrix<T>::Multiply(const vec<-1, T>& rhs, vec<-1, T>& result) const
{
    if (rhs.length() != result.length()) throw new WrongDimensionException();
    for (int i = 0; i < numRows_; i++)
    {
        result[i] = InnerProduct(rows_[i], rhs);
    }
}

template<typename T>
SparseMatrix<T> operator/(const SparseMatrix<T>& a, double b)
{
    return SparseMatrix<T>(a) /= b;
}

template<typename T>
void ConjugateGradient(const SparseMatrix<T>& A, SparseVector<T>& x, const SparseVector<T>& b, double tolerance, double maxIter)
{

    int k = 0;
    SparseVector<T> r(b - A * x);
    SparseVector<T> p(r.length()), w(r.length());

    double rho1 = InnerProduct(r, r);
    double rho2 = rho1;
    double bScale = std::sqrt(InnerProduct(b, b));
    while ((std::sqrt(rho2) > (bScale * tolerance)) && (k < maxIter))
    {
        ++k;
        if (k == 1)
            p = r;
        else
        {
            double beta = rho2 / rho1;
            p *= beta;
            p += r;
        }
        w = A * p;
        double alpha = rho2 / InnerProduct(p, w);
        x += alpha * p;
        r -= alpha * w;
        rho1 = rho2;
        rho2 = InnerProduct(r, r);
    }
}

template<typename T>
void ConjugateGradient(const SparseMatrix<T>& A, vec<-1, T>& x, const vec<-1, T>& b, double tolerance, double maxIter)
{
    int k = 0;
    vec<-1, T> r(b - A * x);
    vec<-1, T> p(r.length()), w(r.length());

    double rho1 = r.norm2();
    double rho2 = rho1;
    double bScale = b.norm();
    double errBound = bScale * tolerance;
    while ((std::sqrt(rho2) > errBound) && (k < maxIter))
    {
        ++k;
        if (k == 1)
        {
            p = r;
        }
        else
        {
            double beta = rho2 / rho1;
            p *= beta;
            p += r;
        }
        w = A * p;

        double alpha = rho2 / InnerProduct(p, w);
        x += alpha * p;
        r -= alpha * w;
        rho1 = rho2;
        rho2 = r.norm2();
    }
}

template<typename T>
void JacobiIteration(const SparseMatrix<T>& A, vec<-1, T>& x, const vec<-1, T>& b, int maxIter)
{
    for (int k = 0; k < maxIter; k++)
    {
        vec<-1, T> xNext(x);
        for (int i = 0; i < A.Rows(); i++)
        {
            double divisor = A[i][i];
            xNext[i] = (b[i] - InnerProduct(A[i], x) + divisor * x[i]) / divisor;
        }
        x = xNext;
        vec<-1, T> estimate = A * x;
        estimate -= b;
        double error = InnerProduct(estimate, estimate);
    }
}

template<typename T>
void GaussSeidelIteration(const SparseMatrix<T>& A, vec<-1, T>& x, const vec<-1, T>& b, int maxIter)
{
    for (int k = 0; k < maxIter; k++)
    {
        for (int i = 0; i < A.Rows(); i++)
        {
            double divisor = A[i][i];
            x[i] = (b[i] - InnerProduct(A[i], x) + divisor * x[i]) / divisor;
        }
        vec<-1, T> estimate = A * x;
        estimate -= b;
        double error = InnerProduct(estimate, estimate);
    }
}

typedef SparseMatrix<float> SparseMatrixf;
typedef SparseMatrix<double> SparseMatrixd;