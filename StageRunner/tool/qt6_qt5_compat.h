#ifndef QT6_QT5_COMPAT_H
#define QT6_QT5_COMPAT_H

#include <Qt>

#ifdef IS_QT6

#else

namespace Qt {
#define MiddleButton MidButton
}

#endif


#endif // QT6_QT5_COMPAT_H
