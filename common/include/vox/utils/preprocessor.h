#pragma once

#define PP_STRINGIZE(M) #M
#define PP_STRINGIZE_VALUE(M) PP_STRINGIZE(M)

#define CONCAT_IMPL__(s1, s2) s1##s2
#define PP_CONCATENATE(s1, s2) CONCAT_IMPL__(s1, s2)

#ifdef __COUNTER__
    #define ANONYMOUS_VAR(Prefix) PP_CONCATENATE(Prefix, __COUNTER__)
#else
    #define ANONYMOUS_VAR(Prefix) PP_CONCATENATE(Prefix, _LINE__)
#endif


#ifdef _WIN32
#define VOX_PLATFORM_HEADER_DIR_NAME windows
#else
#warning "Unknown platform dir name"
#endif

#define VOX_PLATFORM_HEADER(FileName) PP_STRINGIZE_VALUE(vox/platform/VOX_PLATFORM_HEADER_DIR_NAME/FileName)


#if defined(_MSC_VER)
    #define VOX_ABSTRACT_TYPE __declspec(novtable)
#else
    #define VOX_ABSTRACT_TYPE
#endif
 