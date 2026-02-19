#include "ocl_ctx.h"

ocl_error_t ocl_ctx_create(cl_device_id device, ocl_ctx_t *out)
{
    if (!out)
        return OCL_ERR_INVALID_ARG;

    cl_int err;

    out->device  = device;
    out->context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS)
        return OCL_ERR_CONTEXT_CREATE;

    out->queue = clCreateCommandQueue(out->context, device, 0, &err);
    if (err != CL_SUCCESS) {
        clReleaseContext(out->context);
        out->context = NULL;
        return OCL_ERR_QUEUE_CREATE;
    }

    return OCL_SUCCESS;
}

void ocl_ctx_destroy(ocl_ctx_t *ctx)
{
    if (!ctx)
        return;
    if (ctx->queue) {
        clReleaseCommandQueue(ctx->queue);
        ctx->queue = NULL;
    }
    if (ctx->context) {
        clReleaseContext(ctx->context);
        ctx->context = NULL;
    }
}

ocl_error_t ocl_queue_flush(ocl_ctx_t *ctx)
{
    if (!ctx)
        return OCL_ERR_INVALID_ARG;
    return clFlush(ctx->queue) == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_QUEUE_FLUSH;
}

ocl_error_t ocl_queue_finish(ocl_ctx_t *ctx)
{
    if (!ctx)
        return OCL_ERR_INVALID_ARG;
    return clFinish(ctx->queue) == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_QUEUE_FINISH;
}
