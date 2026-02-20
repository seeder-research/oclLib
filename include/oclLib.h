/**********
Overall header file
**********/

#ifdef __cplusplus
extern "C" {
#endif

#include "CL/opencl.h"
#include "oclQueue.h"
#include "oclBuffer.h"
#include "oclPlatform.h"
#include "oclUtils.h"

#ifdef __cplusplus
}
#endif

struct oclLibMain {
    oclLibHardware  *_oclLibHardware;
    cl_context      *_oclLibContext;
};

oclLibMain *_settings;

// Library initialization function
cl_int initializeOclLib(cl_uint idx, uint device_type_idx);
