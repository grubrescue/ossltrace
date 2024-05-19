#pragma once

#define TCAT2(a, b) a##_##b

#define CAT2(a, b) TCAT2(a, b)
#define CAT3(a, b, c) CAT2(CAT2(a, b), c)
#define CAT4(a, b, c, d) CAT2(CAT3(a, b, c), d)
