#ifndef BASICRANDGENERATOR_H
#define BASICRANDGENERATOR_H

#include "randlib_global.h"

enum GeneratorType {
    JKISS, // 2 ^ 1271
    JLKISS64 // 2 ^ 250
};


/**
 * @brief The BasicRandGenerator class
 */
template < char Generator >
class RANDLIBSHARED_EXPORT BasicRandGenerator
{

public:
    BasicRandGenerator() {}

    static unsigned long long variate();

    static constexpr unsigned long long maxValue() {
        return (Generator == JLKISS64) ? 18446744073709551615ULL : 4294967295UL;
    }
    static size_t maxDecimals();
};

#ifdef JLKISS64RAND
typedef BasicRandGenerator<JLKISS64RAND> RandGenerator;
#else
typedef BasicRandGenerator<JKISS> RandGenerator;
#endif


#endif // BASICRANDGENERATOR_H
