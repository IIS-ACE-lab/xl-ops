#include "bit.h"
#include "bit_vector.h"

// x^8 + x^4 + x^3 + x^2 + 1 the reduction polynomial for GF(256)
vector<int> red_poly = {0, 2, 3, 4, 8};


vector<bit> shortvec(const vector<bit> &a)
{
    vector<bit> res = bit_vector_from_integer(0, 8);
    
    for (int i = 0; i < 8; i++)
    {
        res.at(i) = a.at(i).value();
    }
    return res;
}

vector<bit> gf256_add(const vector<bit> &a, const vector<bit> &b)
{
    vector<bit> add = bit_vector_xor(a,b);

    return add;
}

vector<bit> gf256_dbl(const vector<bit>& a)
{
    vector<bit> res(a.size(), bit(0));

    for (int i = 1; i < a.size(); i++)
        res.at(i) = a.at(i-1);

    res.at(0)  = a.at(a.size()-1);
    res.at(2) ^= a.at(a.size()-1);
    res.at(3) ^= a.at(a.size()-1);
    res.at(4) ^= a.at(a.size()-1);

    return res;
}


// fast multiplication for gf256 using Bernstein's code (https://binary.cr.yp.to/bbe251/8.gz)
vector<bit> gf256_mul_bs(const vector<bit> &a, const vector<bit> &b)
{
    vector<bit> res = bit_vector_from_integer(0, 16);

    bit t1 = a.at(3) & b.at(3);
    bit t2 = a.at(3) & b.at(0);
    bit t3 = a.at(3) & b.at(1);
    bit t4 = a.at(3) & b.at(2);
    bit t5 = a.at(0) & b.at(3);
    bit t6 = a.at(1) & b.at(3);
    bit t7 = a.at(2) & b.at(3);
    bit t8 = a.at(2) & b.at(2);
    bit t9 = a.at(2) & b.at(0);
    bit t10 = a.at(2) & b.at(1);
    bit t11 = a.at(0) & b.at(2);
    bit t12 = a.at(1) & b.at(2);
    bit t13 = a.at(1) & b.at(1);
    bit t14 = a.at(1) & b.at(0);
    bit t15 = a.at(0) & b.at(1);
    bit t16 = a.at(0) & b.at(0);
    bit t17 = t15 ^ t14;
    bit t18 = t13 ^ t11;
    bit t19 = t18 ^ t9;
    bit t20 = t12 ^ t10;
    bit t21 = t20 ^ t5;
    bit t22 = t8 ^ t6;
    bit t23 = t21 ^ t2;
    bit t24 = t22 ^ t3;
    bit t25 = t7 ^ t4;
    bit t26 = a.at(7) & b.at(7);
    bit t27 = a.at(7) & b.at(4);
    bit t28 = a.at(7) & b.at(5);
    bit t29 = a.at(7) & b.at(6);
    bit t30 = a.at(4) & b.at(7);
    bit t31 = a.at(5) & b.at(7);
    bit t32 = a.at(6) & b.at(7);
    bit t33 = a.at(6) & b.at(6);
    bit t34 = a.at(6) & b.at(4);
    bit t35 = a.at(6) & b.at(5);
    bit t36 = a.at(4) & b.at(6);
    bit t37 = a.at(5) & b.at(6);
    bit t38 = a.at(5) & b.at(5);
    bit t39 = a.at(5) & b.at(4);
    bit t40 = a.at(4) & b.at(5);
    bit t41 = a.at(4) & b.at(4);
    bit t42 = t40 ^ t39;
    bit t43 = t38 ^ t36;
    bit t44 = t43 ^ t34;
    bit t45 = t37 ^ t35;
    bit t46 = t45 ^ t30;
    bit t47 = t33 ^ t31;
    bit t48 = t46 ^ t27;
    bit t49 = t47 ^ t28;
    bit t50 = t32 ^ t29;
    bit t51 = b.at(0) ^ b.at(4);
    bit t52 = b.at(1) ^ b.at(5);
    bit t53 = b.at(2) ^ b.at(6);
    bit t54 = b.at(3) ^ b.at(7);
    bit t55 = a.at(0) ^ a.at(4);
    bit t56 = a.at(1) ^ a.at(5);
    bit t57 = a.at(2) ^ a.at(6);
    bit t58 = a.at(3) ^ a.at(7);
    bit t59 = t58 & t54;
    bit t60 = t58 & t51;
    bit t61 = t58 & t52;
    bit t62 = t58 & t53;
    bit t63 = t55 & t54;
    bit t64 = t56 & t54;
    bit t65 = t57 & t54;
    bit t66 = t57 & t53;
    bit t67 = t57 & t51;
    bit t68 = t57 & t52;
    bit t69 = t55 & t53;
    bit t70 = t56 & t53;
    bit t71 = t56 & t52;
    bit t72 = t56 & t51;
    bit t73 = t55 & t52;
    bit t74 = t55 & t51;
    bit t75 = t73 ^ t72;
    bit t76 = t71 ^ t69;
    bit t77 = t76 ^ t67;
    bit t78 = t70 ^ t68;
    bit t79 = t78 ^ t63;
    bit t80 = t66 ^ t64;
    bit t81 = t79 ^ t60;
    bit t82 = t80 ^ t61;
    bit t83 = t65 ^ t62;
    bit t84 = t24 ^ t41;
    bit t85 = t25 ^ t42;
    bit t86 = t1 ^ t44;
    bit t87 = t74 ^ t84;
    bit t88 = t75 ^ t85;
    bit t89 = t77 ^ t86;
    bit t90 = t81 ^ t48;
    bit t91 = t82 ^ t49;
    bit t92 = t83 ^ t50;
    bit t93 = t59 ^ t26;
    bit t94 = t87 ^ t16;
    bit t95 = t88 ^ t17;
    bit t96 = t89 ^ t19;
    bit t97 = t90 ^ t23;
    bit t98 = t91 ^ t84;
    bit t99 = t92 ^ t85;
    bit t100 = t93 ^ t86;
    res.at(0) = t16;
    res.at(1) = t17;
    res.at(2) = t19;
    res.at(3) = t23;
    res.at(4) = t94;
    res.at(5) = t95;
    res.at(6) = t96; 
    res.at(7) = t97;
    res.at(8) = t98;
    res.at(9) = t99;
    res.at(10) = t100;
    res.at(11) = t48;
    res.at(12) = t49;
    res.at(13) = t50;
    res.at(14) = t26;

    return res;
}

vector<bit> gf256_mul_bs_red(const vector<bit> &a, const vector<bit> &b)
{
    vector<bit> res = gf256_mul_bs(a, b);

    for (int i = res.size() - 1; i >= red_poly.at(red_poly.size()-1); i--)
    {        
        for (int j = 0; j < red_poly.size()-1; j++)
        {
            res.at(i - red_poly.at(red_poly.size()-1) + red_poly.at(j)) ^= res.at(i);
        }
    }

    vector<bit> endres = shortvec(res);

    return endres;
}

vector<bit> gf256_sqr(const vector<bit> &a)
{
    vector<bit> res = bit_vector_from_integer(0, 16);
    vector<bit> endres = bit_vector_from_integer(0, 8);

    for (int i = 0; i < a.size(); i++)
    {
        res.at(2*i) = a.at(i).value();
    }

    for (int i = res.size() - 1; i >= red_poly.at(red_poly.size()-1); i--)
    {        
        for (int j = 0; j < red_poly.size()-1; j++)
        {
            res.at(i - red_poly.at(red_poly.size()-1) + red_poly.at(j)) ^= res.at(i);
        }
    }

    endres = shortvec(res);

    return endres;
}

//inverse on gf256 with the shortest addition chain 
vector<bit> gf256_inverse(const vector<bit> &a)
{
    vector<bit> x0 = a;
    vector<bit> x1 = gf256_sqr(x0);              // 2
    vector<bit> x2 = gf256_mul_bs_red(x1,x0);    // 3
    vector<bit> x3 = gf256_sqr(x2);              // 6
    vector<bit> x4 = gf256_sqr(x3);              // 12
    vector<bit> x5 = gf256_mul_bs_red(x4,x2);    // 15
    vector<bit> x6 = gf256_sqr(x5);              // 30
    vector<bit> x7 = gf256_sqr(x6);              // 60 
    vector<bit> x8 = gf256_mul_bs_red(x7,x2);    // 63
    vector<bit> x9 = gf256_sqr(x8);              // 126
    vector<bit> x10 = gf256_mul_bs_red(x9, x0);  // 127
    vector<bit> res = gf256_sqr(x10);            // 254

    return res;
}

