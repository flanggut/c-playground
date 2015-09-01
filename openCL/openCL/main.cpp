#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
//#include <exception>
#include <math.h>

#include "clpp.h"
#include "timer.h"


bool matricesEqual(float * a, float * b, unsigned int size)
{
    // comparing results
	float const epsilon = 0.00001f;
	for(unsigned int i = 0; i < size; ++i)
		if(fabs(a[i] - b[i]) > epsilon)
			return false;
    return true;
}


void matrixMulCPU( float* a, float* b, float* c, unsigned int widthA, unsigned int heightA, unsigned int widthB)
{
	// matrix multiplication on cpu. Solution suboptimal
	for(unsigned int y = 0; y < heightA; ++y)
	{
		for(unsigned int x = 0; x < widthB; ++x)
		{
			float result = 0.0f;
			for(unsigned int i = 0; i < widthA; ++i)
				result += a[y * widthA + i] * b[i * widthB + x];
			c[y * widthB + x] = result;
		}
	}
}


void
read_file (std::string const& filename, std::string* data)
{
    std::ifstream in(filename.c_str(), std::ios::binary);
    if (!in)
        throw std::runtime_error(::strerror(errno));
    
    in.seekg(0, std::ios::end);
    data->resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&data->at(0), data->size());
    in.close();
}


bool matrixMulGPU(cl::Context const & context, cl::CommandQueue * queue,
                  cl::Kernel * kernel, float* a, float* b, float* c,
                  unsigned int widthA, unsigned int heightA, unsigned int widthB)
{
	unsigned int sizeA = widthA * heightA;
	unsigned int sizeB = widthB * widthA;
	unsigned int sizeC = widthB * heightA;
    
    
    // create buffers for OpenCL Device
    cl::Buffer bufferA, bufferB, bufferC;
    try {
        // create input buffers and copy from host
        bufferA = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * sizeA, a);
        bufferB = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * sizeB, b);
        // create output buffer
        bufferC = cl::Buffer(context, CL_MEM_WRITE_ONLY,  sizeC * sizeof(float), NULL);
    } catch (...) {
        return false;
    }
    
    kernel->setArg(0, bufferC);
    kernel->setArg(1, bufferA);
    kernel->setArg(2, bufferB);
    kernel->setArg(3, widthA);
    kernel->setArg(4, heightA);
    kernel->setArg(5, widthB);

    /* set kernel launch configuration */
    // Size of work group
    cl::NDRange localWorkSize(8, 8);
    // Global size
    cl::NDRange globalWorkSize(widthB, heightA);
	   
	// launch kernel
    cl::Event event;
    queue->enqueueNDRangeKernel(*kernel, cl::NDRange(), globalWorkSize, localWorkSize, NULL, &event);
    event.wait();
    
    queue->enqueueReadBuffer(bufferC, true, 0, sizeC * sizeof(float), c);
    
    queue->finish();
	return true;
}


cl::Kernel compileKernel(cl::Context const& context,
                         std::vector<cl::Device> const & devices,
                         std::string const& filename,
                         std::string const& kernelname)
{
    std::ifstream kernelFile(filename.c_str());
    if (! kernelFile.is_open())
        throw ("Cannot open source file for compiling!");
    
    std::string source;
    read_file(filename, &source);
    cl::Program::Sources sources(1, std::make_pair(source.c_str(), source.length()));
    cl::Program matrixMulProgram(context, sources);
    if (matrixMulProgram.build(devices) != 0)
        std::cout << "Error: " << matrixMulProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
    cl::Kernel matrixMulKernel(matrixMulProgram, kernelname.c_str());
    return matrixMulKernel;
}

int main(int argc, const char * argv[])
{
    /* get context and print all OpenCL Devices */
    cl::Context context = cl::Context(CL_DEVICE_TYPE_ALL);
    std::vector<cl::Device> devices;
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    if (devices.size() < 1) {
        std::cerr << "No OpenCL devices detected!" << std::endl;
        return 0;
    }

    
    std::cout << "Found " << devices.size() << " OpenCL devices:" << std::endl;
    for (std::size_t i = 0; i < devices.size(); ++i) {
        std::cout << devices[i].getInfo<CL_DEVICE_VENDOR>() << " ";
        std::cout << devices[i].getInfo<CL_DEVICE_NAME>() << std::endl;
        std::cout << devices[i].getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT>() << std::endl;
        std::cout << devices[i].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[0] << "x";
        std::cout << devices[i].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[1] << "x";
        std::cout << devices[i].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>()[2] << std::endl;
        std::cout << devices[i].getInfo<CL_DEVICE_LOCAL_MEM_SIZE>()/1024 << " kb" << std::endl;
        std::cout << devices[i].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>()/(1024*1024) << " mb" << std::endl;
    }
    std::cout << "Compile OpenCL Kernel ..." << std::endl;
    cl::Kernel matrixMulKernel;
    try {
        matrixMulKernel = compileKernel(context, devices, "matrixMulKernel.cl", "matrixMul");
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        return 0;
    }
    
    /* define matrix dimensions */
	unsigned int widthA = 768;
	unsigned int heightA = 4096;
	unsigned int widthB = 4096;

	unsigned int heightB = widthA;
	unsigned int widthC = widthB;
	unsigned int heightC = heightA;
	// print matrix dimensions
	std::cout << "Matrices dimensions: A(" << widthA <<" x " << heightA
        << "), B(" << widthB <<" x " << heightB
        << "), C(" << widthC <<" x " << heightC << ")" << std::endl;
    
    /* allocate memory for matrices */
	unsigned int sizeA = widthA * heightA;
	float* hostDataA = new float[sizeA];
    
	unsigned int sizeB = widthB * heightB;
	float* hostDataB = new float[sizeB];
    
	unsigned int sizeC = widthC * heightC;
	float* hostCgpu = new float[sizeC];
    float* hostCcpu = new float[sizeC];
    
	// initialize matrices with random numbers from the interval [0,1]
    std::srand(2006);
	const float fScale = 1.0f / (float)RAND_MAX;
    
	for(size_t i = 0; i < sizeA; ++i)
		hostDataA[i] = fScale * rand();
	for(size_t i = 0; i < sizeB; ++i)
        hostDataB[i] = fScale * rand();
    for(size_t i = 0; i < sizeB; ++i)
        hostCgpu[i] = 0;

    /* create timer */
    WallTimer timer;

	std::cout << "Multiplying matrices on GPU ... " << std::endl;
    /* generate queue for first device */
    cl::CommandQueue gpuQueue(context, devices[0]);
    
    timer.reset();
	/* start computation */
	if(!matrixMulGPU(context, &gpuQueue, &matrixMulKernel, hostDataA, hostDataB, hostCgpu, widthA, heightA, widthB)) {
		std::cout << "ERROR: executing OpenCL matrix multiplication failed..." << std::endl;
		return 1;
	};

	std::cout << "Finished multiplying matrices on GPU" << std::endl;
    float gpuTime = timer.get_elapsed_sec();
	std::cout << "Time: " << gpuTime << " s" << std::endl;
    
    std::cout << "Multiplying matrices on CPU ... " << std::endl;
    timer.reset();
	/* start computation */
    matrixMulCPU(hostDataA, hostDataB, hostCcpu, widthA, heightA, widthB);
	std::cout << "Finished multiplying matrices on CPU" << std::endl;
    float cpuTime = timer.get_elapsed_sec();
	std::cout << "Time: " << cpuTime << " s" << std::endl;
    
    std::cout << "Speedup: " << cpuTime/gpuTime << std::endl;
    
    std::cout << "Verifying correctness... ";
	// comparing results
	if(matricesEqual(hostCgpu, hostCcpu, sizeC))
		std::cout << "SUCCESS!" << std::endl;
	else
		std::cout << "FAILURE!" << std::endl;
    
	delete[] hostDataA;
	delete[] hostDataB;
	delete[] hostCcpu;
	delete[] hostCgpu;
	return 0;
}
