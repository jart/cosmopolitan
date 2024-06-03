// -*- mode:c++;indent-tabs-mode:nil;c-basic-offset:4;coding:utf-8 -*-
// vi: set et ft=cpp ts=4 sts=4 sw=4 fenc=utf-8 :vi
//
// Copyright 2024 Mozilla Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ctl/vector.h"

#include <cosmo.h>
#include <new>

#include "ctl/string.h"

// #include <string>
// #include <vector>
// #define String std::string
// #define Vector std::vector

int
main(int argc, char* argv[])
{

    {
        int x = 3;
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(x);
        if (A[0] != 1)
            return 1;
        if (A[1] != 2)
            return 2;
        if (A[2] != 3)
            return 3;
        if (A.size() != 3)
            return 4;
    }

    {
        String yo = "foo";
        Vector<String> A;
        A.push_back("fun");
        A.push_back(std::move(yo));
        if (yo != "")
            return 5;
        A.emplace_back("bar");
        if (A[0] != "fun")
            return 7;
        if (A[1] != "foo")
            return 8;
        if (A[2] != "bar")
            return 9;
        if (A.size() != 3)
            return 10;
    }

    {
        Vector<int> A;
        if (!A.empty())
            return 11;
        A.push_back(5);
        if (A.empty())
            return 12;
        if (A.front() != 5)
            return 13;
        if (A.back() != 5)
            return 14;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B(A);
        if (B.size() != 3)
            return 15;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 16;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B(std::move(A));
        if (A.size() != 0)
            return 17;
        if (B.size() != 3)
            return 18;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 19;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B;
        B = A;
        if (B.size() != 3)
            return 20;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 21;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B;
        B = std::move(A);
        if (A.size() != 0)
            return 22;
        if (B.size() != 3)
            return 23;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 24;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        A.pop_back();
        if (A.size() != 2)
            return 25;
        if (A[0] != 1 || A[1] != 2)
            return 26;
    }

    {
        Vector<int> A;
        A.resize(5);
        if (A.size() != 5)
            return 27;
        A.resize(3);
        if (A.size() != 3)
            return 28;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B;
        B.push_back(4);
        B.push_back(5);
        A.swap(B);
        if (A.size() != 2)
            return 29;
        if (B.size() != 3)
            return 30;
        if (A[0] != 4 || A[1] != 5)
            return 31;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 32;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        A.clear();
        if (A.size() != 0)
            return 33;
        if (!A.empty())
            return 34;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int>::iterator it = A.begin();
        if (*it != 1)
            return 35;
        ++it;
        if (*it != 2)
            return 36;
        ++it;
        if (*it != 3)
            return 37;
        ++it;
        if (it != A.end())
            return 38;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int>::const_iterator cit = A.cbegin();
        if (*cit != 1)
            return 39;
        ++cit;
        if (*cit != 2)
            return 40;
        ++cit;
        if (*cit != 3)
            return 41;
        ++cit;
        if (cit != A.cend())
            return 42;
    }

    {
        Vector<int> A;
        for (int i = 0; i < 100; ++i) {
            A.push_back(i);
        }
        if (A.size() != 100)
            return 51;
        for (int i = 0; i < 100; ++i) {
            if (A[i] != i)
                return 52;
        }
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B(A);
        if (B.size() != 3)
            return 53;
        B.push_back(4);
        if (A.size() != 3)
            return 54;
        if (B.size() != 4)
            return 55;
    }

    {
        Vector<int> A;
        A.reserve(100);
        if (A.size() != 0)
            return 56;
        if (A.capacity() != 100)
            return 57;
        A.push_back(1);
        if (A.size() != 1)
            return 58;
        if (A.capacity() != 100)
            return 59;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B;
        B = A;
        if (B.size() != 3)
            return 60;
        B.push_back(4);
        if (A.size() != 3)
            return 61;
        if (B.size() != 4)
            return 62;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B;
        B = std::move(A);
        if (A.size() != 0)
            return 63;
        if (B.size() != 3)
            return 64;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 65;
    }

    {
        Vector<int> A;
        A.push_back(1);
        A.push_back(2);
        A.push_back(3);
        Vector<int> B;
        B.push_back(4);
        B.push_back(5);
        A.swap(B);
        if (A.size() != 2)
            return 66;
        if (B.size() != 3)
            return 67;
        if (A[0] != 4 || A[1] != 5)
            return 68;
        if (B[0] != 1 || B[1] != 2 || B[2] != 3)
            return 69;
    }

    CheckForMemoryLeaks();
    return 0;
}
