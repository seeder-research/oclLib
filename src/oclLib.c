#include "oclLib.h"

cl_int initializeOclLib(cl_uint idx, uint device_type_idx) {
    // If device_type_idx is out of range, error out
    if (device_type_idx > 3)
        return CL_INVALID_VALUE;

    _settings = malloc(sizeof(oclLibMain));
    _settings->_oclLibHardware = malloc(sizeof(oclLibHardware));

    cl_int res = _query_platforms(_settings->_oclHardware->platforms, _settings->_oclHardware->num_platforms);

    if (res != CL_SUCCESS) {
        free(_settings->_oclLibHardware);
        free(_settings);
        return res;
    }

    cl_uint num_platforms = *(_settings->_oclHardware->num_platforms);
    _settings->_oclLibHardware->devices = malloc(num_platforms * sizeof(**cl_device_id));
    _settings->_oclLibHardware->num_devices = malloc(num_platforms * sizeof(*cl_uint));

    // Track total number of devices found
    cl_uint total_num_devices = 0;

    // Go through all platforms and enumerate all devices on every platform
    for (cl_uint i = 0; i < *(_settings->_oclHardware->num_platforms); i++) {
        res = _query_devices(&(_settings->_oclHardware->platforms[i]), &(_settings->_oclLibHardware->devices[i]), &(_settings->_oclLibHardware->num_devices[i]));

        // Free previously allocated device arrays if unable to query a platform for devices
        if (res != CL_SUCCESS) {
            for (cl_uint j = 0; j < i; j++) {
                cl_device_id **device_list = _settings->_oclLibHardware->devices[j];
                cl_uint *device_counts = _settings->_oclLibHardware->num_devices[j];
                for (uint k = 0; k < 4; k++)
                    free(device_list[k]);

                free(device_counts);
            }
            free(_settings->_oclLibHardware);
            free(_settings);
            return res;
        }

        total_num_devices_of_type += _settings->_oclLibHardware->num_devices[i][device_type_idx];
    }

    // Fall back to first device found if wanted index exceeds available devices of desired type
    uint dev_num = (idx < total_num_devices_of_type) ? idx : 0;

    for (cl_uint i = 0; i < *(_settings->_oclHardware->num_platforms); i++) {
        cl_uint total_devs = _settings->_oclHardware->num_devices[i][device_type_idx];
        for (cl_uint j = 0; j < total_devs; j++) {
            if (dev_num == 0) {
                 _settings->_oclHardware->platform = &(_settings->_oclHardware->platform[0][i]);
                 _settings->_oclHardware->device = &(_settings->_oclHardware->devices[i][device_type_idx][j]);
                 goto exit_device_selection_loop; // __dev_selection_loop
            }
            dev_num--;
        }
    }

exit_device_selection_loop:
    // target __dev_selection_loop
    _settings->_oclLibContext = malloc(sizeof(cl_context));

    return CL_SUCCESS;
}

// Initialize the context with a single device. Only run after running initializeOclLib
cl_int initializeSingleDeviceContext() {
    cl_int res = CL_INVALID_OPERATION;
    if (_settings->_oclLibContext) {
        cl_context_properties properties[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(*(_settings->_oclHardware->platform)), 0 };
        *(_settings->_oclLibContext) = clCreateContext(&properties[0], 1, _settings->_oclHardware->device, NULL, NULL, &res);
    }
    return res;
}
