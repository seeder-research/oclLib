#ifndef OCL_PLATFORM_H
#define OCL_PLATFORM_H

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

#include "ocl_error.h"

/*
 * ocl_platform_devices_t — all devices found on a single platform,
 * stored separately for each OpenCL device type.
 *
 * One element of this struct is kept per platform in ocl_devices[].
 */
typedef struct {
    cl_device_id *cpu;        /* CL_DEVICE_TYPE_CPU         */
    cl_uint       num_cpu;

    cl_device_id *gpu;        /* CL_DEVICE_TYPE_GPU         */
    cl_uint       num_gpu;

    cl_device_id *accel;      /* CL_DEVICE_TYPE_ACCELERATOR */
    cl_uint       num_accel;

    cl_device_id *custom;     /* CL_DEVICE_TYPE_CUSTOM (OpenCL 1.2+) */
    cl_uint       num_custom;
} ocl_platform_devices_t;

/*
 * Global state populated by ocl_platform_init().
 *
 * These are shared by every other module in the library.  Do not modify
 * them directly; call ocl_platform_init() to change the active device.
 */

/** Total number of OpenCL platforms found on the system. */
extern cl_uint                  ocl_num_platforms;

/** Array of all platform IDs (length: ocl_num_platforms). */
extern cl_platform_id          *ocl_platforms;

/**
 * Per-platform device lists (length: ocl_num_platforms).
 * Devices are stored separately for each device type.
 */
extern ocl_platform_devices_t  *ocl_devices;

/** Platform that owns the device selected by ocl_platform_init(). */
extern cl_platform_id           ocl_platform;

/** Device selected by ocl_platform_init(). */
extern cl_device_id             ocl_device;

/** OpenCL context created for the selected device by ocl_platform_init(). */
extern cl_context               ocl_context;

/*
 * ocl_platform_init — initialise the library.
 *
 * Enumerates every OpenCL platform on the system.  For each platform all
 * devices are queried and stored in ocl_devices[] organised by type (CPU,
 * GPU, accelerator, custom).
 *
 * A running counter tracks how many devices of @p device_type have been
 * seen across all platforms (in platform-enumeration order).  When that
 * counter equals @p device_index the corresponding platform is stored in
 * ocl_platform, the device is stored in ocl_device, and the counter is
 * advanced.  After all platforms have been processed a context is created
 * for the selected device and stored in ocl_context.
 *
 * Calling this function again releases the previous context and
 * reinitialises all global state.
 *
 * Queues, buffers, programs, and kernels created by the rest of the library
 * will use ocl_device and ocl_context set here.
 *
 * @param device_index  0-based index of the desired device among all devices
 *                      of @p device_type found across the entire system.
 * @param device_type   CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU,
 *                      CL_DEVICE_TYPE_ACCELERATOR, CL_DEVICE_TYPE_CUSTOM,
 *                      CL_DEVICE_TYPE_ALL, or CL_DEVICE_TYPE_DEFAULT.
 *                      The latter two count every device regardless of type.
 *
 * @return OCL_SUCCESS         on success.
 *         OCL_ERR_NO_PLATFORMS if no OpenCL platforms exist.
 *         OCL_ERR_ALLOC        if a memory allocation fails.
 *         OCL_ERR_DEVICE_QUERY if a device enumeration call fails unexpectedly.
 *         OCL_ERR_DEVICE_INDEX if device_index >= number of matching devices.
 *         OCL_ERR_CONTEXT_CREATE if the OpenCL context cannot be created.
 */
ocl_error_t ocl_platform_init(cl_uint device_index, cl_device_type device_type);

#endif /* OCL_PLATFORM_H */
