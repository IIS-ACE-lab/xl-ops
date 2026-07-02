#ifndef gf2_h
#define gf2_h

#include <iostream>

#include "field.h"

using namespace std;

class GF2 : public Field<GF2>
{
    public:
    bit v;

    inline static const int q = 2;
    inline static const int ceil_log2_q = 1;

    //base_field<2>()
    GF2()
    {
        this->v = 0;
    }

    //base_field<2>(const bit &v)
    GF2(const bit &v)
    {
        this->v = bit(v.value());
    }

    GF2(const vector<bit> &v)
    {
        this->v = bit(v.at(0).value());
    }


    //base_field<2>(const int v)
    GF2(const int v)
    {
        this->v = bit(v);
    }

    //base_field<2>(const bigint v)
    GF2(const bigint v)
    {
        this->v = bit(v);
    }

    static GF2 random_element()
    {
       return GF2(random_int(1));
    }

    vector<bit> to_bit_vec() const
    {
       vector<bit> ret = {bit(this->v)};
       return ret;
    }

//    operator int() const {
//       return this->v.value();
//    }


  protected:

    GF2 _add(const GF2 &c) const
    {
        return GF2(this->v ^ c.v);
    }

    GF2 _dbl() const {
        return GF2(this->v);
    }

    GF2 _sub(const GF2 &c) const
    {
        return GF2(this->v ^ c.v);
    }

    GF2 _mul(const GF2 &c) const
    {
       return GF2(this->v & c.v);
    }

    GF2 _inv() const
    {
        return GF2(this->v);
    }

    bit _not_zero() const
    {
       return bit(this->v);
    }

    //bit operator==(const GF2 &c) const
    //{
    //   return bit(this->v.at(0).value() == c.v.at(0).value());
    //}

    friend ostream& operator<<(ostream& os, const GF2& v);
};

ostream& operator<<(ostream& os, const GF2& v)
{
   os << v.v.value();

   return os;
}

void mux(vector<bit> &a, vector<bit> &b, bit &c)
{
   bit_vector_mux(a, b, c);
}

void mux(bit &a, bit &b, bit &c)
{
   a = c.mux(a, b);
}

#endif

