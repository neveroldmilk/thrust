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


#include <thrust/detail/config.h>

// don't attempt to #include this file without omp support
#if (THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE == THRUST_TRUE)
#include <omp.h>
#endif // omp support

#include <thrust/iterator/iterator_traits.h>
#include <thrust/sort.h>
#include <thrust/system/cpp/detail/merge.h>
#include <thrust/system/cpp/detail/tag.h>

namespace thrust
{
namespace system
{
namespace omp
{
namespace detail
{

template<typename RandomAccessIterator,
         typename StrictWeakOrdering>
void stable_sort(tag,
                 RandomAccessIterator first,
                 RandomAccessIterator last,
                 StrictWeakOrdering comp)
{
  // we're attempting to launch an omp kernel, assert we're compiling with omp support
  // ========================================================================
  // X Note to the user: If you've found this line due to a compiler error, X
  // X you need to enable OpenMP support in your compiler.                  X
  // ========================================================================
  THRUST_STATIC_ASSERT( (thrust::detail::depend_on_instantiation<RandomAccessIterator,
                        (THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE == THRUST_TRUE)>::value) );

#if (THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE == THRUST_TRUE)
  typedef typename thrust::iterator_difference<RandomAccessIterator>::type IndexType;
  
  if (first == last)
    return;

  #pragma omp parallel
  {
    thrust::detail::backend::uniform_decomposition<IndexType> decomp(last - first, 1, omp_get_num_threads());

    // process id
    IndexType p_i = omp_get_thread_num();

    // every thread sorts its own tile
    if (p_i < decomp.size())
    {
      thrust::stable_sort(thrust::retag<thrust::cpp::tag>(first) + decomp[p_i].begin(),
                          thrust::retag<thrust::cpp::tag>(first) + decomp[p_i].end(),
                          comp);
    }

    #pragma omp barrier

    IndexType nseg = decomp.size();
    IndexType h = 2;

    // keep track of which sub-range we're processing
    IndexType a=p_i, b=p_i, c=p_i+1;

    while( nseg>1 )
    {
        if(c >= decomp.size())
          c = decomp.size() - 1;

        if((p_i % h) == 0 && c > b)
        {
            thrust::system::cpp::detail::
              inplace_merge(thrust::retag<thrust::cpp::tag>(first) + decomp[a].begin(),
                            thrust::retag<thrust::cpp::tag>(first) + decomp[b].end(),
                            thrust::retag<thrust::cpp::tag>(first) + decomp[c].end(),
                            comp);
            b = c;
            c += h;
        }

        nseg = (nseg + 1) / 2;
        h *= 2;

        #pragma omp barrier
    }
  }
#endif // THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE
}

template<typename RandomAccessIterator1,
         typename RandomAccessIterator2,
         typename StrictWeakOrdering>
void stable_sort_by_key(tag,
                        RandomAccessIterator1 keys_first,
                        RandomAccessIterator1 keys_last,
                        RandomAccessIterator2 values_first,
                        StrictWeakOrdering comp)
{
  // we're attempting to launch an omp kernel, assert we're compiling with omp support
  // ========================================================================
  // X Note to the user: If you've found this line due to a compiler error, X
  // X you need to enable OpenMP support in your compiler.                  X
  // ========================================================================
  THRUST_STATIC_ASSERT( (thrust::detail::depend_on_instantiation<RandomAccessIterator1,
                        (THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE == THRUST_TRUE)>::value) );

#if (THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE == THRUST_TRUE)
  typedef typename thrust::iterator_difference<RandomAccessIterator1>::type IndexType;
  
  if (keys_first == keys_last)
    return;

  #pragma omp parallel
  {
    thrust::detail::backend::uniform_decomposition<IndexType> decomp(keys_last - keys_first, 1, omp_get_num_threads());

    // process id
    IndexType p_i = omp_get_thread_num();

    // every thread sorts its own tile
    if (p_i < decomp.size())
    {
      thrust::stable_sort_by_key(thrust::retag<thrust::cpp::tag>(keys_first) + decomp[p_i].begin(),
                                 thrust::retag<thrust::cpp::tag>(keys_first) + decomp[p_i].end(),
                                 thrust::retag<thrust::cpp::tag>(values_first) + decomp[p_i].begin(),
                                 comp);
    }

    #pragma omp barrier

    IndexType nseg = decomp.size();
    IndexType h = 2;

    // keep track of which sub-range we're processing
    IndexType a=p_i, b=p_i, c=p_i+1;

    while( nseg>1 )
    {
        if(c >= decomp.size())
          c = decomp.size() - 1;

        if((p_i % h) == 0 && c > b)
        {
            thrust::system::cpp::detail::
              inplace_merge_by_key(thrust::retag<thrust::cpp::tag>(keys_first) + decomp[a].begin(),
                                   thrust::retag<thrust::cpp::tag>(keys_first) + decomp[b].end(),
                                   thrust::retag<thrust::cpp::tag>(keys_first) + decomp[c].end(),
                                   thrust::retag<thrust::cpp::tag>(values_first) + decomp[a].begin(),
                                   comp);
            b = c;
            c += h;
        }

        nseg = (nseg + 1) / 2;
        h *= 2;

        #pragma omp barrier
    }
  }
#endif // THRUST_DEVICE_COMPILER_IS_OMP_CAPABLE
}

} // end namespace detail
} // end namespace omp
} // end namespace system
} // end namespace thrust

