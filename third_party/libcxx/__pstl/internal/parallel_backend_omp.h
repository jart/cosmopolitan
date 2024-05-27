// -*- C++ -*-
// -*-===----------------------------------------------------------------------===//
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
//
//===----------------------------------------------------------------------===//

#ifndef _PSTL_PARALLEL_BACKEND_OMP_H
#define _PSTL_PARALLEL_BACKEND_OMP_H

//------------------------------------------------------------------------
// parallel_invoke
//------------------------------------------------------------------------

#include "third_party/libcxx/__pstl/internal/omp/parallel_invoke.h"

//------------------------------------------------------------------------
// parallel_for
//------------------------------------------------------------------------

#include "third_party/libcxx/__pstl/internal/omp/parallel_for.h"

//------------------------------------------------------------------------
// parallel_for_each
//------------------------------------------------------------------------

#include "third_party/libcxx/__pstl/internal/omp/parallel_for_each.h"

//------------------------------------------------------------------------
// parallel_reduce
//------------------------------------------------------------------------

#include "third_party/libcxx/__pstl/internal/omp/parallel_reduce.h"
#include "third_party/libcxx/__pstl/internal/omp/parallel_transform_reduce.h"

//------------------------------------------------------------------------
// parallel_scan
//------------------------------------------------------------------------

#include "third_party/libcxx/__pstl/internal/omp/parallel_scan.h"
#include "third_party/libcxx/__pstl/internal/omp/parallel_transform_scan.h"

//------------------------------------------------------------------------
// parallel_stable_sort
//------------------------------------------------------------------------

#include "third_party/libcxx/__pstl/internal/omp/parallel_stable_partial_sort.h"
#include "third_party/libcxx/__pstl/internal/omp/parallel_stable_sort.h"

//------------------------------------------------------------------------
// parallel_merge
//------------------------------------------------------------------------
#include "third_party/libcxx/__pstl/internal/omp/parallel_merge.h"

#endif //_PSTL_PARALLEL_BACKEND_OMP_H
