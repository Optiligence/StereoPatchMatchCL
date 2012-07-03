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
/** \file StereoPatchMatchCL.h
 *  Declaration of the parameter class StereoPatchMatchCL.
 *  \author <a href="mailto:norbert.pfeiler@gmail.com">
 *      Norbert Pfeiler</a>
 *  \date 27.06.2012
 */

#ifndef _STEREOPATCHMATCHCL_H_
#define _STEREOPATCHMATCHCL_H_

#ifdef _MSC_VER
#ifdef stereopatchmatchcl_EXPORTS
/// Visual C++ specific code
#define stereopatchmatchcl_DECLDIR __declspec(dllexport)
#else
#define stereopatchmatchcl_DECLDIR __declspec(dllimport)
#endif /*Export or import*/
#else
/// Not needed without MSVC
#define stereopatchmatchcl_DECLDIR
#endif

#include <charon-core/ParameteredObject.h>
#include <charon-utils/CImg.h>
#include <iostream>
#define __NO_STD_VECTOR // Use cl::vector instead of STL version
#include <CL\cl.hpp>

/// StereoPatchMatch using Open CL
/** TODO detailed desciption
 */
template <typename T>
class stereopatchmatchcl_DECLDIR StereoPatchMatchCL :
		public TemplatedParameteredObject<T> {
public:
	/// default constructor
	/// \param name          Instance name
	StereoPatchMatchCL(const std::string& name = "");

	/// TODO docu
	InputSlot< cimg_library::CImgList<T> > imgSrc;

	/// TODO docu
	OutputSlot< cimg_library::CImgList<T> > imgOut;

	/// TODO docu
	Parameter< int > cl_Platform;
	/// TODO docu
	ParameterList< int > cl_Devices;

	/// Update object.
	virtual void execute();
	void work();
	std::string kernelDecl();
};

#endif // _STEREOPATCHMATCHCL_H_
