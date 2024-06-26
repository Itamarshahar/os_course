/**
Conversation opened. 7 messages. 1 message unread.


Skip to content
    Using Gmail with screen readers
git link
Inbox

    Itamar Shahar
16:58 (1 hour ago)
https://github.com/shmuel-cohen/os.git
4

Itamar Shahar
17:52 (34 minutes ago)
max_size: 6000000000 factor: 1.1 repeats: 100000000

Itamar Shahar <itamar.degania@gmail.com>
Attachments
18:26 (0 minutes ago)
to me


One attachment
• Scanned by Gmail
// OS 24 EX1
*/

#include "memory_latency.h"
#include "measure.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#define GALOIS_POLYNOMIAL ((1ULL << 63) | (1ULL << 62) | (1ULL << 60) | (1ULL << 59))
#define MIN_ARRAY_BYTES_SIZE 100
#define INVALID_ARGUMENTS -1;
#include <cmath>

/**
 * Converts the struct timespec to time in nano-seconds.
 * @param t - the struct timespec to convert.
 * @return - the value of time in nano-seconds.
 */
uint64_t nanosectime (struct timespec t)
{
  return t.tv_sec * 1000000000 + t.tv_nsec; //TODO check once again
//  return static_cast<uint64_t>(t.tv_sec) * 1000000000
//         + static_cast<uint64_t>(t.tv_nsec); //TODO check once again
}

/**
* Measures the average latency of accessing a given array in a sequential order.
* @param repeat - the number of times to repeat the measurement for and average on.
* @param arr - an allocated (not empty) array to preform measurement on.
* @param arr_size - the length of the array arr.
* @param zero - a variable containing zero in a way that the compiler doesn't "know" it in compilation time.
* @return struct measurement containing the measurement with the following fields:
*      double baseline - the average time (ns) taken to preform the measured operation without memory access.
*      double access_time - the average time (ns) taken to preform the measured operation with memory access.
*      uint64_t rnd - the variable used to randomly access the array, returned to prevent compiler optimizations.
*/
struct measurement
measure_sequential_latency (uint64_t repeat, array_element_t *arr, uint64_t arr_size, uint64_t zero)
{
  repeat =
      arr_size > repeat ? arr_size : repeat; // Make sure repeat >= arr_size

  // Baseline measurement:
  struct timespec t0;
  timespec_get (&t0, TIME_UTC);
  register uint64_t rnd = 12345;
  for (register uint64_t i = 0; i < repeat; i++)
  {
    register uint64_t index = i % arr_size;
    rnd ^= index & zero;
    rnd = (rnd >> 1) ^ ((0 - (rnd & 1))
                        & GALOIS_POLYNOMIAL);  // Advance rnd pseudo-randomly (using Galois LFSR)
  }
  struct timespec t1;
  timespec_get (&t1, TIME_UTC);

  // Memory access measurement:
  struct timespec t2;
  timespec_get (&t2, TIME_UTC);
  rnd = (rnd & zero) ^ 12345;
  for (register uint64_t i = 0; i < repeat; i++)
  {
    register uint64_t index = i % arr_size; //
    rnd ^= arr[index] & zero;
    rnd = (rnd >> 1) ^ ((0 - (rnd & 1))
                        & GALOIS_POLYNOMIAL);  // Advance rnd pseudo-randomly (using Galois LFSR)
  }
  struct timespec t3;
  timespec_get (&t3, TIME_UTC);

  // Calculate baseline and memory access times:
  double baseline_per_cycle =
      (double) (nanosectime (t1) - nanosectime (t0)) / (repeat);
  double memory_per_cycle =
      (double) (nanosectime (t3) - nanosectime (t2)) / (repeat);
  struct measurement result;

  result.baseline = baseline_per_cycle;
  result.access_time = memory_per_cycle;
  result.rnd = rnd;
  return result;
}

bool valid_arguments (uint64_t max_size, double factor, uint64_t repeat)
{
    if (max_size < 100)
    {
        std::cerr << "max_size must be at least 100"<< std::endl;
        return false;
    }
    if (factor <= 1)
    {
        std::cerr << "factor must great than least 1"<< std::endl;
        return false;
    }
    if (repeat <= 0)
    {
        std::cerr << "repeat must great than 0 "<< std::endl;
        return false;
    }
    return true;
}
/**
 * Runs the logic of the memory_latency program. Measures the access latency for random and sequential memory access
 * patterns.
 * Usage: './memory_latency max_size factor repeat' where:
 *      - max_size - the maximum size in bytes of the array to measure access latency for.
 *      - factor - the factor in the geometric series representing the array sizes to check.
 *      - repeat - the number of times each measurement should be repeated for and averaged on.
 * The program will print output to stdout in the following format:
 *      mem_size_1,offset_1,offset_sequential_1
 *      mem_size_2,offset_2,offset_sequential_2
 *              ...
 *              ...
 *              ...
 */
int main (int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cerr << "Usage: " << argv[0] << " max_size factor repeat"<< std::endl;
    return EXIT_FAILURE;
  }
  // zero==0, but the compiler doesn't know it. Use as the zero arg of measure_latency and measure_sequential_latency.
  struct timespec t_dummy;
  timespec_get (&t_dummy, TIME_UTC);
  const uint64_t zero = nanosectime (t_dummy) > 1000000000ull ? 0 : nanosectime (t_dummy);
  uint64_t max_size = std::strtoull(argv[1], nullptr, 10); // TODO:verify!
  double factor = std::strtod(argv[2], nullptr);// 10); // TODO:verify!
  uint64_t repeat = std::strtoull(argv[3], nullptr, 10); // TODO:verify!
  if (!valid_arguments(max_size, factor, repeat))
        {
            return INVALID_ARGUMENTS;
        }
  uint64_t cur_array_size = MIN_ARRAY_BYTES_SIZE;
  while (cur_array_size){//) * sizeof(array_element_t) <= max_size) {
//  while (cur_array_size * sizeof(array_element_t) <= max_size) {
    auto* cur_array = (array_element_t*)(malloc(cur_array_size * sizeof(array_element_t)));
    struct measurement measurement_random_access_latency = measure_latency
        (repeat,cur_array, cur_array_size, zero);
    struct measurement measurement_sequential_access_latency
        = measure_sequential_latency(repeat,cur_array, cur_array_size, zero
        );
    std::cout <<  cur_array_size  <<  "," << measurement_random_access_latency.access_time - measurement_random_access_latency.baseline << "," <<
              measurement_sequential_access_latency.access_time - measurement_sequential_access_latency.baseline << std::endl;
    free(cur_array);
    cur_array_size = static_cast<uint64_t>(std::ceil(cur_array_size * factor));
  }
  return EXIT_SUCCESS;
}
