#include "ocl_error.h"

const char *ocl_strerror(ocl_error_t err)
{
    switch (err) {
    case OCL_SUCCESS:            return "success";
    case OCL_ERR_CONTEXT_CREATE: return "failed to create OpenCL context";
    case OCL_ERR_QUEUE_CREATE:   return "failed to create command queue";
    case OCL_ERR_BUFFER_CREATE:  return "failed to create device buffer";
    case OCL_ERR_BUFFER_WRITE:   return "failed to enqueue buffer write";
    case OCL_ERR_BUFFER_READ:    return "failed to enqueue buffer read";
    case OCL_ERR_PROGRAM_CREATE: return "failed to create program from source";
    case OCL_ERR_PROGRAM_BUILD:  return "failed to build program (see stderr for build log)";
    case OCL_ERR_KERNEL_CREATE:  return "failed to create kernel";
    case OCL_ERR_KERNEL_SET_ARG: return "failed to set kernel argument";
    case OCL_ERR_KERNEL_ENQUEUE: return "failed to enqueue ND-range kernel";
    case OCL_ERR_QUEUE_FINISH:   return "clFinish failed";
    case OCL_ERR_QUEUE_FLUSH:    return "clFlush failed";
    case OCL_ERR_INVALID_ARG:    return "invalid argument";
    default:                     return "unknown error";
    }
}
