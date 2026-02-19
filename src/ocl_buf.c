#include "ocl_buf.h"

ocl_error_t ocl_buf_alloc(ocl_ctx_t *ctx, size_t size, cl_mem_flags flags,
                           ocl_buf_t *out)
{
    if (!ctx || !out || size == 0)
        return OCL_ERR_INVALID_ARG;

    cl_int err;
    out->mem  = clCreateBuffer(ctx->context, flags, size, NULL, &err);
    if (err != CL_SUCCESS)
        return OCL_ERR_BUFFER_CREATE;

    out->size = size;
    return OCL_SUCCESS;
}

void ocl_buf_free(ocl_buf_t *buf)
{
    if (!buf)
        return;
    if (buf->mem) {
        clReleaseMemObject(buf->mem);
        buf->mem  = NULL;
    }
    buf->size = 0;
}

ocl_error_t ocl_buf_write(ocl_ctx_t *ctx, ocl_buf_t *buf, const void *src,
                           size_t size, size_t offset)
{
    if (!ctx || !buf || !src)
        return OCL_ERR_INVALID_ARG;

    cl_int err = clEnqueueWriteBuffer(ctx->queue, buf->mem,
                                      CL_TRUE,      /* blocking */
                                      offset, size, src,
                                      0, NULL, NULL);
    return err == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_BUFFER_WRITE;
}

ocl_error_t ocl_buf_read(ocl_ctx_t *ctx, ocl_buf_t *buf, void *dst,
                          size_t size, size_t offset)
{
    if (!ctx || !buf || !dst)
        return OCL_ERR_INVALID_ARG;

    cl_int err = clEnqueueReadBuffer(ctx->queue, buf->mem,
                                     CL_TRUE,       /* blocking */
                                     offset, size, dst,
                                     0, NULL, NULL);
    return err == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_BUFFER_READ;
}
