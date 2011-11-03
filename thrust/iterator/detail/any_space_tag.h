/*
 *  Copyright 2008-2011 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <thrust/detail/config.h>
#include <thrust/iterator/detail/host_space_tag.h>
#include <thrust/iterator/detail/device_space_tag.h>

namespace thrust
{

struct any_space_tag
{
  // use conversion operators instead of inheritance to avoid ambiguous conversion errors
  operator host_space_tag () {return host_space_tag();};

  operator device_space_tag () {return device_space_tag();};

  // allow any_space_tag to convert to any type at all
  // XXX make this safer using enable_if<is_tag<T>> upon c++11
  template<typename T> operator T () const {return T();}
};

} // end thrust

