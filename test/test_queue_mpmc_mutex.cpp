/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
http://www.etlcpp.com

Copyright(c) 2018 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "UnitTest++.h"

#include <thread>
#include <chrono>
#include <vector>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <ostream>

#include "queue_mpmc_mutex.h"

#if defined(ETL_COMPILER_MICROSOFT)
  #include <Windows.h>
#endif

#define REALTIME_TEST 1

namespace
{
  struct Data
  {
    Data(int a, int b = 2, int c = 3, int d = 4)
    {
    }

    Data()
    {
    }

    int a;
    int b;
    int c;
    int d;
  };

  bool operator ==(const Data& lhs, const Data& rhs)
  {
    return (lhs.a == rhs.a) && (lhs.b == rhs.b) && (lhs.c == rhs.c) && (lhs.d == rhs.d);
  }

  std::ostream& operator <<(std::ostream& os, const Data& data)
  {
    os << data.a << " " << data.b << " " << data.c << " " << data.d;

    return os;
  }

  SUITE(test_queue_mpmc_mutex)
  {
    //*************************************************************************
    TEST(test_constructor)
    {
      etl::queue_mpmc_mutex<int, 4> queue;

      CHECK_EQUAL(4U, queue.max_size());
      CHECK_EQUAL(4U, queue.capacity());
    }

    //*************************************************************************
    TEST(test_size_push_pop)
    {
      etl::queue_mpmc_mutex<int, 4> queue;

      CHECK_EQUAL(0U, queue.size());

      CHECK_EQUAL(4U, queue.available());
      CHECK_EQUAL(0U, queue.size());

      queue.push(1);
      CHECK_EQUAL(1U, queue.size());
      CHECK_EQUAL(3U, queue.available());

      queue.push(2);
      CHECK_EQUAL(2U, queue.size());
      CHECK_EQUAL(2U, queue.available());

      queue.push(3);
      CHECK_EQUAL(3U, queue.size());
      CHECK_EQUAL(1U, queue.available());

      queue.push(4);
      CHECK_EQUAL(4U, queue.size());
      CHECK_EQUAL(0U, queue.available());

      CHECK(!queue.push(5));
      CHECK(!queue.push(5));

      int i;

      CHECK(queue.pop(i));
      CHECK_EQUAL(1, i);
      CHECK_EQUAL(3U, queue.size());

      CHECK(queue.pop(i));
      CHECK_EQUAL(2, i);
      CHECK_EQUAL(2U, queue.size());

      CHECK(queue.pop(i));
      CHECK_EQUAL(3, i);
      CHECK_EQUAL(1U, queue.size());

      CHECK(queue.pop(i));
      CHECK_EQUAL(4, i);
      CHECK_EQUAL(0U, queue.size());

      CHECK(!queue.pop(i));
      CHECK(!queue.pop(i));
    }

    //*************************************************************************
    TEST(test_size_emplace_pop)
    {
      etl::queue_mpmc_mutex<Data, 4> queue;

      CHECK_EQUAL(0U, queue.size());

      CHECK_EQUAL(4U, queue.available());
      CHECK_EQUAL(0U, queue.size());

      queue.emplace(1);
      CHECK_EQUAL(1U, queue.size());
      CHECK_EQUAL(3U, queue.available());

      queue.emplace(1, 2);
      CHECK_EQUAL(2U, queue.size());
      CHECK_EQUAL(2U, queue.available());

      queue.emplace(1, 2, 3);
      CHECK_EQUAL(3U, queue.size());
      CHECK_EQUAL(1U, queue.available());

      queue.emplace(1, 2, 3, 4);
      CHECK_EQUAL(4U, queue.size());
      CHECK_EQUAL(0U, queue.available());

      CHECK(!queue.emplace(1, 2, 3, 4));
      CHECK(!queue.emplace(1, 2, 3, 4));

      Data test;

      CHECK(queue.pop(test));
      CHECK_EQUAL(Data(1), test);
      CHECK_EQUAL(3U, queue.size());

      CHECK(queue.pop(test));
      CHECK_EQUAL(Data(1, 2), test);
      CHECK_EQUAL(2U, queue.size());

      CHECK(queue.pop(test));
      CHECK_EQUAL(Data(1, 2, 3), test);
      CHECK_EQUAL(1U, queue.size());

      CHECK(queue.pop(test));
      CHECK_EQUAL(Data(1, 2, 3, 4), test);
      CHECK_EQUAL(0U, queue.size());

      CHECK(!queue.pop(test));
      CHECK(!queue.pop(test));
    }

    //*************************************************************************
    TEST(test_size_push_pop_iqueue)
    {
      etl::queue_mpmc_mutex<int, 4> queue;

      etl::iqueue_mpmc_mutex<int>& iqueue = queue;

      CHECK_EQUAL(0U, iqueue.size());

      iqueue.push(1);
      CHECK_EQUAL(1U, iqueue.size());

      iqueue.push(2);
      CHECK_EQUAL(2U, iqueue.size());

      iqueue.push(3);
      CHECK_EQUAL(3U, iqueue.size());

      iqueue.push(4);
      CHECK_EQUAL(4U, iqueue.size());

      CHECK(!iqueue.push(5));
      CHECK(!iqueue.push(5));

      int i;

      CHECK(iqueue.pop(i));
      CHECK_EQUAL(1, i);
      CHECK_EQUAL(3U, iqueue.size());

      CHECK(iqueue.pop(i));
      CHECK_EQUAL(2, i);
      CHECK_EQUAL(2U, iqueue.size());

      CHECK(iqueue.pop(i));
      CHECK_EQUAL(3, i);
      CHECK_EQUAL(1U, iqueue.size());

      CHECK(iqueue.pop(i));
      CHECK_EQUAL(4, i);
      CHECK_EQUAL(0U, iqueue.size());

      CHECK(!iqueue.pop(i));
      CHECK(!iqueue.pop(i));
    }

    //*************************************************************************
    TEST(test_size_push_pop_void)
    {
      etl::queue_mpmc_mutex<int, 4> queue;

      CHECK_EQUAL(0U, queue.size());

      queue.push(1);
      CHECK_EQUAL(1U, queue.size());

      queue.push(2);
      CHECK_EQUAL(2U, queue.size());

      queue.push(3);
      CHECK_EQUAL(3U, queue.size());

      queue.push(4);
      CHECK_EQUAL(4U, queue.size());

      CHECK(!queue.push(5));
      CHECK(!queue.push(5));

      CHECK(queue.pop());
      CHECK_EQUAL(3U, queue.size());

      CHECK(queue.pop());
      CHECK_EQUAL(2U, queue.size());

      CHECK(queue.pop());
      CHECK_EQUAL(1U, queue.size());

      CHECK(queue.pop());
      CHECK_EQUAL(0U, queue.size());

      CHECK(!queue.pop());
      CHECK(!queue.pop());
    }

    //*************************************************************************
    TEST(test_clear)
    {
      etl::queue_mpmc_mutex<int, 4> queue;

      CHECK_EQUAL(0U, queue.size());

      queue.push(1);
      queue.push(2);
      queue.clear();
      CHECK_EQUAL(0U, queue.size());

      // Do it again to check that clear() didn't screw up the internals.
      queue.push(1);
      queue.push(2);
      CHECK_EQUAL(2U, queue.size());
      queue.clear();
      CHECK_EQUAL(0U, queue.size());
    }

    //*************************************************************************
    TEST(test_empty)
    {
      etl::queue_mpmc_mutex<int, 4> queue;
      CHECK(queue.empty());

      queue.push(1);
      CHECK(!queue.empty());

      queue.clear();
      CHECK(queue.empty());

      queue.push(1);
      CHECK(!queue.empty());
    }

    //*************************************************************************
    TEST(test_full)
    {
      etl::queue_mpmc_mutex<int, 4> queue;
      CHECK(!queue.full());

      queue.push(1);
      queue.push(2);
      queue.push(3);
      queue.push(4);
      CHECK(queue.full());

      queue.clear();
      CHECK(!queue.full());

      queue.push(1);
      queue.push(2);
      queue.push(3);
      queue.push(4);
      CHECK(queue.full());
    }

    //=========================================================================
#if REALTIME_TEST && defined(ETL_COMPILER_MICROSOFT)
    #if defined(ETL_TARGET_OS_WINDOWS) // Only Windows priority is currently supported
      #define SET_THREAD_PRIORITY  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL)
      #define FIX_PROCESSOR_AFFINITY1 SetThreadAffinityMask(GetCurrentThread(), 1);
      #define FIX_PROCESSOR_AFFINITY2 SetThreadAffinityMask(GetCurrentThread(), 2);
      #define FIX_PROCESSOR_AFFINITY3 SetThreadAffinityMask(GetCurrentThread(), 4);
      #define FIX_PROCESSOR_AFFINITY4 SetThreadAffinityMask(GetCurrentThread(), 8);
    #else
      #error No thread priority modifier defined
    #endif

    etl::queue_mpmc_mutex<int, 10> queue;

    const size_t LENGTH = 100000;

    std::vector<int> push1;
    std::vector<int> push2;

    std::vector<int> pop1;
    std::vector<int> pop2;

    volatile std::atomic_bool start;

    void push_thread1()
    {
      FIX_PROCESSOR_AFFINITY1;
      SET_THREAD_PRIORITY;

      size_t count = 0;
      int value = 0;

      while (!start.load());

      while (count < (LENGTH / 2))
      {
        if (queue.push(value))
        {
          push1.push_back(value);
          ++count;
          ++value;
        }
      }
    }

    void push_thread2()
    {
      FIX_PROCESSOR_AFFINITY2;
      SET_THREAD_PRIORITY;

      size_t count = 0;
      int value = LENGTH / 2;

      while (!start.load());

      while (count < (LENGTH / 2))
      {
        if (queue.push(value))
        {
          push2.push_back(value);
          ++count;
          ++value;
        }
      }
    }

    void pop_thread1()
    {
      FIX_PROCESSOR_AFFINITY3;
      SET_THREAD_PRIORITY;

      size_t count = 0;

      while (!start.load());

      while (count < (LENGTH / 2))
      {
        int i;

        if (queue.pop(i))
        {
          pop1.push_back(i);
          ++count;
        }
      }
    }

    void pop_thread2()
    {
      FIX_PROCESSOR_AFFINITY4;
      SET_THREAD_PRIORITY;

      size_t count = 0;

      while (!start.load());

      while (count < (LENGTH / 2))
      {
        int i;

        if (queue.pop(i))
        {
          pop2.push_back(i);
          ++count;
        }
      }
    }

    TEST(queue_threads)
    {
      push1.reserve(LENGTH / 2);
      push2.reserve(LENGTH / 2);;

      pop1.reserve(LENGTH / 2);;
      pop2.reserve(LENGTH / 2);;

      start = false;

      std::thread t1(push_thread1);
      std::thread t2(push_thread2);
      std::thread t3(pop_thread1);
      std::thread t4(pop_thread2);
    
      start.store(true);

      // Join the threads with the main thread
      t1.join();
      t2.join();
      t3.join();
      t4.join();

      // Combine input vectors.
      std::vector<int> push;
      push.insert(push.end(), push1.begin(), push1.end());
      push.insert(push.end(), push2.begin(), push2.end());
      std::sort(push.begin(), push.end());

      // Combine output vectors.
      std::vector<int> pop;
      pop.insert(pop.end(), pop1.begin(), pop1.end());
      pop.insert(pop.end(), pop2.begin(), pop2.end());
      std::sort(pop.begin(), pop.end());

      CHECK_EQUAL(LENGTH, push.size());
      CHECK_EQUAL(LENGTH, pop.size());

      for (size_t i = 0; i < LENGTH; ++i)
      {
        CHECK_EQUAL(push[i], pop[i]);
        CHECK_EQUAL(i, pop[i]);
      }
    }
#endif
  };
}