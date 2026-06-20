#define TINYOBJLOADER_IMPLEMENTATION

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wall"
    #pragma GCC diagnostic ignored "-Wextra"
    #pragma GCC diagnostic ignored "-Wpedantic"
    #pragma GCC diagnostic ignored "-Wconversion"
    #pragma GCC diagnostic ignored "-Wshadow"
#endif

#if defined(_MSC_VER)
    #pragma warning(push, 0)
#endif

#include <tiny_obj_loader.h>

#if defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif