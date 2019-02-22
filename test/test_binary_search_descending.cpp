// MIT License
//
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Google Test
#include <gtest/gtest.h>

// Thrust
#include <thrust/binary_search.h>
#include <thrust/functional.h>

#include <thrust/sequence.h>
#include <thrust/sort.h>

// HIP API
#if THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC
#include <hip/hip_runtime_api.h>

#define HIP_CHECK(condition) ASSERT_EQ(condition, hipSuccess)
#endif // THRUST_DEVICE_COMPILER == THRUST_DEVICE_COMPILER_HCC

#include "test_utils.hpp"
#include "test_assertions.hpp"

template<
    class InputType
>
struct Params
{
    using input_type = InputType;
};

template<class Params>
class BinarySearchDescendingTests : public ::testing::Test
{
public:
    using input_type = typename Params::input_type;
};

typedef ::testing::Types<
    Params<thrust::host_vector<short>>,
    Params<thrust::host_vector<int>>,
    Params<thrust::host_vector<long long>>,
    Params<thrust::host_vector<unsigned short>>,
    Params<thrust::host_vector<unsigned int>>,
    Params<thrust::host_vector<unsigned long long>>,
    Params<thrust::host_vector<float>>,
    Params<thrust::host_vector<double>>,
    Params<thrust::device_vector<short>>,
    Params<thrust::device_vector<int>>,
    Params<thrust::device_vector<long long>>,
    Params<thrust::device_vector<unsigned short>>,
    Params<thrust::device_vector<unsigned int>>,
    Params<thrust::device_vector<unsigned long long>>,
    Params<thrust::device_vector<float>>,
    Params<thrust::device_vector<double>>
> BinarySearchDescendingTestsParams;

TYPED_TEST_CASE(BinarySearchDescendingTests, BinarySearchDescendingTestsParams);

TYPED_TEST(BinarySearchDescendingTests, TestScalarLowerBoundDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    using T = typename Vector::value_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::lower_bound(vec.begin(), vec.end(), 0, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::lower_bound(vec.begin(), vec.end(), 1, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::lower_bound(vec.begin(), vec.end(), 2, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::lower_bound(vec.begin(), vec.end(), 3, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::lower_bound(vec.begin(), vec.end(), 4, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::lower_bound(vec.begin(), vec.end(), 5, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::lower_bound(vec.begin(), vec.end(), 6, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 1, thrust::lower_bound(vec.begin(), vec.end(), 7, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 0, thrust::lower_bound(vec.begin(), vec.end(), 8, thrust::greater<T>()));
    ASSERT_EQ_QUIET(vec.begin() + 0, thrust::lower_bound(vec.begin(), vec.end(), 9, thrust::greater<T>()));
}

TYPED_TEST(BinarySearchDescendingTests, TestScalarUpperBoundDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    using T = typename Vector::value_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    ASSERT_EQ_QUIET(vec.begin() + 5, thrust::upper_bound(vec.begin(), vec.end(), 0, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::upper_bound(vec.begin(), vec.end(), 1, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::upper_bound(vec.begin(), vec.end(), 2, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::upper_bound(vec.begin(), vec.end(), 3, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::upper_bound(vec.begin(), vec.end(), 4, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::upper_bound(vec.begin(), vec.end(), 5, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::upper_bound(vec.begin(), vec.end(), 6, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::upper_bound(vec.begin(), vec.end(), 7, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 1, thrust::upper_bound(vec.begin(), vec.end(), 8, thrust::greater<int>()));
    ASSERT_EQ_QUIET(vec.begin() + 0, thrust::upper_bound(vec.begin(), vec.end(), 9, thrust::greater<int>()));
}

TYPED_TEST(BinarySearchDescendingTests, TestScalarBinarySearchDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    using T = typename Vector::value_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    ASSERT_EQ(true,  thrust::binary_search(vec.begin(), vec.end(), 0, thrust::greater<int>()));
    ASSERT_EQ(false, thrust::binary_search(vec.begin(), vec.end(), 1, thrust::greater<int>()));
    ASSERT_EQ(true,  thrust::binary_search(vec.begin(), vec.end(), 2, thrust::greater<int>()));
    ASSERT_EQ(false, thrust::binary_search(vec.begin(), vec.end(), 3, thrust::greater<int>()));
    ASSERT_EQ(false, thrust::binary_search(vec.begin(), vec.end(), 4, thrust::greater<int>()));
    ASSERT_EQ(true,  thrust::binary_search(vec.begin(), vec.end(), 5, thrust::greater<int>()));
    ASSERT_EQ(false, thrust::binary_search(vec.begin(), vec.end(), 6, thrust::greater<int>()));
    ASSERT_EQ(true,  thrust::binary_search(vec.begin(), vec.end(), 7, thrust::greater<int>()));
    ASSERT_EQ(true,  thrust::binary_search(vec.begin(), vec.end(), 8, thrust::greater<int>()));
    ASSERT_EQ(false, thrust::binary_search(vec.begin(), vec.end(), 9, thrust::greater<int>()));
}

TYPED_TEST(BinarySearchDescendingTests, TestScalarEqualRangeDescendingSimple)
{
    using Vector = typename TestFixture::input_type;
    using T = typename Vector::value_type;
    Vector vec(5);

    vec[0] = 8;
    vec[1] = 7;
    vec[2] = 5;
    vec[3] = 2;
    vec[4] = 0;

    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::equal_range(vec.begin(), vec.end(), 0, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::equal_range(vec.begin(), vec.end(), 1, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::equal_range(vec.begin(), vec.end(), 2, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::equal_range(vec.begin(), vec.end(), 3, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::equal_range(vec.begin(), vec.end(), 4, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::equal_range(vec.begin(), vec.end(), 5, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::equal_range(vec.begin(), vec.end(), 6, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 1, thrust::equal_range(vec.begin(), vec.end(), 7, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 0, thrust::equal_range(vec.begin(), vec.end(), 8, thrust::greater<int>()).first);
    ASSERT_EQ_QUIET(vec.begin() + 0, thrust::equal_range(vec.begin(), vec.end(), 9, thrust::greater<int>()).first);

    ASSERT_EQ_QUIET(vec.begin() + 5, thrust::equal_range(vec.begin(), vec.end(), 0, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::equal_range(vec.begin(), vec.end(), 1, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 4, thrust::equal_range(vec.begin(), vec.end(), 2, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::equal_range(vec.begin(), vec.end(), 3, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::equal_range(vec.begin(), vec.end(), 4, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 3, thrust::equal_range(vec.begin(), vec.end(), 5, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::equal_range(vec.begin(), vec.end(), 6, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 2, thrust::equal_range(vec.begin(), vec.end(), 7, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 1, thrust::equal_range(vec.begin(), vec.end(), 8, thrust::greater<int>()).second);
    ASSERT_EQ_QUIET(vec.begin() + 0, thrust::equal_range(vec.begin(), vec.end(), 9, thrust::greater<int>()).second);
}
