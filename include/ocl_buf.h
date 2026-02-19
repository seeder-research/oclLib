#ifndef OCL_BUF_H
#define OCL_BUF_H

#include <stddef.h>
#include <CL/cl.h>
#include "ocl_ctx.h"
#include "ocl_error.h"

/*
 * ocl_buf_t wraps a cl_mem object and its byte size.
 * Obtain one via ocl_buf_alloc(); release it with ocl_buf_free().
 */
typedef struct {
    cl_mem mem;
    size_t size;
} ocl_buf_t;

/*
 * Allocate a device buffer of `size` bytes.
 * `flags` is passed directly to clCreateBuffer (e.g. CL_MEM_READ_WRITE).
 *
 * Returns OCL_SUCCESS on success, or an error code on failure.
 */
ocl_error_t ocl_buf_alloc(ocl_ctx_t *ctx, size_t size, cl_mem_flags flags,
                           ocl_buf_t *out);

/*
 * Release the underlying cl_mem object.  Safe to call with NULL.
 */
void ocl_buf_free(ocl_buf_t *buf);

/*
 * Blocking write: copy `size` bytes from `src` into the device buffer
 * starting at byte `offset`.
 */
ocl_error_t ocl_buf_write(ocl_ctx_t *ctx, ocl_buf_t *buf, const void *src,
                           size_t size, size_t offset);

/*
 * Blocking read: copy `size` bytes from the device buffer (starting at
 * byte `offset`) into `dst`.
 */
ocl_error_t ocl_buf_read(ocl_ctx_t *ctx, ocl_buf_t *buf, void *dst,
                          size_t size, size_t offset);

#endif /* OCL_BUF_H */
