#ifndef xl_h
#define xl_h

#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
#include <map>

#include "bigint.h"

#include "bit.h"
#include "bit_vector.h"
#include "ram.h"

#include "index.h"

#include "PRNG.h"
#include "monomial.h"

#include "field.h"

#include "GF2-const.h"
#include "GF31-const.h"
#include "GF256-const.h"

using namespace std;


extern bool trace;
extern bool bucket;
extern uint64_t perm_seed;

#ifndef TRACE
#define TRACE(x) {if (trace) {x;}; }
#endif

extern bigint bm_ops;
extern bigint la_ops;

template <typename GF>
ostream& operator<<(ostream& os, const vector<GF>& v)
{
   os << "[";

   for (int i = 0; i < v.size() - 1; i++)
     os << v.at(i) << ", ";

   os << v.at(v.size()-1) << "]\n";

   return os;
}


template <typename GF, typename GFsys>
int macaulay_matrix(
    vector<vector<GFsys>> &coeff_list, vector<vector<int>> &idx_list, // A 
    vector<GF> &b, // Ax = b
    const vector<vector<GFsys>> &p, int n, int m, int d)
{
    // p: list of polynomials
    // m: number of polynomials
    // n: number of variables 
    // d: degree

    int num_colms = 0;      //extended matrix's column numbers
    int num_row_blocks = 0; //number of polynomial blocks

    if constexpr (std::is_same_v<GF, GF2>)  // GF2 optimization
    {
       for (int dd = 0; dd <= d; dd++)
          num_colms += binomial(n, dd);

       for (int dd = 0; dd <= d-2; dd++)
          num_row_blocks += binomial(n, dd);
    }
    else
    {
       num_colms = binomial(n+d, d);
       num_row_blocks = binomial(n+d-2, d-2);
    }

    cout << "n " << n << "  d " << d << "  ncols " << num_colms << "  nrows " << (num_row_blocks*m) << "\n";

    monomial ext_mon(GF::q, n);

    for (int i = 0; i < num_row_blocks; i++) //each monomial of the monomial list
    {
        for (int j = 0; j < m; j++) //each row of the system
        {
            vector<GFsys> coeff; //vector includes the coeff of the polynomial
            vector<int> idx;     //index of the coeff of the polynomial

            monomial sys_mon(GF::q, n);

            for (int t = 0; t < p.at(j).size(); t++) //each coef of the each row
            {
                monomial prod_mon = ext_mon * sys_mon;

                if (prod_mon.monomial_to_index() != 0)
                {
                   int l = idx.size();

                   if constexpr (std::is_same_v<GF, GF2>)  // GF2 optimization
                   {
                      for (l = 0; l < idx.size(); l++)
                         if (idx[l] == prod_mon.monomial_to_index()-1)
                         {
                            coeff[l] = coeff[l] + p.at(j).at(t);
                            break;
                         }
                   }

                   if (l == idx.size())
                   {
                      idx.insert(idx.end(), prod_mon.monomial_to_index()-1);
                      coeff.insert(coeff.end(), p.at(j).at(t));
                   }
                }

                sys_mon.step();            
            }

            idx_list.insert(idx_list.end(), idx);
            coeff_list.insert(coeff_list.end(), coeff);

        }
        
        ext_mon.step();
    }
    
    b = vector<GF>(num_colms-1, GF(0));

    for (int i = 0; i < p.size(); i++)
        b.at(i) = static_cast<GF>(p.at(i).at(0));

    cout << "num_cols: " << num_colms << "\n\n";

    uint64_t new_seed = random_val(0xffffffffffffffff);

    random_init((GF::q << 24) | (n << 16) | (m << 8) | d);

    while (idx_list.size() >= num_colms)
    {
        int idx = random_val(idx_list.size()-m)+m;

        idx_list.erase(idx_list.begin() + idx);
        coeff_list.erase(coeff_list.begin() + idx);

        if (idx < p.size())
        {
           b.erase(b.begin() + idx);
           b.insert(b.end(), GF(0));
        }
    }

    random_init(new_seed);

    return 0;
}

template <typename GF>
vector<GF> bm(const vector<GF> &s)
{
    bm_ops = - bit::ops();

    int t = s.size() >> 1;
    
    vector<GF> sigma(t + 1, GF(0));
    vector<GF> beta(t + 1, GF(0));

    sigma[0] = GF(1);
    beta[1] = GF(1);

    GF delta(1);
 
    vector<bit> L(ceil(log2(2.0*t)), 0);

    for (int i = 0; i < 2*t; i++)
    {
        GF d = GF(0);

        TRACE(cout << "\n");
        TRACE(cout << "i: " << (i) << "\n");
        TRACE(cout << "L: " << index_value(L) << "\n");

        for (int j = 0; j <= min(i, t); j++)
            d = d + (sigma.at(j) * s.at(i-j));    

        TRACE(cout << "beta: " << beta);
        TRACE(cout << "sigma: " << sigma);
        TRACE(cout << "d: " << d << "\n");
        TRACE(cout << "delta: " << delta << "\n");

        vector<GF> mul1(t + 1, GF(0));
        
        for (int j = 0; j <= min(i+1, t); j++)
            mul1.at(j) = delta*sigma.at(j);

        TRACE(cout << "mul1: " << mul1);

        vector<GF> psi(t+1, GF(0));
        
        for (int j = 0; j <= min(i+1, t); j++)
            psi.at(j) = d*beta.at(j);

        TRACE(cout << "mul2: " << psi);

        for (int j = 0; j <= min(i+1, t); j++)
           psi.at(j) = mul1.at(j) - psi.at(j);

        TRACE(cout << "psi: " << psi);

        bit cond = 0;

        // d == 0 -> (d != 0)
        cond = d.not_zero();


        // (i < 2*L) -> (i+1 <= 2*L) -> ~(i+1 > 2*L)
        vector<bit> ibit = bit_vector_from_integer(i+1, (int)ceil(log2(2.0*t) + 1));

        L.insert(L.begin(), bit(0));

        bit gt = bit_vector_gt(ibit, L);


        // if (d == 0) or (i < 2*L) -> (d != 0) and ~(i+1 > 2*L)
        cond = cond & gt;


        // fix lengths
        L.erase(L.begin());
        ibit.pop_back();


        // Lnew = i + 1 (-L)
        vector<bit> L_new(L.size(), 0);

        for (int j = 0; j < L.size(); j++)
           L_new[j] = ~L[j];

        bit_vector_add(L_new, L_new, ibit, 1);


        // muxes based on cond
        for (int j = 0; j <= min(i + 1, t); j++)
           mux(beta.at(j).v, sigma.at(j).v, cond);

        bit_vector_mux(L, L_new, cond);

        beta.insert(beta.begin(), GF(0));
        beta.pop_back();

        TRACE(cout << "beta new " << cond.value() << ": " << beta);


        mux(delta.v, d.v, cond);

        sigma = psi;
    }

    bm_ops += bit::ops();

    return sigma;
}

template <typename GF, typename GFsys>
vector<GF> wiedemann(vector<vector<GFsys>> &coeff_list, vector<vector<int>> &idx_list,
    vector<GF> &b, int n)
{
    la_ops = - bit::ops();

    int num_rows = coeff_list.size();
    TRACE(cout << __LINE__ << '\n');

    vector<GF> sequence = {b.at(n-1)};

    vector<vector<GF>> X;

    X.insert(X.begin(), vector<GF>(n, GF(0)));

    for (int i = 0; i < n; i++)
       X.at(0).at(n-i-1) = b.at(i);

    TRACE(cout << X.at(X.size()-1));


    vector<GF> vec1 = b;
    vector<GF> vec2(num_rows, GF(0));
    TRACE(cout << __LINE__ << '\n');

    map<int,int> s;

    for (int i=0; i < coeff_list.size(); i++)
       s[coeff_list.at(i).size()]++;


    cout << "entries per row:\n";
    for (auto [key, count] : s) {
        cout << key << ": " << count << "\n";
    }
    cout << "\n";


////////////////// print stat begin
   if constexpr (std::is_same_v<GFsys, GF31const> or std::is_same_v<GFsys, GF256const> or std::is_same_v<GFsys, GF2const>)
   {
        uint64_t not_zero = 0;
        uint64_t struct_not_zero = 0;

        for (int row = 0; row < coeff_list.size(); row++)
        {
            GF v(0);

            vector<GFsys> coeff = coeff_list.at(row);
            vector<int> idx = idx_list.at(row);

            for (int i = 0; i < coeff.size(); i++)
            {
               v = vec1.at(idx.at(i)) * coeff.at(i);

               if (coeff.at(i).v > 0)
                  not_zero += 1;

               struct_not_zero += 1;
            }
        }

       cout << "\n";
       cout << "Macaulay mul bit ops: " << bit::ops() << "\n";
       cout << "Macaulay not zero: " << not_zero << "\n";
       cout << "Macaulay structural not zero: " << struct_not_zero << "\n";
       cout << "\n";

       cout << "Macaulay mul field add: " << GF::ops(field_ops_add) << "\n";
       cout << "Macaulay mul field dbl: " << GF::ops(field_ops_dbl) << "\n";
       cout << "Macaulay mul field sub: " << GF::ops(field_ops_sub) << "\n";
       cout << "Macaulay mul field mul: " << GF::ops(field_ops_mul) << "\n";
       cout << "Macaulay mul field inv: " << GF::ops(field_ops_inv) << "\n";
       cout << "\n";

       bit::clear_all();
       GF::clear_all();
       GFsys::clear_all();
   }
////////////////// stat end

    while (sequence.size() < 2*num_rows)
    {
        for (int row = 0; row < coeff_list.size(); row++)
        {
            GF v(0);

            vector<GFsys> coeff = coeff_list.at(row);
            vector<int> idx = idx_list.at(row);

            if (bucket)
            {
               if constexpr (std::is_same_v<GFsys, GF31const> or std::is_same_v<GFsys, GF256const> or std::is_same_v<GFsys, GF2const>)
               {
                  GF acc[GFsys::order];

                  for (int i = 0; i < coeff.size(); i++)
                     if (coeff.at(i).v > 0)
                        acc[coeff.at(i).v] = acc[coeff.at(i).v] + vec1.at(idx.at(i));

                  v = acc[1];

                  for (int i = 2; i < GFsys::order; i++)
                     v = v + GFsys(i) * acc[i];
               }
               else
               {
                  vector<vector<bit>> acc;

                  for (int i = 0; i < GFsys::q; i++)
                     acc.push_back(GFsys(0).to_bit_vec());

                  for (int i = 0; i < coeff.size(); i++)
                  {
                     vector<bit> tmp = ram_read(acc, coeff.at(i).to_bit_vec());

                     GF tmp2 = GF(tmp);
                     tmp2 = tmp2 + vec1.at(idx.at(i));

                     ram_write(acc, coeff.at(i).to_bit_vec(), tmp2.to_bit_vec());
                  }

                  v = acc.at(1);

                  for (int i = 2; i < GFsys::q; i++)
                     v = v + GFsys(i) * acc.at(i);
               }
            }
            else
            {
               for (int i = 0; i < coeff.size(); i++)
                  v = v + vec1.at(idx.at(i)) * coeff.at(i);
            }

            vec2.at(row) = v;
        }

        TRACE(cout << vec2);

        sequence.insert(sequence.begin(), vec2.at(n-1));

        X.insert(X.end(), vector<GF>(n, GF(0)));

        for (int i = 0; i < n; i++)
            X.at(X.size()-1).at(n-i-1) = vec2.at(i);

        TRACE(cout << X.at(X.size()-1));


        vector<GF> tmp = vec2;
        vec2 = vec1;
        vec1 = tmp;
    }
    TRACE(cout << __LINE__ << '\n');

    TRACE(cout << sequence);

    la_ops += bit::ops();
    
    vector<GF> min_poly = bm(sequence);

    TRACE(cout << min_poly);

    GF norm = min_poly.at(0);
    norm = norm.inv();

    for (int i = 0; i < min_poly.size(); i++)
        min_poly.at(i) = min_poly.at(i) * norm;

    TRACE(cout << min_poly);

    vector<GF> ret(n, GF(0));

    for (int i = 1; i < min_poly.size(); i++)
    {
      TRACE(cout << X.at(i-1));
      for (int j = 0; j < n; j++)
            ret.at(j) = ret.at(j) + min_poly.at(i) * X.at(i-1).at(j);

      TRACE(cout << ret);
    }

    return ret;
}

template <typename GF, typename GFsys>
int XL(int n, int m, int d)
{
    vector<GFsys> sol(n, GFsys(0));
    vector<GF> sol2;

    for (int i=0; i < n; i++)
       sol.at(i) = GFsys::random_element();

    cout << "sol: " << sol << "\n";

    vector<vector<GFsys>> sys;        //list of each polynomial's coefficient

    vector<vector<GFsys>> coeff_list; //list of each polynomial's coefficient
    vector<vector<int>> idx_list;     //list of index, where the coefficient on the matrix exactly

    for (int i = 0; i < m; i++)
    {
       vector<GFsys> poly;

       {
          GFsys v = GFsys::random_element();
          poly.insert(poly.end(), v);
       }

       GFsys eval = poly[0];

       for (int j = 0; j < n; j++)
       {
          GFsys v = GFsys::random_element();
          poly.insert(poly.end(), v);

          eval = eval + v*sol.at(n-j-1);
       }

       int start_k_off;

       if constexpr (std::is_same_v<GF, GF2>)  // GF2 optimization
          start_k_off = 1;
       else
          start_k_off = 0;

       for (int j = 0; j < n; j++)
          for (int k=j+start_k_off; k < n; k++)
          {
             GFsys v = GFsys::random_element();
             poly.insert(poly.end(), v);

             eval = eval + v*sol.at(n-j-1)*sol.at(n-k-1);
          }

       TRACE(cout << poly << "\n" << eval << "\n");
       poly[0] = poly[0] - eval;
       TRACE(cout << poly << "\n" << eval << "\n");

       sys.insert(sys.end(), poly);
    }

    TRACE(cout << "system:\n");

    for (int i = 0; i < sys.size(); i++)
       TRACE(cout << sys[i]);

    TRACE(cout << "\n");

    std::mt19937 rng(perm_seed);

    shuffle(sys.begin(), sys.end(), rng);


    vector<GF> b;

    macaulay_matrix(coeff_list, idx_list, b, sys, n, m, d);

    for (int i = 0; i < coeff_list.size(); i++)
    {
       vector<GFsys> vec(coeff_list.size(), GFsys(0));

       int idx = 0;

       for (int j = 0; j < coeff_list.size(); j++)
          if (j == idx_list[i][idx])
          {
             vec[j] = coeff_list[i][idx];
             idx++;

             if (idx >= idx_list[i].size())
                break;
          }

       TRACE(cout << vec);
    }

    TRACE(cout << "\n" << b << "\n");

    bit::clear_all();
    GF::clear_all();
    GFsys::clear_all();


    sol2 = wiedemann<GF, GFsys>(coeff_list, idx_list, b, n);

    cout << "\n";
    cout << "got: " << sol2 ;

    cout << "\n";
    cout << "bit ops: " << bit::ops() << "\n";
    cout << "bm bit ops: " << bm_ops << "\n";
    cout << "LA bit ops: " << la_ops << "\n";
    cout << "\n";

    cout << "GF:\n";
    cout << "field add: " << GF::ops(field_ops_add) << "\n";
    cout << "field dbl: " << GF::ops(field_ops_dbl) << "\n";
    cout << "field sub: " << GF::ops(field_ops_sub) << "\n";
    cout << "field mul: " << GF::ops(field_ops_mul) << "\n";
    cout << "field inv: " << GF::ops(field_ops_inv) << "\n";
    cout << "\n";

    if constexpr (std::is_same_v<GFsys, GF31const> or std::is_same_v<GFsys, GF256const> or std::is_same_v<GFsys, GF2const>)
    {
       cout << "field mixed mul: " << GFsys::nummixedmul << "\n";
       cout << "\n";
    }

    for (int i = 0; i < n; i++)
       if ((sol[i] - sol2[i]).not_zero().value())
       {
          cout << "ERR\n";
          break;
       }

    return 0;
}
#endif

