#include "BitVector.h"
#include <cstdint>  // ::std::uint64_t type
#include <cstddef> // ::std::size_t type
#include <algorithm>

template < ::std::size_t Size >
BitVector::BitVector() 
{ 
	::std::fill(array, array + numints, 0); 
}

template < ::std::size_t Size >
bool BitVector::operator [](::std::size_t bitnum) const 
{
    const ::std::size_t bytenum = bit / 64;
    bitnum = bitnum % 64;
    return ((ints_[bytenum] & (::std::uint64_t(1) << bitnum)) != 0) ? true : false;
}

template < ::std::size_t Size >
bitref BitVector::operator[](::std::size_t bitnum) 
{
    const ::std::size_t bytenum = bit / 64;
    bitnum = bitnum % 64;
    ::std::uint64_t mask = ::std::uint64_t(1) << bitnum;
    return bitref(ints_[bytenum], mask);
}