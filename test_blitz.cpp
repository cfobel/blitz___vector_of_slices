#include <iostream>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <blitz/array.h>
#include "timing.hpp"


using blitz::Array;
using blitz::Range;
using blitz::firstDim;
using namespace std;


int main(int argc, char** argv) {
    typedef boost::format _f;
    typedef boost::unordered_map<string, double> duration_map_t;
    duration_map_t duration_map;

    timespec start, end, delta;
    if(argc != 2) {
        cerr << _f("usage: %s <N>") % argv[0] << endl;
        exit(-1);
    }
    const int N = atoi(argv[1]);

    Array<int, 1> blitz_array(10 * N);
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < blitz_array.extent(firstDim); i++) {
        blitz_array(i) = i;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["blitz array"] = delta.tv_sec + delta.tv_nsec * 1e-9;


    vector<Array<int, 1> > blitz_slices(N);

    for(int i = 0; i < 10; i++) {
        blitz_slices[i].reference(blitz_array(Range(i * N, (i + 1) * N - 1)));
        cout << blitz_slices[i] << endl;
    }

    blitz_slices.clear();
    blitz_slices.reserve(10);

    for(int i = 0; i < 10; i++) {
        blitz_slices.push_back(blitz_array(Range(i * N, (i + 1) * N - 1)));
        cout << blitz_slices[i] << endl;
    }

#if 0
    BOOST_FOREACH(duration_map_t::value_type &item, duration_map) {
        cout << _f("%s,%.2g") % item.first % item.second << endl;
    }
#endif
    
    return 0;
}
