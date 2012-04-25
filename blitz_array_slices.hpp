#include <algorithm>
#include <iostream>
#include <vector>
#include <blitz/array.h>


namespace blitz {


template <class T>
class ArraySlices {
public:
    Array<T, 1> blitz_array_;
    std::vector<Array<T, 1> > blitz_slices_;
    Array<T, 1> slice_sizes_;

    ArraySlices() {}

    ArraySlices(Array<T, 1> blitz_array)
            : blitz_array_(blitz_array) {
        slice_by_step_size(blitz_array.size());
    }

    ArraySlices(Array<T, 1> blitz_array, int step)
            : blitz_array_(blitz_array) {
        slice_by_step_size(step);
    }

    ArraySlices(Array<T, 1> blitz_array, Array<int, 1> const &slice_sizes)
            : blitz_array_(blitz_array) {
        slice_by_sizes(slice_sizes);
    }

    ArraySlices(ArraySlices const &other)
            : blitz_array_(other) {
        Array<int, 1> slice_sizes(other.blitz_slices_.size());
        for(int i = 0; i < slice_sizes.size(); i++) {
            slice_sizes(i) = other.blitz_slices_[i].size();
        }
        slice_by_sizes(slice_sizes);
    }

    ArraySlices &operator=(ArraySlices const &other) {
        // protect against invalid self-assignment
        if(this != &other) {
            this->blitz_array_.reference(other.blitz_array_);
            Array<int, 1> slice_sizes(other.blitz_slices_.size());
            for(int i = 0; i < slice_sizes.size(); i++) {
                slice_sizes(i) = other.blitz_slices_[i].size();
            }
            this->slice_by_sizes(slice_sizes);
        }
        // by convention, always return *this
        return *this;
    }

    void slice_by_sizes(Array<int, 1> const &slice_sizes) {
        assert(blitz::sum(slice_sizes) == blitz_array_.size());
        int running_count = 0;

        blitz_slices_.resize(slice_sizes.size());
        slice_sizes_.resize(slice_sizes.size());

        /* Copy contents of slice_sizes into member slice_sizes_ */
        slice_sizes_ = slice_sizes;
        for(int i = 0; i < slice_sizes.size(); i++) {
            blitz_slices_[i].reference(blitz_array_(Range(running_count,
                    running_count + slice_sizes(i) - 1)));
            running_count += slice_sizes(i);
        }
    }

    void slice_by_step_size(int step) {
        assert(step <= blitz_array_.size());
        int slice_count = std::ceil((float)blitz_array_.size() / step);
        blitz_slices_.resize(slice_count);
        for(int i = 0; i < slice_count; i++) {
            int end = std::min((int)(i + 1) * step - 1, blitz_array_.size() - 1);
            blitz_slices_[i].reference(blitz_array_(Range(i * step, end)));
        }
        slice_sizes_.resize(blitz_slices_.size());
        for(int i = 0; i < slice_sizes_.size(); i++) {
            slice_sizes_(i) = blitz_slices_[i].size();
        }
    }

    void reorder(vector<int> const &new_positions) {
        /* Verify that all indexes appear exactly once in new_positions.  This
         * can be accomplished by copying new_positions to verify and sorting
         * "verify".  After sorting, the minimum element must be 0 and the
         * maximum value must be one less than the number of slices and the
         * number of elements in new_positions must equal the number of slices.
         */
        assert(new_positions.size() == blitz_slices_.size());
        vector<int> verify = new_positions;
        std::sort(verify.begin(), verify.end());
        assert(*std::min_element(verify.begin(), verify.end()) == 0);
        assert(*std::max_element(verify.begin(), verify.end()) == (blitz_slices_.size() - 1));

        /* Create temporary array, the same size as blitz_array_ */
        Array<T, 1> temp_array(blitz_array_.size());

        int running_count = 0;
        Array<T, 1> slice_sizes(blitz_slices_.size());
        for(int ordered_id = 0; ordered_id < new_positions.size();
                ordered_id++) {
            int slice_size = blitz_slices_[new_positions[ordered_id]].size();
            Array<T, 1> new_slice(temp_array(blitz::Range(running_count,
                    running_count + slice_size - 1)));
            new_slice = blitz_slices_[new_positions[ordered_id]];
            running_count += slice_size;
            slice_sizes(ordered_id) = slice_size;
        }
        blitz_array_ = temp_array;

        slice_by_sizes(slice_sizes);
    }
};


template <bool reverse=false>
struct sort_tiny_vector {
    int operator()(TinyVector<int, 2> const &i, TinyVector<int, 2> const &j) {
        if(reverse) {
            return !compare(i, j);
        }
        return compare(i, j);
    }

    int compare(TinyVector<int, 2> const &i, TinyVector<int, 2> const &j) {
        if(i(0) == j(0)) {
            return i(1) < j(1);
        }
        return i(0) < j(0);
    }
};


template <class T>
class SortableArraySlices : public ArraySlices<T> {
public:
    vector<int> slice_order_;
    vector<int> pre_sort_slice_order_;

    void set_default_slice_order() {
        slice_order_.resize(this->blitz_slices_.size());
        for(int i = 0; i < slice_order_.size(); i++) {
            slice_order_[i] = i;
        }
    }

    SortableArraySlices() {}

    SortableArraySlices(Array<T, 1> blitz_array) : ArraySlices<T>(blitz_array) {
        set_default_slice_order();
    }

    SortableArraySlices(Array<T, 1> blitz_array, int step)
            : ArraySlices<T>(blitz_array, step) {
        set_default_slice_order();
    }

    SortableArraySlices(Array<T, 1> blitz_array,
            Array<int, 1> const &slice_sizes) : ArraySlices<T>(blitz_array,
                    slice_sizes) {
        set_default_slice_order();
    }

    void sort(bool ascending=true) {
        vector<TinyVector<int, 2> > sort_pairs(this->blitz_slices_.size());

        for(int i = 0; i < this->blitz_slices_.size(); i++) {
            sort_pairs[i](0) = this->blitz_slices_[i].size();
            sort_pairs[i](1) = i;
        }
        if(ascending) {
            std::sort(sort_pairs.begin(), sort_pairs.end(), sort_tiny_vector<false>());
        } else {
            std::sort(sort_pairs.begin(), sort_pairs.end(), sort_tiny_vector<true>());
        }
        for(int i = 0; i < this->blitz_slices_.size(); i++) {
            slice_order_[i] = sort_pairs[i](1);
            
        }
        this->reorder(slice_order_);
    }

    SortableArraySlices(SortableArraySlices const &other)
            : ArraySlices<T>(other), slice_order_(other.slice_order_),
                    pre_sort_slice_order_(other.pre_sort_slice_order_) {}

    SortableArraySlices &operator=(SortableArraySlices const &other) {
        // protect against invalid self-assignment
        if(this != &other) {
            this->blitz_array_.reference(other.blitz_array_);
            Array<int, 1> slice_sizes(other.blitz_slices_.size());
            for(int i = 0; i < slice_sizes.size(); i++) {
                slice_sizes(i) = other.blitz_slices_[i].size();
            }
            this->slice_by_sizes(slice_sizes);
            this->slice_order_ = other.slice_order_;
            this->pre_sort_slice_order_ = other.pre_sort_slice_order_;
        }
        // by convention, always return *this
        return *this;
    }




};

}
