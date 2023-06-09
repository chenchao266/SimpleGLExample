/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_MIXIN_VECTOR
#define OSG_MIXIN_VECTOR 1

#include <vector>

  

/** MixinVector is a base class that allows inheritance to be used to easily
 *  emulate derivation from std::vector but without introducing undefined
 *  behaviour through violation of virtual destructor rules.
 *
 *  @author Neil Groves
 */
template<class ValueT>
class MixinVector
{
    typedef typename std::vector<ValueT> vector_type;
public:
    typedef typename vector_type::allocator_type allocator_type;
    typedef typename vector_type::value_type value_type;
    typedef typename vector_type::const_pointer const_pointer;
    typedef typename vector_type::pointer pointer;
    typedef typename vector_type::const_reference const_reference;
    typedef typename vector_type::reference reference;
    typedef typename vector_type::const_iterator const_iterator;
    typedef typename vector_type::iterator iterator;
    typedef typename vector_type::const_reverse_iterator const_reverse_iterator;
    typedef typename vector_type::reverse_iterator reverse_iterator;
    typedef typename vector_type::size_type size_type;
    typedef typename vector_type::difference_type difference_type;

    explicit MixinVector() : _impl()
    {
    }

    explicit MixinVector(size_type initial_size, const value_type& fill_value = value_type())
    : _impl(initial_size, fill_value)
    {
    }

    template<class InputIterator>
    MixinVector(InputIterator first, InputIterator last)
    : _impl(first, last)
    {
    }

    MixinVector(const vector_type& other)
    : _impl(other)
    {
    }

    MixinVector(const MixinVector& other)
    : _impl(other._impl)
    {
    }

    MixinVector& operator=(const vector_type& other)
    {
        _impl = other;
        return *this;
    }

    MixinVector& operator=(const MixinVector& other)
    {
        _impl = other._impl;
        return *this;
    }

    virtual ~MixinVector() {}

    void clear() { _impl.clear(); }
    void resize(size_type new_size, const value_type& fill_value = value_type()) { _impl.resize(new_size, fill_value); }
    void reserve(size_type new_capacity) { _impl.reserve(new_capacity); }

    void swap(vector_type& other) { _impl.swap(other); }
    void swap(MixinVector& other) { _impl.swap(other._impl); }

    bool empty() const { return _impl.empty(); }
    size_type size() const { return _impl.size(); }
    size_type capacity() const { return _impl.capacity(); }
    size_type max_size() const { return _impl.max_size(); }
    allocator_type get_allocator() const { return _impl.get_allocator(); }

    const_iterator begin() const { return _impl.begin(); }
    iterator begin() { return _impl.begin(); }
    const_iterator end() const { return _impl.end(); }
    iterator end() { return _impl.end(); }

    const_reverse_iterator rbegin() const { return _impl.rbegin(); }
    reverse_iterator rbegin() { return _impl.rbegin(); }
    const_reverse_iterator rend() const { return _impl.rend(); }
    reverse_iterator rend() { return _impl.rend(); }

    const_reference operator[](size_type index) const { return _impl[index]; }
    reference operator[](size_type index) { return _impl[index]; }

    const_reference at(size_type index) const { return _impl.at(index); }
    reference at(size_type index) { return _impl.at(index); }

    void assign(size_type count, const value_type& value) { _impl.assign(count, value); }
    template<class Iter>
    void assign(Iter first, Iter last) { _impl.assign(first, last); }

    void push_back(const value_type& value) { _impl.push_back(value); }
    void pop_back() { _impl.pop_back(); }

    iterator erase(iterator where) { return _impl.erase(where); }
    iterator erase(iterator first, iterator last) { return _impl.erase(first, last); }

    iterator insert(iterator where, const value_type& value) { return _impl.insert(where, value); }

    template<class InputIterator>
    void insert(iterator where, InputIterator first, InputIterator last)
    {
        _impl.insert(where, first, last);
    }

    void insert(iterator where, size_type count, const value_type& value)
    {
        _impl.insert(where, count, value);
    }

    const_reference back() const { return _impl.back(); }
    reference back() { return _impl.back(); }
    const_reference front() const { return _impl.front(); }
    reference front() { return _impl.front(); }

    vector_type& asVector() { return _impl; }
    const vector_type& asVector() const { return _impl; }

    friend inline bool operator==(const MixinVector<ValueT>& left, const MixinVector<ValueT>& right) { return left._impl == right._impl; }
    friend inline bool operator==(const MixinVector<ValueT>& left, const std::vector<ValueT>& right) { return left._impl == right; }
    friend inline bool operator==(const std::vector<ValueT>& left, const MixinVector<ValueT>& right) { return left == right._impl; }

    friend inline bool operator!=(const MixinVector<ValueT>& left, const MixinVector<ValueT>& right) { return left._impl != right._impl; }
    friend inline bool operator!=(const MixinVector<ValueT>& left, const std::vector<ValueT>& right) { return left._impl != right; }
    friend inline bool operator!=(const std::vector<ValueT>& left, const MixinVector<ValueT>& right) { return left != right._impl; }

    friend inline bool operator<(const MixinVector<ValueT>& left, const MixinVector<ValueT>& right) { return left._impl < right._impl; }
    friend inline bool operator<(const MixinVector<ValueT>& left, const std::vector<ValueT>& right) { return left._impl < right; }
    friend inline bool operator<(const std::vector<ValueT>& left, const MixinVector<ValueT>& right) { return left < right._impl; }

    friend inline bool operator>(const MixinVector<ValueT>& left, const MixinVector<ValueT>& right) { return left._impl > right._impl; }
    friend inline bool operator>(const MixinVector<ValueT>& left, const std::vector<ValueT>& right) { return left._impl > right; }
    friend inline bool operator>(const std::vector<ValueT>& left, const MixinVector<ValueT>& right) { return left > right._impl; }

    friend inline bool operator<=(const MixinVector<ValueT>& left, const MixinVector<ValueT>& right) { return left._impl <= right._impl; }
    friend inline bool operator<=(const MixinVector<ValueT>& left, const std::vector<ValueT>& right) { return left._impl <= right; }
    friend inline bool operator<=(const std::vector<ValueT>& left, const MixinVector<ValueT>& right) { return left <= right._impl; }

    friend inline bool operator>=(const MixinVector<ValueT>& left, const MixinVector<ValueT>& right) { return left._impl >= right._impl; }
    friend inline bool operator>=(const MixinVector<ValueT>& left, const std::vector<ValueT>& right) { return left._impl >= right; }
    friend inline bool operator>=(const std::vector<ValueT>& left, const MixinVector<ValueT>& right) { return left >= right._impl; }

private:
    vector_type _impl;
};

template<class ValueT> inline
void
swap(MixinVector<ValueT>& left,
     MixinVector<ValueT>& right)
{
    std::swap(left.asVector(), right.asVector());
}

template<class ValueT> inline
void
swap(MixinVector<ValueT>& left,
     std::vector<ValueT>& right)
{
    std::swap(left.asVector(), right);
}

template<class ValueT> inline
void
swap(std::vector<ValueT>& left,
     MixinVector<ValueT>& right)
{
    std::swap(left, right.asVector());
}

 
#endif

