#include <stdio.h>
#include <stdlib.h>
#include "ocl_kernel.h"

ocl_error_t ocl_program_create(ocl_ctx_t *ctx, const char *source,
                                ocl_program_t *out)
{
    if (!ctx || !source || !out)
        return OCL_ERR_INVALID_ARG;

    cl_int err;
    out->program = clCreateProgramWithSource(ctx->context, 1, &source,
                                             NULL, &err);
    if (err != CL_SUCCESS)
        return OCL_ERR_PROGRAM_CREATE;

    err = clBuildProgram(out->program, 1, &ctx->device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        /* Print the build log to stderr to aid debugging. */
        size_t log_size = 0;
        clGetProgramBuildInfo(out->program, ctx->device,
                              CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        if (log_size > 1) {
            char *log = (char *)malloc(log_size);
            if (log) {
                clGetProgramBuildInfo(out->program, ctx->device,
                                      CL_PROGRAM_BUILD_LOG,
                                      log_size, log, NULL);
                fprintf(stderr, "OpenCL build log:\n%s\n", log);
                free(log);
            }
        }
        clReleaseProgram(out->program);
        out->program = NULL;
        return OCL_ERR_PROGRAM_BUILD;
    }

    return OCL_SUCCESS;
}

void ocl_program_destroy(ocl_program_t *prog)
{
    if (!prog)
        return;
    if (prog->program) {
        clReleaseProgram(prog->program);
        prog->program = NULL;
    }
}

ocl_error_t ocl_kernel_create(ocl_ctx_t *ctx, ocl_program_t *prog,
                               const char *name, ocl_kernel_t *out)
{
    (void)ctx; /* ctx is not used but kept for API consistency */
    if (!prog || !name || !out)
        return OCL_ERR_INVALID_ARG;

    cl_int err;
    out->kernel = clCreateKernel(prog->program, name, &err);
    return err == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_KERNEL_CREATE;
}

ocl_error_t ocl_kernel_set_arg(ocl_kernel_t *kernel, cl_uint index,
                                size_t size, const void *value)
{
    if (!kernel)
        return OCL_ERR_INVALID_ARG;

    cl_int err = clSetKernelArg(kernel->kernel, index, size, value);
    return err == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_KERNEL_SET_ARG;
}

ocl_error_t ocl_kernel_enqueue(ocl_ctx_t *ctx, ocl_kernel_t *kernel,
                                cl_uint dims,
                                const size_t *global_work_size,
                                const size_t *local_work_size)
{
    if (!ctx || !kernel || !global_work_size)
        return OCL_ERR_INVALID_ARG;

    cl_int err = clEnqueueNDRangeKernel(ctx->queue, kernel->kernel,
                                        dims, NULL,
                                        global_work_size, local_work_size,
                                        0, NULL, NULL);
    return err == CL_SUCCESS ? OCL_SUCCESS : OCL_ERR_KERNEL_ENQUEUE;
}

void ocl_kernel_destroy(ocl_kernel_t *kernel)
{
    if (!kernel)
        return;
    if (kernel->kernel) {
        clReleaseKernel(kernel->kernel);
        kernel->kernel = NULL;
    }
}
