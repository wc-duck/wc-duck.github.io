#include <functional>

#include "ubench.h"

#define NOINLINE __attribute__((noinline))

constexpr int ITERS = 1000;

struct the_system_msg
{
    int i;
};

NOINLINE void std_function(int sys, std::function<void(const the_system_msg&)> cb)
{
    for(int i = 0; i < ITERS; ++i)
    {
        the_system_msg msg {sys + i};
        cb(msg);
    }
}

inline void std_function_inlined(int sys, std::function<void(const the_system_msg&)> cb)
{
    for(int i = 0; i < ITERS; ++i)
    {
        the_system_msg msg {sys + i};
        cb(msg);
    }
}

template<typename FUNC>
void inlined_functor(int sys, FUNC&& cb)
{
    for(int i = 0; i < ITERS; ++i)
    {
        the_system_msg msg {sys + i};
        cb(msg);
    }
}

NOINLINE void c_style(int sys, void(*cb)(const the_system_msg&, void*), void* userdata)
{
    for(int i = 0; i < ITERS; ++i)
    {
        the_system_msg msg {sys + i};
        cb(msg, userdata);
    }
}

template<typename FUNC>
void kih_reverse(int sys, FUNC&& cb)
{
    auto wrap = [](const the_system_msg& msg, void* userdata) {
        FUNC& f = *(FUNC*)userdata;
        f(msg);
    };
    c_style(sys, wrap, &cb);
}

struct small_capture { int i[1]; };
struct big_capture   { int i[16]; };

// std::function

UBENCH_EX(std_function, small)
{
    int output = 0; small_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        std_function(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};

UBENCH_EX(std_function, big)
{
    int output = 0; big_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        std_function(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};


// inlined std::function

UBENCH_EX(std_function_inline, small)
{
    int output = 0; small_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        std_function_inlined(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};

UBENCH_EX(std_function_inline, big)
{
    int output = 0; big_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        std_function_inlined(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};


// inlined

UBENCH_EX(inlined, small)
{
    int output = 0; small_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        inlined_functor(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};

UBENCH_EX(inlined, big)
{
    int output = 0; big_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        inlined_functor(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};


// c-style

UBENCH_EX(c_style, small)
{
    struct c_user_data
    {
        int* out;
        small_capture cap;
    } capture;

    static auto c_style_cb = [](const the_system_msg& msg, void* user_data)
    {
        c_user_data* capture = (c_user_data*)user_data;
        *capture->out += msg.i + capture->cap.i[0];
    };

    int output = 0;
    capture.out = &output;
	UBENCH_DO_BENCHMARK()
    {
        c_style(1337, c_style_cb, &capture);
        UBENCH_DO_NOTHING(&capture);
    }
};

UBENCH_EX(c_style, big)
{
    struct c_user_data
    {
        int* out;
        big_capture cap;
    } capture;

    static auto c_style_cb = [](const the_system_msg& msg, void* user_data)
    {
        c_user_data* capture = (c_user_data*)user_data;
        *capture->out += msg.i + capture->cap.i[0];
    };

    int output = 0;
    capture.out = &output;
	UBENCH_DO_BENCHMARK()
    {
        c_style(1337, c_style_cb, &capture);
        UBENCH_DO_NOTHING(&capture);
    }
};


// kih_reverse

UBENCH_EX(kih_reverse, small)
{
    int output = 0; small_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        kih_reverse(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};

UBENCH_EX(kih_reverse, big)
{
    int output = 0; big_capture capture{};
	UBENCH_DO_BENCHMARK()
    {
        kih_reverse(1337, [&output, capture](const the_system_msg& msg) { output += msg.i + capture.i[0]; });
        UBENCH_DO_NOTHING(&capture);
    }
};

UBENCH_STATE();

int main(int argc, const char *const argv[])
{
#if defined(__clang__)
    #define COMPILER_STR "clang"
#else
    #define COMPILER_STR "gcc"
#endif

#if defined(__OPTIMIZE__)
    #define OPTIMIZE_STR "02"
#else
    #define OPTIMIZE_STR "O0"
#endif

    printf(COMPILER_STR "-" OPTIMIZE_STR "\n");
    return ubench_main(argc, argv);
}
