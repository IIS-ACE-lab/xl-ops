#include <iostream>
#include <iomanip>
#include <getopt.h>

#include "bit.h"

#include "PRNG.h"
#include "XL.h"

#include "GF2.h"
#include "GF2_const.h"
#include "GF256.h"
#include "GF256_const.h"
#include "GF31.h"
#include "GF31_const.h"

using namespace std;


bool trace = false;
bool fcost = false;
bool bucket = false;
bool const_cost_average = true;
uint64_t perm_seed = 0;

bigint bm_ops;
bigint la_ops;

int deg_info(int q, int m, int n, int maxDeg = 1000);

template <typename field, typename const_field>
void field_cost()
{
   field a(0);
   field b(0);
   field c(0);

   c = a + b;
   cout << "bit ops add: " << bit::ops() << "\n";
   bit::clear_all();

   c = a.dbl();
   cout << "bit ops dbl: " << bit::ops() << "\n";
   bit::clear_all();

   c = a - b;
   cout << "bit ops sub: " << bit::ops() << "\n";
   bit::clear_all();

   c = a * b;
   cout << "bit ops mul: " << bit::ops() << "\n";
   bit::clear_all();

   c = a.inv();
   cout << "bit ops inv: " << bit::ops() << "\n";
   bit::clear_all();

   field::clear_all();

   cout << "\n";

   long total = 0;

   for (int i = 0; i < field::q; i++)
   {
      field a(0);
      field c(0);
      const_field b(i);

      c = a * b;

      total += (long)bit::ops();

      bit::clear_all();
   }

   cout << "avg bit ops mul const: "
     << std::setprecision(17)
     << ((double)total/field::q)
     << "\n";

   bit::clear_all();
   field::clear_all();
   const_field::clear_all();

   cout << "\n";

   if (fcost)
      exit(0);
}

void print_help(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  -q, --field <arg>     Specify the field (required argument).\n"
              << "                        Allowed values:\n"
              << "                            2: GF(2)\n"
              << "                          256: GF(256)\n"
              << "                           31: GF(31)\n"
              << "  -c  --const           Use constant field elements for system (no argument).\n"
              << "  -b  --bucket          Use constant with buckets (no argument, requires '-c').\n"
              << "  -s, --seed <arg>      Specify the seed (required argument).\n"
              << "      --perm <arg>      Specify the seed for system permutation (required argument).\n"
              << "      --trace           Enable trace output (no argument).\n"
              << "      --f-cost          Print field cost and exit.\n"
              << "  -h, --help            Show this help message and exit.\n"
              << std::endl;
}

int main(int argc, char* argv[])
{
   uint64_t seed = 0;

   int field = 256;

   int m = 10;
   int n = 5;
   int D = -1;

   bool fconst = false;

   struct option longopts[] = {
      {"field", required_argument, NULL, 'q'},
      {"const", no_argument, NULL, 'c'},
      {"bucket", no_argument, NULL, 'b'},
      {"seed", required_argument, NULL, 's'},
      {"perm", required_argument, NULL, 0},
      {"trace", no_argument, NULL, 0},
      {"f-cost", no_argument, NULL, 0},
      {"help", no_argument, NULL, 'h'},
      {0, 0, 0, 0}
   };


   while (1)
   {
      int longindex = 0;

      const int opt = getopt_long(argc, argv, "n:m:D:q:cbas:eh", longopts, &longindex);

      if (opt == -1) {
         break;
      }

      if (opt == 0) // Long options without short options return 0
      {
         if (std::string(longopts[longindex].name) == "perm")
            perm_seed = stoull(optarg, nullptr, 0);

         if (std::string(longopts[longindex].name) == "trace")
            trace = true;

         if (std::string(longopts[longindex].name) == "f-cost")
            fcost = true;

         if (std::string(longopts[longindex].name) == "help")
         {
             print_help(argv[0]);
             return 0;
         }
      }
      else
      {
         switch (opt) {
            case 'n':
               n = atoi(optarg);
               break;
            case 'm':
               m = atoi(optarg);
               break;
            case 'D':
               D = atoi(optarg);
               break;
            case 'q':
               field = atoi(optarg);
               break;
            case 'c':
               fconst = true;
               break;
            case 'b':
               bucket = true;
               break;
            case 'a':
               const_cost_average = true;
               break;
            case 'e':
               const_cost_average = false;
               break;
            case 's':
               seed = stoull(optarg, nullptr, 0);
               break;
            case 'h':
               print_help(argv[0]);
               return 0;
            default:
               fprintf(stderr, "Unknown option.\n");
               return 1;
         }
      }
   }

   if (D < 0)
      D = deg_info(field, m, n, 100);

   cout << "q: " << field << "  n: " << n << "  m: " << m << "  D: " << D << "\n\n";

   cout << "seed: " << seed << "\n";
   cout << "field: " << field << "\n";
   cout << "\n";

   bit::clear_all();
   random_init(seed);

   switch (field)
   {
      case 2:
         field_cost<GF2, GF2const>();

         if (fconst)
           XL<GF2, GF2const>(n, m, D);
         else
           XL<GF2, GF2>(n, m, D);
         break;
      case 31:
         field_cost<GF31, GF31const>();

         if (fconst)
           XL<GF31, GF31const>(n, m, D);
         else
           XL<GF31, GF31>(n, m, D);
         break;
      case 256:
         field_cost<GF256, GF256const>();

         if (fconst)
           XL<GF256, GF256const>(n, m, D);
         else
           XL<GF256, GF256>(n, m, D);
         break;
      default:
         cerr << "Field " << field << " not supported!\n";
         return -1;
   }

   return 0;
}
