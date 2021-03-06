#if !defined(_MSC_VER)

#include "libcopp/stack/stack_traits.h"

extern "C" {
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
}

//#if _POSIX_C_SOURCE >= 200112L

#include <algorithm>
#include <cmath>

#if !defined (SIGSTKSZ)
# define SIGSTKSZ (8 * 1024)
# define UDEF_SIGSTKSZ
#endif

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif


namespace copp {

    namespace detail {
        static std::size_t pagesize() {
            std::size_t size = ::sysconf(_SC_PAGESIZE);
            return size;
        }

        static rlimit stacksize_limit_() {
            rlimit limit;
            // conforming to POSIX.1-2001
            ::getrlimit( RLIMIT_STACK, & limit);
            return limit;
        }

        static rlimit stacksize_limit() {
            static rlimit limit = stacksize_limit_();
            return limit;
        }
    }

    bool stack_traits::is_unbounded() COPP_MACRO_NOEXCEPT {
        return RLIM_INFINITY == detail::stacksize_limit().rlim_max;
    }

    std::size_t stack_traits::page_size() COPP_MACRO_NOEXCEPT {
        return detail::pagesize();
    }

    std::size_t stack_traits::default_size() COPP_MACRO_NOEXCEPT {
        std::size_t size = 8 * minimum_size(); // 64 KB
        if (is_unbounded())
            return size;

        assert(maximum_size() >= minimum_size());
        return maximum_size() == size
               ? size
               : (std::min)(size, maximum_size());
    }

    std::size_t stack_traits::minimum_size() COPP_MACRO_NOEXCEPT {
        return SIGSTKSZ;
    }

    std::size_t stack_traits::maximum_size() COPP_MACRO_NOEXCEPT {
        if(is_unbounded())
            return std::numeric_limits<std::size_t>::max();
        return detail::stacksize_limit().rlim_max;
    }

    std::size_t stack_traits::round_to_page_size(std::size_t stacksize) COPP_MACRO_NOEXCEPT {
        // page size must be 2^N
        return static_cast<std::size_t>((stacksize + detail::pagesize() - 1) & (~(detail::pagesize() - 1)));
    }
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#ifdef UDEF_SIGSTKSZ
# undef SIGSTKSZ
#endif

#endif