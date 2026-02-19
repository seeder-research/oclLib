#ifndef OCL_H
#define OCL_H

/*
 * oclLib - a thin C wrapper around the OpenCL 1.2 C API
 *
 * Include this single header to pull in the full library interface:
 *
 *   ocl_error.h  - ocl_error_t return codes and ocl_strerror()
 *   ocl_ctx.h    - context creation, command queue, flush/finish
 *   ocl_buf.h    - device buffer allocation and host<->device transfers
 *   ocl_kernel.h - program compilation, kernel creation and dispatch
 */

#include "ocl_error.h"
#include "ocl_ctx.h"
#include "ocl_buf.h"
#include "ocl_kernel.h"

#endif /* OCL_H */
