#ifndef GF2_CONST_H
#define GF2_CONST_H

#include <array>

#include "GF2.h"
#include "field.h"

class GF2const : public Field<GF2const>
{
    public:
    int32_t v;

    inline static const int q = 2;
    inline static const int order = 2;

    static bigint nummixedmul;

    GF2const()
    {
        this->v = 0;
    }

    GF2const(const int32_t v)
    {
        this->v = v;
    }

    static GF2const random_element()
    {
       return GF2const(random_int(1));
    }

  protected:

    GF2const _add(const GF2const &c) const override
    {
        return GF2const(this->v ^ c.v);
    }

    GF2const _dbl() const {
        return GF2const(this->v);
    }


    GF2const _sub(const GF2const &c) const override
    {
        return GF2const(this->v ^ c.v);
    }

    GF2const _mul(const GF2const &c) const override
    {
       return GF2const(this->v & c.v);
    }

    GF2const _inv() const override
    {
        return GF2const(this->v);
    }

    bit _not_zero() const override
    {
       return bit(this->v == 0 ? 0 : 1);
    }

  public:

    explicit operator GF2() const {
        return GF2(v);
    }

    using Field<GF2const>::operator-;
    using Field<GF2const>::operator*;

    GF2 operator-(const GF2 &c) const {
       return GF2(this->v) - c;
    }

    GF2 operator*(const GF2 &c) const {
       nummixedmul += 1;
       return this->v ? GF2(c) : GF2(0);
    }

    friend ostream& operator<<(ostream& os, const GF2const& v);
};

bigint GF2const::nummixedmul = 0;

ostream& operator<<(ostream& os, const GF2const& v)
{
   os << int(v.v);

   return os;
}

inline GF2 operator*(const GF2& a, const GF2const& b) {
   return b * a;
}

#endif // GF2_CONST_H

