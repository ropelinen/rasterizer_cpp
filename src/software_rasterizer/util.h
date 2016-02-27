#ifndef RPLNN_UTIL_H
#define RPLNN_UTIL_H

#include <algorithm>

/* All kinds of utility functions/macros.
 * In case the file starts to bloat just split to math etc. */

#define min3(a, b, c) std::min(std::min((a), (b)), (c))
#define max3(a, b, c) std::max(std::max((a), (b)), (c))

#endif /* RPLNN_UTIL_H */
