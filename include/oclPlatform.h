/**********
oclPlatform.h
**********/

cl_device_type _oclDevice_types[4] = {
    CL_DEVICE_TYPE_CPU,
    CL_DEVICE_TYPE_GPU,
    CL_DEVICE_TYPE_ACCELERATOR,
    CL_DEVICE_TYPE_CUSTOM
};

struct oclLibHardware {
    // Platform
    cl_platform_id **platforms;
    cl_platform_id  *platform;
    cl_uint         *num_platforms;

    // Devices
    cl_device_id ***devices;
    cl_device_id   *device;
    cl_uint       **num_devices;
};
