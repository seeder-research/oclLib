/*
 * ocl_platform.c — global state definitions and ocl_platform_init().
 */

#include <stdlib.h>

#include "ocl_platform.h"

/*===========================================================================*
 * Global variable definitions                                               *
 *===========================================================================*/

cl_uint                  ocl_num_platforms = 0;
cl_platform_id          *ocl_platforms     = NULL;
ocl_platform_devices_t  *ocl_devices       = NULL;

cl_platform_id           ocl_platform      = NULL;
cl_device_id             ocl_device        = NULL;
cl_context               ocl_context       = NULL;

/*===========================================================================*
 * Internal helpers                                                           *
 *===========================================================================*/

/*
 * query_type — query all devices of @p type on @p platform, allocate a
 * buffer, and store the results in *out_devs / *out_count.
 *
 * CL_DEVICE_NOT_FOUND is treated as "zero devices of this type" and is not
 * propagated as an error.  The caller owns the returned allocation.
 */
static ocl_error_t query_type(cl_platform_id  platform,
                               cl_device_type  type,
                               cl_device_id  **out_devs,
                               cl_uint        *out_count)
{
    cl_uint      count = 0;
    cl_int       err;
    cl_device_id *buf;

    err = clGetDeviceIDs(platform, type, 0, NULL, &count);
    if (err == CL_DEVICE_NOT_FOUND || count == 0) {
        *out_devs  = NULL;
        *out_count = 0;
        return OCL_SUCCESS;
    }
    if (err != CL_SUCCESS) return OCL_ERR_DEVICE_QUERY;

    buf = (cl_device_id *)malloc(count * sizeof(cl_device_id));
    if (!buf) return OCL_ERR_ALLOC;

    err = clGetDeviceIDs(platform, type, count, buf, NULL);
    if (err != CL_SUCCESS) {
        free(buf);
        *out_devs  = NULL;
        *out_count = 0;
        return OCL_ERR_DEVICE_QUERY;
    }

    *out_devs  = buf;
    *out_count = count;
    return OCL_SUCCESS;
}

/*
 * free_device_lists — release every per-platform device array and the
 * ocl_devices array itself.  Resets ocl_devices to NULL.
 */
static void free_device_lists(void)
{
    cl_uint i;

    if (!ocl_devices) return;

    for (i = 0; i < ocl_num_platforms; i++) {
        free(ocl_devices[i].cpu);
        free(ocl_devices[i].gpu);
        free(ocl_devices[i].accel);
        free(ocl_devices[i].custom);
    }
    free(ocl_devices);
    ocl_devices = NULL;
}

/*
 * advance_index — walk @p count devices belonging to platform index
 * @p plat_idx.  For each device the running counter *total is compared to
 * @p target; if they match the platform and device are recorded in the
 * library globals.  *total is incremented for every device regardless of
 * whether a match occurred.
 */
static void advance_index(cl_uint       plat_idx,
                           cl_device_id *devs,
                           cl_uint       count,
                           cl_uint       target,
                           cl_uint      *total)
{
    cl_uint j;

    for (j = 0; j < count; j++) {
        if (*total == target) {
            ocl_platform = ocl_platforms[plat_idx];
            ocl_device   = devs[j];
        }
        (*total)++;
    }
}

/*===========================================================================*
 * ocl_platform_init                                                         *
 *===========================================================================*/

ocl_error_t ocl_platform_init(cl_uint device_index, cl_device_type device_type)
{
    ocl_error_t  err;
    cl_int        cl_err;
    cl_uint       total;
    cl_uint       i;

    /* ------------------------------------------------------------------ *
     * Release any previous library state                                  *
     * ------------------------------------------------------------------ */
    if (ocl_context) {
        clReleaseContext(ocl_context);
        ocl_context = NULL;
    }
    free_device_lists();
    free(ocl_platforms);
    ocl_platforms     = NULL;
    ocl_num_platforms = 0;
    ocl_platform      = NULL;
    ocl_device        = NULL;

    /* ------------------------------------------------------------------ *
     * Enumerate all platforms                                              *
     * ------------------------------------------------------------------ */
    cl_err = clGetPlatformIDs(0, NULL, &ocl_num_platforms);
    if (cl_err != CL_SUCCESS || ocl_num_platforms == 0)
        return OCL_ERR_NO_PLATFORMS;

    ocl_platforms = (cl_platform_id *)malloc(
        ocl_num_platforms * sizeof(cl_platform_id));
    if (!ocl_platforms) return OCL_ERR_ALLOC;

    cl_err = clGetPlatformIDs(ocl_num_platforms, ocl_platforms, NULL);
    if (cl_err != CL_SUCCESS) {
        err = OCL_ERR_DEVICE_QUERY;
        goto err_free_platforms;
    }

    /* ------------------------------------------------------------------ *
     * Allocate per-platform device storage                                *
     * ------------------------------------------------------------------ */
    ocl_devices = (ocl_platform_devices_t *)calloc(
        ocl_num_platforms, sizeof(ocl_platform_devices_t));
    if (!ocl_devices) {
        err = OCL_ERR_ALLOC;
        goto err_free_platforms;
    }

    /* ------------------------------------------------------------------ *
     * For every platform: store all devices by type, then advance the     *
     * running index counter over devices matching the requested type.     *
     * ------------------------------------------------------------------ */
    total = 0;

    for (i = 0; i < ocl_num_platforms; i++) {

        err = query_type(ocl_platforms[i], CL_DEVICE_TYPE_CPU,
                         &ocl_devices[i].cpu,   &ocl_devices[i].num_cpu);
        if (err != OCL_SUCCESS) goto err_free_all;

        err = query_type(ocl_platforms[i], CL_DEVICE_TYPE_GPU,
                         &ocl_devices[i].gpu,   &ocl_devices[i].num_gpu);
        if (err != OCL_SUCCESS) goto err_free_all;

        err = query_type(ocl_platforms[i], CL_DEVICE_TYPE_ACCELERATOR,
                         &ocl_devices[i].accel, &ocl_devices[i].num_accel);
        if (err != OCL_SUCCESS) goto err_free_all;

        err = query_type(ocl_platforms[i], CL_DEVICE_TYPE_CUSTOM,
                         &ocl_devices[i].custom, &ocl_devices[i].num_custom);
        if (err != OCL_SUCCESS) goto err_free_all;

        switch (device_type) {
        case CL_DEVICE_TYPE_CPU:
            advance_index(i, ocl_devices[i].cpu,    ocl_devices[i].num_cpu,
                          device_index, &total);
            break;

        case CL_DEVICE_TYPE_GPU:
            advance_index(i, ocl_devices[i].gpu,    ocl_devices[i].num_gpu,
                          device_index, &total);
            break;

        case CL_DEVICE_TYPE_ACCELERATOR:
            advance_index(i, ocl_devices[i].accel,  ocl_devices[i].num_accel,
                          device_index, &total);
            break;

        case CL_DEVICE_TYPE_CUSTOM:
            advance_index(i, ocl_devices[i].custom, ocl_devices[i].num_custom,
                          device_index, &total);
            break;

        default: /* CL_DEVICE_TYPE_ALL / CL_DEVICE_TYPE_DEFAULT */
            advance_index(i, ocl_devices[i].cpu,    ocl_devices[i].num_cpu,
                          device_index, &total);
            advance_index(i, ocl_devices[i].gpu,    ocl_devices[i].num_gpu,
                          device_index, &total);
            advance_index(i, ocl_devices[i].accel,  ocl_devices[i].num_accel,
                          device_index, &total);
            advance_index(i, ocl_devices[i].custom, ocl_devices[i].num_custom,
                          device_index, &total);
            break;
        }
    }

    /* ------------------------------------------------------------------ *
     * Confirm that a device was found at the requested index              *
     * ------------------------------------------------------------------ */
    if (!ocl_device) {
        err = OCL_ERR_DEVICE_INDEX;
        goto err_free_all;
    }

    /* ------------------------------------------------------------------ *
     * Create an OpenCL context for the selected device                    *
     * ------------------------------------------------------------------ */
    ocl_context = clCreateContext(NULL, 1, &ocl_device, NULL, NULL, &cl_err);
    if (cl_err != CL_SUCCESS) {
        ocl_context = NULL;
        err = OCL_ERR_CONTEXT_CREATE;
        goto err_free_all;
    }

    return OCL_SUCCESS;

    /* ------------------------------------------------------------------ *
     * Error unwind                                                         *
     * ------------------------------------------------------------------ */
err_free_all:
    free_device_lists();

err_free_platforms:
    free(ocl_platforms);
    ocl_platforms     = NULL;
    ocl_num_platforms = 0;
    ocl_platform      = NULL;
    ocl_device        = NULL;
    return err;
}
