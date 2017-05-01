#include "App.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

App::App() {
  init();
}

App::~App() {

}

void App::init() {
  std::cout << std::endl;

  cl_uint platformsCount = getPlatformsCount();
  std::vector<cl_platform_id> platformIds = getPlatformIds(platformsCount);

  std::cout << "Platforms:" << std::endl;

  for (cl_platform_id id : platformIds) {
    std::cout << std::endl << getPlatformInfos(id) << std::endl;
  }

  cl_uint devicesCount = getDevicesCount(platformIds[0]);
  std::vector<cl_device_id> deviceIds = getDevicesIds(platformIds[0], devicesCount);

  std::cout << "Devices:" << std::endl;

  for (int i = 0; i < (int) devicesCount; ++i) {
    std::string infos = getDeviceInfos(deviceIds[i]);
    std::cout << std::endl << i << std:: endl << infos;
  }

  const cl_context_properties ctxProps[] = {
    CL_CONTEXT_PLATFORM,
    reinterpret_cast<cl_context_properties> (platformIds[0]),
    0,
    0
  };

  cl_int err;

  cl_context ctx = clCreateContext(ctxProps, devicesCount, deviceIds.data(), nullptr, nullptr, &err);

  checkError(err);
  
  const size_t SAMPLE_SIZE = 2000000;

  float* inData = new float[SAMPLE_SIZE];

  for (size_t i = 0; i < SAMPLE_SIZE; ++i) {
    inData[i] = (float) i;
  }

  float outData[SAMPLE_SIZE];

  cl_mem inBuffer = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * SAMPLE_SIZE, inData, &err);

  checkError(err);

  cl_mem outBuffer = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, sizeof(float) * SAMPLE_SIZE, nullptr, &err);

  checkError(err);

  std::string kernelSrc = R"(
  __kernel void square(__global float* in, __global float* out) {
    const int i = get_global_id(0);

    out[i] = in[i] * in[i];
  }
  )";

  cl_program prog = getProgram(ctx, kernelSrc);
  checkError(clBuildProgram(prog, devicesCount, deviceIds.data(), nullptr, nullptr, nullptr));

  cl_kernel kernel = clCreateKernel(prog, "square", &err);
  checkError(err);

  clSetKernelArg(kernel, 0, sizeof(cl_mem), &inBuffer);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &outBuffer);

  cl_command_queue queue = clCreateCommandQueue(ctx, deviceIds[0], 0, &err);
  checkError(err);

  size_t offset[2]= { 0 };
  size_t size[2] = { SAMPLE_SIZE, SAMPLE_SIZE };

  const clock_t begin = std::clock();

  checkError(
      clEnqueueNDRangeKernel(queue, kernel, 1, offset, size, nullptr, 0, nullptr, nullptr)
      );

	checkError (
      clEnqueueReadBuffer(queue, outBuffer, CL_TRUE, 0, sizeof(float) * SAMPLE_SIZE, outData, 0, nullptr, nullptr)
      );

  const clock_t end = std::clock();
  const float elpased = (float) (end - begin) / CLOCKS_PER_SEC;

  std::cout << std::endl << "Results: " << std::endl << std::endl;

  std::cout << "Executed in: " << elpased << "s" << std::endl << std::endl;
}

cl_uint App::getPlatformsCount() {
  cl_uint count;

  clGetPlatformIDs(0, nullptr, &count);

  return count;
}

std::vector<cl_platform_id> App::getPlatformIds(const cl_uint& platformsCount) {
  std::vector<cl_platform_id> ids(platformsCount);

  clGetPlatformIDs(platformsCount, ids.data(), nullptr);

  return ids;
}

std::string App::getPlatformInfos(const cl_platform_id& id) {
  const char* names[] = { "Name: ", "Vendor: ", "Version: ", "Profile: ", "Extensions: " };
  const cl_platform_info attribs[] = { CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION, CL_PLATFORM_PROFILE, CL_PLATFORM_EXTENSIONS };
  const int count = sizeof(names) / sizeof(char*);

  size_t size;

  std::string infos;

  for (int i = 0; i < count; ++i) {
    const cl_platform_info attrib = attribs[i];

    clGetPlatformInfo(id, attrib, 0, nullptr, &size);

    char value[size];

    clGetPlatformInfo(id, attrib, size, &value, nullptr);

    infos += names[i];
    infos += value;
    infos += '\n';
  }

  return infos;
}

cl_uint App::getDevicesCount(const cl_platform_id& platformId) {
  cl_uint count;

  clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, nullptr, &count);

  return count;
}

std::vector<cl_device_id> App::getDevicesIds(const cl_platform_id& platformId, const cl_uint& count) {
  std::vector<cl_device_id> ids(count);

  clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, count, ids.data(), nullptr);

  return ids;
}

std::string App::getDeviceInfos(const cl_device_id& id) {
  const char* names[] = { "Name: ", "Version: ", "Driver: " };
  const cl_device_info attribs[] = { CL_DEVICE_NAME, CL_DEVICE_VERSION, CL_DRIVER_VERSION };
  const int count = sizeof(names) / sizeof(char*);

  size_t size;

  std::string infos;

  for (int i = 0; i < count; ++i) {
    const cl_device_info attrib = attribs[i];

    clGetDeviceInfo(id, attrib, 0, nullptr, &size);

    char value[size];

    clGetDeviceInfo(id, attrib, size, &value, nullptr);

    infos += names[i];
    infos += value;
    infos += '\n';
  }

  cl_uint maxComputeUnits;

  clGetDeviceInfo(id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &maxComputeUnits, nullptr);

  infos += "Max compute units: ";
  infos += std::to_string(maxComputeUnits);
  infos += '\n';

  return infos;
}

cl_program App::getProgram(const cl_context& ctx, std::string& kernelSrc) {
  size_t lens[] = { kernelSrc.size() };
  const char* srcs[] = { kernelSrc.data() };

  cl_int err;

  cl_program program = clCreateProgramWithSource(ctx, 1, srcs, lens, &err);

  checkError(err);

  return program;
}

void App::checkError(const cl_int& err) {
  if (err == CL_SUCCESS) {
    return;
  }

  std::cout << "OpenCL error: ";

  if (m_errors.find(err) == m_errors.end()) {
    std::cout << "UNKNOWN ERROR";
  }
  else {
    std::cout << m_errors[err];
  }

  std::cout << std::endl;

  std::exit(1);
}

std::string App::getFileContent(const char* path) {
  std::ifstream stream(path);
  std::stringstream buffer;
  buffer << stream.rdbuf();

  return buffer.str();
}

