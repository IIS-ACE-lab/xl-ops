#ifndef gf31fop_h
#define gf31fop_h

#include "field.h"

class GF31fop : public Field<GF31fop>
{
    public:
    int32_t v;

    inline static const int q = 31;
    inline static const int ceil_log2_q = 5;

    GF31fop()
    {
        this->v = 0;
    }

    GF31fop(const int32_t v)
    {
        this->v = ((v % 31) + 31) % 31;
    }

    GF31fop(const vector<bit> &v)
    {
       uint32_t val = 0;

       for (int i = v.size()-1; i >= 0; i--)
          val = (val << 1) | (v.at(i).value() ? 1 : 0);

        this->v = ((val % 31) + 31) % 31;
    }

    static GF31fop random_element()
    {
       return GF31fop(random_val(31));
    }

    vector<bit> to_bit_vec() const
    {
       return bit_vector_from_integer(this->v, ceil_log2_q);
    }

    operator int() const {
       return this->v % 31;
    }


  protected:

    GF31fop _add(const GF31fop &c) const
    {
        return GF31fop(this->v + c.v);
    }

    GF31fop _dbl() const
    {
        return GF31fop(this->v * 2);
    }

    GF31fop _sub(const GF31fop &c) const
    {
        return GF31fop(this->v - c.v);
    }

    GF31fop _mul(const GF31fop &c) const
    {
       return GF31fop(this->v * c.v);
    }

    inline static const uint8_t __inv[31] = {0, 1, 16, 21, 8, 25, 26, 9, 4, 7, 28, 17, 13, 12, 20, 29, 2, 11, 19, 18, 14, 3, 24, 27, 22, 5, 6, 23, 10, 15, 30};

    GF31fop _inv() const
    {
        return GF31fop(GF31fop::__inv[this->v]);
    }

    bit _not_zero() const
    {
       return bit(this->v == 0 ? 0 : 1);
    }

    //bit operator==(const GF31fop &c) const
    //{
    //   return bit(this->v.at(0).value() == c.v.at(0).value());
    //}

    friend ostream& operator<<(ostream& os, const GF31fop& v);
};

ostream& operator<<(ostream& os, const GF31fop& v)
{
   os << int(v.v);

   return os;
}

//void mux(int32_t &a, int32_t &b, bit &c)
//{
//   a = c.value() ? b : a;
//}

#endif

