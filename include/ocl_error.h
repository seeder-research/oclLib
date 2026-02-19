#ifndef OCL_ERROR_H
#define OCL_ERROR_H

typedef enum {
    OCL_SUCCESS = 0,
    OCL_ERR_CONTEXT_CREATE,
    OCL_ERR_QUEUE_CREATE,
    OCL_ERR_BUFFER_CREATE,
    OCL_ERR_BUFFER_WRITE,
    OCL_ERR_BUFFER_READ,
    OCL_ERR_PROGRAM_CREATE,
    OCL_ERR_PROGRAM_BUILD,
    OCL_ERR_KERNEL_CREATE,
    OCL_ERR_KERNEL_SET_ARG,
    OCL_ERR_KERNEL_ENQUEUE,
    OCL_ERR_QUEUE_FINISH,
    OCL_ERR_QUEUE_FLUSH,
    OCL_ERR_INVALID_ARG,
    /* platform / initialisation errors */
    OCL_ERR_NO_PLATFORMS,    /* no OpenCL platforms found on the system    */
    OCL_ERR_ALLOC,           /* host memory allocation failed               */
    OCL_ERR_DEVICE_QUERY,    /* clGetDeviceIDs returned an unexpected error */
    OCL_ERR_DEVICE_INDEX,    /* device_index >= number of matching devices  */
} ocl_error_t;

const char *ocl_strerror(ocl_error_t err);

#endif /* OCL_ERROR_H */
