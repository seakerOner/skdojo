#ifndef INTTYPE_H
#define INTTYPE_H


typedef unsigned char           u8;
typedef unsigned short int     u16;
typedef unsigned int           u32;
typedef unsigned long long int u64;

typedef signed char             i8;
typedef signed short int       i16;
typedef signed int             i32;
typedef signed long long int   i64;

typedef float                  f32;
typedef double                 f64;
typedef u64                 size_t;
typedef u8                 boolean;

#define NULL (void *)0

#define FILL(ptr, c, len_bytes)             \
    do {                                    \
        for (u64 x = 0; x < len_bytes; x++) \
            *(((u8*)ptr)+x) = c;            \
    } while (0);

#define ALIGN_UP(addr, a) (((addr) + (a-1)) & ~(a-1))
#define ALIGN_DOWN(addr, a) ((addr) & ~(a-1))

#define TRUE  1 
#define FALSE 0

#define UNUSED(x) (void)x

#define KB(n) ((u64)(n * 1024ULL))
#define MB(n) ((u64)(n * (1024ULL * 1024ULL)))
#define GB(n) ((u64)(n * (1024ULL * 1024ULL * 1024ULL)))

#define POISON 0xDEDEDEDEDEDEDEDEULL


#endif
