#ifndef OCL_CTX_H
#define OCL_CTX_H

#include <CL/cl.h>
#include "ocl_error.h"

/*
 * ocl_ctx_t bundles a device, its context, and a single command queue.
 * Obtain one via ocl_ctx_create(); release it with ocl_ctx_destroy().
 */
typedef struct {
    cl_device_id     device;
    cl_context       context;
    cl_command_queue queue;
} ocl_ctx_t;

/*
 * Create a context and in-order command queue for the given device.
 * The device must remain valid for the lifetime of the context.
 *
 * Returns OCL_SUCCESS on success, or an error code on failure.
 */
ocl_error_t ocl_ctx_create(cl_device_id device, ocl_ctx_t *out);

/*
 * Release the command queue and context.  Safe to call with NULL.
 */
void ocl_ctx_destroy(ocl_ctx_t *ctx);

/*
 * Issue all previously-queued commands to the device without blocking.
 */
ocl_error_t ocl_queue_flush(ocl_ctx_t *ctx);

/*
 * Block until all previously-queued commands have completed.
 */
ocl_error_t ocl_queue_finish(ocl_ctx_t *ctx);

#endif /* OCL_CTX_H */
