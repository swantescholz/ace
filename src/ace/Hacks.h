#ifndef ACE_HACKS_H_
#define ACE_HACKS_H_

#include "Common.h"

namespace ace {

using namespace std;


struct ScopeExit {
    ScopeExit(std::function<void(void)> g) : f(g) {}
    ~ScopeExit() { f(); }
    std::function<void(void)> f;
};

#define fora(X,Y,Z) for(int X = (Y), z=(int)(Z); X < z; ++X)
#define fori(X,Y) fora(i,X,Y)
#define forito(X) fora(i,0,X)

#define defer(code)  ScopeExit STRING_JOIN(scope_exit_, __LINE__)([&](){code;});
#define pln(X) std::cout << (X) << std::endl;
#define pnl std::cout << std::endl;
#define del( X ) {if( X ) delete ( X ); ( X ) = nullptr;}
#define delarr( X ) {if( X ) delete [] ( X ); ( X ) = nullptr;}
#define nil nullptr
#define self (*this)

const std::string tab("\t");
const std::string nl("\n");
const std::string estr("");
typedef long long int llint;
template<typename T> using sptr = std::shared_ptr<T>;

} // namespace ace





#endif /* HACKS_H_ */
