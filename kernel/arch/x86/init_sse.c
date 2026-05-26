#include "../../inttype.h"

static boolean is_sse_supported() {
}

static u32 enable_sse() {
}

static i32 x86_enable_sse() {
    if ( !is_sse_supported() )
        return -1;

    return enable_sse();
}

