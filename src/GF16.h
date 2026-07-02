#ifndef gf16_h
#define gf16_h

#include "bit_vector.h"

#include "field.h"

vector<bit> gf16_add(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf16_dbl(const vector<bit>& a);
vector<bit> gf16_mul_bs(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf16_mul_bs_red(const vector<bit> &a, const vector<bit> &b);
vector<bit> shortvec(const vector<bit> &a);
vector<bit> gf16_inverse(const vector<bit> &a);

class GF16 : public Field<GF16>
{
    public:
    vector<bit> v;

    inline static const int q = 16;
    inline static const int ceil_log2_q = 8;

    // x^4 + x + 1 the reduction polynomial for GF(16)
    inline static const bit GF16red[] = {bit(1), bit(0), bit(0), bit(1), bit(1)};


    static const size_t ext_deg = std::extent<decltype(GF16red)>::value - 1;

    //extension_field<bit, GF16red>(const vector<bit> &v)
    GF16(const vector<bit> &v)
    {
        this->v = v;
    }

    //extension_field<bit, GF16red>(const bigint v)
    GF16(const bigint v)
    {
        this->v = bit_vector_from_integer(v, 8);
    }

    GF16() {
        v = bit_vector_from_integer(bigint(0), ceil_log2_q);
    }

    static GF16 random_element()
    {
       return GF16(random_val(16));
    }

    vector<bit> to_bit_vec() const
    {
       vector<bit> ret = this->v;
       return ret;
    }

//    operator int() const {
//       int val = 0;
//
//       for (int i = this->v.size() - 1; i >= 0; i--)
//          val = (val << 1) | this->v.at(i).value();
//
//       return val;
//    }


  protected:

    GF16 _add(const GF16 &c) const
    {
        return GF16(gf16_add(this->v, c.v));
    }

    GF16 _dbl() const {
        return GF16(gf16_dbl(this->v));
    }


    GF16 _sub(const GF16 &c) const
    {
        // in binary fields addition IS subtraction
        return GF16(gf16_add(this->v, c.v));
    }

    GF16 _mul(const GF16 &c) const
    {
       vector<bit> res = gf16_mul_bs(this->v, c.v);

       for (int i = res.size() - 1; i >= ext_deg; i--)
          for (int j = 0; j < ext_deg; j++)
             if (GF16red[j].value() == 1)
             {
                res.at(i - ext_deg + j) ^= res.at(i);
             }

       vector<bit> endres = shortvec(res);

       return GF16(endres);
    }

    bit _not_zero() const
    {
       bit cond = 0;

       for (int j = 0; j < this->v.size(); j++)
          cond |= this->v.at(j);

       return cond;
    }

    GF16 _inv() const
    {
        return GF16(gf16_inverse(this->v));
    }

    //template <unsigned p, int red_poly[]>
    //friend ostream& operator<<(ostream& os, const extension_field<p, red_poly>& v);
    friend ostream& operator<<(ostream& os, const GF16& v);
};

//template <unsigned p, int red_poly[]>
//ostream& operator<<(ostream& os, const extension_field<p, red_poly>& v)
ostream& operator<<(ostream& os, const GF16& v)
{
    int val = 0;

   for (int i = v.v.size()-1; i >= 0; i--)
      val = (val << 1) | v.v.at(i).value();

    os << val;

    return os;

   bool printed = false;

   for (int i = v.v.size()-1; i > 0; i--)
   {
      if (v.v.at(i).value() == 1)
      {
         if (printed)
            os << " + ";

         printed = true;
         os << "a";
         if (i > 1)
            os << "^" << i;
      }
   }

   if (v.v.at(0).value() == 1)
   {
      if (printed)
         os << " + ";

      os << v.v.at(0).value();    
   }

   return os;
}

#endif

