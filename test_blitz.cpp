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


class DummyClass {
public:
    Array<int, 1> blitz_array_;
    vector<Array<int, 1> > blitz_slices_;

    DummyClass(int N) : blitz_array_(10 * N), blitz_slices_(10) {
        for(int i = 0; i < blitz_array_.extent(firstDim); i++) {
            blitz_array_(i) = i;
        }

        for(int i = 0; i < 10; i++) {
            blitz_slices_[i].reference(blitz_array_(Range(i * N, (i + 1) * N - 1)));
        }
    }

    ~DummyClass() {
        cout << "[DummyClass] destructor" << endl;
    }
};



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

    /* Create pointer to new DummyClass instance so we can destruct it
     * manually.
     */
    DummyClass *test = new DummyClass(N);
    cout << test->blitz_array_ << endl;

    for(int i = 0; i < 10; i++) {
        cout << test->blitz_slices_[i] << endl;
    }

    /* Test copy constructor */
    DummyClass bar(*test);
    /* Test copy constructor via assignment */
    DummyClass foo = *test;

    /* Force destruction of test here. */
    delete test;

    cout << foo.blitz_array_ << endl;
    for(int i = 0; i < foo.blitz_slices_.size(); i++) {
        cout << foo.blitz_slices_[i] << endl;
    }

    cout << bar.blitz_array_ << endl;
    for(int i = 0; i < bar.blitz_slices_.size(); i++) {
        cout << bar.blitz_slices_[i] << endl;
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
