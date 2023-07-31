#pragma once

#ifndef XYUTILS_H__

#define XY_ALIGN(n, b) ((n) < 0 ? (n) & ~((b) - 1) : ((n + (b) - 1) & ~((b) - 1))

#endif // !XYUTILS_H__
