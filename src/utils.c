/********
Utility functions
*********/
#include "CL/opencl.h"

// _query_platforms: enumerate opencl platforms found on system
// inputs:
//    platforms: the memory location to store the list
//               of platforms
// Returns:
//    cl_int based status of opencl API calls
cl_int _query_platforms(cl_platform_id **platforms, cl_uint *platform_count) {
    cl_int  err;

    platform_count = malloc(sizeof(cl_uint));
    if (!platform_count)
        return CL_OUT_OF_HOST_MEMORY;
    
    err = clGetPlatformIDs(0, NULL, platform_count);
    if (err != CL_SUCCESS)
        return err;

    *platforms = malloc((*platform_count) * sizeof(cl_platform_id));
    if (!*platforms)
        return CL_OUT_OF_HOST_MEMORY;

    err = clGetPlatformIDs(num_platforms, *platforms, NULL);
    if (err != CL_SUCCESS) {
        free(*platforms);
        *platforms = NULL;
    }

    return err;
}

// _query_devices: enumerate opencl devices on a platform
// inputs:
//    platform:      the memory location with the cl_platform_id
//                   to query
//    devices:       the list of devices
//    device_counts: the number of devices found for each type
//    devices[0] - CPU devices
//    devices[1] - GPU devices
//    devices[2] - accelerator devices
//    devices[3] - custom devices
// Returns:
//    cl_int based status of opencl API calls
cl_int _query_devices(cl_platform_id *platform, cl_device_id ***devices, cl_uint **device_counts) {
    cl_int err;
    cl_device_type types[4] = {
        CL_DEVICE_TYPE_CPU,
        CL_DEVICE_TYPE_GPU,
        CL_DEVICE_TYPE_ACCELERATOR,
        CL_DEVICE_TYPE_CUSTOM
    };

    *devices = (cl_device_id **)malloc(4 * sizeof(cl_device_id *));
    if (!*devices)
        return CL_OUT_OF_HOST_MEMORY;

    *device_counts = (cl_uint *)malloc(4 * sizeof(cl_uint));
    if (!*device_counts) {
        free(*devices);
        return CL_OUT_OF_HOST_MEMORY;
    }

    for (int i = 0; i < 4; i++) {
        (*device_counts)[i] = 0;
        (*devices)[i] = NULL;

        err = clGetDeviceIDs(*platform, types[i], 0, NULL, &(*device_counts)[i]);
        if (err == CL_DEVICE_NOT_FOUND) {
            continue;
        } else if (err != CL_SUCCESS) {
            /* Free previously allocated device arrays */
            for (int j = 0; j < i; j++)
                free((*devices)[j]);
            free(*devices);
            free(*device_counts);
            return err;
        }

        (*devices)[i] = (cl_device_id *)malloc((*device_counts)[i] * sizeof(cl_device_id));
        if (!(*devices)[i]) {
            for (int j = 0; j < i; j++)
                free((*devices)[j]);
            free(*devices);
            free(*device_counts);
            return CL_OUT_OF_HOST_MEMORY;
        }

        err = clGetDeviceIDs(*platform, types[i], (*device_counts)[i], (*devices)[i], NULL);
        if (err != CL_SUCCESS) {
            for (int j = 0; j <= i; j++)
                free((*devices)[j]);
            free(*devices);
            free(*device_counts);
            return err;
        }
    }

    return CL_SUCCESS;
}
