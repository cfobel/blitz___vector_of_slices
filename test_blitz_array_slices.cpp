#include <iostream>
#include <vector>
#include <algorithm>
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
    if(argc != 3) { cerr << _f("usage: %s <array size> <step size>") % argv[0] << endl;
        exit(-1);
    }
    const int N = atoi(argv[1]);
    const int step_size = atoi(argv[2]);

    Array<int, 1> test_array(N);
    for(int i = 0; i < test_array.size(); i++) {
        test_array(i) = i;
    }

    ArraySlices<int> test(test_array.copy(), step_size);
    cout << "Entire Array:" << endl;
    cout << test.blitz_array_ << endl;

    cout << endl << endl;

    cout << "Sliced into steps of " << step_size << ":" << endl;
    for(int i = 0; i < test.blitz_slices_.size(); i++) {
        cout << test.blitz_slices_[i] << endl;
    }

    Array<int, 1> slice_sizes((float)N / step_size + 1);
    slice_sizes = 0;

    for(int i = 0; i < slice_sizes.size() - 1; i++) {
        int slice_size = rand() % step_size + 1;
        slice_sizes(i) = slice_size;
    }
    int slice_total = blitz::sum(slice_sizes);
    if(slice_total < N) {
        slice_sizes(slice_sizes.size() - 2) += N - slice_total;
    }
    /* Add an empty slice to show they can be handled. */
    slice_sizes(slice_sizes.size() - 1) = 0;

    ArraySlices<int> test2(test_array.copy(), slice_sizes);

    cout << endl << endl;
    cout << "Sliced into steps of the following sizes:" << slice_sizes << ":" << endl << endl;
    for(int i = 0; i < test2.blitz_slices_.size(); i++) {
        cout << test2.blitz_slices_[i] << endl;
    }

    vector<int> slice_order(test.blitz_slices_.size());
    for(int i = 0; i < slice_order.size(); i++) {
        slice_order[i] = i;
    }
    random_shuffle(slice_order.begin(), slice_order.end());
    cout << "Reorder array with the following order: ";
    copy(slice_order.begin(), slice_order.end(), ostream_iterator<int>(cout,", "));
    cout << endl;

    test.reorder(slice_order);
    cout << "Reordered Array:" << endl;
    cout << test.blitz_array_ << endl;
    for(int i = 0; i < test.blitz_slices_.size(); i++) {
        cout << test.blitz_slices_[i] << endl;
    }

    SortableArraySlices<int> sortable_test;
    sortable_test = SortableArraySlices<int>(test_array.copy(), slice_sizes);
    sortable_test.sort();
    cout << "Sorted ascending array:" << endl;
    cout << sortable_test.blitz_array_ << endl;
    for(int i = 0; i < sortable_test.blitz_slices_.size(); i++) {
        cout << sortable_test.blitz_slices_[i] << endl;
    }

    SortableArraySlices<int> descend_test(test_array.copy(), slice_sizes);
    descend_test.sort(false);
    cout << "Sorted descending array:" << endl;
    cout << descend_test.blitz_array_ << endl;
    for(int i = 0; i < descend_test.blitz_slices_.size(); i++) {
        cout << descend_test.blitz_slices_[i] << endl;
    }
    return 0;
}
