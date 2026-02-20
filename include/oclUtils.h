// Header file for function prototypes

// enumerate opencl platforms
cl_int _query_platforms(cl_platform_id **platforms, cl_uint *platform_count);
// enumerate opencl devices on a platform
cl_int _query_devices(cl_platform_id *platform, cl_device_id ***devices, cl_uint **device_counts);
