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
template <typename T>
inline std::string StereoPatchMatchCL<T>::kernelDecl() {
	std::wcout << "only float and int templates are applicable – terminating" << std::endl;
	exit(1);
}
template<>
inline std::string StereoPatchMatchCL<float>::kernelDecl() {
	return "float";
}
template<>
inline std::string StereoPatchMatchCL<int>::kernelDecl() {
	return "int";
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
void kernelTest(const T *in, const T *in2, T *out, int *xNNF, int *yNNF, const int *offset, const int xmax, const int ymax, const int patchsize, int gid) {
	int xi, yi, patchcenter, patchhalf, dif, bestDif, pixelCount;
	size_t tid = gid * patchsize;
	xi = patchsize * (tid % xmax / patchsize);
	yi = tid / xmax * patchsize;
	pixelCount = xmax*ymax;
	patchhalf = patchsize/2;
	if(patchsize > xmax-1-xi || patchsize > ymax-1-yi)
		return;
	{
		int subpixel = xi + yi * xmax;
		patchcenter = subpixel + patchhalf + patchhalf * xmax;
		int subpixel2 = subpixel + (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;//x und y verschiebung
		//Patch-Distance
		dif=0;
		for(int row = patchsize; row > 0; --row) {
			for(int i = patchsize; i > 0; --i, ++subpixel, ++subpixel2) {
				dif += abs(in[subpixel] - in2[subpixel2]);//R
				int blue_subpixel = subpixel + pixelCount, blue_subpixel2 = subpixel2 + pixelCount;
				dif += abs(in[blue_subpixel] - in2[blue_subpixel2]);//G
				dif += abs(in[blue_subpixel + pixelCount] - in2[blue_subpixel2 + pixelCount]);//B
			}
			//plus one row, minus patch width
			subpixel += xmax - patchsize;
			subpixel2 += xmax - patchsize;
		}
	}
	bestDif = dif;//inital difference
	//eins nach rechts
	++xi;
	++patchcenter;
	{
		int subpixel = xi + yi * xmax;
		int subpixel2 = subpixel + (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;//x und y verschiebung
		//Patch-Distance
		dif=0;
		for(int row = patchsize; row > 0; --row) {
			for(int i = patchsize; i > 0; --i, ++subpixel, ++subpixel2) {
				dif += abs(in[subpixel] - in2[subpixel2]);//R
				int blue_subpixel = subpixel + pixelCount, blue_subpixel2 = subpixel2 + pixelCount;
				dif += abs(in[blue_subpixel] - in2[blue_subpixel2]);//G
				dif += abs(in[blue_subpixel + pixelCount] - in2[blue_subpixel2 + pixelCount]);//B
			}
			//plus one row, minus patch width
			subpixel += xmax - patchsize;
			subpixel2 += xmax - patchsize;
		}
	}
	if(bestDif > dif) {//better patch match – switch patches
		bestDif = dif;
		int oriPatch = patchcenter - 1;
		xNNF[oriPatch] = xNNF[patchcenter];
		yNNF[oriPatch] = yNNF[patchcenter];
	}
	out[patchcenter] = 0; out[patchcenter + xmax*ymax] = 255; out[patchcenter + 2*xmax*ymax] = 0;//rechts
	--xi;//original x
	--patchcenter;//original patch
	out[patchcenter] = 255; out[patchcenter + xmax*ymax] = 0; out[patchcenter + 2*xmax*ymax] = 0;//original
	//eins nach unten
	++yi;
	patchcenter += xmax;
	out[patchcenter] = 0; out[patchcenter + xmax*ymax] = 0; out[patchcenter + 2*xmax*ymax] = 255;//unten
	{
		int subpixel = xi + yi * xmax;
		int subpixel2 = subpixel + (xNNF[patchcenter]) + (yNNF[patchcenter]) * xmax;//x und y verschiebung
		//Patch-Distance
		dif=0;
		for(int row = patchsize; row > 0; --row) {
			for(int i = patchsize; i > 0; --i, ++subpixel, ++subpixel2) {
				dif += abs(in[subpixel] - in2[subpixel2]);//R
				int blue_subpixel = subpixel + pixelCount, blue_subpixel2 = subpixel2 + pixelCount;
				dif += abs(in[blue_subpixel] - in2[blue_subpixel2]);//G
				dif += abs(in[blue_subpixel + pixelCount] - in2[blue_subpixel2 + pixelCount]);//B
			}
			//plus one row, minus patch width
			subpixel += xmax - patchsize;
			subpixel2 += xmax - patchsize;
		}
	}
	if(bestDif > dif) {//better patch match – switch patches
		bestDif = dif;
		int oriPatch = patchcenter - xmax;
		xNNF[oriPatch] = xNNF[patchcenter];
		yNNF[oriPatch] = yNNF[patchcenter];
	}
}
template <typename T>
void StereoPatchMatchCL<T>::execute() {
	PARAMETEREDOBJECT_AVOID_REEXECUTION;
	ParameteredObject::execute();

	// your code goes here :-)
	int patchsize=5, offset;
	auto &image = (imgOut().push_back(imgSrc()[0]))[0];//copy input image to output
	imgOut().push_back(imgSrc()[1]);
	imgOut().push_back(imgSrc()[0]);
	cimg_library::CImg<int> xNNF(image.width(), image.height());//x deviation
	cimg_library::CImg<int> yNNF(xNNF);//y deviation
	cimg_forXYZC(image, x, y, z, c) {//noise
		image(x, y, z, c) = (T)(255);
		xNNF(x, y) = (int)(cimg_library::cimg::rand() * (image.width()-1-patchsize) - x + patchsize/2+1);
		yNNF(x, y) = (int)(cimg_library::cimg::rand() * (image.height()-1-patchsize) - y + patchsize/2+1);
	}
	//backup original
	imgOut().push_back(xNNF);
	imgOut().push_back(yNNF);

	std::wostringstream sss;
	std::ostringstream ss;
	cl_int err;

	cl::Context context(CL_DEVICE_TYPE_ALL);//get compute context
	//cl::Context context = cl::Context::getDefault(&err);
	//checkErr(err, L"Conext::Context()");

	int width=image.width(), height=image.height();
	int buffersize=width*height;//4(Integer) Bytes pro SubPixel

	cl::Buffer inCL(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 3*sizeof(T)*buffersize, const_cast<T*>(imgSrc()[0].data()), &err); //const cast – read only flag
	checkErr(err, L"Buffer::InputBuffer()");
	cl::Buffer inCL2(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, 3*sizeof(T)*buffersize, const_cast<T*>(imgSrc()[1].data()), &err); //const cast – read only flag
	checkErr(err, L"Buffer::InputBuffer2()");
	cl::Buffer outCL(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 3*sizeof(T)*buffersize, image.data(), &err);
	checkErr(err, L"Buffer::OutputBuffer()");
	cl::Buffer xCL(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(T)*buffersize, xNNF.data(), &err);
	checkErr(err, L"Buffer::xBuffer()");
	cl::Buffer yCL(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(T)*buffersize, yNNF.data(), &err);
	checkErr(err, L"Buffer::yBuffer()");
	cl::Buffer offsetCL(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(int), &offset, &err);
	checkErr(err, L"Buffer::offset()");

	//Kernel
	ss.str(std::string());//clear string
	//add image size constants to kernel source
	ss << "__constant int xmax = " << width << ';' << std::endl;
	ss << "__constant int ymax = " << height << ';' << std::endl;
	ss << "__constant int patchsize = " << patchsize << ';' << std::endl;
	ss << "__kernel void run(__global const " << kernelDecl() << " *in"
		<< ", __global const " << kernelDecl() << " *in2"
		<< ", __global " << kernelDecl() << " *out"
		<< ", __global int *xNNF"
		<< ", __global int *yNNF"
		<< ", __global const int *offset)"
		<< std::endl;//adjust kernel arguments based on template type
	std::ifstream file(L"E:/_Own-Zone/_Studium/Praktikum/StereoPatchMatchCL/kernel.h");//load kernel code from disk, charon searches where your source code resides and in its core directory
	checkErr(file.is_open() ? CL_SUCCESS:-1, L"load ./kernel.h");
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
	checkErr(err, L"Kernel::setArg(2)");
	err = kernel.setArg(3, xCL);
	checkErr(err, L"Kernel::setArg(3)");
	err = kernel.setArg(4, yCL);
	checkErr(err, L"Kernel::setArg(4)");
	err = kernel.setArg(5, offsetCL);
	checkErr(err, L"Kernel::setArg(5)");

	clock_t start, intermediate, end;//time
	//add kernel to device execution queue
	cl::CommandQueue queue(context, devices[0], 0, &err);
	checkErr(err, L"CommandQueue::CommandQueue()");
	cl::Event event;
	start = clock();
//#define X
#ifdef X
	for(int i=0; i<width*height/(patchsize*patchsize); i++)
		kernelTest(const_cast<T*>(imgSrc()[0].data()), const_cast<T*>(imgSrc()[1].data()), image.data(), xNNF.data(), yNNF.data(), &offset, width, height, patchsize, i);
#endif
#ifndef X
	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, 
			cl::NDRange(width*height/(patchsize*patchsize)), //global range
			cl::NDRange(1, 1), //local range
			nullptr, 
			&event);//object to wait for
	checkErr(err, L"ComamndQueue::enqueueNDRangeKernel()");
	event.wait();//return when execution is finished
	intermediate = clock();//pure execution time
	err = queue.enqueueReadBuffer(outCL, CL_TRUE, 0, buffersize, image.data());//copy buffer back to main memory
	checkErr(err, L"ComamndQueue::enqueueReadBuffer()");
	err = queue.enqueueReadBuffer(xCL, CL_TRUE, 0, buffersize, xNNF.data());//copy buffer back to main memory
	checkErr(err, L"ComamndQueue::enqueueReadBuffer2()");
	err = queue.enqueueReadBuffer(yCL, CL_TRUE, 0, buffersize, yNNF.data());//copy buffer back to main memory
	checkErr(err, L"ComamndQueue::enqueueReadBuffer3()");
#endif
	intermediate = clock();//TODO remove
	end = clock();
	std::wcout << L"Time: " << ((intermediate-start)/(double)CLOCKS_PER_SEC) << L"s";
	std::wcout << L" + " << ((end-intermediate)/(double)CLOCKS_PER_SEC) << L"s" << std::endl;//buffer copy
	//changed NNFs
	imgOut().push_back(xNNF);
	imgOut().push_back(yNNF);
	cimg_forXYZC(image, x, y, z, c)//reconstruct image
		image(x, y, z, c) = (T)imgSrc()[0](x+xNNF(x, y), y+yNNF(x, y));
}

#endif /* _STEREOPATCHMATCHCL_HXX_ */
