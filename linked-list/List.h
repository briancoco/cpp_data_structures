#pragma once

#include <cstddef> // size_t
#include <iterator> // std::bidirectional_iterator_tag
#include <type_traits> // std::is_same, std::enable_if

template <class T>
class List {
    private:
    struct Node {
        Node *next, *prev;
        T data;
        explicit Node(Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev} {}
        explicit Node(const T& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{data} {}
        explicit Node(T&& data, Node* prev = nullptr, Node* next = nullptr)
        : next{next}, prev{prev}, data{std::move(data)} {}
    };

    template <typename pointer_type, typename reference_type>
    class basic_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = T;
        using difference_type   = ptrdiff_t;
        using pointer           = pointer_type;
        using reference         = reference_type;
    private:
        friend class List<value_type>;
        using Node = typename List<value_type>::Node;

        Node* node;

        explicit basic_iterator(Node* ptr) noexcept : node{ptr} {}
        explicit basic_iterator(const Node* ptr) noexcept : node{const_cast<Node*>(ptr)} {}

    public:
        basic_iterator() {
            //default constructor
            node = nullptr;
        };
        basic_iterator(const basic_iterator&) = default;
        basic_iterator(basic_iterator&&) = default;
        ~basic_iterator() = default;
        basic_iterator& operator=(const basic_iterator&) = default;
        basic_iterator& operator=(basic_iterator&&) = default;

        reference operator*() const {
            // return a reference to the current node's data
            return node->data;
        }
        pointer operator->() const {
            //return pointer to data managed by Node
            return &(node->data);
        }

        // Prefix Increment: ++a
        basic_iterator& operator++() {
            //move to next node in list and return iterator
            node = node->next;
            return *this;
        }
        // Postfix Increment: a++
        basic_iterator operator++(int) {
            // create a copy of the iterator
            //move to next node
            //return copy
            basic_iterator copy = *this;
            node = node->next;

            return copy;
        }
        // Prefix Decrement: --a
        basic_iterator& operator--() {
            node = node->prev;
            return *this;
        }
        // Postfix Decrement: a--
        basic_iterator operator--(int) {
            basic_iterator copy = *this;
            node = node->prev;

            return copy;
        }

        bool operator==(const basic_iterator& other) const noexcept {
            return node == other.node;
        }
        bool operator!=(const basic_iterator& other) const noexcept {
            return node != other.node;
        }
    };

public:
    using value_type      = T;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = basic_iterator<pointer, reference>;
    using const_iterator  = basic_iterator<const_pointer, const_reference>;

private:
    Node head, tail;
    size_type _size;

public:
    List() {
        //default constructor
        //set head next to tail
        //set tail prev to head
        head.next = &tail;
        tail.prev = &head;
        _size = 0;
    }
    List( size_type count, const T& value ) {
        //constructs a linked list with count nodes all of the same value
        //construct new node
        //set new node's next to tail
        //set new node's prev to tail.prev
        //set tail.prev.next = new node
        //set tail.prev to new node

        head.next = &tail;
        tail.prev = &head;
        _size = count;

        for(size_type i = 0; i < count; i++) {
            Node* newNode = new Node(value, tail.prev, &tail);
            (tail.prev)->next = newNode;
            tail.prev = newNode;
        }
    }
    explicit List( size_type count ) {
        //construct linked list with count default-inserted instances of T
        head.next = &tail;
        tail.prev = &head;
        _size = count;

        for(size_type i = 0; i < count; i++) {
            Node* newNode = new Node(T(), tail.prev, &tail);
            (tail.prev)->next = newNode;
            tail.prev = newNode;
        }

    }
    List( const List& other ) {
        //copy constructor
        //creates a new list which is a deep copy of the list passed in
        //1. traverse thru the other linked list
        //2. each time, create a new node with the value of the other node
        //3. add it to the list
        //4. continue until we exhaust all node elements. 

        //since size is given to us, we can rely on that to traverse thru the linked list
        head.next = &tail;
        tail.prev = &head;
        _size = other._size;

        Node* curr = other.head.next;
        for(size_type i = 0; i < other._size; i++) {
            Node* newNode = new Node(curr->data, tail.prev, &tail);
            (tail.prev)->next = newNode;
            tail.prev = newNode;
            curr = curr->next;
        }

    }
    List( List&& other ) {
        //move constructor
        //simply take the data members of the other linked list
        //and move them over here.
        if(!other.empty()) {
            head.next = other.head.next;
            (other.head.next)->prev = &head;

            tail.prev = other.tail.prev;
            (other.tail.prev)->next = &tail;
            _size = other._size;

            other.head.next = &(other.tail);
            other.tail.prev = &(other.head);
            other._size = 0;
        } else {
            _size = 0;
        }
    }
    ~List() {
        //destructor
        //traverse the linked list and deallocate all nodes

        //1. traverse thru the linked list using a pointer
        //2. each time, create a temp node pointer to hold curr node
        //3. increment our curr node pointer
        //4. delete prev node
        //5. continue _size times
        //6. in the end set head and tail to nullptr for safety

        Node* curr = head.next;
        for(size_type i = 0; i < _size; i++) {
            Node* node = curr;
            curr = curr->next;
            delete node;
        }
        head.next = nullptr;
        tail.prev = nullptr;
        _size = 0;
    }
    List& operator=( const List& other ) {
        // copy assignment operator
        // 1. check if the two lists are the same
        // 2. if not, then deallocate current list
        // 3. then create a deep copy of other
        // 4. return reference to current list
        if(this != &other) {
            this->clear();
            _size = other._size;

            Node* curr = other.head.next;
            for(size_type i = 0; i < other._size; i++) {
                Node* newNode = new Node(curr->data, tail.prev, &tail);
                (tail.prev)->next = newNode;
                tail.prev = newNode;
                curr = curr->next;
            }
        }
        return *this;
    }
    List& operator=( List&& other ) noexcept {
        //move assignment operator
        //1. check if both lists are the safe
        //2. if no, then deallocate curr list
        //3. and move all data members from other over
        //4. return ref to curr list
        if(this != &other) {
            this->clear();
            if(!other.empty()) {
                //must redefine first/last nodes of other LL to point to new list's sentiel nodes
                (other.tail.prev)->next = &tail;
                (other.head.next)->prev = &head;

                head.next = other.head.next;
                tail.prev = other.tail.prev;
                _size = other._size;

                other.head.next = &(other.tail);
                other.tail.prev = &(other.head);
                other._size = 0;
            }
        }
        return *this;
    }

    reference front() {
        //returns reference to first node in the list
        return (head.next)->data;
    }
    const_reference front() const {
        return (head.next)->data;
    }
	
    reference back() {
        // returns reference to last node of the list
        return (tail.prev)->data;
    }
    const_reference back() const {
        return (tail.prev)->data;
    }
	
    iterator begin() noexcept {
        //creates and returns an interator pointing to the first node in the linked list
        iterator start(head.next);

        return start;
    }
    const_iterator begin() const noexcept {
        const_iterator start(head.next);

        return start;
    }
    const_iterator cbegin() const noexcept {
        const_iterator start(head.next);

        return start;
    }

    iterator end() noexcept {
        iterator end(&tail);

        return end;
    }
    const_iterator end() const noexcept {
        const_iterator end(&tail);

        return end;
    }
    const_iterator cend() const noexcept {
        const_iterator end(&tail);

        return end;
    }

    bool empty() const noexcept {
        return _size == 0;
    }

    size_type size() const noexcept {
        return _size;
    }

    void clear() noexcept {
        Node* curr = head.next;
        for(size_type i = 0; i < _size; i++) {
            Node* node = curr;
            curr = curr->next;
            delete node;
        }
        head.next = &tail;
        tail.prev = &head;
        _size = 0;
    }

    iterator insert( const_iterator pos, const T& value ) {
        //inserts a single element at pos
        //create a newNode 
        // set prev to pos.prev, set next to pos
        // set pos.prev.next to newNode
        // set pos.prev to newNode
        Node* newNode = new Node(value, (pos.node)->prev, pos.node);
        (pos.node)->prev->next = newNode;
        (pos.node)->prev = newNode;

        iterator insertedNode(newNode);
        _size++;
        return insertedNode;
    }
    iterator insert( const_iterator pos, T&& value ) {
        Node* newNode = new Node(std::move(value), (pos.node)->prev, pos.node);
        (pos.node)->prev->next = newNode;
        (pos.node)->prev = newNode;

        iterator insertedNode(newNode);
        _size++;
        return insertedNode;
    }

    iterator erase( const_iterator pos ) {
        //removes node at given pos iterator
        //to remove we need to define the pointers directly to the left/right of
        //this node.
        //we can do this in a straightforward way by first capturing both adj nodes in a variable
        //then simply reassigning their pointers
        

        //edge case: what if list is empty?
        //do nothing?
        iterator res((pos.node)->next);
        if(!this->empty()) {
            Node* prev = (pos.node)->prev;
            Node* next = (pos.node)->next;

            prev->next = next;
            next->prev = prev;

            _size--;
            delete pos.node;
        }

        return res;

    }

    void push_back( const T& value ) {
        // adds element to back of the linked list
        //1. allocate new node
        //2. set node's next pointer to tail and prev to tail prev
        //3. set tail.prev.next = newNode
        //4. set tail.prev to newNode
        Node* newNode = new Node(value, tail.prev, &tail);

        (tail.prev)->next = newNode;
        tail.prev = newNode;
        _size++;
    }
    void push_back( T&& value ) {
        Node* newNode = new Node(std::move(value), tail.prev, &tail);
        //why do we need std::move again if it is already an r-value??
        (tail.prev)->next = newNode;
        tail.prev = newNode;
        _size++;
    }

    void pop_back() {
        // removes last element from list
        //1. check if list is empty, if yes do nothing
        //2. otherwise, use a temp node pointer to get capture the node we're deleting
        //3. then set tail.prev.prev.next equal to tail
        //4. set tail.prev = tail.prev.prev
        //5. delete node
        if(this->empty()) {
            return;
        }
        Node* deleteNode = tail.prev;
        
        (tail.prev)->prev->next = &tail;
        tail.prev = (tail.prev)->prev;

        delete deleteNode;
        _size--;
    }
	
    void push_front( const T& value ) {
        //adds node to front of the list
        //1. allocate new node
        //2. set node's prev to head and next to head.next
        //3. set head.next->prev to newNode
        //4. set head.next to newNode
        Node* newNode = new Node(value, &head, head.next);

        (head.next)->prev = newNode;
        head.next = newNode;
        _size++;
    }
	void push_front( T&& value ) {
        Node* newNode = new Node(std::move(value), &head, head.next);

        (head.next)->prev = newNode;
        head.next = newNode;
        _size++;
    }

    void pop_front() {
        if(this->empty()) {
            return;
        }

        Node* deleteNode = head.next;
        (head.next)->next->prev = &head;
        head.next = (head.next)->next;

        delete deleteNode;
        _size--;
    }

    /*
      You do not need to modify these methods!
      
      These method provide the non-const complement 
      for the const_iterator methods provided above.
    */
    iterator insert( iterator pos, const T & value) { 
        return insert((const_iterator &) (pos), value);
    }

    iterator insert( iterator pos, T && value ) {
        return insert((const_iterator &) (pos), std::move(value));
    }

    iterator erase( iterator pos ) {
        return erase((const_iterator&)(pos));
    }
};


/*
    You do not need to modify these methods!

    These method provide a overload to compare const and 
    non-const iterators safely.
*/
 
namespace {
    template<typename Iter, typename ConstIter, typename T>
    using enable_for_list_iters = typename std::enable_if<
        std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::iterator, 
            Iter
        >{} && std::is_same<
            typename List<typename std::iterator_traits<Iter>::value_type>::const_iterator,
            ConstIter
        >{}, T>::type;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) == rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator==(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) == lhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const Iterator & lhs, const ConstIter & rhs) {
    return (const ConstIter &)(lhs) != rhs;
}

template<typename Iterator, typename ConstIter>
enable_for_list_iters<Iterator, ConstIter, bool> operator!=(const ConstIter & lhs, const Iterator & rhs) {
    return (const ConstIter &)(rhs) != lhs;
}