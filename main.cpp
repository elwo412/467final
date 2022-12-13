#include "cppmain.h"
extern "C" {
#include "cmain.h"
}

using namespace std;
using namespace NTL;

NTL_CLIENT

int td(ZZ n, long bound){
   //trial division
   PrimeSeq s;  // generates primes in sequence quickly
   long p;

   p = s.next();  // first prime is always 2
   while (p && p < bound-1) {
      if ((n % p) == 0) return COMPOSITE;
      p = s.next();  
   }
   return PRIME;
}

int step5(ZZ n, long r, long bound){
   ZZ_p::init(ZZ(n));

   //'binary' form
   ZZ n_bits = ZZ(n);

   long a = 2;

   //ZZ_pX P;
   //NTL::BuildIrred(P, 10);

   ZZ_pX q(r, 1); // (1)X^r
   q -= 1; // X^r - 1
   const ZZ_pXModulus qmod(q);

   ZZ_pX p_2Poly(1, 1); //X
   PowerMod(p_2Poly, p_2Poly, n, qmod); //X^n % X^r - 1
   p_2Poly += 2;

   ZZ_pX p_2Polycmp;

   cout << bound << endl;


   while (a <= bound){

      //p_2Poly += a; // X^n + a

      ZZ_pX pPoly(1, 1); //X
      pPoly += a; //X + a

      ZZ_pX temp = ZZ_pX(pPoly);
      //NEED TO IMPLEMENT SUCCESSIVE SQUARING
      ///*
      int k = 0;
      long size_n_bits = NumBits(n);
      //ZZ_pX *cached = (ZZ_pX *)malloc(sizeof(ZZ_pX)*size_n_bits);
      //cached[0] = ZZ_pX(temp);
      while (k < size_n_bits){
         if (bit(n_bits, k) == 1){
            //cout << "ADD TO TOTAL" << endl;
            if (k != 0) pPoly = MulMod(pPoly, temp, qmod);
            //cout << "[pPoly] LEADING DEG: " << deg(pPoly) << endl;
         }  
         temp = MulMod(temp, temp, qmod);
         //cached[k] = ZZ_pX(temp);
         //cout << "[temp] LEADING DEG: " << deg(temp) << endl;
         k++;
      }
      //*/
      //PowerMod(pPoly, pPoly, n, qmod); //(X + a)^n % X^r - 1
      //cout << "[final] LEADING DEG: " << deg(pPoly) << endl;

      SetCoeff(p_2Poly, 0, a);


      
      if (pPoly != p_2Poly) return COMPOSITE;


      //GEN p, p_2;
      //p = FpXQ_pow(gadd(x, a), n, q, n); //As defined in paper
      //p_2 = gadd(FpXQ_pow(x, n, q, n), a); //As defined in paper
      ////p_2 = gadd(gpow(x, gmod(n, r), DEFAULTPREC), a); /* X ^ (n % r) */
      
      //// Check if p and p_2 are equal
      //int res = gequal(p, p_2);
      //if (!res)
      //   return COMPOSITE;
      a += 1;
   }

   return PRIME;
}

int aks(ZZ n, char *n_pari){
   mpz_t n_mp;
   mpz_init(n_mp);

   stringstream ssa;
   ssa << n;
   mpz_set_str(n_mp, ssa.str().c_str(), 10);

   // (step 1)
   if (mpz_perfect_power_p(n_mp)){
      return COMPOSITE;
   }
   mpz_clear(n_mp);

   // (step 2) note: if r and n are not coprime, then skip this r
   long *step5bound = (long *)malloc(sizeof(long));
   long r = smallestR(n_pari, step5bound);

   // (step 3/4)
   divisibility_check();

   // (step 5)
   return step5(n,r, *step5bound);
}

int main(int argc, char* argv[])
{
   pari_init(7000000000, 10000000000);
   char n_str[1000];
   FILE* fp = fopen(argv[1], "r");
   print_test();
   clock_t t_start = clock();
   long ln;
   while (fscanf(fp, "%s", n_str) != EOF) {
      ZZ n = conv<ZZ>(n_str);
      if (n < 10) {ln = strtol(n_str,NULL,10); cout << "TD| " << n << ": " << td(n, ln) << endl; }
      else
         cout << "AKS| " << n << ": " << aks(n, n_str) << endl;
   }
   printf("Total Elapsed Time: %f\n", (double)(clock() - t_start) / (double)CLOCKS_PER_SEC);
   pari_close();
   return 0;
}