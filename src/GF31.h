#ifndef gf31_h
#define gf31_h

#include "PRNG.h"
#include "bit_vector.h"

#include "field.h"

bit bit_sum_carry(bit a, bit b, bit carry);
bit get_carry(bit a, bit b, bit carry_in);
vector<bit> gf31_add(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf31_dbl(const vector<bit> &a);
vector<bit> gf31_sub(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf31_mul(const vector<bit> &a, const vector<bit> &b);
vector<bit> gf31_muladd(const vector<bit> &a, const vector<bit> &b);

class GF31 : public Field<GF31> {
   public:
    vector<bit> v;

    inline static const int q = 31;
    inline static const int ceil_log2_q = 5;

    GF31(const vector<bit> &v) {
        this->v = v;
    }

    GF31(int32_t val) {
        v = bit_vector_from_integer(bigint(val), ceil_log2_q);
    }

    GF31() {
        v = bit_vector_from_integer(bigint(0), ceil_log2_q);
    }

    static GF31 random_element() {
        return GF31(random_val(31));
    }

    vector<bit> to_bit_vec() const
    {
       vector<bit> ret = this->v;
       return ret;
    }

  protected:    
    GF31 _add(const GF31 &c) const {
        return GF31(gf31_add(this->v, c.v));
    }

    GF31 _dbl() const {
        return GF31(gf31_dbl(this->v));
    }

    GF31 _sub(const GF31 &c) const {
        return GF31(gf31_sub(this->v, c.v));
    }

    GF31 _mul(const GF31 &c) const {
        return GF31(gf31_mul(this->v, c.v));
    }

    GF31 _inv() const {
        // Adder chain from https://wwwhomes.uni-bielefeld.de/achim/addition_chain.html
        GF31  v2 = (*this) * (*this);    // 1( 0)            2
        GF31  v3 = v2 * (*this);         // 2( 1, 0)         3
        GF31  v6 = v3 * v3;              // 3( 2)            6
        GF31  v7 = v6 * (*this);         // 4( 3, 0)         7
        GF31 v14 = v7 * v7;              // 5( 4)           14
        GF31 v28 = v14 * v14;            // 6( 5)           28
        GF31 v29 = v28 * (*this);        // 7( 6, 0)        29

        return v29;
    }

    bit _not_zero() const {
        bit cond = this->v.at(0);

        for (int j = 1; j < this->v.size(); j++)
            cond |= this->v.at(j);

        return cond;
    }

    friend ostream &operator<<(ostream &os, const GF31 &v);
};

ostream &operator<<(ostream &os, const GF31 &v) {
    int val = 0;

    for (int i = v.v.size() - 1; i >= 0; i--)
        val = (val << 1) | v.v.at(i).value();

    os << val;

    return os;

    bool printed = false;

    for (int i = v.v.size() - 1; i > 0; i--) {
        if (v.v.at(i).value() == 1) {
            if (printed)
                os << " + ";

            printed = true;
            os << "a";
            if (i > 1)
                os << "^" << i;
        }
    }

    if (v.v.at(0).value() == 1) {
        if (printed)
            os << " + ";

        os << v.v.at(0).value();
    }

    return os;
}

void mux(int32_t &a, int32_t &b, bit &c) {
    a = c.value() ? b : a;
}

#endif
