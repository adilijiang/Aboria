/*

Copyright (c) 2005-2016, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Aboria.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#ifndef NEIGHBOURS_H_
#define NEIGHBOURS_H_

#include <cxxtest/TestSuite.h>

#include "Aboria.h"

using namespace Aboria;

class NeighboursTest : public CxxTest::TestSuite {
public:

    void test_documentation(void) {
//[neighbour_search
/*`
[section Neighbourhood Searching]

The [classref Aboria::Particles] container gives you neighbourhood searching 
functionality, using a simple bucket-search approach. The domain is divided into 
a regular grid of hypercubes with side length equal to a lengthscale that is 
supplied by the user. Each particle in the container is assigned to the cell 
that contains its position. Neighbourhood search queries at a given point return 
all the particles within the cell that contains this point and the immediate 
cell neighbours.

To start with, we will create a particle set in three dimensions (the default) 
containing a few randomly placed particles
*/

        const size_t N = 100;
        typedef Particles<> particle_type;
        typedef particle_type::position position;
        particle_type particles(N);
        std::default_random_engine gen; 
        std::uniform_real_distribution<double> uniform(-1,1);
        for (int i=0; i<N; ++i) {
            get<position>(particles)[i] = double3(uniform(gen),uniform(gen),uniform(gen));
        }

/*`

Before you can use the neighbourhood searching, you need to initialise the
domain using the [memberref Aboria::Particles::init_neighbour_search] function.

In this case, we will initialise a domain from $(-1,-1,-1)$ to $(1,1,1)$, which
is periodic in all directions.

*/

        double3 min(-1);
        double3 max(1);
        bool3 periodic(true);
        particles.init_neighbour_search(min,max,periodic);
/*`

Once this is done you can begin using the neighbourhood search queries using the 
[funcref Aboria::euclidean_search] function. This returns a lightweight container with `begin()` and
`end()` functions that return `const` forward only iterators to the particles
that satisfy the neighbour search. For example, the following counts all the
particles within a distance `radius` of the point $(0,0,0)$. Note that
[funcref Aboria::euclidean_search] uses the euclidean or L2-norm distance, but there
are other functions for other distance norms.

*/

        double radius = 0.2;
        int count = 0;
        for (const auto& i: euclidean_search(particles.get_query(),double3(0),radius)) {
            count++;
        }
        std::cout << "There are "<< count << " particles.\n";

/*`

When dereferenced, the neighbourhood iterator returns a tuple of size 2 
containing 

# A constant reference to the found particle object, with type
`particle_type::const_reference`

# A vector $\mathbf{dx}\_{ij}$ pointing to the found point from the query 
point. I.e. if $\mathbf{x}\_i$ is the query point and $\mathbf{x}\_j$ is the
found point, then $\mathbf{dx}\_{ij} = \mathbf{x}\_j - \mathbf{x}\_i$.

The latter is useful for periodic domains, the returned vector
$\mathbf{dx}\_{ij}$ takes periodic domains into account and returns the
$\mathbf{dx}\_{ij}$ with the smallest length. 

For example, 

*/

        for (const auto& i: euclidean_search(particles.get_query(),double3(0),radius)) {
            particle_type::const_reference b = std::get<0>(i);
            const double3& dx = std::get<1>(i);
            std::cout << "Found a particle with dx = " << dx << " and id = " << get<id>(b) << "\n";
        }

/*`
[endsect]
*/
//]
    }



    template<template <typename,typename> class Vector,template <typename> class SearchMethod>
    void helper_single_particle(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
    	typedef Particles<std::tuple<scalar>,3,Vector,SearchMethod> Test_type;
        typedef position_d<3> position;
    	Test_type test;
    	double3 min(-1,-1,-1);
    	double3 max(1,1,1);
    	double3 periodic(true,true,true);
    	double radius = 0.1;
    	test.init_neighbour_search(min,max,periodic);
    	typename Test_type::value_type p;

        get<position>(p) = double3(0,0,0);
    	test.push_back(p);

    	int count = 0;
    	for (auto tpl: euclidean_search(test.get_query(),double3(radius/2,radius/2,0),radius)) {
    		count++;
    	}
    	TS_ASSERT_EQUALS(count,1);

    	auto tpl = euclidean_search(test.get_query(),double3(radius/2,radius/2,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),1);

    	tpl = euclidean_search(test.get_query(),double3(2*radius,0,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),0);
    }

    template<template <typename,typename> class Vector,template <typename> class SearchMethod>
    void helper_two_particles(void) {
        ABORIA_VARIABLE(scalar,double,"scalar")
    	typedef Particles<std::tuple<scalar>,3,Vector,SearchMethod> Test_type;
        typedef position_d<3> position;
    	Test_type test;
    	double3 min(-1,-1,-1);
    	double3 max(1,1,1);
    	double3 periodic(true,true,true);
    	double radius = 0.1;
    	test.init_neighbour_search(min,max,periodic);
    	typename Test_type::value_type p;

        get<position>(p) = double3(0,0,0);
    	test.push_back(p);

        get<position>(p) = double3(radius/2,0,0);
    	test.push_back(p);

    	auto tpl = euclidean_search(test.get_query(),double3(1.1*radius,0,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),1);
    	const typename Test_type::value_type &pfound = tuple_ns::get<0>(*tpl.begin());
    	TS_ASSERT_EQUALS(get<id>(pfound),get<id>(test[1]));

    	tpl = euclidean_search(test.get_query(),double3(0.9*radius,0,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),2);

    	tpl = euclidean_search(test.get_query(),double3(1.6*radius,0,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),0);

    	tpl = euclidean_search(test.get_query(),double3(0.25*radius,0.9*radius,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),2);

    	tpl = euclidean_search(test.get_query(),double3(0.25*radius,0.99*radius,0),radius);
    	TS_ASSERT_EQUALS(std::distance(tpl.begin(),tpl.end()),0);
    }

    template <typename Search, int LNormNumber>
    struct has_n_neighbours {
        unsigned int n;
        double max_distance;
        Search search;

        CUDA_HOST_DEVICE 
        has_n_neighbours(const Search& search, const double max_distance, const unsigned int n):
            search(search),n(n),max_distance(max_distance) {}

        CUDA_HOST_DEVICE 
        void operator()(typename Search::reference i) {
            auto tpl = distance_search<LNormNumber>(
                            search,get<typename Search::position>(i),max_distance);
            TS_ASSERT_EQUALS(tpl.end()-tpl.begin(),n);
        }
    };

    template<unsigned int D, 
             template <typename,typename> class VectorType,
             template <typename> class SearchMethod>
    void helper_d(const int n, const double r, const int neighbour_n) {
        ABORIA_VARIABLE(scalar,double,"scalar")
    	typedef Particles<std::tuple<scalar>,D,VectorType,SearchMethod> Test_type;
        typedef position_d<D> position;
        typedef Vector<double,D> double_d;
        typedef Vector<bool,D> bool_d;
        typedef Vector<unsigned int,D> uint_d;
    	Test_type test;
    	double_d min(0);
    	double_d max(n);
    	bool_d periodic(true);
    	typename Test_type::value_type p;
        uint_d index(0);
        double dx = 1.0;

        bool finished = false;
        while (finished != true) {
            double_d pos = index*dx+min+dx/2;
            get<position>(p) = pos;
            test.push_back(p);
            index[0]++;
            for (int i=0; i<D; i++) {
                if (index[i] >= n) {
                    if (i==D-1) {
                        finished = true;
                    } else {
                        index[i+1]++;
                        index[i] = 0;
                    }
                } else {
                    break;
                }
            }
        }

    	test.init_neighbour_search(min,max,periodic,neighbour_n);
        if (D==2) {
            // Gauss circle problem (L2 in D=2)
            int n_expect = 0;
            for (int i = 0; i < 100; ++i) {
                n_expect += int(std::floor(std::pow(r,2)/(4*i+1))) - int(std::floor(std::pow(r,2)/(4*i+3)));
            }
            n_expect = 1 + 4*n_expect;
            std::cout << "L2 norm test (r="<<r<<"): expecting "<<n_expect<<" points"<<std::endl;
            Aboria::detail::for_each(test.begin(),test.end(),
                    has_n_neighbours<typename Test_type::query_type,
                    2>(test.get_query(),r,n_expect));
        }
        // Box search (Linf)
        int n_expect = std::pow(2*int(std::floor(r)) + 1,D);
        std::cout << "Linf norm test (r="<<r<<", D="<<D<<"): expecting "<<n_expect<<" points"<<std::endl;
        Aboria::detail::for_each(test.begin(),test.end(),
                has_n_neighbours<typename Test_type::query_type,
                                 -1>(test.get_query(),r,n_expect));

    }

    void test_std_vector_bucket_search_serial(void) {
        helper_single_particle<std::vector,bucket_search_serial>();
        helper_two_particles<std::vector,bucket_search_serial>();
        helper_d<1,std::vector,bucket_search_serial>(100,1.5,10);
        helper_d<2,std::vector,bucket_search_serial>(50,1.0001,10);
        helper_d<2,std::vector,bucket_search_serial>(50,1.5,10);
        helper_d<2,std::vector,bucket_search_serial>(20,2.1,10);
        helper_d<3,std::vector,bucket_search_serial>(10,1.9,10);
        helper_d<3,std::vector,bucket_search_serial>(10,1.0001,10);
        helper_d<4,std::vector,bucket_search_serial>(10,1.0001,10);
    }

    void test_std_vector_bucket_search_parallel(void) {
        helper_single_particle<std::vector,bucket_search_parallel>();
        helper_two_particles<std::vector,bucket_search_parallel>();
        helper_d<1,std::vector,bucket_search_parallel>(100,1.5,10);
        helper_d<2,std::vector,bucket_search_parallel>(50,1.0001,10);
        helper_d<2,std::vector,bucket_search_parallel>(50,1.5,10);
        helper_d<2,std::vector,bucket_search_parallel>(20,2.1,10);
        helper_d<3,std::vector,bucket_search_parallel>(10,1.9,10);
        helper_d<3,std::vector,bucket_search_parallel>(10,1.0001,10);
        helper_d<4,std::vector,bucket_search_parallel>(10,1.0001,10);
    }

    void test_thrust_vector_bucket_search_serial(void) {
#if defined(__CUDACC__)
        helper_d<1,thrust::device_vector,bucket_search_serial>(100,1.5,10);
        helper_d<2,thrust::device_vector,bucket_search_serial>(50,1.0001,10);
        helper_d<2,thrust::device_vector,bucket_search_serial>(50,1.5,10);
        helper_d<2,thrust::device_vector,bucket_search_serial>(20,2.1,10);
        helper_d<3,thrust::device_vector,bucket_search_serial>(10,1.9,10);
        helper_d<3,thrust::device_vector,bucket_search_serial>(10,1.0001,10);
        helper_d<4,thrust::device_vector,bucket_search_serial>(10,1.0001,10);

#endif
    }

    void test_thrust_vector_bucket_search_parallel(void) {
#if defined(__CUDACC__)
        helper_d<1,thrust::device_vector,bucket_search_parallel>(100,1.5,10);
        helper_d<2,thrust::device_vector,bucket_search_parallel>(50,1.0001,10);
        helper_d<2,thrust::device_vector,bucket_search_parallel>(50,1.5,10);
        helper_d<2,thrust::device_vector,bucket_search_parallel>(20,2.1,10);
        helper_d<3,thrust::device_vector,bucket_search_parallel>(10,1.9,10);
        helper_d<3,thrust::device_vector,bucket_search_parallel>(10,1.0001,10);
        helper_d<4,thrust::device_vector,bucket_search_parallel>(10,1.0001,10);
#endif
    }

};



#endif /* NEIGHBOURS_H_ */
