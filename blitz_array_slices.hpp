#include <vector>
#include <blitz/array.h>


namespace blitz {


template <class T>
struct ArraySlices {
    Array<T, 1> blitz_array_;
    std::vector<Array<T, 1> > blitz_slices_;

    ArraySlices(Array<T, 1> blitz_array, int step)
            : blitz_array_(blitz_array) {
        assert(step <= blitz_array.size());
        int slice_count = std::ceil((float)blitz_array.size() / step);
        blitz_slices_.resize(slice_count);
        for(int i = 0; i < slice_count; i++) {
            int end = std::min((int)(i + 1) * step - 1, blitz_array.size() - 1);
            blitz_slices_[i].reference(blitz_array_(Range(i * step, end)));
        }
    }

    ArraySlices(Array<T, 1> blitz_array, Array<int, 1> const &slice_sizes)
            : blitz_array_(blitz_array) {
        assert(blitz::sum(slice_sizes) == blitz_array.size());
        int running_count = 0;
        blitz_slices_.resize(slice_sizes.size());
        for(int i = 0; i < slice_sizes.size(); i++) {
            blitz_slices_[i].reference(blitz_array_(Range(running_count,
                    running_count + slice_sizes(i) - 1)));
            running_count += slice_sizes(i);
        }
    }
};

}
