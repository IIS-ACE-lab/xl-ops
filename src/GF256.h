#ifndef gf256_h
#define gf256_h

#include "bit_vector.h"

#include "field.h"

vector<bit> gf256_add(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf256_dbl(const vector<bit>& a);
vector<bit> gf256_mul_bs(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf256_mul_bs_red(const vector<bit> &a, const vector<bit> &b);
vector<bit> shortvec(const vector<bit> &a);
vector<bit> gf256_inverse(const vector<bit> &a);

class GF256 : public Field<GF256>
{
    public:
    vector<bit> v;

    inline static const int q = 256;
    inline static const int ceil_log2_q = 8;

    // x^8 + x^4 + x^3 + x^2 + 1 the reduction polynomial for GF(256)
    inline static const bit GF256red[] = {bit(1), bit(0), bit(1), bit(1), bit(1), bit(0), bit(0), bit(0), bit(1)};


    static const size_t ext_deg = std::extent<decltype(GF256red)>::value - 1;

    GF256(const vector<bit> &v)
    {
        this->v = v;
    }

    GF256(const bigint v)
    {
        this->v = bit_vector_from_integer(v, 8);
    }

    GF256() {
        v = bit_vector_from_integer(bigint(0), ceil_log2_q);
    }

    static GF256 random_element()
    {
       return GF256(random_val(256));
    }

    vector<bit> to_bit_vec() const
    {
       vector<bit> ret = this->v;
       return ret;
    }

  protected:

    GF256 _add(const GF256 &c) const
    {
        return GF256(gf256_add(this->v, c.v));
    }

    GF256 _dbl() const {
        return GF256(gf256_dbl(this->v));
    }


    GF256 _sub(const GF256 &c) const
    {
        return GF256(gf256_add(this->v, c.v));
    }

    GF256 _mul(const GF256 &c) const
    {
       vector<bit> res = gf256_mul_bs(this->v, c.v);

       for (int i = res.size() - 1; i >= ext_deg; i--)
          for (int j = 0; j < ext_deg; j++)
             if (GF256red[j].value() == 1)
             {
                res.at(i - ext_deg + j) ^= res.at(i);
             }

       vector<bit> endres = shortvec(res);

       return GF256(endres);
    }

    bit _not_zero() const
    {
       bit cond = this->v.at(0);

       for (int j = 1; j < this->v.size(); j++)
          cond |= this->v.at(j);

       return cond;
    }

    GF256 _inv() const
    {
        return GF256(gf256_inverse(this->v));
    }

    friend ostream& operator<<(ostream& os, const GF256& v);
};

ostream& operator<<(ostream& os, const GF256& v)
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

