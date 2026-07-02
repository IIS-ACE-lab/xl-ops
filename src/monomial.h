#ifndef TMATRIX_H
#define TMATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

using namespace std;

#define ECHO_NL() \
   { printf("\n"); fflush(stdout); }
#define ECHO(...) \
   { printf(__VA_ARGS__); fflush(stdout); }
#define ECHO_R(...) \
   { printf(__VA_ARGS__); fflush(stdout); }

unsigned int binomial(int m, int n)
{
   if (m < n)
      return 0;
   unsigned int ret = 1;
   int i = 1;
   for(; i <= n; i++)
      ret = ret*(i + m - n)/i;
   return ret;
}

unsigned int sum_binomial(int n, int d)
{
   uint64_t ret = 0;

   for (int i = 0; i <= d; i++)
      ret += binomial(n, i);

   return ret;
}


class monomial
{
   private:
      vector<unsigned> mon;

      /* GF(2) is a special case because x^2 = x */
      int q;

      bool GF2_opt;

   public:

      monomial (int q, unsigned n, bool GF2_opt)
      {
         this->q = q;
         this->GF2_opt = (q == 2) && GF2_opt;
         this->mon = vector<unsigned>(n, 0);
      }

      unsigned& operator[] (int i)
      {
         return this->mon[i];
      }


      // count index in grevlex mode
      unsigned int monomial_to_index()
      {
         if (this->GF2_opt)
         {
            unsigned int i;
            int d = 0;
            unsigned int index = 0;
            unsigned int index_d = 0;
            for (i = 0; i < this->mon.size(); i++)
               if (this->mon[i] >= 1)
               {
                  d++;

                  index += binomial(i, d);
                  index_d += binomial(this->mon.size(), d);	
               }
            index = index_d - index;

            return index;
         }
         else
         {
            unsigned int index = 0;

            int deg = 0;

            for (unsigned int i = 0; i < this->mon.size(); i++)
            {
               deg += mon[i];

               if(deg >= 1)
                  index += binomial(deg+i, deg-1);
            }

            return index;
         }
      }

      // compute next monomial in grvlex order
      void step()
      {
         if (this->GF2_opt)
         {
            unsigned int i,j;

            if (this->mon[0] == 0)
            {
               for (i = 1; this->mon[i] == 0; i++)
                  if (i == this->mon.size()-1)
                  {
                     this->mon[i] = 1;
                     return;
                  }

               this->mon[i] = 0;
               this->mon[i-1] = 1;  

               return;
            }

            for (i = 1; this->mon[i] == 1; i++)
               if (i == this->mon.size()-1)	
                  return;

            for (;i < this->mon.size() && this->mon[i] == 0; i++);
            if (i < this->mon.size())
               this->mon[i] = 0;
            this->mon[i-1] = 1;	

            j = i-2;
            i = 0;
            for (; (i < j) & (j < this->mon.size()); i++, j--)
            {
               unsigned int temp = this->mon[i];
               this->mon[i] = this->mon[j];
               this->mon[j] = temp;
            } 	

            return;	
         }
         else
         {
            int i;

            if (mon[0] == 0)
            {
               for (i = 1; mon[i] == 0; i++)
                  if (i == this->mon.size()-1)
                  {
                     mon[i] = 1;
                     return;
                  }

               mon[i] = mon[i] - 1;
               mon[i-1] = mon[i-1] + 1;

               return;
            }

            for (i = 1; mon[i] == 0; i++)
               if (i == this->mon.size()-1)
               {
                  mon[this->mon.size()-1] = mon[0] + 1;
                  mon[0] = 0;
                  return;
               }

            mon[i] = mon[i] - 1;
            mon[i-1] = mon[0] + 1;

            if (i != 1)
               mon[0] = 0;

            return;	
         }
      }

      monomial operator* (monomial &x)
      {
         if (this->GF2_opt)
         {
            monomial ret(this->q, this->mon.size(), this->GF2_opt);

            for (unsigned k = 0; k < this->mon.size(); k++)
               ret[k] = this->mon[k] | x.mon[k];

            return ret;
         }
         else
         {

            monomial ret(this->q, this->mon.size(), this->GF2_opt);

            for (unsigned k = 0; k < this->mon.size(); k++)
               ret[k] = this->mon[k] + x.mon[k];

            return ret;
         }
      }
};

#endif

