/***
 * https://github.com/sgtlaugh/algovault/blob/master/code_library/hashing.cpp
 * 64-bit hashing for vectors or strings
 * Get the forward and reverse hash of any segment
 * Base is chosen randomly to prevent anti-hash cases from being constructed
 * 
 * Complexity - O(n) to build, O(1) for each hash query
 *
***/
#include <bits/stdc++.h>

using namespace std;

const int nax = 1000010;
const uint64_t mod = (1ULL << 61) - 1;

const uint64_t seed = chrono::system_clock::now().time_since_epoch().count();
const uint64_t base = mt19937_64(seed)() % (mod / 3) + (mod / 3);

uint64_t base_pow[nax];

int64_t modmul(uint64_t a, uint64_t b){
  uint64_t l1 = (uint32_t)a, h1 = a >> 32, l2 = (uint32_t)b, h2 = b >> 32;
  uint64_t l = l1 * l2, m = l1 * h2 + l2 * h1, h = h1 * h2;
  uint64_t ret = (l & mod) + (l >> 61) + (h << 3) + (m >> 29) + (m << 35 >> 3) + 1;
  ret = (ret & mod) + (ret >> 61);
  ret = (ret & mod) + (ret >> 61);
  return ret - 1;
}

void init(){
  base_pow[0] = 1;
  for (int i = 1; i < nax; i++){
    base_pow[i] = modmul(base_pow[i - 1], base);
  }
}

struct wasPolyHash{
  /// Remove suff vector and usage if reverse hash is not required for more speed
  vector<int64_t> pref, suff;

  wasPolyHash() {}

  template <typename T>
  wasPolyHash(const vector<T>& ar){
    if (!base_pow[0]) init();

    int n = ar.size();
    assert(n < nax);
    pref.resize(n + 3, 0), suff.resize(n + 3, 0);

    for (int i = 1; i <= n; i++){
      pref[i] = modmul(pref[i - 1], base) + ar[i - 1] + 997;
      if (pref[i] >= mod) pref[i] -= mod;
    }

    for (int i = n; i >= 1; i--){
      suff[i] = modmul(suff[i + 1], base) + ar[i - 1] + 997;
      if (suff[i] >= mod) suff[i] -= mod;
    }
  }
 
  wasPolyHash(const char* str)
    : wasPolyHash(vector<char> (str, str + strlen(str))) {}

  uint64_t get_hash(int l, int r){
    int64_t h = pref[r + 1] - modmul(base_pow[r - l + 1], pref[l]);
    return h < 0 ? h + mod : h;
  }

  uint64_t rev_hash(int l, int r){
    int64_t h = suff[l + 1] - modmul(base_pow[r - l + 1], suff[r + 2]);
    return h < 0 ? h + mod : h;
  }
};

template <int base> struct PolyHash {
  static const int Mod = 998'244'353;
  static int mod_mul(int x, int y) { return 1ll * x * y % Mod; }
  int n;
  std::vector<int> pref, suff, bpow;

  PolyHash() {}
  template <typename T_container> PolyHash(const T_container& v) {
    n = v.size();
    pref.resize(n + 1);
    suff.resize(n + 1);
    bpow.resize(n + 1);
    bpow[0] = 1;
    for (int i = 1; i <= n; i++) {
      bpow[i] = mod_mul(bpow[i - 1], base);
    }
    for (int i = 0; i < n; i++) {
      pref[i + 1] = mod_mul(pref[i], base) + v[i];
      if (pref[i + 1] >= Mod) pref[i + 1] -= Mod;
    }
    for (int i = n - 1; i >= 0; i--) {
      suff[i] = mod_mul(suff[i + 1], base) + v[i];
      if (suff[i] >= Mod) suff[i] -= Mod;
    }
  }

  int get_hash(int l, int r) {
    int64_t h = pref[r + 1] - mod_mul(bpow[r - l + 1], pref[l]);
    return h < 0 ? h + Mod : h;
  }
  int rev_hash(int l, int r) {
    int64_t h = suff[l] - mod_mul(bpow[r - l + 1], suff[r + 1]);
    return h < 0 ? h + Mod : h;
  }
};

int main(){
  wasPolyHash H = wasPolyHash("racecar");

  assert(H.get_hash(0, 6) == H.rev_hash(0, 6));
  assert(H.get_hash(1, 5) != H.rev_hash(0, 4));
  assert(H.get_hash(1, 1) == H.rev_hash(5, 5));
  assert(H.get_hash(1, 1) != H.rev_hash(5, 6));
  assert(H.get_hash(2, 4) == H.rev_hash(2, 4));

  H = wasPolyHash(vector<int> {1, 2, 3, 2, 1});
  assert(H.get_hash(0, 4) == H.rev_hash(0, 4));
  return 0;
}
