#ifndef GF31_CONST_H
#define GF31_CONST_H

#include <array>

#include "GF31.h"
#include "bit_vector.h"
#include "field.h"
#include "index.h"

inline GF31 mul0 (const GF31& a){ return GF31(0); }
inline GF31 mul1 (const GF31& a){ return a; }
inline GF31 mul2 (const GF31& a){ GF31 x2 = a.dbl(); return x2; }
inline GF31 mul3 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; return x3; }
inline GF31 mul4 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); return x4; }
inline GF31 mul5 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; return x5; }
inline GF31 mul6 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); return x6; }
inline GF31 mul7 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x7 = x6 + a; return x7; }
inline GF31 mul8 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x8 = x4.dbl(); return x8; }
inline GF31 mul9 (const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x8 = x4.dbl(); GF31 x9 = x8 + a; return x9; }
inline GF31 mul10(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; GF31 x10 = x5.dbl(); return x10; }
inline GF31 mul11(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; GF31 x10 = x5.dbl(); GF31 x11 = x10 + a; return x11; }
inline GF31 mul12(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); return x12; }
inline GF31 mul13(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x13 = x12 + a; return x13; }
inline GF31 mul14(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x7 = x6 + a; GF31 x14 = x7.dbl(); return x14; }
inline GF31 mul15(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x15 = x3 + x12; return x15; }
inline GF31 mul16(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x8 = x4.dbl(); GF31 x16 = x8.dbl(); return x16; }
inline GF31 mul17(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x8 = x4.dbl(); GF31 x16 = x8.dbl(); GF31 x17 = x16 + a; return x17; }
inline GF31 mul18(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x8 = x4.dbl(); GF31 x9 = x8 + a; GF31 x18 = x9.dbl(); return x18; }
inline GF31 mul19(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x8 = x4.dbl(); GF31 x9 = x8 + a; GF31 x18 = x9.dbl(); GF31 x19 = x18 + a; return x19; }
inline GF31 mul20(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; GF31 x10 = x5 + x5; GF31 x20 = x10.dbl(); return x20; }
inline GF31 mul21(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; GF31 x10 = x5 + x5; GF31 x20 = x10.dbl(); GF31 x21 = x20 + a; return x21; }
inline GF31 mul22(const GF31& a){ GF31 x2 = a.dbl(); GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; GF31 x10 = x5 + x5; GF31 x11 = x10 + a; GF31 x22 = x11.dbl(); return x22; }
inline GF31 mul23(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x4 = x2.dbl(); GF31 x5 = x4 + a; GF31 x10 = x5 + x5; GF31 x20 = x10.dbl(); GF31 x23 = x3 + x20; return x23; }
inline GF31 mul24(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x24 = x12.dbl(); return x24; }
inline GF31 mul25(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x24 = x12.dbl(); GF31 x25 = x24 + a; return x25; }
inline GF31 mul26(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x13 = x12 + a; GF31 x26 = x13.dbl(); return x26; }
inline GF31 mul27(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x24 = x12.dbl(); GF31 x27 = x3 + x24; return x27; }
inline GF31 mul28(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x7 = x6 + a; GF31 x14 = x7.dbl(); GF31 x28 = x14.dbl(); return x28; }
inline GF31 mul29(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x7 = x6 + a; GF31 x14 = x7.dbl(); GF31 x28 = x14.dbl(); GF31 x29 = x28 + a; return x29; }
inline GF31 mul30(const GF31& a){ GF31 x2 = a.dbl(); GF31 x3 = x2 + a; GF31 x6 = x3.dbl(); GF31 x12 = x6.dbl(); GF31 x15 = x3 + x12; GF31 x30 = x15.dbl(); return x30; }
using mul_func = GF31(*)(const GF31&);

inline static std::array<mul_func,31> mul_funcs = {{
    mul0,  mul1,  mul2,  mul3,  mul4,  mul5,  mul6,  mul7,
    mul8,  mul9,  mul10, mul11, mul12, mul13, mul14, mul15,
    mul16, mul17, mul18, mul19, mul20, mul21, mul22, mul23,
    mul24, mul25, mul26, mul27, mul28, mul29, mul30
}};


// direct counting of gf31 additions in each chain
inline static std::array<int,31> mul_funcs_add_counts = []() {
    std::array<int,31> counts{};
    GF31 probe(1);
    for (int i = 0; i < 31; i++) {
        GF31::clear_all();
        (void)mul_funcs[i](probe);
        counts[i] = int(GF31::ops(field_ops_add));
    }
    GF31::clear_all();
    return counts;
}();

// direct counting of gf31 doublings in each chain
inline static std::array<int,31> mul_funcs_dbl_counts = []() {
    std::array<int,31> counts{};
    GF31 probe(1);
    for (int i = 0; i < 31; i++) {
        GF31::clear_all();
        (void)mul_funcs[i](probe);
        counts[i] = int(GF31::ops(field_ops_dbl));
    }
    GF31::clear_all();
    return counts;
}();

// Convert bit vector to integer index
inline static int bit_vector_to_int(const std::vector<bit>& v)
{
    int result = 0;
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i].value()) {
            result |= (1 << i);
        }
    }
    return result;
}

// inline GF31 GF31_mult(const GF31& a, const GF31& b)
// {
//     int idx = bit_vector_to_int(b.v);
//     return mul_funcs[idx](a);
// }

class GF31const : public Field<GF31const>
{
    public:
    int32_t v;

    inline static const int q = 31;
    inline static const int order = 31;

    static bigint nummixedmul;

    static void clear_all()
    {
       nummixedmul = 0;

       Field<GF31const>::clear_all();
    }

    GF31const()
    {
        this->v = 0;
    }

    GF31const(const int32_t v)
    {
        this->v = ((v % 31) + 31) % 31;
    }

    static GF31const random_element()
    {
       return GF31const(random_val(31));
    }

//    operator int() const {
//       return this->v % 31;;
//    }


  protected:

    GF31const _add(const GF31const &c) const override
    {
        return GF31const(this->v + c.v);
    }

    GF31const _dbl() const {
        return GF31const(this->v * 2);
    }


    GF31const _sub(const GF31const &c) const override
    {
        return GF31const(this->v - c.v);
    }

    GF31const _mul(const GF31const &c) const override
    {
       return GF31const(this->v * c.v);
    }

    inline static const uint8_t __inv[31] = {0, 1, 16, 21, 8, 25, 26, 9, 4, 7, 28, 17, 13, 12, 20, 29, 2, 11, 19, 18, 14, 3, 24, 27, 22, 5, 6, 23, 10, 15, 30};

    GF31const _inv() const override
    {
        return GF31const(GF31const::__inv[this->v]);
    }

    bit _not_zero() const override
    {
       return bit(this->v == 0 ? 0 : 1);
    }

  public:

    explicit operator GF31() const {          // implicit conversion
        return GF31(v);              // adapt to however GF31 is constructed
    }

    using Field<GF31const>::operator-;
    using Field<GF31const>::operator*;

    GF31 operator-(const GF31 &c) const {
       return GF31(this->v) - c;
    }

    GF31 operator*(const GF31 &c) const {
       nummixedmul += 1;
       return mul_funcs[this->v](c);
    }

    friend ostream& operator<<(ostream& os, const GF31const& v);
};

bigint GF31const::nummixedmul = 0;

ostream& operator<<(ostream& os, const GF31const& v)
{
   os << int(v.v);

   return os;
}

inline GF31 operator*(const GF31& a, const GF31const& b) {
   return b * a;
}

// inline GF31 GF31_mult(const GF31& a, const GF31const& b)
// {
//     return mul_funcs[b.v](a);
// }


#endif // GF31_CONST_H
