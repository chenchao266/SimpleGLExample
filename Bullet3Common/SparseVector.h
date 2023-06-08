#pragma once
#include <list>

 
#include <algorithm>
#include <utility>
#include <iterator>
#include "geometry.h"


template<typename T>
class SparseVector
{
 
public:
//private:
 
	struct SparseNode
	{
		SparseNode(int i, T v)
		{
			index = i;
			value = v;
		}
		int index;
		T value;
		struct multiply
		{
			T value;
			multiply(T v) { value = v; }
			SparseNode operator()(const SparseNode& node) const
			{
				return SparseNode(node.index, node.value * value);
			}
		};
		struct divide
		{
            T value;
			divide(T v) { value = v; }
			SparseNode operator()(const SparseNode& node) const
			{
				return SparseNode(node.index, node.value / value);
			}
		};
		struct negate
		{
			SparseNode operator()(const SparseNode& node) const
			{
				return SparseNode(node.index, -node.value);
			}
		};
	};
	struct NodeIsZero
	{
		bool operator()(const SparseNode& node)
		{
			return node.value == 0;
		}
	};
	struct IndexGreaterThan
	{
		int n;
		IndexGreaterThan(int i)
		{
			n = i;
		}
		bool operator()(const SparseNode& node)
		{
			return node.index > n;
		}
	};
public:
	SparseVector()
		:length_(0)
	{
	}

	explicit SparseVector(int length)
		:length_(length)
	{
	}
	//use default op=, copy ctr, and dtr

	int length() const;
    T& operator[](int index);
    T operator[](int index) const;
	bool operator==(const SparseVector& rhs) const;
	bool operator != (const SparseVector& rhs) const;
    T at(int index) const;
	SparseVector operator *(const SparseVector& rhs) const;
	SparseVector operator /(const SparseVector& rhs) const;
 
	SparseVector& operator+=(const SparseVector& rhs);
	SparseVector& operator-=(const SparseVector& rhs);
	SparseVector& operator*=(T rhs);
	SparseVector& operator/=(T rhs);

	//this zeroes out this vector
	void clear()
    {
        nodes_.clear();
    }

	void makeSparse();
    T sparsity() const
    {
        return T(nodes_.size()) / T(length_);
    }
	void resize(int size);
    void addback(SparseNode node)
    {
        nodes_.push_back(node);
    }
    void addfront(SparseNode node)
    {
        nodes_.push_front(node);
    }
 
    T InnerProduct(const SparseVector& rhs) const;

	class iterator {
	public:
		iterator() {}
		iterator(SparseVector& toIter) { target = &toIter.nodes_; iter = target->begin(); }
		~iterator() {}

        T&		getVal() { return iter->value; }
		int			getIndex() { return iter->index; }

		operator bool() const { return (iter != target->end()); }
		bool		valid() const { return (iter != target->end()); }

		void		reset() { iter = target->begin(); }
		void		reset(SparseVector& toIter) { target = &toIter.nodes_; iter = target->begin(); }

		bool		seek(int index) { while (valid() && getIndex() < index) { (*this)++; } return (valid() && getIndex() == index); }

        T&		operator *	() { return iter->value; }
		iterator	operator ++	(int) { iterator i = *this; iter++; return i; }
		iterator&	operator ++	() { iter++; return *this; }

	private:
        typename std::list<SparseNode>::iterator	iter;
		std::list<SparseNode>* target;
	};

	class const_iterator {
	public:
		const_iterator() {}
		const_iterator(const SparseVector& toIter) { target = &toIter.nodes_; iter = target->begin(); }
		~const_iterator() {}

        T		getVal() const { return iter->value; }
		int			getIndex() const { return iter->index; }

		operator bool() const { return (iter != target->end()); }
		bool		valid() const { return (iter != target->end()); }

		void		reset() { iter = target->begin(); }
		void		reset(const SparseVector& toIter) { target = &toIter.nodes_; iter = target->begin(); }

		bool		seek(int index) { while (valid() && getIndex() < index) { (*this)++; } return (valid() && getIndex() == index); }

        T			operator *	() { return iter->value; }
		const_iterator	operator ++	(int) { const_iterator i = *this; iter++; return i; }
		const_iterator&	operator ++	() { iter++; return *this; }

	private:
        typename std::list<SparseNode>::const_iterator	iter;
		const std::list<SparseNode>*			target;
	};
private:
	int length_;
	std::list<SparseNode> nodes_;
};


template<typename T>
int SparseVector<T>::length() const { return length_; }
template<typename T>
T& SparseVector<T>::operator[](int index)
{
    std::list<SparseNode>::iterator it = nodes_.begin();
    while (it != nodes_.end() && it->index < index)
    {
        it++;
    }
    if (it == nodes_.end() || it->index != index)
    {
        it = nodes_.insert(it, SparseNode(index, 0));
        return it->value;
    }

    return it->value;
}
template<typename T>
T SparseVector<T>::operator[](int index) const
{
    std::list<SparseNode>::const_iterator it = nodes_.begin();
    while (it != nodes_.end() && it->index < index)
    {
        it++;
    }
    if (it == nodes_.end() || it->index != index)
        return 0;

    return it->value;

}

//todo - make this faster
template<typename T>
bool SparseVector<T>::operator==(const SparseVector<T>& rhs) const
{
    for (int i = 0; i < rhs.length_; i++)
    {
        if (rhs[i] != (*this)[i])
            return false;
    }
    return true;
}
template<typename T>
bool SparseVector<T>::operator != (const SparseVector<T>& rhs) const
{
    return !((*this) == rhs);
}

template<typename T>
T SparseVector<T>::at(int index) const
{
    std::list<SparseNode>::const_iterator it = nodes_.begin();
    while (it != nodes_.end() && it->index < index)
    {
        it++;
    }
    if (it == nodes_.end() || it->index != index)
        return 0;

    return it->value;
}
template<typename T>
SparseVector<T> SparseVector<T>::operator *(const SparseVector<T>& rhs) const
{
    // note: assuming two vectors have same length
    SparseVector<T> product(length_);

    std::list<SparseVector<T>::SparseNode>::const_iterator ita = nodes_.begin();
    std::list<SparseVector<T>::SparseNode>::const_iterator itb = rhs.nodes_.begin();

    while (ita != nodes_.end() && itb != rhs.nodes_.end())
    {
        if (itb->index == ita->index)
        {
            product[itb->index] = ita->value * itb->value;
            ++ita;
            ++itb;
        }
        else if (itb->index < ita->index)
        {
            ++itb;
        }
        else
        {
            ++ita;
        }
    }
    return product;
}
template<typename T>
SparseVector<T> SparseVector<T>::operator /(const SparseVector<T>& rhs) const
{
    // note: assuming two vectors have same length
    SparseVector quotient(length_);

    std::list<SparseVector<T>::SparseNode>::const_iterator ita = nodes_.begin();
    std::list<SparseVector<T>::SparseNode>::const_iterator itb = rhs.nodes_.begin();

    while (ita != nodes_.end() && itb != rhs.nodes_.end())
    {
        if (itb->index == ita->index)
        {
            quotient[itb->index] = ita->value / itb->value;
            ++ita;
            ++itb;
        }
        else if (itb->index < ita->index)
        {
            ++itb;
        }
        else
        {
            ++ita;
        }
    }
    return quotient;
}
template<typename T>
SparseVector<T>& SparseVector<T>::operator+=(const SparseVector<T>& rhs)
{
    std::list<SparseVector<T>::SparseNode>::iterator ita = nodes_.begin();
    std::list<SparseVector<T>::SparseNode>::const_iterator itb = rhs.nodes_.begin();

    while (ita != nodes_.end() && itb != rhs.nodes_.end())
    {
        if (itb->index == ita->index)
        {
            ita->value += itb->value;
            ++ita;
            ++itb;
        }
        else if (itb->index < ita->index)
        {
            nodes_.insert(ita, SparseVector<T>::SparseNode(itb->index, itb->value));
            ++itb;
        }
        else
        {
            ++ita;
        }
    }
    std::copy(itb, rhs.nodes_.end(), std::inserter(nodes_, ita));
    return *this;
}
template<typename T>
SparseVector<T>& SparseVector<T>::operator-=(const SparseVector<T>& rhs)
{
    std::list<SparseVector<T>::SparseNode>::iterator ita = nodes_.begin();
    std::list<SparseVector<T>::SparseNode>::const_iterator itb = rhs.nodes_.begin();

    while (ita != nodes_.end() && itb != rhs.nodes_.end())
    {
        if (itb->index == ita->index)
        {
            ita->value -= itb->value;
            ++ita;
            ++itb;
        }
        else if (itb->index < ita->index)
        {
            nodes_.insert(ita, SparseVector<T>::SparseNode(itb->index, -itb->value));
            ++itb;
        }
        else
        {
            ++ita;
        }
    }
    std::transform(itb, rhs.nodes_.end(), inserter(nodes_, ita),
        SparseNode::negate());
    return *this;
}
template<typename T>
SparseVector<T>& SparseVector<T>::operator*=(T rhs)
{
    std::transform(nodes_.begin(), nodes_.end(), nodes_.begin(),
        SparseNode::multiply(rhs));
    return *this;
}
template<typename T>
SparseVector<T>& SparseVector<T>::operator/=(T rhs)
{
    transform(nodes_.begin(), nodes_.end(), nodes_.begin(),
        SparseNode::divide(rhs));
    return *this;
}
 
template<typename T>
void SparseVector<T>::makeSparse()
{
    nodes_.erase(std::remove_if(nodes_.begin(), nodes_.end(), NodeIsZero()), nodes_.end());
}
 
template<typename T>
void SparseVector<T>::resize(int size)
{
    if (size < length_)
    {
        nodes_.erase(std::remove_if(nodes_.begin(), nodes_.end(), IndexGreaterThan(size - 1)), nodes_.end());
    }
    length_ = size;
}
template<typename T>
T  SparseVector<T>::InnerProduct(  const SparseVector<T>& rhs) const
{
    T product = 0;
    std::list<SparseVector<T>::SparseNode>::const_iterator ita = nodes_.begin();
    std::list<SparseVector<T>::SparseNode>::const_iterator itb = rhs.nodes_.begin();

    while (ita != nodes_.end() && itb != rhs.nodes_.end())
    {
        if (itb->index == ita->index)
        {
            product += ita->value * itb->value;
            ++ita;
            ++itb;
        }
        else if (itb->index < ita->index)
        {
            ++itb;
        }
        else
        {
            ++ita;
        }
    }
    return product;
}

template<typename T>
T InnerProduct(const SparseVector<T>& lhs, const vec<-1, T>& rhs)
{
    if (lhs.length() != rhs.length()) throw WrongDimensionException();

    T product = 0;
    SparseVector<T>::const_iterator it(lhs);

    while (it)
    {
        product += it.getVal() * rhs[it.getIndex()];

        ++it;
    }

    return product;
}

template<typename T>
T InnerProduct(const VecXd& lhs, const SparseVector<T>& rhs)
{
    if (lhs.length() != rhs.length()) throw WrongDimensionException();

    T product = 0;
    SparseVector<T>::const_iterator it(rhs);

    while (it)
    {
        product += it.getVal() * lhs[it.getIndex()];

        ++it;
    }

    return product;
}
template<typename T>
SparseVector<T> operator+(const SparseVector<T>& a, const SparseVector<T>& b)
{
    return SparseVector<T>(a) += b;
}
template<typename T>
SparseVector<T> operator-(const SparseVector<T>& a, const SparseVector<T>& b)
{
    return SparseVector<T>(a) -= b;
}
template<typename T>
SparseVector<T> operator*(const SparseVector<T>& a, T b)
{
    return SparseVector<T>(a) *= b;
}
template<typename T>
SparseVector<T> operator*(T a, const SparseVector<T>& b)
{
    return SparseVector<T>(b) *= a;
}
template<typename T>
SparseVector<T> operator/(const SparseVector<T>& a, T b)
{
    return SparseVector<T>(a) /= b;
}


typedef SparseVector<float> SparseVectorf;
typedef SparseVector<double> SparseVectord;
