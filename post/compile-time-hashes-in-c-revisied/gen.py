import sys
import random
import string
import pymmh3

random.seed(1337)

str_len = 16
str_cnt = int(sys.argv[1])

strings = []

def as_unsigned(v):
    if v < 0:
        return '0x{:08x}'.format(v + 2**32)
    return '0x{:08x}'.format(v)

for i in range(str_cnt):
    strings.append(''.join(random.choice(string.ascii_lowercase) for _ in range(str_len)))

cpp = '''
#include <stdint.h>
#include "StaticMurmur.hpp"

#if defined(ONLY_CONSTANTS)
    #if CONSTEXPR_HASH
        {const_constexpr_cases}
    #else
        {const_prehash_cases}
    #endif
#else
    int switch_me(uint32_t val)
    {{
        switch(val)
        {{
        #if CONSTEXPR_HASH
            {constexpr_cases}
        #else
            {prehash_cases}
        #endif
            default:
                break;
        }}
        return 0;
    }}
#endif
'''

print(cpp.format(
    constexpr_cases = '\n            '.join(
        'case murmur::static_hash_x86_32("{}", 0): return {};'.format(s, i) for i, s in enumerate(strings)
    ),
    prehash_cases = '\n            '.join(
        'case {}: return {};'.format(as_unsigned(pymmh3.hash(s)), i) for i, s in enumerate(strings)
    ),
    const_constexpr_cases = '\n            '.join(
        'constexpr uint32_t const_{} = murmur::static_hash_x86_32("{}", 0);'.format(i, s) for i, s in enumerate(strings)
    ),
    const_prehash_cases = '\n            '.join(
        'constexpr uint32_t const_{} = {};'.format(i, as_unsigned(pymmh3.hash(s))) for i, s in enumerate(strings)
    )
))
