#ifndef GF256_CONST_H
#define GF256_CONST_H

#include <array>

#include "GF256.h"
#include "bit_vector.h"
#include "field.h"

inline GF256 mul0 (const GF256& a){ return GF256(0); }
inline GF256 mul1 (const GF256& a){ return a; }
inline GF256 mul2 (const GF256& a){ GF256 x2 = a.dbl(); return x2; }
inline GF256 mul3 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; return x3; }
inline GF256 mul4 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); return x4; }
inline GF256 mul5 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; return x5; }
inline GF256 mul6 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); return x6; }
inline GF256 mul7 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; return x7; }
inline GF256 mul8 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); return x8; }
inline GF256 mul9 (const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; return x9; }
inline GF256 mul10(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); return x10; }
inline GF256 mul11(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; return x11; }
inline GF256 mul12(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); return x12; }
inline GF256 mul13(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; return x13; }
inline GF256 mul14(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); return x14; }
inline GF256 mul15(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; return x15; }
inline GF256 mul16(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); return x16; }
inline GF256 mul17(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; return x17; }
inline GF256 mul18(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); return x18; }
inline GF256 mul19(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x19 = x18 + a; return x19; }
inline GF256 mul20(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); return x20; }
inline GF256 mul21(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x21 = x20 + a; return x21; }
inline GF256 mul22(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); return x22; }
inline GF256 mul23(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; return x23; }
inline GF256 mul24(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); return x24; }
inline GF256 mul25(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; return x25; }
inline GF256 mul26(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); return x26; }
inline GF256 mul27(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; return x27; }
inline GF256 mul28(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); return x28; }
inline GF256 mul29(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x29 = x28 + a; return x29; }
inline GF256 mul30(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); return x30; }
inline GF256 mul31(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x31 = x30 + a; return x31; }
inline GF256 mul32(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); return x32; }
inline GF256 mul33(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x33 = x32 + a; return x33; }
inline GF256 mul34(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); return x34; }
inline GF256 mul35(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; return x35; }
inline GF256 mul36(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); return x36; }
inline GF256 mul37(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x37 = x36 + a; return x37; }
inline GF256 mul38(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x19 = x18 + a; GF256 x38 = x19.dbl(); return x38; }
inline GF256 mul39(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; return x39; }
inline GF256 mul40(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); return x40; }
inline GF256 mul41(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x41 = x40 + a; return x41; }
inline GF256 mul42(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x21 = x20 + a; GF256 x42 = x21.dbl(); return x42; }
inline GF256 mul43(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; return x43; }
inline GF256 mul44(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); return x44; }
inline GF256 mul45(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; return x45; }
inline GF256 mul46(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); return x46; }
inline GF256 mul47(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x47 = x46 + a; return x47; }
inline GF256 mul48(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); return x48; }
inline GF256 mul49(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x49 = x48 + a; return x49; }
inline GF256 mul50(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); return x50; }
inline GF256 mul51(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x51 = x3 + x48; return x51; }
inline GF256 mul52(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); return x52; }
inline GF256 mul53(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x53 = x52 + a; return x53; }
inline GF256 mul54(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); return x54; }
inline GF256 mul55(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x55 = x54 + a; return x55; }
inline GF256 mul56(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); return x56; }
inline GF256 mul57(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x57 = x56 + a; return x57; }
inline GF256 mul58(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x29 = x28 + a; GF256 x58 = x29.dbl(); return x58; }
inline GF256 mul59(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x59 = x3 + x56; return x59; }
inline GF256 mul60(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); return x60; }
inline GF256 mul61(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x61 = x60 + a; return x61; }
inline GF256 mul62(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x31 = x30 + a; GF256 x62 = x31.dbl(); return x62; }
inline GF256 mul63(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x63 = x3 + x60; return x63; }
inline GF256 mul64(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); return x64; }
inline GF256 mul65(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x65 = x64 + a; return x65; }
inline GF256 mul66(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x33 = x32 + a; GF256 x66 = x33.dbl(); return x66; }
inline GF256 mul67(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x33 = x32 + a; GF256 x66 = x33.dbl(); GF256 x67 = x66 + a; return x67; }
inline GF256 mul68(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x68 = x34.dbl(); return x68; }
inline GF256 mul69(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x68 = x34.dbl(); GF256 x69 = x68 + a; return x69; }
inline GF256 mul70(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; GF256 x70 = x35.dbl(); return x70; }
inline GF256 mul71(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; GF256 x70 = x35.dbl(); GF256 x71 = x70 + a; return x71; }
inline GF256 mul72(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); return x72; }
inline GF256 mul73(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x73 = x72 + a; return x73; }
inline GF256 mul74(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x37 = x36 + a; GF256 x74 = x37.dbl(); return x74; }
inline GF256 mul75(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x75 = x3 + x72; return x75; }
inline GF256 mul76(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x19 = x18 + a; GF256 x38 = x19.dbl(); GF256 x76 = x38.dbl(); return x76; }
inline GF256 mul77(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x77 = x5 + x72; return x77; }
inline GF256 mul78(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; GF256 x78 = x39.dbl(); return x78; }
inline GF256 mul79(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; GF256 x78 = x39.dbl(); GF256 x79 = x78 + a; return x79; }
inline GF256 mul80(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); return x80; }
inline GF256 mul81(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x81 = x80 + a; return x81; }
inline GF256 mul82(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x41 = x40 + a; GF256 x82 = x41.dbl(); return x82; }
inline GF256 mul83(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x83 = x3 + x80; return x83; }
inline GF256 mul84(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x21 = x20 + a; GF256 x42 = x21.dbl(); GF256 x84 = x42.dbl(); return x84; }
inline GF256 mul85(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x85 = x5 + x80; return x85; }
inline GF256 mul86(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; GF256 x86 = x43.dbl(); return x86; }
inline GF256 mul87(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; GF256 x86 = x43.dbl(); GF256 x87 = x86 + a; return x87; }
inline GF256 mul88(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); GF256 x88 = x44.dbl(); return x88; }
inline GF256 mul89(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); GF256 x88 = x44.dbl(); GF256 x89 = x88 + a; return x89; }
inline GF256 mul90(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); return x90; }
inline GF256 mul91(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); GF256 x91 = x90 + a; return x91; }
inline GF256 mul92(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); return x92; }
inline GF256 mul93(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x93 = x92 + a; return x93; }
inline GF256 mul94(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x47 = x46 + a; GF256 x94 = x47.dbl(); return x94; }
inline GF256 mul95(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x95 = x3 + x92; return x95; }
inline GF256 mul96(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); return x96; }
inline GF256 mul97(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x97 = x96 + a; return x97; }
inline GF256 mul98(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x49 = x48 + a; GF256 x98 = x49.dbl(); return x98; }
inline GF256 mul99(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x99 = x3 + x96; return x99; }
inline GF256 mul100(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); GF256 x100 = x50.dbl(); return x100; }
inline GF256 mul101(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); GF256 x100 = x50.dbl(); GF256 x101 = x100 + a; return x101; }
inline GF256 mul102(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x51 = x3 + x48; GF256 x102 = x51.dbl(); return x102; }
inline GF256 mul103(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x51 = x3 + x48; GF256 x102 = x51.dbl(); GF256 x103 = x102 + a; return x103; }
inline GF256 mul104(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); return x104; }
inline GF256 mul105(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x105 = x104 + a; return x105; }
inline GF256 mul106(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x53 = x52 + a; GF256 x106 = x53.dbl(); return x106; }
inline GF256 mul107(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x107 = x3 + x104; return x107; }
inline GF256 mul108(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); return x108; }
inline GF256 mul109(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x109 = x108 + a; return x109; }
inline GF256 mul110(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x55 = x54 + a; GF256 x110 = x55.dbl(); return x110; }
inline GF256 mul111(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x111 = x3 + x108; return x111; }
inline GF256 mul112(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); return x112; }
inline GF256 mul113(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x113 = x112 + a; return x113; }
inline GF256 mul114(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x57 = x56 + a; GF256 x114 = x57.dbl(); return x114; }
inline GF256 mul115(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x115 = x3 + x112; return x115; }
inline GF256 mul116(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x29 = x28 + a; GF256 x58 = x29.dbl(); GF256 x116 = x58.dbl(); return x116; }
inline GF256 mul117(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x117 = x5 + x112; return x117; }
inline GF256 mul118(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x59 = x3 + x56; GF256 x118 = x59.dbl(); return x118; }
inline GF256 mul119(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x119 = x7 + x112; return x119; }
inline GF256 mul120(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); return x120; }
inline GF256 mul121(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x121 = x120 + a; return x121; }
inline GF256 mul122(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x61 = x60 + a; GF256 x122 = x61.dbl(); return x122; }
inline GF256 mul123(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x123 = x3 + x120; return x123; }
inline GF256 mul124(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x31 = x30 + a; GF256 x62 = x31.dbl(); GF256 x124 = x62.dbl(); return x124; }
inline GF256 mul125(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x125 = x5 + x120; return x125; }
inline GF256 mul126(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x63 = x3 + x60; GF256 x126 = x63.dbl(); return x126; }
inline GF256 mul127(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x63 = x3 + x60; GF256 x126 = x63.dbl(); GF256 x127 = x126 + a; return x127; }
inline GF256 mul128(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x128 = x64.dbl(); return x128; }
inline GF256 mul129(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x128 = x64.dbl(); GF256 x129 = x128 + a; return x129; }
inline GF256 mul130(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x65 = x64 + a; GF256 x130 = x65.dbl(); return x130; }
inline GF256 mul131(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x65 = x64 + a; GF256 x130 = x65.dbl(); GF256 x131 = x130 + a; return x131; }
inline GF256 mul132(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x33 = x32 + a; GF256 x66 = x33.dbl(); GF256 x132 = x66.dbl(); return x132; }
inline GF256 mul133(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x33 = x32 + a; GF256 x66 = x33.dbl(); GF256 x132 = x66.dbl(); GF256 x133 = x132 + a; return x133; }
inline GF256 mul134(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x33 = x32 + a; GF256 x66 = x33.dbl(); GF256 x67 = x66 + a; GF256 x134 = x67.dbl(); return x134; }
inline GF256 mul135(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x128 = x64.dbl(); GF256 x135 = x128 + x4 + x2 + a; return x135; }
inline GF256 mul136(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x68 = x34.dbl(); GF256 x136 = x68.dbl(); return x136; }
inline GF256 mul137(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x68 = x34.dbl(); GF256 x136 = x68.dbl(); GF256 x137 = x136 + a; return x137; }
inline GF256 mul138(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x68 = x34.dbl(); GF256 x69 = x68 + a; GF256 x138 = x69.dbl(); return x138; }
inline GF256 mul139(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x68 = x34.dbl(); GF256 x69 = x68 + a; GF256 x138 = x69.dbl(); GF256 x139 = x138 + a; return x139; }
inline GF256 mul140(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; GF256 x70 = x35.dbl(); GF256 x140 = x70.dbl(); return x140; }
inline GF256 mul141(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; GF256 x70 = x35.dbl(); GF256 x140 = x70.dbl(); GF256 x141 = x140 + a; return x141; }
inline GF256 mul142(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; GF256 x70 = x35.dbl(); GF256 x71 = x70 + a; GF256 x142 = x71.dbl(); return x142; }
inline GF256 mul143(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x17 = x16 + a; GF256 x34 = x17.dbl(); GF256 x35 = x34 + a; GF256 x70 = x35.dbl(); GF256 x140 = x70.dbl(); GF256 x143 = x3 + x140; return x143; }
inline GF256 mul144(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x144 = x72.dbl(); return x144; }
inline GF256 mul145(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x144 = x72.dbl(); GF256 x145 = x144 + a; return x145; }
inline GF256 mul146(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x73 = x72 + a; GF256 x146 = x73.dbl(); return x146; }
inline GF256 mul147(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x144 = x72.dbl(); GF256 x147 = x3 + x144; return x147; }
inline GF256 mul148(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x37 = x36 + a; GF256 x74 = x37.dbl(); GF256 x148 = x74.dbl(); return x148; }
inline GF256 mul149(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x144 = x72.dbl(); GF256 x149 = x5 + x144; return x149; }
inline GF256 mul150(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x75 = x3 + x72; GF256 x150 = x75.dbl(); return x150; }
inline GF256 mul151(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x75 = x3 + x72; GF256 x150 = x75.dbl(); GF256 x151 = x150 + a; return x151; }
inline GF256 mul152(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x19 = x18 + a; GF256 x38 = x19.dbl(); GF256 x76 = x38.dbl(); GF256 x152 = x76.dbl(); return x152; }
inline GF256 mul153(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x144 = x72.dbl(); GF256 x153 = x9 + x144; return x153; }
inline GF256 mul154(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x77 = x5 + x72; GF256 x154 = x77.dbl(); return x154; }
inline GF256 mul155(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x72 = x36.dbl(); GF256 x77 = x5 + x72; GF256 x154 = x77.dbl(); GF256 x155 = x154 + a; return x155; }
inline GF256 mul156(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; GF256 x78 = x39.dbl(); GF256 x156 = x78.dbl(); return x156; }
inline GF256 mul157(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; GF256 x78 = x39.dbl(); GF256 x156 = x78.dbl(); GF256 x157 = x156 + a; return x157; }
inline GF256 mul158(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; GF256 x78 = x39.dbl(); GF256 x79 = x78 + a; GF256 x158 = x79.dbl(); return x158; }
inline GF256 mul159(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x18 = x9.dbl(); GF256 x36 = x18.dbl(); GF256 x39 = x3 + x36; GF256 x78 = x39.dbl(); GF256 x156 = x78.dbl(); GF256 x159 = x3 + x156; return x159; }
inline GF256 mul160(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x160 = x80.dbl(); return x160; }
inline GF256 mul161(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x160 = x80.dbl(); GF256 x161 = x160 + a; return x161; }
inline GF256 mul162(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x81 = x80 + a; GF256 x162 = x81.dbl(); return x162; }
inline GF256 mul163(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x160 = x80.dbl(); GF256 x163 = x3 + x160; return x163; }
inline GF256 mul164(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x41 = x40 + a; GF256 x82 = x41.dbl(); GF256 x164 = x82.dbl(); return x164; }
inline GF256 mul165(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x160 = x80.dbl(); GF256 x165 = x5 + x160; return x165; }
inline GF256 mul166(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x83 = x3 + x80; GF256 x166 = x83.dbl(); return x166; }
inline GF256 mul167(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x83 = x3 + x80; GF256 x166 = x83.dbl(); GF256 x167 = x166 + a; return x167; }
inline GF256 mul168(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x21 = x20 + a; GF256 x42 = x21.dbl(); GF256 x84 = x42.dbl(); GF256 x168 = x84.dbl(); return x168; }
inline GF256 mul169(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x21 = x20 + a; GF256 x42 = x21.dbl(); GF256 x84 = x42.dbl(); GF256 x168 = x84.dbl(); GF256 x169 = x168 + a; return x169; }
inline GF256 mul170(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x85 = x5 + x80; GF256 x170 = x85.dbl(); return x170; }
inline GF256 mul171(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x80 = x40.dbl(); GF256 x85 = x5 + x80; GF256 x170 = x85.dbl(); GF256 x171 = x170 + a; return x171; }
inline GF256 mul172(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; GF256 x86 = x43.dbl(); GF256 x172 = x86.dbl(); return x172; }
inline GF256 mul173(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; GF256 x86 = x43.dbl(); GF256 x172 = x86.dbl(); GF256 x173 = x172 + a; return x173; }
inline GF256 mul174(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; GF256 x86 = x43.dbl(); GF256 x87 = x86 + a; GF256 x174 = x87.dbl(); return x174; }
inline GF256 mul175(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x43 = x3 + x40; GF256 x86 = x43.dbl(); GF256 x172 = x86.dbl(); GF256 x175 = x3 + x172; return x175; }
inline GF256 mul176(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); GF256 x88 = x44.dbl(); GF256 x176 = x88.dbl(); return x176; }
inline GF256 mul177(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); GF256 x88 = x44.dbl(); GF256 x176 = x88.dbl(); GF256 x177 = x176 + a; return x177; }
inline GF256 mul178(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); GF256 x88 = x44.dbl(); GF256 x89 = x88 + a; GF256 x178 = x89.dbl(); return x178; }
inline GF256 mul179(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x11 = x10 + a; GF256 x22 = x11.dbl(); GF256 x44 = x22.dbl(); GF256 x88 = x44.dbl(); GF256 x176 = x88.dbl(); GF256 x179 = x3 + x176; return x179; }
inline GF256 mul180(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); GF256 x180 = x90.dbl(); return x180; }
inline GF256 mul181(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); GF256 x180 = x90.dbl(); GF256 x181 = x180 + a; return x181; }
inline GF256 mul182(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); GF256 x91 = x90 + a; GF256 x182 = x91.dbl(); return x182; }
inline GF256 mul183(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); GF256 x180 = x90.dbl(); GF256 x183 = x3 + x180; return x183; }
inline GF256 mul184(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x184 = x92.dbl(); return x184; }
inline GF256 mul185(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x184 = x92.dbl(); GF256 x185 = x184 + a; return x185; }
inline GF256 mul186(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x93 = x92 + a; GF256 x186 = x93.dbl(); return x186; }
inline GF256 mul187(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x184 = x92.dbl(); GF256 x187 = x3 + x184; return x187; }
inline GF256 mul188(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x47 = x46 + a; GF256 x94 = x47.dbl(); GF256 x188 = x94.dbl(); return x188; }
inline GF256 mul189(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x184 = x92.dbl(); GF256 x189 = x5 + x184; return x189; }
inline GF256 mul190(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x23 = x3 + x20; GF256 x46 = x23.dbl(); GF256 x92 = x46.dbl(); GF256 x95 = x3 + x92; GF256 x190 = x95.dbl(); return x190; }
inline GF256 mul191(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x10 = x5.dbl(); GF256 x20 = x10.dbl(); GF256 x40 = x20.dbl(); GF256 x45 = x5 + x40; GF256 x90 = x45.dbl(); GF256 x95 = x5 + x90; GF256 x190 = x95.dbl(); GF256 x191 = x190 + a; return x191; }
inline GF256 mul192(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x192 = x96.dbl(); return x192; }
inline GF256 mul193(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x192 = x96.dbl(); GF256 x193 = x192 + a; return x193; }
inline GF256 mul194(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x97 = x96 + a; GF256 x194 = x97.dbl(); return x194; }
inline GF256 mul195(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x192 = x96.dbl(); GF256 x195 = x3 + x192; return x195; }
inline GF256 mul196(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x49 = x48 + a; GF256 x98 = x49.dbl(); GF256 x196 = x98.dbl(); return x196; }
inline GF256 mul197(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x128 = x64.dbl(); GF256 x197 = x128 + x64 + x4 + a; return x197; }
inline GF256 mul198(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x99 = x3 + x96; GF256 x198 = x99.dbl(); return x198; }
inline GF256 mul199(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x96 = x48.dbl(); GF256 x99 = x3 + x96; GF256 x198 = x99.dbl(); GF256 x199 = x198 + a; return x199; }
inline GF256 mul200(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); GF256 x100 = x50.dbl(); GF256 x200 = x100.dbl(); return x200; }
inline GF256 mul201(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); GF256 x100 = x50.dbl(); GF256 x200 = x100.dbl(); GF256 x201 = x200 + a; return x201; }
inline GF256 mul202(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); GF256 x100 = x50.dbl(); GF256 x101 = x100 + a; GF256 x202 = x101.dbl(); return x202; }
inline GF256 mul203(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x25 = x24 + a; GF256 x50 = x25.dbl(); GF256 x100 = x50.dbl(); GF256 x200 = x100.dbl(); GF256 x203 = x3 + x200; return x203; }
inline GF256 mul204(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x51 = x3 + x48; GF256 x102 = x51.dbl(); GF256 x204 = x102.dbl(); return x204; }
inline GF256 mul205(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x128 = x64.dbl(); GF256 x205 = x128 + x64 + x8 + x4 + a; return x205; }
inline GF256 mul206(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x48 = x24.dbl(); GF256 x51 = x3 + x48; GF256 x102 = x51.dbl(); GF256 x103 = x102 + a; GF256 x206 = x103.dbl(); return x206; }
inline GF256 mul207(const GF256& a){ GF256 x2 = a.dbl(); GF256 x4 = x2.dbl(); GF256 x8 = x4.dbl(); GF256 x16 = x8.dbl(); GF256 x32 = x16.dbl(); GF256 x64 = x32.dbl(); GF256 x128 = x64.dbl(); GF256 x207 = x128 + x64 + x8 + x4 + x2 + a; return x207; }
inline GF256 mul208(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x208 = x104.dbl(); return x208; }
inline GF256 mul209(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x208 = x104.dbl(); GF256 x209 = x208 + a; return x209; }
inline GF256 mul210(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x105 = x104 + a; GF256 x210 = x105.dbl(); return x210; }
inline GF256 mul211(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x208 = x104.dbl(); GF256 x211 = x3 + x208; return x211; }
inline GF256 mul212(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x53 = x52 + a; GF256 x106 = x53.dbl(); GF256 x212 = x106.dbl(); return x212; }
inline GF256 mul213(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x208 = x104.dbl(); GF256 x213 = x5 + x208; return x213; }
inline GF256 mul214(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x107 = x3 + x104; GF256 x214 = x107.dbl(); return x214; }
inline GF256 mul215(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x12 = x6.dbl(); GF256 x13 = x12 + a; GF256 x26 = x13.dbl(); GF256 x52 = x26.dbl(); GF256 x104 = x52.dbl(); GF256 x208 = x104.dbl(); GF256 x215 = x7 + x208; return x215; }
inline GF256 mul216(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x216 = x108.dbl(); return x216; }
inline GF256 mul217(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x216 = x108.dbl(); GF256 x217 = x216 + a; return x217; }
inline GF256 mul218(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x109 = x108 + a; GF256 x218 = x109.dbl(); return x218; }
inline GF256 mul219(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x216 = x108.dbl(); GF256 x219 = x3 + x216; return x219; }
inline GF256 mul220(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x55 = x54 + a; GF256 x110 = x55.dbl(); GF256 x220 = x110.dbl(); return x220; }
inline GF256 mul221(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x216 = x108.dbl(); GF256 x221 = x5 + x216; return x221; }
inline GF256 mul222(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x111 = x3 + x108; GF256 x222 = x111.dbl(); return x222; }
inline GF256 mul223(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x24 = x12.dbl(); GF256 x27 = x3 + x24; GF256 x54 = x27.dbl(); GF256 x108 = x54.dbl(); GF256 x111 = x3 + x108; GF256 x222 = x111.dbl(); GF256 x223 = x222 + a; return x223; }
inline GF256 mul224(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x224 = x112.dbl(); return x224; }
inline GF256 mul225(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x224 = x112.dbl(); GF256 x225 = x224 + a; return x225; }
inline GF256 mul226(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x113 = x112 + a; GF256 x226 = x113.dbl(); return x226; }
inline GF256 mul227(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x224 = x112.dbl(); GF256 x227 = x3 + x224; return x227; }
inline GF256 mul228(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x57 = x56 + a; GF256 x114 = x57.dbl(); GF256 x228 = x114.dbl(); return x228; }
inline GF256 mul229(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x224 = x112.dbl(); GF256 x229 = x5 + x224; return x229; }
inline GF256 mul230(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x115 = x3 + x112; GF256 x230 = x115.dbl(); return x230; }
inline GF256 mul231(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x224 = x112.dbl(); GF256 x231 = x7 + x224; return x231; }
inline GF256 mul232(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x29 = x28 + a; GF256 x58 = x29.dbl(); GF256 x116 = x58.dbl(); GF256 x232 = x116.dbl(); return x232; }
inline GF256 mul233(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x224 = x112.dbl(); GF256 x233 = x9 + x224; return x233; }
inline GF256 mul234(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x117 = x5 + x112; GF256 x234 = x117.dbl(); return x234; }
inline GF256 mul235(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x117 = x5 + x112; GF256 x234 = x117.dbl(); GF256 x235 = x234 + a; return x235; }
inline GF256 mul236(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x59 = x3 + x56; GF256 x118 = x59.dbl(); GF256 x236 = x118.dbl(); return x236; }
inline GF256 mul237(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x59 = x3 + x56; GF256 x118 = x59.dbl(); GF256 x236 = x118.dbl(); GF256 x237 = x236 + a; return x237; }
inline GF256 mul238(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x119 = x7 + x112; GF256 x238 = x119.dbl(); return x238; }
inline GF256 mul239(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x7 = x6 + a; GF256 x14 = x7.dbl(); GF256 x28 = x14.dbl(); GF256 x56 = x28.dbl(); GF256 x112 = x56.dbl(); GF256 x119 = x7 + x112; GF256 x238 = x119.dbl(); GF256 x239 = x238 + a; return x239; }
inline GF256 mul240(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x240 = x120.dbl(); return x240; }
inline GF256 mul241(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x240 = x120.dbl(); GF256 x241 = x240 + a; return x241; }
inline GF256 mul242(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x121 = x120 + a; GF256 x242 = x121.dbl(); return x242; }
inline GF256 mul243(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x240 = x120.dbl(); GF256 x243 = x3 + x240; return x243; }
inline GF256 mul244(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x61 = x60 + a; GF256 x122 = x61.dbl(); GF256 x244 = x122.dbl(); return x244; }
inline GF256 mul245(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x240 = x120.dbl(); GF256 x245 = x5 + x240; return x245; }
inline GF256 mul246(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x123 = x3 + x120; GF256 x246 = x123.dbl(); return x246; }
inline GF256 mul247(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x123 = x3 + x120; GF256 x246 = x123.dbl(); GF256 x247 = x246 + a; return x247; }
inline GF256 mul248(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x31 = x30 + a; GF256 x62 = x31.dbl(); GF256 x124 = x62.dbl(); GF256 x248 = x124.dbl(); return x248; }
inline GF256 mul249(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x6 = x3.dbl(); GF256 x8 = x4.dbl(); GF256 x9 = x8 + a; GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x240 = x120.dbl(); GF256 x249 = x9 + x240; return x249; }
inline GF256 mul250(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x125 = x5 + x120; GF256 x250 = x125.dbl(); return x250; }
inline GF256 mul251(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x4 = x2.dbl(); GF256 x5 = x4 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x125 = x5 + x120; GF256 x250 = x125.dbl(); GF256 x251 = x250 + a; return x251; }
inline GF256 mul252(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x63 = x3 + x60; GF256 x126 = x63.dbl(); GF256 x252 = x126.dbl(); return x252; }
inline GF256 mul253(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x63 = x3 + x60; GF256 x126 = x63.dbl(); GF256 x252 = x126.dbl(); GF256 x253 = x252 + a; return x253; }
inline GF256 mul254(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x63 = x3 + x60; GF256 x126 = x63.dbl(); GF256 x127 = x126 + a; GF256 x254 = x127.dbl(); return x254; }
inline GF256 mul255(const GF256& a){ GF256 x2 = a.dbl(); GF256 x3 = x2 + a; GF256 x6 = x3.dbl(); GF256 x12 = x6.dbl(); GF256 x15 = x3 + x12; GF256 x30 = x15.dbl(); GF256 x60 = x30.dbl(); GF256 x120 = x60.dbl(); GF256 x240 = x120.dbl(); GF256 x255 = x15 + x240; return x255; }


using mul_func256 = GF256(*)(const GF256&);

inline static std::array<mul_func256,256> mul_funcs256 = {{
    mul0,  mul1,  mul2,  mul3,  mul4,  mul5,  mul6,  mul7,
    mul8,  mul9,  mul10, mul11, mul12, mul13, mul14, mul15,
    mul16, mul17, mul18, mul19, mul20, mul21, mul22, mul23,
    mul24, mul25, mul26, mul27, mul28, mul29, mul30, mul31,
    mul32, mul33, mul34, mul35, mul36, mul37, mul38, mul39,
    mul40, mul41, mul42, mul43, mul44, mul45, mul46, mul47,
    mul48, mul49, mul50, mul51, mul52, mul53, mul54, mul55,
    mul56, mul57, mul58, mul59, mul60, mul61, mul62, mul63,
    mul64, mul65, mul66, mul67, mul68, mul69, mul70, mul71,
    mul72, mul73, mul74, mul75, mul76, mul77, mul78, mul79,
    mul80, mul81, mul82, mul83, mul84, mul85, mul86, mul87,
    mul88, mul89, mul90, mul91, mul92, mul93, mul94, mul95,
    mul96, mul97, mul98, mul99, mul100,mul101,mul102,mul103,
    mul104,mul105,mul106,mul107,mul108,mul109,mul110,mul111,
    mul112,mul113,mul114,mul115,mul116,mul117,mul118,mul119,
    mul120,mul121,mul122,mul123,mul124,mul125,mul126,mul127,
    mul128,mul129,mul130,mul131,mul132,mul133,mul134,mul135,
    mul136,mul137,mul138,mul139,mul140,mul141,mul142,mul143,
    mul144,mul145,mul146,mul147,mul148,mul149,mul150,mul151,
    mul152,mul153,mul154,mul155,mul156,mul157,mul158,mul159,
    mul160,mul161,mul162,mul163,mul164,mul165,mul166,mul167,
    mul168,mul169,mul170,mul171,mul172,mul173,mul174,mul175,
    mul176,mul177,mul178,mul179,mul180,mul181,mul182,mul183,
    mul184,mul185,mul186,mul187,mul188,mul189,mul190,mul191,
    mul192,mul193,mul194,mul195,mul196,mul197,mul198,mul199,
    mul200,mul201,mul202,mul203,mul204,mul205,mul206,mul207,
    mul208,mul209,mul210,mul211,mul212,mul213,mul214,mul215,
    mul216,mul217,mul218,mul219,mul220,mul221,mul222,mul223,
    mul224,mul225,mul226,mul227,mul228,mul229,mul230,mul231,
    mul232,mul233,mul234,mul235,mul236,mul237,mul238,mul239,
    mul240,mul241,mul242,mul243,mul244,mul245,mul246,mul247,
    mul248,mul249,mul250,mul251,mul252,mul253,mul254,mul255
}};

//direct counting of gf256 additions in each chain
inline static std::array<int,256> mul_funcs256_add_counts = []() {
    std::array<int,256> counts{};
    GF256 probe(1);
    for (int i = 0; i < 256; i++) {
        GF256::clear_all();
        (void)mul_funcs256[i](probe);
        counts[i] = int(GF256::ops(field_ops_add));
    }
    GF256::clear_all();
    return counts;
}();

//direct counting of gf256 doublings in each chain
inline static std::array<int,256> mul_funcs256_dbl_counts = []() {
   std::array<int,256> counts{};
   GF256 probe(1);
   for (int i = 0; i < 256; i++) {
      GF256::clear_all();
      (void)mul_funcs256[i](probe);
      counts[i] = int(GF256::ops(field_ops_dbl));
   }
   GF256::clear_all();
   return counts;
}();

//// Convert bit vector to integer index
//inline static int bit_vector_to_int(const std::vector<bit>& v)
//{
//    int result = 0;
//    for (size_t i = 0; i < v.size(); i++) {
//        if (v[i].value()) {
//            result |= (1 << i);
//        }
//    }
//    return result;
//}

//inline GF256 GF256_mult(const GF256& a, const GF256& b)
//{
//    int idx = bit_vector_to_int(b.v);
//    return mul_funcs256[idx](a);
//}


class GF256const : public Field<GF256const>
{
    public:
    int32_t v;

    inline static const int q = 256;
    inline static const int order = 256;

    static bigint nummixedmul;

    GF256const()
    {
        this->v = 0;
    }

    GF256const(const int32_t v)
    {
        this->v = v;// & 0xff;
    }

    static GF256const random_element()
    {
       return GF256const(random_val(256));
    }

//    operator int() const {
//       return this->v;
//    }


  protected:

    GF256const _add(const GF256const &c) const override
    {
        return GF256const(this->v ^ c.v);
    }

    GF256const _dbl() const
    {
       uint32_t a = this->v << 1;

       if (v & 0x80)
          a ^= 0x11d;

       return GF256const(a);
    }


    GF256const _sub(const GF256const &c) const override
    {
        return GF256const(this->v ^ c.v);
    }

    GF256const _mul(const GF256const &c) const override
    {
       uint8_t result = 0;

       uint8_t a = this->v;
       uint8_t b = c.v;

       while (b) {
          if (b & 1) {
             result ^= a;
          }

          uint8_t carry = a & 0x80;
          a <<= 1;

          if (carry) {
             a ^= 0x1D;
          }

          b >>= 1;
       }

       return GF256const(result);
    }

    inline static const uint8_t __inv[256] = {0, 1, 142, 244, 71, 167, 122, 186, 173, 157, 221, 152, 61, 170, 93, 150, 216, 114, 192, 88, 224, 62, 76, 102, 144, 222, 85, 128, 160, 131, 75, 42, 108, 237, 57, 81, 96, 86, 44, 138, 112, 208, 31, 74, 38, 139, 51, 110, 72, 137, 111, 46, 164, 195, 64, 94, 80, 34, 207, 169, 171, 12, 21, 225, 54, 95, 248, 213, 146, 78, 166, 4, 48, 136, 43, 30, 22, 103, 69, 147, 56, 35, 104, 140, 129, 26, 37, 97, 19, 193, 203, 99, 151, 14, 55, 65, 36, 87, 202, 91, 185, 196, 23, 77, 82, 141, 239, 179, 32, 236, 47, 50, 40, 209, 17, 217, 233, 251, 218, 121, 219, 119, 6, 187, 132, 205, 254, 252, 27, 84, 161, 29, 124, 204, 228, 176, 73, 49, 39, 45, 83, 105, 2, 245, 24, 223, 68, 79, 155, 188, 15, 92, 11, 220, 189, 148, 172, 9, 199, 162, 28, 130, 159, 198, 52, 194, 70, 5, 206, 59, 13, 60, 156, 8, 190, 183, 135, 229, 238, 107, 235, 242, 191, 175, 197, 100, 7, 123, 149, 154, 174, 182, 18, 89, 165, 53, 101, 184, 163, 158, 210, 247, 98, 90, 133, 125, 168, 58, 41, 113, 200, 246, 249, 67, 215, 214, 16, 115, 118, 120, 153, 10, 25, 145, 20, 63, 230, 240, 134, 177, 226, 241, 250, 116, 243, 180, 109, 33, 178, 106, 227, 231, 181, 234, 3, 143, 211, 201, 66, 212, 232, 117, 127, 255, 126, 253};

    GF256const _inv() const override
    {
        return GF256const(GF256const::__inv[this->v]);
    }

    bit _not_zero() const override
    {
       return bit(this->v == 0 ? 0 : 1);
    }

  public:

    explicit operator GF256() const {          // implicit conversion
        return GF256(v);              // adapt to however GF256 is constructed
    }

    using Field<GF256const>::operator-;
    using Field<GF256const>::operator*;

    GF256 operator-(const GF256 &c) const {
       return GF256(this->v) - c;
    }

    GF256 operator*(const GF256 &c) const {
       nummixedmul += 1;
       return mul_funcs256[this->v](c);
    }

    friend ostream& operator<<(ostream& os, const GF256const& v);
};

bigint GF256const::nummixedmul = 0;

ostream& operator<<(ostream& os, const GF256const& v)
{
   os << int(v.v);

   return os;
}

inline GF256 operator*(const GF256& a, const GF256const& b) {
   GF256const::nummixedmul += 1;
   return mul_funcs256[b.v](a);
}

// inline GF256 GF256_mult(const GF256& a, const GF256const& b)
// {
//     return mul_funcs256[b.v](a);
// }


#endif // GF256_CONST_H
