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
		cl_Devices("0")
{
	ParameteredObject::_addInputSlot(
		imgScr, "imgScr",
		"TODO docu",
		"CImgList<T>");

	ParameteredObject::_addOutputSlot(
		imgOut, "imgOut",
		"TODO docu",
		"CImgList<T>");

	ParameteredObject::_addParameter< int >(
		cl_Platform, "cl_Platform",
		"TODO docu",
		0, "int");
	ParameteredObject::_addParameter(
		cl_Devices, "cl_Devices",
		"TODO docu",
		"int");

}

template <typename T>
void StereoPatchMatchCL<T>::execute() {
	PARAMETEREDOBJECT_AVOID_REEXECUTION;
	ParameteredObject::execute();

	// your code goes here :-)
}

#endif /* _STEREOPATCHMATCHCL_HXX_ */
