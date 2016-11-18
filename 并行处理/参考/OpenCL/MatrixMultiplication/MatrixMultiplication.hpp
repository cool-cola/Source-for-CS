#ifndef MATRIX_MULTIPLICATION_H_
#define MATRIX_MULTIPLICATION_H_

//HEADER FILES
#include <CL/cl.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>


/*** GLOBALS ***/
#define SDK_SUCCESS 0
#define SDK_FAILURE 1

/*
 * Input data is stored here.
 */
cl_float *inputA;
cl_float *inputB;
cl_int wA, hA, wB, hB;

/*
 * Output data is stored here.
 */
cl_float *output;

/* The memory buffer that is used as input/output for OpenCL kernel */
cl_mem   inputBufferA;
cl_mem   inputBufferB;
cl_mem   outputBuffer;

cl_context          context;
cl_device_id        *devices;
cl_command_queue    commandQueue;

cl_program program;

/* This program uses only one kernel and this serves as a handle to it */
cl_kernel  kernel;


/*** FUNCTION DECLARATIONS ***/
/*
 * OpenCL related initialisations are done here.
 * Context, Device list, Command Queue are set up.
 * Calls are made to set up OpenCL memory buffers that this program uses
 * and to load the programs into memory and get kernel handles.
 * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
 */
int initializeCL(void);

/*
 * * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
 */
int convertToString(const char * filename, std::string& str);

/*
 * This is called once the OpenCL context, memory etc. are set up,
 * the program is loaded into memory and the kernel handles are ready.
 * 
 * It sets the values for kernels' arguments and enqueues calls to the kernels
 * on to the command queue and waits till the calls have finished execution.
 *
 * It also gets kernel start and end time if profiling is enabled.
 * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
 */
int runCLKernels(void);

/**
 * Releases OpenCL resources (Context, Memory etc.)
 * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise
 */
int cleanupCL(void);

/**
 * Releases program's resources
 * @return returns SDK_SUCCESS on success and SDK_FAILURE otherwise 
 */
void cleanupHost(void);

/*
 * Prints no more than 256 elements of the given array.
 * Prints full array if length is less than 256.
 *
 * Prints Array name followed by elements.
 */
void print1DArray(
         const std::string arrayName, 
         float * arrayData, 
         const unsigned int length);


#endif  /* #ifndef TEMPLATE_H_ */
