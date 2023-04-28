// -*- c++ -*-
#ifndef COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_H_
#define COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_H_
#ifdef __cplusplus
#include "third_party/libcxx/algorithm"
#include "third_party/libcxx/cmath"
#include "third_party/libcxx/iterator"
#include "third_party/libcxx/vector"
#include "third_party/libcxx/rmi.h"
#include "third_party/libcxx/utils.h"
// clang-format off

/**
 * @file learned_sort.h
 * @author Ani Kristo, Kapil Vaidya
 * @brief The purpose of this file is to provide an implementation
 *        of Learned Sort, a model-enhanced sorting algorithm.
 *
 * @copyright Copyright (c) 2021 Ani Kristo <anikristo@gmail.com>
 * @copyright Copyright (C) 2021 Kapil Vaidya <kapilv@mit.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

using namespace std;
using namespace learned_sort;

namespace learned_sort {

// Parameters
static constexpr int PRIMARY_FANOUT = 1000;
static constexpr int SECONDARY_FANOUT = 100;
static constexpr int PRIMARY_FRAGMENT_CAPACITY = 100;
static constexpr int SECONDARY_FRAGMENT_CAPACITY = 100;
static constexpr int REP_CNT_THRESHOLD = 5;

// static struct timespec SubtractTime(struct timespec a, struct timespec b) {
//   a.tv_sec -= b.tv_sec;
//   if (a.tv_nsec < b.tv_nsec) {
//     a.tv_nsec += 1000000000;
//     a.tv_sec--;
//   }
//   a.tv_nsec -= b.tv_nsec;
//   return a;
// }

// static time_t ToNanoseconds(struct timespec ts) {
//   return ts.tv_sec * 1000000000 + ts.tv_nsec;
// }

template <class RandomIt>
void sort(RandomIt begin, RandomIt end,
          TwoLayerRMI<typename iterator_traits<RandomIt>::value_type> &rmi) {
  //----------------------------------------------------------//
  //                          INIT                            //
  //----------------------------------------------------------//

  // Determine the data type
  typedef typename iterator_traits<RandomIt>::value_type T;

  // Constants
  const long input_sz = std::distance(begin, end);
  const long TRAINING_SAMPLE_SZ = rmi.training_sample.size();

  // Keeps track of the number of elements in each bucket
  long primary_bucket_sizes[PRIMARY_FANOUT]{0};

  // Counts the number of elements that are done going through the
  // partitioning steps for good
  long num_elms_finalized = 0;

  // Cache the model parameters
  const long num_leaf_models = rmi.hp.num_leaf_models;
  double root_slope = rmi.root_model.slope;
  double root_intercept = rmi.root_model.intercept;
  auto num_models = rmi.hp.num_leaf_models;
  double slopes[num_leaf_models];
  double intercepts[num_leaf_models];
  for (auto i = 0; i < num_leaf_models; ++i) {
    slopes[i] = rmi.leaf_models[i].slope;
    intercepts[i] = rmi.leaf_models[i].intercept;
  }

  // struct timespec ts1, ts2;

  //----------------------------------------------------------//
  //              PARTITION THE KEYS INTO BUCKETS             //
  //----------------------------------------------------------//

  // clock_gettime(CLOCK_REALTIME, &ts1);
  {
    // Keeps track of the number of elements in each fragment
    long fragment_sizes[PRIMARY_FANOUT]{0};

    // An auxiliary set of fragments where the elements will be partitioned
    auto fragments = new T[PRIMARY_FANOUT][PRIMARY_FRAGMENT_CAPACITY];

    // Keeps track of the number of fragments that have been written back to the
    // original array
    long fragments_written = 0;

    // Points to the next free space where to write back
    auto write_itr = begin;

    // For each element in the input, predict which bucket it would go to, and
    // insert to the respective bucket fragment
    for (auto it = begin; it != end; ++it) {
      // Predict the model id in the leaf layer of the RMI
      long pred_bucket_idx = static_cast<long>(std::max(
          0.,
          std::min(num_leaf_models - 1., root_slope * it[0] + root_intercept)));

      // Predict the CDF
      double pred_cdf =
          slopes[pred_bucket_idx] * it[0] + intercepts[pred_bucket_idx];

      // Get the predicted bucket id
      pred_bucket_idx = static_cast<long>(std::max(
          0., std::min(PRIMARY_FANOUT - 1., pred_cdf * PRIMARY_FANOUT)));

      // Place the current element in the predicted fragment
      fragments[pred_bucket_idx][fragment_sizes[pred_bucket_idx]] = it[0];

      // Update the fragment size and the bucket size
      primary_bucket_sizes[pred_bucket_idx]++;
      fragment_sizes[pred_bucket_idx]++;

      if (fragment_sizes[pred_bucket_idx] == PRIMARY_FRAGMENT_CAPACITY) {
        fragments_written++;
        // The predicted fragment is full, place in the array and update bucket
        // size
        std::move(fragments[pred_bucket_idx],
                  fragments[pred_bucket_idx] + PRIMARY_FRAGMENT_CAPACITY,
                  write_itr);
        write_itr += PRIMARY_FRAGMENT_CAPACITY;

        // Reset the fragment size
        fragment_sizes[pred_bucket_idx] = 0;
      }
    }

    //----------------------------------------------------------//
    //                     DEFRAGMENTATION                      //
    //----------------------------------------------------------//

    // Records the ending offset for the buckets
    long bucket_end_offset[PRIMARY_FANOUT]{0};
    bucket_end_offset[0] = primary_bucket_sizes[0];

    // Swap space
    T *swap_buffer = new T[PRIMARY_FRAGMENT_CAPACITY];

    // Maintains a writing iterator for each bucket, initialized at the starting
    // offsets
    long bucket_write_off[PRIMARY_FANOUT]{0};
    bucket_write_off[0] = 0;

    // Calculate the starting and ending offsets of each bucket
    for (long bucket_idx = 1; bucket_idx < PRIMARY_FANOUT; ++bucket_idx) {
      // Calculate the bucket end offsets (prefix sum)
      bucket_end_offset[bucket_idx] =
          primary_bucket_sizes[bucket_idx] + bucket_end_offset[bucket_idx - 1];

      // Calculate the bucket start offsets and assign the writing iterator to
      // that value

      // These offsets are aligned w.r.t. PRIMARY_FRAGMENT_CAPACITY
      bucket_write_off[bucket_idx] = ceil(bucket_end_offset[bucket_idx - 1] *
                                          1. / PRIMARY_FRAGMENT_CAPACITY) *
                                     PRIMARY_FRAGMENT_CAPACITY;

      // Fence the bucket iterator. This might occur because the write offset is
      // not necessarily aligned with PRIMARY_FRAGMENT_CAPACITY
      if (bucket_write_off[bucket_idx] > bucket_end_offset[bucket_idx]) {
        bucket_write_off[bucket_idx] = bucket_end_offset[bucket_idx];
      }
    }

    // This keeps track of which bucket we are operating on
    long stored_bucket_idx = 0;

    // Go over each fragment and re-arrange them so that they are placed
    // contiguously within each bucket boundaries
    for (long fragment_idx = 0; fragment_idx < fragments_written;
         ++fragment_idx) {
      auto cur_fragment_start_off = fragment_idx * PRIMARY_FRAGMENT_CAPACITY;

      // Find the bucket where this fragment is stored into, which is not
      // necessarily the bucket it belongs to
      while (cur_fragment_start_off >= bucket_end_offset[stored_bucket_idx]) {
        ++stored_bucket_idx;
      }

      // Skip this fragment if its starting index is lower than the writing
      // offset for the current bucket. This means that the fragment has already
      // been dealt with
      if (cur_fragment_start_off < bucket_write_off[stored_bucket_idx]) {
        continue;
      }

      // Find out what bucket the current fragment belongs to by looking at RMI
      // prediction for the first element of the fragment.
      auto first_elm_in_fragment = begin[cur_fragment_start_off];
      long pred_bucket_for_cur_fragment = static_cast<long>(std::max(
          0., std::min(num_leaf_models - 1.,
                       root_slope * first_elm_in_fragment + root_intercept)));

      // Predict the CDF
      double pred_cdf =
          slopes[pred_bucket_for_cur_fragment] * first_elm_in_fragment +
          intercepts[pred_bucket_for_cur_fragment];

      // Get the predicted bucket id
      pred_bucket_for_cur_fragment = static_cast<long>(std::max(
          0., std::min(PRIMARY_FANOUT - 1., pred_cdf * PRIMARY_FANOUT)));

      // If the current bucket contains fragments that are not all the way full,
      // no need to use a swap buffer, since there is available space. The first
      // condition checks whether the current fragment will need to be written
      // in a bucket that was already consumed. The second condition checks
      // whether the writing offset of the predicted bucket is beyond the last
      // element written into the array. This means that there is empty space to
      // write the fragments to.
      if (bucket_write_off[pred_bucket_for_cur_fragment] <
              cur_fragment_start_off ||
          bucket_write_off[pred_bucket_for_cur_fragment] >=
              fragments_written * PRIMARY_FRAGMENT_CAPACITY) {
        // If the current fragment will not be the last one to write in the
        // predicted bucket
        if (bucket_write_off[pred_bucket_for_cur_fragment] +
                PRIMARY_FRAGMENT_CAPACITY <=
            bucket_end_offset[pred_bucket_for_cur_fragment]) {
          auto write_itr =
              begin + bucket_write_off[pred_bucket_for_cur_fragment];
          auto read_itr = begin + cur_fragment_start_off;

          // Move the elements of the fragment to the bucket's write offset
          std::copy(read_itr, read_itr + PRIMARY_FRAGMENT_CAPACITY, write_itr);

          // Update the bucket write offset
          bucket_write_off[pred_bucket_for_cur_fragment] +=
              PRIMARY_FRAGMENT_CAPACITY;

        } else {  // This is the last fragment to write into the predicted
                  // bucket
          auto write_itr =
              begin + bucket_write_off[pred_bucket_for_cur_fragment];
          auto read_itr = begin + cur_fragment_start_off;

          // Calculate the fragment size
          auto cur_fragment_sz =
              bucket_end_offset[pred_bucket_for_cur_fragment] -
              bucket_write_off[pred_bucket_for_cur_fragment];

          // Move the elements of the fragment to the bucket's write offset
          std::copy(read_itr, read_itr + cur_fragment_sz, write_itr);

          // Update the bucket write offset for the predicted bucket
          bucket_write_off[pred_bucket_for_cur_fragment] =
              bucket_end_offset[pred_bucket_for_cur_fragment];

          // Place the remaining elements of this fragment into the auxiliary
          // fragment memory. This is needed when the start of the bucket might
          // have empty spaces because the writing iterator was not aligned with
          // FRAGMENT_CAPACITY (not a multiple)
          for (int elm_idx = cur_fragment_sz;
               elm_idx < PRIMARY_FRAGMENT_CAPACITY; elm_idx++) {
            fragments[pred_bucket_for_cur_fragment]
                     [fragment_sizes[pred_bucket_for_cur_fragment] + elm_idx -
                      cur_fragment_sz] = read_itr[elm_idx];
          }

          // Update the auxiliary fragment size
          fragment_sizes[pred_bucket_for_cur_fragment] +=
              PRIMARY_FRAGMENT_CAPACITY - cur_fragment_sz;
        }

      } else {  // The current fragment is to be written in a non-empty space,
                // so an incumbent fragment will need to be evicted

        // If the fragment is already within the correct bucket
        if (pred_bucket_for_cur_fragment == stored_bucket_idx) {
          // If the empty area left in the bucket is not enough for all the
          // elements in the fragment. This is needed when the start of the
          // bucket might have empty spaces because the writing iterator was not
          // aligned with FRAGMENT_CAPACITY (not a multiple)
          if (bucket_end_offset[stored_bucket_idx] -
                  bucket_write_off[stored_bucket_idx] <
              PRIMARY_FRAGMENT_CAPACITY) {
            auto write_itr = begin + bucket_write_off[stored_bucket_idx];
            auto read_itr = begin + cur_fragment_start_off;

            // Calculate the amount of space left for the current bucket
            long remaining_space = bucket_end_offset[stored_bucket_idx] -
                                   bucket_write_off[stored_bucket_idx];

            // Write out the fragment in the remaining space
            std::copy(read_itr, read_itr + remaining_space, write_itr);

            // Update the read iterator to point to the remaining elements
            read_itr = begin + cur_fragment_start_off + remaining_space;

            // Calculate the fragment size
            auto cur_fragment_sz = fragment_sizes[stored_bucket_idx];

            // Write the remaining elements into the auxiliary fragment space
            for (int k = 0; k < PRIMARY_FRAGMENT_CAPACITY - remaining_space;
                 ++k) {
              fragments[stored_bucket_idx][cur_fragment_sz++] = *(read_itr++);
            }

            // Update the fragment size after the new placements
            fragment_sizes[stored_bucket_idx] = cur_fragment_sz;

            // Update the bucket write offset to indicate that the bucket was
            // fully written
            bucket_write_off[stored_bucket_idx] =
                bucket_end_offset[stored_bucket_idx];

          } else {  // The bucket has enough space for the incoming fragment

            auto write_itr = begin + bucket_write_off[stored_bucket_idx];
            auto read_itr = begin + cur_fragment_start_off;

            if (write_itr != read_itr) {
              // Write the elements to the bucket's write offset
              std::copy(read_itr, read_itr + PRIMARY_FRAGMENT_CAPACITY,
                        write_itr);
            }

            // Update the write offset for the current bucket
            bucket_write_off[stored_bucket_idx] += PRIMARY_FRAGMENT_CAPACITY;
          }
        } else {  // The fragment is not in the correct bucket and needs to be
          // swapped out with an incorrectly placed fragment there

          // Predict the bucket of the fragment that will be swapped out
          auto first_elm_in_fragment_to_be_swapped_out =
              begin[bucket_write_off[pred_bucket_for_cur_fragment]];

          long pred_bucket_for_fragment_to_be_swapped_out =
              static_cast<long>(std::max(
                  0., std::min(
                          num_leaf_models - 1.,
                          root_slope * first_elm_in_fragment_to_be_swapped_out +
                              root_intercept)));

          // Predict the CDF
          double pred_cdf =
              slopes[pred_bucket_for_fragment_to_be_swapped_out] *
                  first_elm_in_fragment_to_be_swapped_out +
              intercepts[pred_bucket_for_fragment_to_be_swapped_out];

          // Get the predicted bucket idx
          pred_bucket_for_fragment_to_be_swapped_out = static_cast<long>(
              std::max(0., std::min(PRIMARY_FANOUT - 1.,
                                    pred_cdf * PRIMARY_FANOUT)));

          // If the fragment at the next write offset is not already in the
          // right bucket, swap the fragments
          if (pred_bucket_for_fragment_to_be_swapped_out !=
              pred_bucket_for_cur_fragment) {
            // Move the contents at the write offset into a swap buffer
            auto itr_buf1 =
                begin + bucket_write_off[pred_bucket_for_cur_fragment];
            auto itr_buf2 = begin + cur_fragment_start_off;
            std::copy(itr_buf1, itr_buf1 + PRIMARY_FRAGMENT_CAPACITY,
                      swap_buffer);

            // Write the contents of the incoming fragment
            std::copy(itr_buf2, itr_buf2 + PRIMARY_FRAGMENT_CAPACITY, itr_buf1);

            // Place the swap buffer into the emptied space
            std::copy(swap_buffer, swap_buffer + PRIMARY_FRAGMENT_CAPACITY,
                      itr_buf2);

            pred_bucket_for_cur_fragment =
                pred_bucket_for_fragment_to_be_swapped_out;
          } else {  // The fragment at the write offset is already in the right
                    // bucket
            bucket_write_off[pred_bucket_for_cur_fragment] +=
                PRIMARY_FRAGMENT_CAPACITY;
          }

          // Decrement the fragment index so that the newly swapped in fragment
          // is not skipped over
          --fragment_idx;
        }
      }
    }

    // Add the elements remaining in the auxiliary fragments to the buckets they
    // belong to. This is for when the fragments weren't full and thus not
    // flushed to the input array
    for (long bucket_idx = 0; bucket_idx < PRIMARY_FANOUT; ++bucket_idx) {
      // Set the writing offset to the beggining of the bucket
      long write_off = bucket_end_offset[bucket_idx - 1];
      if (bucket_idx == 0) {
        write_off = 0;
      }

      // Add the elements left in the auxiliary fragments to the beggining of
      // the predicted bucket, since it was not full
      long elm_idx;
      for (elm_idx = 0; (elm_idx < fragment_sizes[bucket_idx]) &&
                        (write_off % PRIMARY_FRAGMENT_CAPACITY != 0);
           ++elm_idx) {
        begin[write_off++] = fragments[bucket_idx][elm_idx];
      }

      // Add the remaining elements from the auxiliary fragments to the end of
      // the bucket it belongs to
      write_off = bucket_end_offset[bucket_idx] - fragment_sizes[bucket_idx];
      for (; elm_idx < fragment_sizes[bucket_idx]; ++elm_idx) {
        begin[write_off + elm_idx] = fragments[bucket_idx][elm_idx];
        ++bucket_write_off[bucket_idx];
      }
    }

    // Cleanup
    delete[] swap_buffer;
    delete[] fragments;
  }
  // clock_gettime(CLOCK_REALTIME, &ts2);
  // printf("PARTITION THE KEYS INTO BUCKETS %ld us\n",
  //        ToNanoseconds(SubtractTime(ts2, ts1)) / 1000);

  //----------------------------------------------------------//
  //                SECOND ROUND OF PARTITIONING              //
  //----------------------------------------------------------//

  // clock_gettime(CLOCK_REALTIME, &ts1);
  {
    // Iterate over each bucket starting from the end so that the merging step
    // later is done in-place
    auto primary_bucket_start = begin;
    auto primary_bucket_end = begin;
    for (long primary_bucket_idx = 0; primary_bucket_idx < PRIMARY_FANOUT;
         ++primary_bucket_idx) {
      auto primary_bucket_sz = primary_bucket_sizes[primary_bucket_idx];

      // Skip bucket if empty
      if (primary_bucket_sz == 0) continue;

      // Update the start and the end of the bucket data
      primary_bucket_start = primary_bucket_end;
      primary_bucket_end += primary_bucket_sz;

      // Check for homogeneity
      bool is_homogeneous = true;
      if (rmi.enable_dups_detection) {
        for (long elm_idx = 1; elm_idx < primary_bucket_sz; ++elm_idx) {
          if (primary_bucket_start[elm_idx] !=
              primary_bucket_start[elm_idx - 1]) {
            is_homogeneous = false;
            break;
          }
        }
      }

      // When the bucket is homogeneous, skip sorting it
      if (rmi.enable_dups_detection && is_homogeneous) {
        num_elms_finalized += primary_bucket_sz;

      }

      // When the bucket is not homogeneous, and it's not flagged for duplicates
      else {
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - -  -//
        //        PARTITION THE KEYS INTO SECONDARY BUCKETS         //
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - -  -//

        // Keeps track of the number of elements in each secondary bucket
        long secondary_bucket_sizes[SECONDARY_FANOUT]{0};

        // Keeps track of the number of elements in each fragment
        long fragment_sizes[SECONDARY_FANOUT]{0};

        // An auxiliary set of fragments where the elements will be partitioned
        auto fragments = new T[SECONDARY_FANOUT][SECONDARY_FRAGMENT_CAPACITY];

        // Keeps track of the number of fragments that have been written back to
        // the original array
        long fragments_written = 0;

        // Points to the next free space where to write back
        auto write_itr = primary_bucket_start;

        // For each element in the input, predict which bucket it would go to,
        // and insert to the respective bucket fragment
        for (auto it = primary_bucket_start; it != primary_bucket_end; ++it) {
          // Predict the model id in the leaf layer of the RMI
          long pred_bucket_idx = static_cast<long>(
              std::max(0., std::min(num_leaf_models - 1.,
                                    root_slope * it[0] + root_intercept)));

          // Predict the CDF
          double pred_cdf =
              slopes[pred_bucket_idx] * it[0] + intercepts[pred_bucket_idx];

          // Get the predicted bucket id
          pred_bucket_idx = static_cast<long>(std::max(
              0., std::min(SECONDARY_FANOUT - 1.,
                           (pred_cdf * PRIMARY_FANOUT - primary_bucket_idx) *
                               SECONDARY_FANOUT)));

          // Place the current element in the predicted fragment
          fragments[pred_bucket_idx][fragment_sizes[pred_bucket_idx]] = it[0];

          // Update the fragment size and the bucket size
          ++secondary_bucket_sizes[pred_bucket_idx];
          ++fragment_sizes[pred_bucket_idx];

          if (fragment_sizes[pred_bucket_idx] == SECONDARY_FRAGMENT_CAPACITY) {
            ++fragments_written;
            // The predicted fragment is full, place in the array and update
            // bucket size
            std::move(fragments[pred_bucket_idx],
                      fragments[pred_bucket_idx] + SECONDARY_FRAGMENT_CAPACITY,
                      write_itr);
            write_itr += SECONDARY_FRAGMENT_CAPACITY;

            // Reset the fragment size
            fragment_sizes[pred_bucket_idx] = 0;
          }
        }  // end of partitioninig over secondary fragments

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - -  -//
        //                      DEFRAGMENTATION                     //
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - -  -//

        // Records the ending offset for the buckets
        long bucket_end_offset[SECONDARY_FANOUT]{0};
        bucket_end_offset[0] = secondary_bucket_sizes[0];

        // Swap space
        T *swap_buffer = new T[SECONDARY_FRAGMENT_CAPACITY];

        // Maintains a writing iterator for each bucket, initialized at the
        // starting offsets
        long bucket_start_off[SECONDARY_FANOUT]{0};
        bucket_start_off[0] = 0;

        // Calculate the starting and ending offsets of each bucket
        for (long bucket_idx = 1; bucket_idx < SECONDARY_FANOUT; ++bucket_idx) {
          // Calculate the bucket end offsets (prefix sum)
          bucket_end_offset[bucket_idx] = secondary_bucket_sizes[bucket_idx] +
                                          bucket_end_offset[bucket_idx - 1];

          // Calculate the bucket start offsets and assign the writing
          // iterator to that value

          // These offsets are aligned w.r.t. SECONDARY_FRAGMENT_CAPACITY
          bucket_start_off[bucket_idx] =
              ceil(bucket_end_offset[bucket_idx - 1] * 1. /
                   SECONDARY_FRAGMENT_CAPACITY) *
              SECONDARY_FRAGMENT_CAPACITY;

          // Fence the bucket iterator. This might occur because the write
          // offset is not necessarily aligned with SECONDARY_FRAGMENT_CAPACITY
          if (bucket_start_off[bucket_idx] > bucket_end_offset[bucket_idx]) {
            bucket_start_off[bucket_idx] = bucket_end_offset[bucket_idx];
          }
        }

        // This keeps track of which bucket we are operating on
        long stored_bucket_idx = 0;

        // Go over each fragment and re-arrange them so that they are placed
        // contiguously within each bucket boundaries
        for (long fragment_idx = 0; fragment_idx < fragments_written;
             ++fragment_idx) {
          auto cur_fragment_start_off =
              fragment_idx * SECONDARY_FRAGMENT_CAPACITY;

          // Find the bucket where this fragment is stored into, which is not
          // necessarily the bucket it belongs to
          while (cur_fragment_start_off >=
                 bucket_end_offset[stored_bucket_idx]) {
            ++stored_bucket_idx;
          }

          // Skip this fragment if its starting index is lower than the writing
          // offset for the current bucket. This means that the fragment has
          // already been dealt with
          if (cur_fragment_start_off < bucket_start_off[stored_bucket_idx]) {
            continue;
          }

          // Find out what bucket the current fragment belongs to by looking at
          // RMI prediction for the first element of the fragment.
          auto first_elm_in_fragment =
              primary_bucket_start[cur_fragment_start_off];
          long pred_bucket_for_cur_fragment = static_cast<long>(std::max(
              0.,
              std::min(num_leaf_models - 1.,
                       root_slope * first_elm_in_fragment + root_intercept)));

          // Predict the CDF
          double pred_cdf =
              slopes[pred_bucket_for_cur_fragment] * first_elm_in_fragment +
              intercepts[pred_bucket_for_cur_fragment];

          // Get the predicted bucket id
          pred_bucket_for_cur_fragment = static_cast<long>(std::max(
              0., std::min(SECONDARY_FANOUT - 1.,
                           (pred_cdf * PRIMARY_FANOUT - primary_bucket_idx) *
                               SECONDARY_FANOUT)));

          // If the current bucket contains fragments that are not all the way
          // full, no need to use a swap fragment, since there is available
          // space. The first condition checks whether the current fragment will
          // need to be written in a bucket that was already consumed. The
          // second condition checks whether the writing offset of the
          // predicted bucket is beyond the last element written into the
          // array. This means that there is empty space to write the fragments
          // to.
          if (bucket_start_off[pred_bucket_for_cur_fragment] <
                  cur_fragment_start_off ||
              bucket_start_off[pred_bucket_for_cur_fragment] >=
                  fragments_written * SECONDARY_FRAGMENT_CAPACITY) {
            // If the current fragment will not be the last one to write in the
            // predicted bucket
            if (bucket_start_off[pred_bucket_for_cur_fragment] +
                    SECONDARY_FRAGMENT_CAPACITY <=
                bucket_end_offset[pred_bucket_for_cur_fragment]) {
              auto write_itr = primary_bucket_start +
                               bucket_start_off[pred_bucket_for_cur_fragment];
              auto read_itr = primary_bucket_start + cur_fragment_start_off;

              // Move the elements of the fragment to the bucket's write offset
              std::copy(read_itr, read_itr + SECONDARY_FRAGMENT_CAPACITY,
                        write_itr);

              // Update the bucket write offset
              bucket_start_off[pred_bucket_for_cur_fragment] +=
                  SECONDARY_FRAGMENT_CAPACITY;

            } else {  // This is the last fragment to write into the predicted
                      // bucket
              auto write_itr = primary_bucket_start +
                               bucket_start_off[pred_bucket_for_cur_fragment];
              auto read_itr = primary_bucket_start + cur_fragment_start_off;

              // Calculate the fragment size
              auto cur_fragment_sz =
                  bucket_end_offset[pred_bucket_for_cur_fragment] -
                  bucket_start_off[pred_bucket_for_cur_fragment];

              // Move the elements of the fragment to the bucket's write offset
              std::copy(read_itr, read_itr + cur_fragment_sz, write_itr);

              // Update the bucket write offset for the predicted bucket
              bucket_start_off[pred_bucket_for_cur_fragment] =
                  bucket_end_offset[pred_bucket_for_cur_fragment];

              // Place the remaining elements of this fragment into the
              // auxiliary fragment memory. This is needed when the start of the
              // bucket might have empty spaces because the writing iterator
              // was not aligned with FRAGMENT_CAPACITY (not a multiple)
              for (long elm_idx = cur_fragment_sz;
                   elm_idx < SECONDARY_FRAGMENT_CAPACITY; elm_idx++) {
                fragments[pred_bucket_for_cur_fragment]
                         [fragment_sizes[pred_bucket_for_cur_fragment] +
                          elm_idx - cur_fragment_sz] = read_itr[elm_idx];
              }

              // Update the auxiliary fragment size
              fragment_sizes[pred_bucket_for_cur_fragment] +=
                  SECONDARY_FRAGMENT_CAPACITY - cur_fragment_sz;
            }

          } else {  // The current fragment is to be written in a non-empty
                    // space, so an incumbent fragment will need to be evicted

            // If the fragment is already within the correct bucket
            if (pred_bucket_for_cur_fragment == stored_bucket_idx) {
              // If the empty area left in the bucket is not enough for all
              // the elements in the fragment. This is needed when the start of
              // the bucket might have empty spaces because the writing
              // iterator was not aligned with FRAGMENT_CAPACITY (not a
              // multiple)
              if (bucket_end_offset[stored_bucket_idx] -
                      bucket_start_off[stored_bucket_idx] <
                  SECONDARY_FRAGMENT_CAPACITY) {
                auto write_itr =
                    primary_bucket_start + bucket_start_off[stored_bucket_idx];
                auto read_itr = primary_bucket_start + cur_fragment_start_off;

                // Calculate the amount of space left for the current bucket
                long remaining_space = bucket_end_offset[stored_bucket_idx] -
                                       bucket_start_off[stored_bucket_idx];

                // Write out the fragment in the remaining space
                std::copy(read_itr, read_itr + remaining_space, write_itr);

                // Update the read iterator to point to the remaining elements
                read_itr = primary_bucket_start + cur_fragment_start_off +
                           remaining_space;

                // Calculate the fragment size
                auto cur_fragment_sz = fragment_sizes[stored_bucket_idx];

                // Write the remaining elements into the auxiliary fragment
                // space
                for (int k = 0;
                     k < SECONDARY_FRAGMENT_CAPACITY - remaining_space; ++k) {
                  fragments[stored_bucket_idx][cur_fragment_sz++] =
                      *(read_itr++);
                }

                // Update the fragment size after the new placements
                fragment_sizes[stored_bucket_idx] = cur_fragment_sz;

                // Update the bucket write offset to indicate that the bucket
                // was fully written
                bucket_start_off[stored_bucket_idx] =
                    bucket_end_offset[stored_bucket_idx];

              } else {  // The bucket has enough space for the incoming fragment

                auto write_itr =
                    primary_bucket_start + bucket_start_off[stored_bucket_idx];
                auto read_itr = primary_bucket_start + cur_fragment_start_off;

                if (write_itr != read_itr) {
                  // Write the elements to the bucket's write offset
                  std::copy(read_itr, read_itr + SECONDARY_FRAGMENT_CAPACITY,
                            write_itr);
                }

                // Update the write offset for the current bucket
                bucket_start_off[stored_bucket_idx] +=
                    SECONDARY_FRAGMENT_CAPACITY;
              }
            } else {  // The fragment is not in the correct bucket and needs to
                      // be
              // swapped out with an incorrectly placed fragment there

              // Predict the bucket of the fragment that will be swapped out
              auto first_elm_in_fragment_to_be_swapped_out =
                  primary_bucket_start
                      [bucket_start_off[pred_bucket_for_cur_fragment]];

              long pred_bucket_for_fragment_to_be_swapped_out =
                  static_cast<long>(std::max(
                      0.,
                      std::min(
                          num_leaf_models - 1.,
                          root_slope * first_elm_in_fragment_to_be_swapped_out +
                              root_intercept)));

              // Predict the CDF
              double pred_cdf =
                  slopes[pred_bucket_for_fragment_to_be_swapped_out] *
                      first_elm_in_fragment_to_be_swapped_out +
                  intercepts[pred_bucket_for_fragment_to_be_swapped_out];

              // Get the predicted bucket idx
              pred_bucket_for_fragment_to_be_swapped_out = static_cast<long>(
                  std::max(0., std::min(SECONDARY_FANOUT - 1.,
                                        (pred_cdf * PRIMARY_FANOUT -
                                         primary_bucket_idx) *
                                            SECONDARY_FANOUT)));

              // If the fragment at the next write offset is not already in the
              // right bucket, swap the fragments
              if (pred_bucket_for_fragment_to_be_swapped_out !=
                  pred_bucket_for_cur_fragment) {
                // Move the contents at the write offset into a swap fragment
                auto itr_buf1 = primary_bucket_start +
                                bucket_start_off[pred_bucket_for_cur_fragment];
                auto itr_buf2 = primary_bucket_start + cur_fragment_start_off;
                std::copy(itr_buf1, itr_buf1 + SECONDARY_FRAGMENT_CAPACITY,
                          swap_buffer);

                // Write the contents of the incoming fragment
                std::copy(itr_buf2, itr_buf2 + SECONDARY_FRAGMENT_CAPACITY,
                          itr_buf1);

                // Place the swap buffer into the emptied space
                std::copy(swap_buffer,
                          swap_buffer + SECONDARY_FRAGMENT_CAPACITY, itr_buf2);

                pred_bucket_for_cur_fragment =
                    pred_bucket_for_fragment_to_be_swapped_out;
              } else {  // The fragment at the write offset is already in the
                        // right bucket
                bucket_start_off[pred_bucket_for_cur_fragment] +=
                    SECONDARY_FRAGMENT_CAPACITY;
              }

              // Decrement the fragment index so that the newly swapped in
              // fragment is not skipped over
              --fragment_idx;
            }
          }
        }

        // Add the elements remaining in the auxiliary fragments to the buckets
        // they belong to. This is for when the fragments weren't full and thus
        // not flushed to the input array
        for (long bucket_idx = 0; bucket_idx < SECONDARY_FANOUT; ++bucket_idx) {
          // Set the writing offset to the beggining of the bucket
          long write_off = bucket_end_offset[bucket_idx - 1];
          if (bucket_idx == 0) {
            write_off = 0;
          }

          // Add the elements left in the auxiliary fragments to the beggining
          // of the predicted bucket, since it was not full
          long elm_idx;
          for (elm_idx = 0; (elm_idx < fragment_sizes[bucket_idx]) &&
                            (write_off % SECONDARY_FRAGMENT_CAPACITY != 0);
               ++elm_idx) {
            primary_bucket_start[write_off++] = fragments[bucket_idx][elm_idx];
          }

          // Add the remaining elements from the auxiliary fragments to the end
          // of the bucket it belongs to
          write_off =
              bucket_end_offset[bucket_idx] - fragment_sizes[bucket_idx];
          for (; elm_idx < fragment_sizes[bucket_idx]; ++elm_idx) {
            primary_bucket_start[write_off + elm_idx] =
                fragments[bucket_idx][elm_idx];
            ++bucket_start_off[bucket_idx];
          }
        }

        // Cleanup
        delete[] swap_buffer;
        delete[] fragments;

        //- - - - - - - - - - - - - - - - - - - - - - - - - - - -  -//
        //                MODEL-BASED COUNTING SORT                 //
        //- - - - - - - - - - - - - - - - - - - - - - - - - - - -  -//
        // Iterate over the secondary buckets
        for (long secondary_bucket_idx = 0;
             secondary_bucket_idx < SECONDARY_FANOUT; ++secondary_bucket_idx) {
          auto secondary_bucket_sz =
              secondary_bucket_sizes[secondary_bucket_idx];
          auto secondary_bucket_start_off = num_elms_finalized;
          auto secondary_bucket_end_off =
              secondary_bucket_start_off + secondary_bucket_sz;

          // Skip bucket if empty
          if (secondary_bucket_sz == 0) continue;

          // Check for homogeneity
          bool is_homogeneous = true;
          if (rmi.enable_dups_detection) {
            for (long elm_idx = secondary_bucket_start_off + 1;
                 elm_idx < secondary_bucket_end_off; ++elm_idx) {
              if (begin[elm_idx] != begin[elm_idx - 1]) {
                is_homogeneous = false;
                break;
              }
            }
          }

          if (!(rmi.enable_dups_detection and is_homogeneous)) {
            long adjustment_offset =
                1. *
                (primary_bucket_idx * SECONDARY_FANOUT + secondary_bucket_idx) *
                input_sz / (PRIMARY_FANOUT * SECONDARY_FANOUT);

            // Saves the predicted CDFs for the Counting Sort subroutine
            vector<long> pred_cache_cs(secondary_bucket_sz);

            // Count array for the model-enhanced counting sort subroutine
            vector<long> cnt_hist(secondary_bucket_sz, 0);

            /*
             * OPTIMIZATION
             * We check to see if the first and last element in the current
             * bucket used the same leaf model to obtain their CDF. If that is
             * the case, then we don't need to traverse the CDF model for
             * every element in this bucket, hence decreasing the inference
             * complexity from O(num_layer) to O(1).
             */

            long pred_model_first_elm = static_cast<long>(std::max(
                0., std::min(num_leaf_models - 1.,
                             root_slope * begin[secondary_bucket_start_off] +
                                 root_intercept)));

            long pred_model_last_elm = static_cast<long>(std::max(
                0., std::min(num_leaf_models - 1.,
                             root_slope * begin[secondary_bucket_end_off - 1] +
                                 root_intercept)));

            if (pred_model_first_elm == pred_model_last_elm) {
              // Avoid CDF model traversal and predict the CDF only using the
              // leaf model

              // Iterate over the elements and place them into the secondary
              // buckets
              for (long elm_idx = 0; elm_idx < secondary_bucket_sz; ++elm_idx) {
                // Find the current element
                auto cur_key = begin[secondary_bucket_start_off + elm_idx];

                // Predict the CDF
                double pred_cdf = slopes[pred_model_first_elm] * cur_key +
                                  intercepts[pred_model_first_elm];

                // Scale the predicted CDF to the input size and save it
                pred_cache_cs[elm_idx] = static_cast<long>(std::max(
                    0., std::min(secondary_bucket_sz - 1.,
                                 (pred_cdf * input_sz) - adjustment_offset)));

                // Update the counts
                ++cnt_hist[pred_cache_cs[elm_idx]];
              }
            } else {
              // Fully traverse the CDF model again to predict the CDF of the
              // current element

              // Iterate over the elements and place them into the minor
              // buckets
              for (long elm_idx = 0; elm_idx < secondary_bucket_sz; ++elm_idx) {
                // Find the current element
                auto cur_key = begin[secondary_bucket_start_off + elm_idx];

                // Predict the model idx in the leaf layer
                auto model_idx_next_layer = static_cast<long>(std::max(
                    0., std::min(num_leaf_models - 1.,
                                 root_slope * cur_key + root_intercept)));
                // Predict the CDF
                double pred_cdf = slopes[model_idx_next_layer] * cur_key +
                                  intercepts[model_idx_next_layer];

                // Scale the predicted CDF to the input size and save it
                pred_cache_cs[elm_idx] = static_cast<long>(std::max(
                    0., std::min(secondary_bucket_sz - 1.,
                                 (pred_cdf * input_sz) - adjustment_offset)));

                // Update the counts
                ++cnt_hist[pred_cache_cs[elm_idx]];
              }
            }

            --cnt_hist[0];

            // Calculate the running totals
            for (long i = 1; i < secondary_bucket_sz; ++i) {
              cnt_hist[i] += cnt_hist[i - 1];
            }

            // Allocate a temporary buffer for placing the keys in sorted
            // order
            vector<T> tmp(secondary_bucket_sz);

            // Re-shuffle the elms based on the calculated cumulative counts
            for (long elm_idx = 0; elm_idx < secondary_bucket_sz; ++elm_idx) {
              // Place the element in the predicted position in the array

              tmp[cnt_hist[pred_cache_cs[elm_idx]]] =
                  begin[secondary_bucket_start_off + elm_idx];

              // Update counts
              --cnt_hist[pred_cache_cs[elm_idx]];
            }

            // Write back the temprorary buffer to the original input
            std::copy(tmp.begin(), tmp.end(),
                      begin + secondary_bucket_start_off);
          }
          // Update the number of finalized elements
          num_elms_finalized += secondary_bucket_sz;
        }  // end of iteration over the secondary buckets

      }  // end of processing for non-flagged, non-homogeneous primary buckets
    }    // end of iteration over primary buckets
  }
  // clock_gettime(CLOCK_REALTIME, &ts2);
  // printf("SECOND ROUND OF PARTITIONING %ld us\n",
  //        ToNanoseconds(SubtractTime(ts2, ts1)) / 1000);

  // Touch up
  // clock_gettime(CLOCK_REALTIME, &ts1);
  learned_sort::utils::insertion_sort(begin, end);
  // clock_gettime(CLOCK_REALTIME, &ts2);
  // printf("INSERTION SORT %ld us\n",
  //        ToNanoseconds(SubtractTime(ts2, ts1)) / 1000);
}

/**
 * @brief Sorts a sequence of numerical keys from [begin, end) using Learned
 * Sort, in ascending order.
 *
 * @tparam RandomIt A bi-directional random iterator over the sequence of keys
 * @param begin Random-access iterators to the initial position of the
 * sequence to be used for sorting. The range used is [begin,end), which
 * contains all the elements between first and last, including the element
 * pointed by first but not the element pointed by last.
 * @param end Random-access iterators to the last position of the sequence to
 * be used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 * @param params The hyperparameters for the CDF model, which describe the
 * architecture and sampling ratio.
 */
template <class RandomIt>
void sort(
    RandomIt begin, RandomIt end,
    typename TwoLayerRMI<typename iterator_traits<RandomIt>::value_type>::Params
        &params) {
  // Check if the data is already sorted
  // struct timespec ts1, ts2;
  // clock_gettime(CLOCK_REALTIME, &ts1);
  if (*(end - 1) >= *begin && std::is_sorted(begin, end)) {
    return;
  }
  // clock_gettime(CLOCK_REALTIME, &ts2);
  // printf("IS SORTED %ld us\n", ToNanoseconds(SubtractTime(ts2, ts1)) / 1000);

  // Check if the data is sorted in descending order
  // clock_gettime(CLOCK_REALTIME, &ts1);
  if (*(end - 1) <= *begin) {
    auto is_reverse_sorted = true;

    for (auto i = begin; i != end - 1; ++i) {
      if (*i < *(i + 1)) {
        is_reverse_sorted = false;
      }
    }

    if (is_reverse_sorted) {
      std::reverse(begin, end);
      return;
    }
  }
  // clock_gettime(CLOCK_REALTIME, &ts2);
  // printf("CHECK ORDER %ld us\n", ToNanoseconds(SubtractTime(ts2, ts1)) / 1000);

  if (std::distance(begin, end) <=
      std::max<long>(params.fanout * params.threshold,
                     5 * params.num_leaf_models)) {
    std::sort(begin, end);
  } else {
    // Initialize the RMI
    TwoLayerRMI<typename iterator_traits<RandomIt>::value_type> rmi(params);

    // Check if the model can be trained
    // clock_gettime(CLOCK_REALTIME, &ts1);
    if (rmi.train(begin, end)) {
      // clock_gettime(CLOCK_REALTIME, &ts2);
      // printf("TRAIN %ld us\n", ToNanoseconds(SubtractTime(ts2, ts1)) / 1000);
      // Sort the data if the model was successfully trained
      learned_sort::sort(begin, end, rmi);
    }

    else {  // Fall back in case the model could not be trained
      std::sort(begin, end);
    }
  }
}

/**
 * @brief Sorts a sequence of numerical keys from [begin, end) using Learned
 * Sort, in ascending order.
 *
 * @tparam RandomIt A bi-directional random iterator over the sequence of keys
 * @param begin Random-access iterators to the initial position of the
 * sequence to be used for sorting. The range used is [begin,end), which
 * contains all the elements between first and last, including the element
 * pointed by first but not the element pointed by last.
 * @param end Random-access iterators to the last position of the sequence to
 * be used for sorting. The range used is [begin,end), which contains all the
 * elements between first and last, including the element pointed by first but
 * not the element pointed by last.
 */
template <class RandomIt>
void sort(RandomIt begin, RandomIt end) {
  if (begin != end) {
    typename TwoLayerRMI<typename iterator_traits<RandomIt>::value_type>::Params
        p;
    learned_sort::sort(begin, end, p);
  }
}

}  // namespace learned_sort

#endif /* __cplusplus */
COSMOPOLITAN_C_START_

void learned_sort_int64(int64_t *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_H_ */
