// to avoid long name warnings
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <iostream>
#include <stdexcept>  // runtime_error 

#include <eoEvalFuncPtr.h>
#include <other/external_eo>
#include <utils/eoRNG.h>

using namespace std;

struct UserDefStruct
{
    int a;
    float b;
    double c;
    enum Enum { just, another, test } d;
};

ostream& operator<<(ostream& os, const UserDefStruct& str)
{
    return os << str.a << ' ' << str.b << ' ' << str.c << ' ' << static_cast<int>(str.d) << ' ';
}

istream& operator>>(istream& is, UserDefStruct& str)
{
    is >> str.a;
    is >> str.b;
    is >> str.c;
    int i;
    is >> i;
    str.d = static_cast<UserDefStruct::Enum>(i);

    return is;
}


UserDefStruct RandomStruct()
{
    cout << "RandomStruct\n";

    UserDefStruct result;

    result.a = rng.random(5);
    result.b = rng.uniform();
    result.c = rng.uniform();
    result.d = UserDefStruct::another;

    return result;
}

// reading and writing


void UserDefMutate(UserDefStruct& a)
{
    cout << "UserDefMutate\n";
    a = RandomStruct(); // just for testing

    if (rng.flip(0.1f))
        a.d = UserDefStruct::test;
    else
        a.d = UserDefStruct::another;
}

void UserDefBinCrossover(UserDefStruct& a, const UserDefStruct& b)
{
    cout << "UserDefBinCrossover\n";

    if (rng.flip(0.5))
        a.a = b.a;
    if (rng.flip(0.5))
        a.b = b.b;
    if (rng.flip(0.5))
        a.c = b.c;
    if (rng.flip(0.5))
        a.d = b.d;
}

void UserDefQuadCrossover(UserDefStruct& a, UserDefStruct& b)
{
    cout << "UserDefQuadCrossover\n";
    if (rng.flip(0.5))
        swap(a.a, b.a);
    if (rng.flip(0.5))
        swap(a.b, b.b);
    if (rng.flip(0.5))
        swap(a.c, b.c);
    if (rng.flip(0.5))
        swap(a.d, b.d);
}

float UserDefEvalFunc(const UserDefStruct& a)
{
    cout << "UserDefEvalFunc\n";
    return a.b;
}

int main()
{
    typedef UserDefStruct External;
    typedef float FitnessType;
    typedef eoExternalEO<float, External> EoType;

    eoExternalInit<FitnessType, External> init(RandomStruct);
    eoExternalMonOp<FitnessType, External> mutate(UserDefMutate);
    eoExternalBinOp<FitnessType, External> cross1(UserDefBinCrossover);
    eoExternalQuadraticOp<FitnessType, External> cross2(UserDefQuadCrossover);

    // eoExternalEvalFunc<FitnessType, External>   eval(UserDefEvalFunc);

    EoType eo1 = init();
    EoType eo2 = init();

    cout << "before mutation " << eo1 << '\n';
    mutate(eo1);
    cout << "after mutation " << eo1 << '\n';
    cross1(eo1, eo2);
    cout << "after crossover " << eo1 << '\n';

    cross2(eo1,eo2);

    return 1;
};
