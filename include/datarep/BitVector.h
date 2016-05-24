#ifndef BITVECTOR_H
#define BITVECTOR_H

class BitVectorBase
{
 protected:
   class bitref 
   { // Prevent this class from being used anywhere else.
    public:
      bitref(::std::uint64_t &an_int, ::std::uint64_t mask)
           : an_int_(an_int), mask_(mask)
      {

      }

      const bitref &operator =(bool val) {
         if (val) 
		 {
            an_int_ |= mask_;
         } else 
		 {
            an_int_ &= ~mask_;
         }
         return *this;
      }
      const bitref &operator =(const bitref &br) 
	  {
         return this->operator =(bool(br));
      }
      operator bool() const 
	  {
         return ((an_int_ & mask_) != 0) ? true : false;
      }

    private:
      ::std::uint64_t &an_int_;
      ::std::uint64_t mask_;
   };
};

template < ::std::size_t Size >
class BitVector : public BitVectorBase 
{
 private:
   static constexpr ::std::size_t numints = ((Size + 63) / 64);
 public:
   BitVector();

   bool operator [](::std::size_t bitnum) const;

   bitref operator[](::std::size_t bitnum);

 private:
   ::std::uint64_t ints_[numints];
};

#endif