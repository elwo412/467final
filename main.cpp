#include "cppmain.h"
extern "C" {
#include "cmain.h"
}

using namespace std;
using namespace NTL;

int current_mode;
Mat<long> p_factors;
int x_dim;
const int y_dim = 2;

NTL_CLIENT

int td(ZZ n, long bound){
   //trial division
   PrimeSeq s;  // generates primes in sequence quickly
   long p;
   long bound2 = ceil(bound/2);

   p = s.next();  // first prime is always 2
   while (p && p <= bound2) {
      if ((n % p) == 0) return COMPOSITE;
      p = s.next();  
   }
   return PRIME;
}

long td_factorization(ZZ n){
   //trial division
   PrimeSeq s;  // generates primes in sequence quickly
   long p;
   x_dim = 0;
   p_factors = Mat<long>();
   long bound;
   stringstream ss;
   ZZ t = n/2;
   ss << t;
   ss >> bound;

   p = s.next();  // first prime is always 2
   while (p && p <= bound) {
      int c = 1;
      while ((n % p) == 0){
         if (c == 1){ x_dim++; p_factors.SetDims(x_dim, y_dim);} 
         p_factors[x_dim-1].put(0,p);
         p_factors[x_dim-1].put(1,c);
         c++; //ha!
         n = n / p;
      }
      p = s.next();  
   }
   return p;
}

void fermat_factorization(ZZ n, ZZ bound){
   ZZ a;
   a = SqrRoot(n + 1); //SqrRoot = floor(a^{1/2})

   if (sqr(a) == n){
      x_dim++; p_factors.SetDims(x_dim, y_dim);
      stringstream s;
      long conv_a;
      s << a;
      s >> conv_a;
      p_factors[x_dim-1].put(0,conv_a);
      p_factors[x_dim-1].put(1,2);
      return;
   }
   ZZ b;
   while(1){
      ZZ b1 = (a * a) - n;
      b = SqrRoot(b1);
      if((b * b) == b1)
         break;
      else
         a += 1;
         if (a > bound) return;
   }
   x_dim++; p_factors.SetDims(x_dim, y_dim);
   stringstream s;
   long conv_a;
   s << (a-b);
   s >> conv_a;
   p_factors[x_dim-1].put(0,conv_a);
   p_factors[x_dim-1].put(1,1);
   x_dim++; p_factors.SetDims(x_dim, y_dim);
   s << (a+b);
   s >> conv_a;
   p_factors[x_dim-1].put(0,conv_a);
   p_factors[x_dim-1].put(1,1);
   return;
}

void ask_for_mode(string *buffer){
   int option;
   string n;
   cout << "FINAL PROJECT\n-----------\nOptions: 1. Run routine on single number N\n2. Run routine on file input\n\n" << endl << "Option (1|2): ";
   cin >> option;
   if (option == 2){
      current_mode = FILE_INPUT;
      cout << "\x1B[2J\x1B[H";
      cout << "File Name: ";
      cin >> n;
      cout << "\x1B[2J\x1B[H";
   } else {
      current_mode = FULL_FACTOR_N;
      cout << "\x1B[2J\x1B[H";
      cout << "Input N: ";
      cin >> n;
   }

   *buffer = n;
}

int main(int argc, char* argv[])
{
   string *inputstring = (string *)malloc(sizeof(string));
   if (!argv[1]) ask_for_mode(inputstring);
   else current_mode = FILE_INPUT;
   if (current_mode == FILE_INPUT){
      pari_init(7000000000, 10000000000);
      char n_str[1000];
      FILE *fp;
      if (!argv[1]) FILE* fp = fopen(inputstring->c_str(), "r");
      else fp = fopen(argv[1], "r");
      print_test();
      clock_t t_start = clock();
      long ln;
      while (fscanf(fp, "%s", n_str) != EOF) {
         ZZ n = conv<ZZ>(n_str);
         if (n < 1000) {ln = strtol(n_str,NULL,10); cout << "TD| " << n << ": " << ( td(n, ln)  ? "COMPOSITE" : "PRIME" ) << endl; }
         else
            cout << "AKS| " << n << ": " << (aks(n_str) ? "COMPOSITE" : "PRIME" ) << endl;
      }
      printf("Total Elapsed Time: %f\n", (double)(clock() - t_start) / (double)CLOCKS_PER_SEC);
      pari_close();
   } else {
      pari_init(7000000000, 10000000000);
      char n_str[1000];
      strcpy(n_str, inputstring->c_str());
      auto start = chrono::high_resolution_clock::now();
      int aks_result = aks(n_str);
      cout << "AKS| " << *inputstring << ": " << (aks_result ? "COMPOSITE" : "PRIME" ) << endl;
      if (aks_result == COMPOSITE){
         ZZ n = conv<ZZ>(n_str);
         ZZ bound = SqrRoot(n+1);
         td_factorization(n);
         //fermat_factorization(n, bound);
         cout << "Factorization| " << *inputstring << ": " << endl;
         cout << p_factors;
      }
      auto stop = chrono::high_resolution_clock::now();
      auto duration_s = chrono::duration_cast<chrono::seconds>(stop - start);
      auto duration_ms = chrono::duration_cast<chrono::milliseconds>(stop - start);
      auto duration_us = chrono::duration_cast<chrono::microseconds>(stop - start);
      cout << "\n\nTotal Elapsed Time: " << duration_s.count() << "." << duration_ms.count()-duration_s.count()*1000 << "." << duration_us.count()-duration_ms.count()*1000 << " (seconds.ms.us)\n";
      
      pari_close();
   }

   free(inputstring);
   
   return 0;
}