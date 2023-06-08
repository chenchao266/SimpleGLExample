/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */
 
 /*
* Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
*
* Please refer to the NVIDIA end user license agreement (EULA) associated
* with this source code for terms and conditions that govern your use of
* this software. Any use, reproduction, disclosure, or distribution of
* this software and related documentation outside the terms of the EULA
* is strictly prohibited.
*
*/


/* CUda UTility Library */

#pragma once

#ifdef _WIN32
#   pragma warning( disable : 4996 ) // disable deprecated warning 
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
 

    ////////////////////////////////////////////////////////////////////////////
    //! CUT bool type
    ////////////////////////////////////////////////////////////////////////////
    enum CUTBoolean 
    {
        CUTFalse = 0,
        CUTTrue = 1
    };
 
    //! Macros

// This is for the CUTIL bank checker
#ifdef _DEBUG
    #if __DEVICE_EMULATION__
        // Interface for bank conflict checker
    #define CUT_BANK_CHECKER( array, index)                                      \
        (cutCheckBankAccess( threadIdx.x, threadIdx.y, threadIdx.z, blockDim.x,  \
        blockDim.y, blockDim.z,                                                  \
        __FILE__, __LINE__, #array, index ),                                     \
        array[index])
    #else
    #define CUT_BANK_CHECKER( array, index)  array[index]
    #endif
#else
    #define CUT_BANK_CHECKER( array, index)  array[index]
#endif

#  define CU_SAFE_CALL_NO_SYNC( call ) {                                     \
    CUresult err = call;                                                     \
    if( CUDA_SUCCESS != err) {                                               \
        fprintf(stderr, "Cuda driver error %x in file '%s' in line %i.\n",   \
                err, __FILE__, __LINE__ );                                   \
        exit(EXIT_FAILURE);                                                  \
    } }

#  define CU_SAFE_CALL( call )       CU_SAFE_CALL_NO_SYNC(call);

#  define CU_SAFE_CTX_SYNC( ) {                                              \
    CUresult err = cuCtxSynchronize();                                       \
    if( CUDA_SUCCESS != err) {                                               \
        fprintf(stderr, "Cuda driver error %x in file '%s' in line %i.\n",   \
                err, __FILE__, __LINE__ );                                   \
        exit(EXIT_FAILURE);                                                  \
    } }

#  define CUDA_SAFE_CALL_NO_SYNC( call) {                                    \
    cudaError err = call;                                                    \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",        \
                __FILE__, __LINE__, cudaGetErrorString( err) );              \
        exit(EXIT_FAILURE);                                                  \
    } }

#  define CUDA_SAFE_CALL( call)     CUDA_SAFE_CALL_NO_SYNC(call);                                            \

#  define CUDA_SAFE_THREAD_SYNC( ) {                                         \
    cudaError err = cudaDeviceSynchronize();                                 \
    if ( cudaSuccess != err) {                                               \
        fprintf(stderr, "Cuda error in file '%s' in line %i : %s.\n",        \
                __FILE__, __LINE__, cudaGetErrorString( err) );              \
    } }

#  define CUFFT_SAFE_CALL( call) {                                           \
    cufftResult err = call;                                                  \
    if( CUFFT_SUCCESS != err) {                                              \
        fprintf(stderr, "CUFFT error in file '%s' in line %i.\n",            \
                __FILE__, __LINE__);                                         \
        exit(EXIT_FAILURE);                                                  \
    } }

#  define CUT_SAFE_CALL( call)                                               \
    if( CUTTrue != call) {                                                   \
        fprintf(stderr, "Cut error in file '%s' in line %i.\n",              \
                __FILE__, __LINE__);                                         \
        exit(EXIT_FAILURE);                                                  \
    } 

    //! Check for CUDA error
#ifdef _DEBUG
#  define CUT_CHECK_ERROR(errorMessage) {                                    \
    cudaError_t err = cudaGetLastError();                                    \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    err = cudaDeviceSynchronize();                                           \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    }
#else
#  define CUT_CHECK_ERROR(errorMessage) {                                    \
    cudaError_t err = cudaGetLastError();                                    \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    }
#endif

    //! Check for malloc error
#  define CUT_SAFE_MALLOC( mallocCall ) {                                    \
    if( !(mallocCall)) {                                                     \
        fprintf(stderr, "Host malloc failure in file '%s' in line %i\n",     \
                __FILE__, __LINE__);                                         \
        exit(EXIT_FAILURE);                                                  \
    } } while(0);

    //! Check if condition is true (flexible assert)
#  define CUT_CONDITION( val)                                                \
    if( CUTFalse == cutCheckCondition( val, __FILE__, __LINE__)) {           \
        exit(EXIT_FAILURE);                                                  \
    }

#if __DEVICE_EMULATION__

#  define CUT_DEVICE_INIT(ARGC, ARGV)

#else

#  define CUT_DEVICE_INIT(ARGC, ARGV) {                                      \
    int deviceCount;                                                         \
    CUDA_SAFE_CALL_NO_SYNC(cudaGetDeviceCount(&deviceCount));                \
    if (deviceCount == 0) {                                                  \
        fprintf(stderr, "cutil error: no devices supporting CUDA.\n");       \
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    int dev = 0;                                                             \
    cutGetCmdLineArgumenti(ARGC, (const char **) ARGV, "device", &dev);      \
	if (dev < 0) dev = 0;                                                    \
    if (dev > deviceCount-1) dev = deviceCount - 1;                          \
    cudaDeviceProp deviceProp;                                               \
    CUDA_SAFE_CALL_NO_SYNC(cudaGetDeviceProperties(&deviceProp, dev));       \
    if (deviceProp.major < 1) {                                              \
        fprintf(stderr, "cutil error: device does not support CUDA.\n");     \
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    if (cutCheckCmdLineFlag(ARGC, (const char **) ARGV, "quiet") == CUTFalse) \
        fprintf(stderr, "Using device %d: %s\n", dev, deviceProp.name);       \
    CUDA_SAFE_CALL(cudaSetDevice(dev));                                      \
}


    //! Check for CUDA context lost
#  define CUDA_CHECK_CTX_LOST(errorMessage) {                                \
    cudaError_t err = cudaGetLastError();                                    \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    err = cudaDeviceSynchronize();                                           \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    } }

//! Check for CUDA context lost
#  define CU_CHECK_CTX_LOST(errorMessage) {                                  \
    cudaError_t err = cudaGetLastError();                                    \
    if( CUDA_ERROR_INVALID_CONTEXT != err) {                                 \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    err = cudaDeviceSynchronize();                                           \
    if( cudaSuccess != err) {                                                \
        fprintf(stderr, "Cuda error: %s in file '%s' in line %i : %s.\n",    \
                errorMessage, __FILE__, __LINE__, cudaGetErrorString( err) );\
        exit(EXIT_FAILURE);                                                  \
    } }


#endif

#  define CUT_DEVICE_INIT_DRV(cuDevice, ARGC, ARGV) {                        \
    cuDevice = 0;                                                            \
    int deviceCount = 0;                                                     \
    CUresult err = cuInit(0);                                                \
    if (CUDA_SUCCESS == err)                                                 \
        CU_SAFE_CALL_NO_SYNC(cuDeviceGetCount(&deviceCount));                \
    if (deviceCount == 0) {                                                  \
        fprintf(stderr, "cutil error: no devices supporting CUDA\n");        \
        exit(EXIT_FAILURE);                                                  \
    }                                                                        \
    int dev = 0;                                                             \
    cutGetCmdLineArgumenti(ARGC, (const char **) ARGV, "device", &dev);      \
	if (dev < 0) dev = 0;                                                    \
    if (dev > deviceCount-1) dev = deviceCount - 1;                          \
    CU_SAFE_CALL_NO_SYNC(cuDeviceGet(&cuDevice, dev));                       \
    char name[100];                                                          \
    cuDeviceGetName(name, 100, cuDevice);                                    \
    if (cutCheckCmdLineFlag(ARGC, (const char **) ARGV, "quiet") == CUTFalse) \
        fprintf(stderr, "Using device %d: %s\n", dev, name);                  \
}

#define CUT_EXIT(argc, argv)                                                 \
    if (!cutCheckCmdLineFlag(argc, (const char**)argv, "noprompt")) {        \
        printf("\nPress ENTER to exit...\n");                                \
        fflush( stdout);                                                     \
        fflush( stderr);                                                     \
        getchar();                                                           \
    }                                                                        \
    exit(EXIT_SUCCESS);


#ifdef __cplusplus
}
#endif  // #ifdef _DEBUG (else branch)
