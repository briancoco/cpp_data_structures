#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm> // std::random_access_iterator_tag
#include <cstddef> // size_t
#include <stdexcept> // std::out_of_range
#include <type_traits> // std::is_same

template <class T>
class Vector {
public:
    class iterator;
private:
    T* array;
    size_t _capacity, _size;

    // You may want to write a function that grows the vector
    void grow() {
        //doubles the capacity of the vector
        //and copies over all the values of the current vector
        //if our array is nullptr, that means there are no values in in
        //allocate a dynamic array of size 1 
        //otherwise allocate a dynamic array of 2x capacity

        //copy over array elements, deallocate old array and set array pointer
        //to new one

        T* newArray;

        //allocates new array
        if(!array) {
            newArray = new T[1];
            _capacity = 1;
        } else {
            newArray = new T[2 * _capacity];
            _capacity = 2 * _capacity;
        }

        //copies old array values into new array
        for(size_t i = 0; i < _size; i++) {
            newArray[i] = std::move(array[i]);
        }

        //deallocate old array and redefine it
        delete[] array;
        array = newArray;


    }

public:
    Vector() noexcept {
        //default constructor for our vector
        //creates an empty vector
        //we want to set our capacity and size data members to 0
        //and we can set our pointer to nullptr
        //there's no point in allocating a dynamic array of size 0
        array = nullptr;
        _capacity = 0;
        _size = 0;
    }
    Vector(size_t count, const T& value) { 
        //parameterized constructor
        //creates a vector of size count and fills in all those vector's elements
        //with value
        //edge case: if count is negative then our count is invalid
        //edge case: if our count equals zero, then we dont want to allocate the DA

        if (count == 0) {
            array = nullptr;
        } else {
            array = new T[count];
            for(size_t i = 0; i < count; i++) {
                array[i] = value;
            }
        }
        _size = count;
        _capacity = count;
    }
    explicit Vector(size_t count) { 
       //parameterized constructor
       //creates a vector of size count 
       //does not give default values to elements
       //if count is negative then we have an invalid count
       //if count is zero then we want to set array to nullptr
       if(count == 0) {
        array = nullptr;
       } else {
        array = new T[count]();
       }
       
       _capacity = count;
       _size = count;
    }
    Vector(const Vector& other) { 
        //copy constructor
        //given another vector, create a deep copy
        //we can do this by first setting our curr vector's _size and _capacity
        //equal to other's, then we can traverse thru other's elements
        //copying the values using the assignment operator
        _size = other._size;
        _capacity = other._capacity;
        array = new T[_capacity];

        //traverse thru our array and copy values
        for(size_t i = 0; i < _size; i++) {
            array[i] = other.array[i];
        }

    }
    Vector(Vector&& other) noexcept { 
        //move Constructor
        //meaning we just want to move the data members from other into curr obj
        //the double ampersand&& means that it is a refernece to an r-value 
        //(a value w/o a memory address)
        //to do this, we'll just initialize all of our vector's properties to
        //other's and then set other's all to zero and nullptr
        _size = other._size;
        _capacity = other._capacity;
        array = other.array;

        other._size = 0;
        other._capacity = 0;
        other.array = nullptr;
    }

    ~Vector() {
        //destructor
        //this function is run when our object is destoryed
        //here we want to deallocate any dynamic memory we allocated in the body of this object
        //in this case it would be our dynamic array
        delete[] array;
        
    }
    Vector& operator=(const Vector& other) {
        //copy assignment operator
        //used when we want want to reassign our vector to using another one
        //we have to first check and make sure that the two vectors aren't the same
        //then we need to deallocate the array inside our curr obj
        //then use the same procedure as in the copy constructor
        if(this != &other) {
            delete[] array;
            _size = other._size;
            _capacity = other._capacity;
            array = new T[_capacity];
            //traverse and copy values
            for(size_t i = 0; i < _size; i++) {
                array[i] = other.array[i];
            }
        }
        return *this;
    }
    Vector& operator=(Vector&& other) noexcept {
        //move assignment operator
        //replaces the contents with those of other 
        //after the move, other needs to be empty
        //check that they're not the same objects
        //then deallocate curr vec's array
        // and perform memberwise copy
        //return reference to curr vec
        if(this != &other) {
            delete[] array;
            _size = other._size;
            _capacity = other._capacity;
            array = other.array;

            other._size = 0;
            other._capacity = 0;
            other.array = nullptr;
        }
        return *this;
    }
    
    iterator begin() noexcept {
        iterator start = array;
        return start;
    }
    iterator end() noexcept {
        //returns one after last element pointer
        iterator end = array + _size;
        return end;
    }

    [[nodiscard]] bool empty() const noexcept {
        //returns true if vector is empty and false otherwise
        if(_size == 0) {
            return true;
        }
        return false;
    }
    size_t size() const noexcept {
        //getter method, returns vector's size
        return _size;
    }
    size_t capacity() const noexcept {
        //getter method, returns vector's capacity
        return _capacity;
    }

    T& at(size_t pos) {
        //returns element at given position
        //must first check if pos is out of bounds
        //if yes then throw an exception
        //otherwise return the position at the given index
        if(pos < 0 || pos >= _size) {
            throw std::out_of_range("given position is out of bounds");
        }
        return array[pos];
    }
    const T& at(size_t pos) const {
        //same as before???
        if(pos < 0 || pos >= _size) {
            throw std::out_of_range("given position is out of bounds");
        }
        return array[pos];

    }
    T& operator[](size_t pos) {
        //square brackets operator
        //no error checking
        return array[pos];
    }
    const T& operator[](size_t pos) const {
        return array[pos];
    }
    T& front() {
        //returns the first element in the array
        if(!array) {
            throw std::invalid_argument("no elements in the array");
        }
        return array[0];
    }
    const T& front() const {
        //returns the first element in the array
        if(!array) {
            throw std::invalid_argument("no elements in the array");
        }
        return array[0];
    }
    T& back() {
        //returns the last element of the array
        if(!array) {
            throw std::invalid_argument("no elements in the array");
        }
        return array[_size - 1];
    }
    const T& back() const {
        //same thing again but const 
        if(!array) {
            throw std::invalid_argument("no elements in the array");
        }
        return array[_size - 1];
    }

    void push_back(const T& value) {
        //adds value to the end of the array
        //we first need to check if there is enough mem to add
        //if there is not then we need to allocate more 
        //we can do this by allocating a new dynamic array doubling the capacity
        //and then copying the values over
        
        //then finally copy over the new value 
        if(_size == _capacity) {
            //this means we need to grow the size of our DA
            this->grow();
        }

        //adds value to end of array and then increments size
        array[_size++] = value;

    }
    void push_back(T&& value) {
        if(_size == _capacity) {
            //this means we need to grow the size of our DA
            this->grow();
        }

        //adds value to end of array and then increments size
        array[_size++] = std::move(value);
    }
    void pop_back() {
        //removes the last element from the vector
        //we can do this by simply decrementing the size
        if(_size > 0) {
            _size--;
        }
    }

    iterator insert(iterator pos, const T& value) {
       //given an iterator pointing to an element at a certain index in our vector
       //insert value at that index and shift all values from pos - end
       //2 steps
        //first check if we have enough space allocated
        //then starting from the end of the array
        //use std::move to move (shift) data until we reach pos
        //then move data at value to pos

        //check memory
        //problem: when we grow our array we are creating a new dynamic array
        //with more memory and copying values over
        //this means that our iterator will be pointing to deallocated heap memory
        //we need to also transfer our iterator over to our new array 
        //since the two arrays will be the same at this point
        //and will only have diff in mem usage
        //we can use the minus operator defined inside our iterator class to
        //determine where our iterator is in comparison to the first element
        //and then reconstruct this iterator using our new array

        if(_size == _capacity) {
            int i = pos - this->begin();
            this->grow();
            pos = this->begin() + i;
        }
        T prev = std::move(value);
        iterator curr = pos;
        while(curr != this->end()) {
            T temp = std::move(*curr);
            *curr = std::move(prev);
            prev = std::move(temp);
            curr++;
        }
        *curr = std::move(prev);
        _size++;
        return pos;

    }
    iterator insert(iterator pos, T&& value) {
       
        if(_size == _capacity) {
            int i = pos - this->begin();
            this->grow();
            pos = this->begin() + i;
        }
        T prev = std::move(value);
        iterator curr = pos;
        while(curr != this->end()) {
            T temp = std::move(*curr);
            *curr = std::move(prev);
            prev = std::move(temp);
            curr++;
        }
        *curr = std::move(prev);
        _size++;
        return pos;

    }
    iterator insert(iterator pos, size_t count, const T& value) {
        //inserts value starting at pos, count # of times
        //we'll first need to check if we have enough space
        //check if size + count <= capacity
        //if yes, then we can go on ahead with out insertion
        //otherwise, we have to first allocate more memory for our vector
        
        //we then need to shift over all elements start at pos
        //down count # of times
        //we can do this using a variable prev to hold the previous value
        //and a iterator curr representing the current element we're at
        //each time we'll save the curr element's val in a temp var
        //we'll then redefine curr's val to prev's value 
        //and redefine prev to temp after
        //do this count amount of times
        int i = pos - this->begin();
        while(_size + count > _capacity) {
            this->grow();
        }
        pos = this->begin() + i;

        //shift all elements count # of times from pos onwards
        for(size_t i = 0; i < count; i++) {
            //shifts elements by 1
            T prev = *(pos + i);
            iterator curr = pos + i + 1;
            while(curr != this->end()) {
                T temp = std::move(*curr);
                *curr = std::move(prev);
                prev = std::move(temp);
                curr += 1;
            }
            *curr = std::move(prev);
            _size++;
        }
        //insert values
        for(size_t i = 0; i < count; i++) {
            *(pos + i) = value;
        }

        return pos;
    }
    iterator erase(iterator pos) {
        //erases element at given position pointer
        //this means that it shifts every element to the right of it left by one
        //filling in the spot of that element
        //edge case: what if the object at pos is allocated on the heap?
            //how can we detect that and deallocate that mem?
        //traverse thru our vector in reverse, starting at the last element all the way to pos
        //we can do this by using a curr interator that starts at the end of our array and runs while it != pos
        //each time we'll hold curr val in temp var, redefine curr, and then set prev to temp
        //at the end of the algorithm, decrement size by 1 and return 
        if(pos == this->end() - 1) {
            _size--;
            return this->end();
        }
        iterator curr = this->end() - 2;
        T& prev = array[_size - 1];
        while(curr != pos) {
            T temp = std::move(*curr);
            *curr = std::move(prev);
            prev = std::move(temp);
            curr--;
        }
        *curr = std::move(prev);
        _size--;
        return pos;
        
    }
    iterator erase(iterator first, iterator last) {
        //when we erase we simply shift the elements to the right of position
        //to the left to cover the spot we removed
        //we do this by simply moving around data
        //so using the - operator we can get the window size of our erase window
        //then we can run erase() on the first iterator that many times

        bool eraseLast = last == this->end() - 1;
        int windowSize = last - first;
        for(int i = 0; i < windowSize; i++) {
            this->erase(first);
        }
        return eraseLast ? this->end() : first;
    }

    class iterator {
    public:
        //guessing these are the public data members?
        //TO-DO: go to office hours and ask
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;
    private:
        // Add your own data members here
        // HINT: For random_access_iterator, the data member is a pointer 99.9% of the time
        pointer position;

    public:
        iterator() {
            //default constructor
            position = nullptr;
        }
        iterator(pointer position) {
            //parametized constructor that takes in a pointer to the array element we want the iterator to start at
            this->position = position;
        }
        // Add any constructors that you may need
        iterator(const iterator& copy) {
            position = copy.position;
        }
        // This assignment operator is done for you, please do not add more
        iterator& operator=(const iterator&) noexcept = default;

        [[nodiscard]] reference operator*() const noexcept {
            //dereferences(gets value at current pos)
            return *position;
        }
        [[nodiscard]] pointer operator->() const noexcept {
            //allows for using arrow operator to use member functions on
            //pointers
            return position;
        }

        // Prefix Increment: ++a
        iterator& operator++() noexcept {
            //increments iterator by one element and returns it
            position++;
            return *this;
        }
        // Postfix Increment: a++
        iterator operator++(int) noexcept {
            //same as other but instead returns a copy to before incremented
            iterator copy = *this;
            position++;
            return copy;
        }
        // Prefix Decrement: --a
        iterator& operator--() noexcept {
            //decrements position by 1 and returns iterator
            position--;
            return *this;
        }
        // Postfix Decrement: a--
        iterator operator--(int) noexcept {
            iterator copy = *this;
            position--;
            return copy;
        
        }

        iterator& operator+=(difference_type offset) noexcept {
            //advance the interator offect amount of times return the interator after
            position += offset;

            return *this;
        }
        [[nodiscard]] iterator operator+(difference_type offset) const noexcept {
            //returns a copy of the iterator advanced by offset
            iterator copy = *this;
            copy.position += offset;

            return copy;
        }
        
        iterator& operator-=(difference_type offset) noexcept {
            position -= offset;
            return *this;
        }
        [[nodiscard]] iterator operator-(difference_type offset) const noexcept {
            iterator copy = *this;
            copy.position -= offset;

            return copy;
        }
        [[nodiscard]] difference_type operator-(const iterator& rhs) const noexcept {
            //finds out how far apart two iterators are.
            return this->position - rhs.position;
        }

        [[nodiscard]] reference operator[](difference_type offset) const noexcept {
            return *(position + offset);

        }

        [[nodiscard]] bool operator==(const iterator& rhs) const noexcept {
            //checks if two iterators are equal meaning they have the same memory address
            return position == rhs.position;
        }
        [[nodiscard]] bool operator!=(const iterator& rhs) const noexcept {
            //checks if two iterators are not equals
            return position != rhs.position;
        }
        [[nodiscard]] bool operator<(const iterator& rhs) const noexcept {
            //checks if an iterator comes before another
            return position < rhs.position;
        }
        [[nodiscard]] bool operator>(const iterator& rhs) const noexcept {
            return position > rhs.position;
        }
        [[nodiscard]] bool operator<=(const iterator& rhs) const noexcept {
            return position <= rhs.position;
        }
        [[nodiscard]] bool operator>=(const iterator& rhs) const noexcept {
            return position >= rhs.position;
        }
    };

    void clear() noexcept {
        //clearing the vector just means erasin all the elements within it
        //do not delete the dynamic array and do not reset capacity
        _size = 0;
    }
};

// This ensures at compile time that the deduced argument _Iterator is a Vector<T>::iterator
// There is no way we know of to back-substitute template <typename T> for external functions
// because it leads to a non-deduced context
namespace {
    template <typename _Iterator>
    using is_vector_iterator = std::is_same<typename Vector<typename _Iterator::value_type>::iterator, _Iterator>;
}

template <typename _Iterator, bool _enable = is_vector_iterator<_Iterator>::value>
[[nodiscard]] _Iterator operator+(typename _Iterator::difference_type offset, _Iterator const& iterator) noexcept {
    return iterator + offset;
}

#endif
