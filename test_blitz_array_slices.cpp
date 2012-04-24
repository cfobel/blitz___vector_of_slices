#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include "timing.hpp"
#include "blitz_array_slices.hpp"

using namespace std;
using namespace blitz;

int main(int argc, char** argv) {
    typedef boost::format _f;
    typedef boost::unordered_map<string, double> duration_map_t;
    duration_map_t duration_map;

    timespec start, end, delta;
    if(argc != 3) {
        cerr << _f("usage: %s <array size> <step size>") % argv[0] << endl;
        exit(-1);
    }
    const int N = atoi(argv[1]);
    const int step_size = atoi(argv[2]);

    Array<int, 1> test_array(N);
    for(int i = 0; i < test_array.size(); i++) {
        test_array(i) = i;
    }

    ArraySlices<int> test(test_array, step_size);
    cout << "Entire Array:" << endl;
    cout << test.blitz_array_ << endl;

    cout << endl << endl;

    cout << "Sliced into steps of " << step_size << ":" << endl;
    for(int i = 0; i < test.blitz_slices_.size(); i++) {
        cout << test.blitz_slices_[i] << endl;
    }

    Array<int, 1> slice_sizes((float)N / step_size);
    slice_sizes = 0;

    for(int i = 0; i < slice_sizes.size(); i++) {
        int slice_size = rand() % step_size + 1;
        slice_sizes(i) = slice_size;
    }
    int slice_total = blitz::sum(slice_sizes);
    if(slice_total < N) {
        slice_sizes(slice_sizes.size() - 1) += N - slice_total;
    }

    ArraySlices<int> test2(test_array, slice_sizes);

    cout << endl << endl;
    cout << "Sliced into steps of the following sizes:" << slice_sizes << ":" << endl << endl;
    for(int i = 0; i < test2.blitz_slices_.size(); i++) {
        cout << test2.blitz_slices_[i] << endl;
    }

#if 0
    blitz_slices.clear();
    blitz_slices.reserve(10);

    for(int i = 0; i < 10; i++) {
        blitz_slices.push_back(test.blitz_array_(Range(i * N, (i + 1) * N - 1)));
        cout << blitz_slices[i] << endl;
    }

    BOOST_FOREACH(duration_map_t::value_type &item, duration_map) {
        cout << _f("%s,%.2g") % item.first % item.second << endl;
    }
#endif
    
    return 0;
}
