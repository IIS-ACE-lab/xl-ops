#ifndef field_h
#define field_h

#include "bit.h"

enum field_ops_selector {
  field_ops_add,
  field_ops_dbl,
  field_ops_sub,
  field_ops_mul,
  field_ops_inv,
} ;

const std::vector<field_ops_selector> field_ops_selectors = {
  field_ops_add,
  field_ops_dbl,
  field_ops_sub,
  field_ops_mul,
  field_ops_inv,
} ;


template <typename Instance>
class Field {
  static bigint numadd;
  static bigint numdbl;
  static bigint numsub;
  static bigint nummul;
  static bigint numinv;
     
  public:

    static bigint ops(field_ops_selector t) {
      switch(t) {
        case field_ops_add: return numadd;
        case field_ops_dbl: return numdbl;
        case field_ops_sub: return numsub;
        case field_ops_mul: return nummul;
        case field_ops_inv: return numinv;
        default: return 0;
      }
    }

    static void clear_all()
    {
       numadd = 0;
       numdbl = 0;
       numsub = 0;
       nummul = 0;
       numinv = 0;
    }

    Instance operator+(const Instance &c) const {
       ++numadd;
       return _add(c);
    }

    Instance operator-(const Instance &c) const {
       ++numsub;
       return _sub(c);
    }

    Instance operator*(const Instance &c) const {
       ++nummul;
       return _mul(c);
    }

    Instance dbl() const{
       ++numdbl;
       return _dbl();
    }

    Instance inv() const{
       ++numinv;
       return _inv();
    }

    bit not_zero() const {
       return _not_zero();
    }

  protected:
     virtual Instance _add(const Instance &c) const = 0;
     virtual Instance _dbl() const = 0;
     virtual Instance _sub(const Instance &c) const = 0;
     virtual Instance _mul(const Instance &c) const = 0;
     virtual Instance _inv() const = 0;
     virtual bit _not_zero() const = 0;

};

template <typename Instance>
bigint Field<Instance>::numadd = 0;

template <typename Instance>
bigint Field<Instance>::numdbl = 0;

template <typename Instance>
bigint Field<Instance>::numsub = 0;

template <typename Instance>
bigint Field<Instance>::nummul = 0;

template <typename Instance>
bigint Field<Instance>::numinv = 0;
 
#endif
