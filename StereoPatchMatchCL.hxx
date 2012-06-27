/*  Copyright (C) 2012 Norbert Pfeiler

	This file is part of Charon.

	Charon is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Charon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with Charon.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file StereoPatchMatchCL.hxx
 *  Implementation of the parameter class StereoPatchMatchCL.
 *  \author <a href="mailto:norbert.pfeiler@gmail.com">
 *      Norbert Pfeiler</a>
 *  \date 27.06.2012
 */

#ifndef _STEREOPATCHMATCHCL_HXX_
#define _STEREOPATCHMATCHCL_HXX_

#include <charon-core/ParameteredObject.hxx>
#include "StereoPatchMatchCL.h"

template <typename T>
StereoPatchMatchCL<T>::StereoPatchMatchCL(const std::string& name) :
		TemplatedParameteredObject<T>(
			"StereoPatchMatchCL", name,
			"<h2>StereoPatchMatch using Open CL</h2><br>"
			"TODO detailed desciption"
		),
		cl_Devices("-1")
{
	ParameteredObject::_addInputSlot(
		imgSrc, "imgSrc",
		"TODO docu",
		"CImgList<T>");

	ParameteredObject::_addOutputSlot(
		imgOut, "img",
		"TODO docu",
		"CImgList<T>");

	ParameteredObject::_addParameter< int >(
		cl_Platform, "cl_Platform",
		"TODO docu",
		-1, "int");
	ParameteredObject::_addParameter(
		cl_Devices, "cl_Devices",
		"TODO docu",
		"int");

}
void checkErr(cl_int err, const wchar_t* name)
{
	if (err != CL_SUCCESS) {
		std::wostringstream sss;
		sss << L"ERROR: " << name << L" (" << err << L")" << std::endl;
		sss << L"terminating";
		std::wcout << L"OpenCL Error: " << sss.str() << std::endl;
		exit(EXIT_FAILURE);
	} else std::wcout << name << std::endl;
}
template <typename T>
inline std::string StereoPatchMatchCL<T>::kernelDecl() {
	std::wcout << "only float and int templates are applicable – terminating" << std::endl;
	exit(1);
}
template<>
inline std::string StereoPatchMatchCL<float>::kernelDecl() {
	return "__kernel void run(__global const float *in, __global const float *in2, __global float *out)";
}
template<>
inline std::string StereoPatchMatchCL<int>::kernelDecl() {
	return "__kernel void run(__global const int *in, __global const int *in2, __global int *out)";
}
template <typename T>
void StereoPatchMatchCL<T>::execute() {
	PARAMETEREDOBJECT_AVOID_REEXECUTION;
	ParameteredObject::execute();

	// your code goes here :-)
	auto &image = (imgOut().push_back(this->imgSrc()[0]))[0];//copy input image to output
	cimg_forXYZC(image, x, y, z, c)//noise
		image(x, y, z, c) = (T)(cimg_library::cimg::rand() * 255);
	
	std::wostringstream sss;
	std::ostringstream ss;
	cl_int err;

	cl::Context context(CL_DEVICE_TYPE_ALL);//get compute context
	//cl::Context context = cl::Context::getDefault(&err);
	//checkErr(err, L"Conext::Context()");

	int width=image.width(), height=image.height();
	int buffersize=4*3*width*height;//4(Integer) Bytes pro SubPixel

	cl::Buffer inCL(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, buffersize, const_cast<T*>(imgSrc()[0].data()), &err); //const cast – read only flag
	checkErr(err, L"Buffer::InputBuffer()");
	cl::Buffer inCL2(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, buffersize, const_cast<T*>(imgSrc()[1].data()), &err); //const cast – read only flag
	checkErr(err, L"Buffer::InputBuffer2()");
	cl::Buffer outCL(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, buffersize, image.data(), &err);
	checkErr(err, L"Buffer::OutputBuffer()");

	//Kernel
	ss.str(std::string());//clear string
	//add image size constants to kernel source
	ss << "__constant int xmax = " << width << ';' << std::endl;
	ss << "__constant int ymax = " << height << ';' << std::endl;
	ss << kernelDecl() << std::endl;//adjust kernel arguments based on template type
	std::ifstream file(L"E:/_Own-Zone/_Studium/Praktikum/StereoPatchMatchCL/kernel.cl");//load kernel code from disk, charon searches where your source code resides and in its core directory
	checkErr(file.is_open() ? CL_SUCCESS:-1, L"load ./kernel.cl");
	//std::string prog(std::istreambuf_iterator<wchar_t>(file), (std::istreambuf_iterator<wchar_t>()));//parse file into string
	ss << file.rdbuf();//add remaining source
	sout << "#Source#\n" << ss.str() << std::endl;//print source
	cl::Program program(context, ss.str());//apply source to current context

	//get devices from context of platform
	cl::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
	std::wcout << L"Number of available Devices: " << devices.size() << std::endl;
	checkErr(cl_Devices[0] < (int)devices.size() ? CL_SUCCESS : -1 , L"devices.size()");
	if(cl_Devices[0]<0) devices[0] = cl::Device::getDefault(&err);//default device
	else std::swap(devices[0], devices[cl_Devices[0]]);//select choice as first
	//std::swap(devices[0], devices[1]);//make CPU first

	err = program.build(devices, "");//compile source for given devices
	std::wcout << L"#Compiler Output#\n";
	if (true)//TODO (err != CL_SUCCESS)
	for(unsigned int i=0; i<devices.size(); ++i) {
		auto device = devices[i];
		ss.str(std::string());//clear string
		ss << "Device: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
		ss << "Build Status:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(device) << std::endl;
		ss << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(device) << std::endl;
		ss << "Build Log:\n" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
		sout << ss.str() << std::endl;
	}
	checkErr(err, L"Program::build()");

	cl::Kernel kernel(program, "run", &err);//the kernel object
	checkErr(err, L"Kernel::Kernel()");
	//define kernel arguments
	err = kernel.setArg(0, inCL);
	checkErr(err, L"Kernel::setArg(0)");
	err = kernel.setArg(1, inCL2);
	checkErr(err, L"Kernel::setArg(1)");
	err = kernel.setArg(2, outCL);
	checkErr(err, L"Kernel::setArg(3)");

	clock_t start, intermediate, end;//time
	//add kernel to device execution queue
	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, L"CommandQueue::CommandQueue()");
	cl::Event event;
	start = clock();
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, 
		cl::NDRange(width*height), //global range
		cl::NDRange(1, 1), //local range
		nullptr, 
		&event);//object to wait for
	checkErr(err, L"ComamndQueue::enqueueNDRangeKernel()");
	event.wait();//return when execution is finished
	intermediate = clock();//pure execution time
	err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, buffersize, image.data());//copy buffer back to main memory
	checkErr(err, L"ComamndQueue::enqueueReadBuffer()");
	
	end = clock();
	std::wcout << L"Time: " << ((intermediate-start)/(double)CLOCKS_PER_SEC) << L"s";
	std::wcout << L" + " << ((end-intermediate)/(double)CLOCKS_PER_SEC) << L"s" << std::endl;//buffer copy
}

#endif /* _STEREOPATCHMATCHCL_HXX_ */
