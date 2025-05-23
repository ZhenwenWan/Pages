/***************************************************************************
 *   Copyright (c) YEAR YOUR NAME         <Your e-mail address>            *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef MySimII_PRECOMPILED_H
#define MySimII_PRECOMPILED_H

#include <FCConfig.h>

// Exporting of App classes
#ifdef FC_OS_WIN32
# define AppMySimIIExport __declspec(dllexport)
# define MySimIIExport    __declspec(dllexport)
# define PartExport     __declspec(dllimport)
# define MeshExport     __declspec(dllimport)
# define BaseExport     __declspec(dllimport)
#else // for Linux
# define AppMySimIIExport
# define MySimIIExport
# define PartExport
# define MeshExport
# define BaseExport
#endif

#ifdef _MSC_VER
# pragma warning(disable : 4290)
# pragma warning(disable : 4275)
#endif

#ifdef _PreComp_

// standard
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <bitset>
#include <cstdlib>
#include <memory>
#include <cmath>

#include <math.h>

#include <algorithm>
#include <stdexcept>
// Python
#include <Python.h>

// Boost
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>


#endif //_PreComp_

#endif

