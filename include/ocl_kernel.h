#ifndef OCL_KERNEL_H
#define OCL_KERNEL_H

#include <stddef.h>
#include <CL/cl.h>
#include "ocl_ctx.h"
#include "ocl_error.h"

/*
 * ocl_program_t wraps a compiled cl_program.
 * Build one with ocl_program_create(); release with ocl_program_destroy().
 */
typedef struct {
    cl_program program;
} ocl_program_t;

/*
 * ocl_kernel_t wraps a cl_kernel extracted from a compiled program.
 * Create one with ocl_kernel_create(); release with ocl_kernel_destroy().
 */
typedef struct {
    cl_kernel kernel;
} ocl_kernel_t;

/*
 * Compile and link a program from a NUL-terminated OpenCL C source string.
 * Build log is written to stderr on failure.
 *
 * Returns OCL_SUCCESS on success, or an error code on failure.
 */
ocl_error_t ocl_program_create(ocl_ctx_t *ctx, const char *source,
                                ocl_program_t *out);

/*
 * Release the underlying cl_program.  Safe to call with NULL.
 */
void ocl_program_destroy(ocl_program_t *prog);

/*
 * Extract a kernel by name from a compiled program.
 *
 * Returns OCL_SUCCESS on success, or an error code on failure.
 */
ocl_error_t ocl_kernel_create(ocl_ctx_t *ctx, ocl_program_t *prog,
                               const char *name, ocl_kernel_t *out);

/*
 * Bind a kernel argument.  Wraps clSetKernelArg directly:
 *   - for scalar types: pass &value and sizeof(type)
 *   - for device buffers: pass &buf.mem and sizeof(cl_mem)
 */
ocl_error_t ocl_kernel_set_arg(ocl_kernel_t *kernel, cl_uint index,
                                size_t size, const void *value);

/*
 * Enqueue an ND-range kernel dispatch.
 *
 * `dims`             : number of work dimensions (1–3)
 * `global_work_size` : total number of work-items in each dimension
 * `local_work_size`  : work-group size per dimension (may be NULL)
 *
 * The call is non-blocking; use ocl_queue_finish() to wait for completion.
 *
 * Returns OCL_SUCCESS on success, or an error code on failure.
 */
ocl_error_t ocl_kernel_enqueue(ocl_ctx_t *ctx, ocl_kernel_t *kernel,
                                cl_uint dims,
                                const size_t *global_work_size,
                                const size_t *local_work_size);

/*
 * Release the underlying cl_kernel.  Safe to call with NULL.
 */
void ocl_kernel_destroy(ocl_kernel_t *kernel);

#endif /* OCL_KERNEL_H */
