#pragma once

#include <functional>
#include <utility>
#include <vector>

template <class T, class Container = std::vector<T>, class Compare = std::less<T>>
class PriorityQueue {
public:
    using value_compare = Compare;
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using const_reference = const T&;

private:
    Container c;
    value_compare comp;

    size_type parent(size_type index) { return (index - 1) / 2; }
    size_type left_child(size_type index) { return 2 * index + 1; }
    size_type right_child(size_type index) { return 2 * (index + 1); }
    bool is_internal(size_t index) { return left_child(index) < c.size(); }
    bool is_leaf(size_t index) { return left_child(index) >= c.size(); }

    
    /**
     * @brief Recursively (or Iteratively) swap the value at index up the heap until it is in the correct position.
     * 
     * @note This is a max heap, so assume Compare is less, but promote the larger value.
     * 
     * Used by push
     * 
     * O(log(size()))
     * 
     * @param index the current position to swap upwards
     */
    void upheap(size_type index) {
        //this function will continue to swap the curr value with it's parent while
        //the parent's value is less than it or we are on the root node
        //we use upheap to maintain max heap properties when inserting

        //steps:
        //while index is greater than 0 and val at index is greater than parent
        //swap the two values and redefine index as the parent index
        
        size_type parentIndex = parent(index);
        while(index > 0 && comp(c[parentIndex], c[index])) {
            std::swap(c[parentIndex], c[index]);
            index = parentIndex;
            parentIndex = parent(index);
        }
    }
    
    /**
     * @brief Recursively swap the value at index down the heap until it is in the correct position.
     * 
     * @note This is a max heap, so assume Compare is less, so promote the larger value.
     * 
     * Used by pop
     * 
     * O(log(size()))
     * 
     * @param index the current position to swap downwards
     */
    void downheap(size_type index) {
        //continues to swap value at current index with it's largest children 
        //while it's children are greater than it

        //steps:
        //create a bool variable that will be set to true if children nodes
        //are greater than it
        //then have a while loop that runs while our index is not at leaf
        //and bool var is true
        //each iteration, we want to get the index of both the right and left children
        //and determine the max between the two, and then determine if that max is greater than parent
        //if yes, then perform swap, otherwise do nothing

        bool invalid = true;
        while(!is_leaf(index) && invalid) {
            invalid = false;
            size_type right = right_child(index);
            size_type left = left_child(index);
            size_type maxIndex = index;
            if(right < c.size() && comp(c[maxIndex], c[right])) {
                invalid = true;
                maxIndex = right;
            }
            if(left < c.size() && comp(c[maxIndex], c[left])) {
                invalid = true;
                maxIndex = left;  
            }
            if(invalid) {
                std::swap(c[index], c[maxIndex]);
                index = maxIndex;
            }

        }
    }

public:
    PriorityQueue() = default;
    PriorityQueue( const PriorityQueue& other ) = default;
    PriorityQueue( PriorityQueue&& other ) = default;
    ~PriorityQueue() = default;
    PriorityQueue& operator=( const PriorityQueue& other ) = default;
    PriorityQueue& operator=( PriorityQueue&& other ) = default;

    /**
     * @brief Return a const reference to the element at the top of the heap.
     * 
     * O(1)
     * 
     * @return const_reference to the element at the top of the heap.
     */
    const_reference top() const {

        return c[0];
    }

    /**
     * @brief Return whether the heap is empty. This is the same as whether the underlying container, c, is empty.
     * 
     * O(1)
     * 
     * @return true c is empty
     * @return false c is not empty
     */
    bool empty() const {
        return c.size() == 0;
    }

    /**
     * @brief Return the number of elements in the heap. This is the same as the number of elements in the underlying container, c.
     * 
     * O(1)
     * 
     * @return size_type of the number of elements in the heap
     */
    size_type size() const {
        return c.size();
    }
	
    /**
     * @brief Inserts element and sorts the underlying container, c.
     * 
     * Uses upheap
     * 
     * O(log(size())) 
     * 
     * @param value inserted by copying into c 
     */
    void push( const value_type& value ) {
        //adds element to bottom of the heap
        //then uses upheap to sort it into the right place
        //time complexity: O(logn) because we are doing at most log(n) rotations 
        //since our tree is balanced
        c.push_back(value);
        upheap(c.size() - 1);
    }

    /**
     * @brief Inserts element and sorts the underlying container, c.
     * 
     * Uses upheap
     * 
     * O(log(size())) 
     * 
     * @param value inserted by moving into c 
     */
	void push( value_type&& value ) {
        c.push_back(std::move(value));
        upheap(c.size() - 1);
    }

    /**
     * @brief Removes the top element.
     * 
     * Uses downheap
     * 
     * O(log(size())) 
     * 
     */
    void pop() {
        //removes the top of the heap, replacing it with the last element in the heap
        //then uses down heap to re sort heap to maintain max heap properties
        if(c.empty()) {
            return;
        }
        std::swap(c[0], c[c.size() - 1]);
        c.pop_back();
        downheap(0);
    }
};