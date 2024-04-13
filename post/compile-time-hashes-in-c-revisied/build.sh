mkdir -p build

cp StaticMurmur.hpp build/

for SIZE in 16 128 1024 2048 4096
do
    python3 gen.py $SIZE > build/hash$SIZE.cpp
done

TIMEFORMAT="%E"

#style=""
#style="-D CONSTEXPR_HASH"
style="-D ONLY_CONSTANTS "
#style="-D ONLY_CONSTANTS -D CONSTEXPR_HASH"

for COMPILER in g++ clang++
do
    compiler=COMPILER

    echo -----------------------
    echo $COMPILER
    echo -----------------------

    for OPT in O0 O2
    do
        opt=OPT
        for SIZE in 16 128 1024 2048 4096
        do
            size=SIZE
            CMD="${!compiler} build/hash${!size}.cpp $style -${!opt} -c -o build/hash_${!size}_${!compiler}_${!opt}.o"; echo $CMD; time $CMD
        done
    done
done
