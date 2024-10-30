#include <cstddef>    // size_t
#include <functional> // std::hash
#include <ios>
#include <utility>    // std::pair
#include <iostream>

#include "primes.h"



template <typename Key, typename T, typename Hash = std::hash<Key>, typename Pred = std::equal_to<Key>>
class UnorderedMap {
    public:

    using key_type = Key;
    using mapped_type = T;
    using const_mapped_type = const T;
    using hasher = Hash;
    using key_equal = Pred;
    using value_type = std::pair<const key_type, mapped_type>;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    private:

    struct HashNode {
        HashNode *next;
        value_type val;

        HashNode(HashNode *next = nullptr) : next{next} {}
        HashNode(const value_type & val, HashNode * next = nullptr) : next { next }, val { val } { }
        HashNode(value_type && val, HashNode * next = nullptr) : next { next }, val { std::move(val) } { }
    };

    size_type _bucket_count;
    HashNode **_buckets;

    HashNode * _head;
    size_type _size;

    Hash _hash;
    key_equal _equal;

    static size_type _range_hash(size_type hash_code, size_type bucket_count) {
        return hash_code % bucket_count;
    }

    public:

    template <typename pointer_type, typename reference_type, typename _value_type>
    class basic_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = _value_type;
        using difference_type = ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

    private:
        friend class UnorderedMap<Key, T, Hash, key_equal>;
        using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

        const UnorderedMap * _map;
        HashNode * _ptr;

        explicit basic_iterator(UnorderedMap const * map, HashNode *ptr) noexcept {
            _map = map;
            _ptr = ptr;
        }

    public:
        basic_iterator() {
            _map = nullptr;
            _ptr = nullptr;
        };

        basic_iterator(const basic_iterator &) = default;
        basic_iterator(basic_iterator &&) = default;
        ~basic_iterator() = default;
        basic_iterator &operator=(const basic_iterator &) = default;
        basic_iterator &operator=(basic_iterator &&) = default;
        reference operator*() const {
            return _ptr->val;
        }
        pointer operator->() const {
            return &(_ptr->val);
        }
        basic_iterator &operator++() {
            //changes _ptr to the next node in the unordered map
            //even if that node is in another bucket
            //we want to first check if the next node is nullptr
            //if yes, then our next node is in the next bucket
            //we can find that bucket using the _bucket method

            if(_ptr->next) {
                _ptr = _ptr->next;
                return *this;
            }
            size_t bucket_index = _map->_bucket(_ptr->val) + 1;
            while(bucket_index < (_map->_bucket_count) && !(_map->_buckets[bucket_index])){
                bucket_index++;
            }
            if(bucket_index >= _map->_bucket_count)
                _ptr = nullptr;
            else
                _ptr = _map->_buckets[bucket_index];
            return *this;
        }
        basic_iterator operator++(int) {
            basic_iterator copy = *this;
            if(_ptr->next) {
                _ptr = _ptr->next;
                return copy;
            }
            size_t bucket_index = _map->_bucket(_ptr->val) + 1;
            while(bucket_index < (_map->_bucket_count) && !(_map->_buckets[bucket_index])){
                bucket_index++;
            }
            if(bucket_index >= _map->_bucket_count)
                _ptr = nullptr;
            else
                _ptr = _map->_buckets[bucket_index];
            return copy;

        }
        bool operator==(const basic_iterator &other) const noexcept {
            return _ptr == other._ptr;
        }
        bool operator!=(const basic_iterator &other) const noexcept {
            return _ptr != other._ptr;
        }
    };

    using iterator = basic_iterator<pointer, reference, value_type>;
    using const_iterator = basic_iterator<const_pointer, const_reference, const value_type>;

    class local_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::pair<const key_type, mapped_type>;
            using difference_type = ptrdiff_t;
            using pointer = value_type *;
            using reference = value_type &;

        private:
            friend class UnorderedMap<Key, T, Hash, key_equal>;
            using HashNode = typename UnorderedMap<Key, T, Hash, key_equal>::HashNode;

            HashNode * _node;

            explicit local_iterator( HashNode * node ) noexcept {
                _node = node;
            }

        public:
            local_iterator() {
                _node = nullptr;
            }

            local_iterator(const local_iterator &) = default;
            local_iterator(local_iterator &&) = default;
            ~local_iterator() = default;
            local_iterator &operator=(const local_iterator &) = default;
            local_iterator &operator=(local_iterator &&) = default;
            reference operator*() const {
                return _node->val;
            }
            pointer operator->() const {
                return &(_node->val);
            }
            local_iterator & operator++() {
                //increments iterator
                _node = _node->next;
                return *this;
            }
            local_iterator operator++(int) {
                local_iterator temp = *this;
                _node = _node->next;
                return temp;
            }

            bool operator==(const local_iterator &other) const noexcept {
                //determines if two iterators refer to the same HashNode
                return _node == other._node;
            }
            bool operator!=(const local_iterator &other) const noexcept {
                return _node != other._node;
            }
    };

private:

    size_type _bucket(size_t code) const { 
        //returns the index of the bucket for hash code
        return _range_hash(code, _bucket_count);
    }
    size_type _bucket(const Key & key) const {
        //hashes a given key then returns the index of the bucket containing the value 
        //associated with a given key
        size_t code = _hash(key);
        return _bucket(code);
    }
    size_type _bucket(const value_type & val) const {
        //hashes the key in the pair
        //and returns the index of the bucket for the given key
        return _bucket(val.first);
    }

    HashNode*& _find(size_type code, size_type bucket, const Key & key) {
        //traverses bucket for given hash code
        //returns node with given key if it exists
        //otherwise returns nullptr

        HashNode* curr = _buckets[bucket];

        while(curr) {
            if((curr->val).first == key) {
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    HashNode*& _find(const Key & key) {
        //same as above but we need to calculate the hash code, and the bucket index
        size_type code = _hash(key);
        size_type bucket = _bucket(code);
        return _find(code, bucket, key);
    }

    HashNode * _insert_into_bucket(size_type bucket, value_type && value) {
        //the global _head HashNode pointer should point to the first populated
        //bucket in our HashNode

        //everytime we insert node into a given bucket, we also want to check the indexes
        //of the buckets, if the curr bucket has an index less than or eq to _head's bucket
        //then set head to this bucket
        
        //algorithm:
        //1. get bucket that our _head points to using the private _bucket() helper fn
        //2. create new HashNode on the heap using given value
        //3. set bucket's new head to that HashNode
        //4. compare _head bucket and curr bucket and reassign accordingly

        size_type head_bucket = 0;
        bool exists = false;
        HashNode* newNode;
        
        if(_head) {
            head_bucket = _bucket(_head->val);
        }

        HashNode* curr = _buckets[bucket];
        while(curr) {
            if((curr->val).first == value.first) {
                exists = true;
                newNode = curr;
                if((curr->val).second != value.second) {
                    (curr->val).second = value.second;
                }
                break;
            }
            curr = curr->next;
        }
        if(!exists) {
            newNode = new HashNode(std::move(value), _buckets[bucket]);
            _buckets[bucket] = newNode;
            _size++;
        }

        if(!_head || bucket <= head_bucket) {
            _head = newNode;
        }
        

        return newNode;
    }

    void _move_content(UnorderedMap & src, UnorderedMap & dst) {
        //moves the contents from src to dst
        //what to move:
            //_buckets
            //_hash
            //_bucket_count
            //_size
            //_equal
        dst._buckets = std::move(src._buckets);
        dst._hash = std::move(src._hash);
        dst._bucket_count = std::move(src._bucket_count);
        dst._equal = std::move(src._equal);
        dst._size = std::move(src._size);
        dst._head = std::move(src._head);

    }

public:
    explicit UnorderedMap(size_type bucket_count, const Hash & hash = Hash { },
                const key_equal & equal = key_equal { }) {
        bucket_count = next_greater_prime(bucket_count);
        _buckets = new HashNode*[bucket_count]{};
        _bucket_count = bucket_count;
        _hash = hash;
        _equal = equal;
        _head = nullptr;
        _size = 0;
    }

    ~UnorderedMap() {
        //destructor for our hash map
        //we need to deallocate all nodes we have allocated
        
        //1. for loop to traverse thru bucket indexes
        //2. for each bucket in buckets, traverse bucket and delete all nodes in bucket

        for(size_t i = 0; i < _bucket_count; i++) {
            HashNode* curr = _buckets[i];
            while(curr) {
                HashNode* node = curr;
                curr = curr->next;
                delete node;
            }
            _buckets[i] = nullptr;
        }
        delete _buckets;
        _head = nullptr;
        _buckets = nullptr;


    }

    UnorderedMap(const UnorderedMap & other) {
        //copy constructor
        //instantiates a new hashmap using another hashmap's values
        
        //for _hash, _equal, and _bucket_count we can just copy normally
        //for our _buckets we will need to traverse thru other's buckets
        //and allocate nodes manually

        _hash = other._hash;
        _equal = other._equal;
        _bucket_count = other._bucket_count;
        _buckets = new HashNode*[_bucket_count]{};
        _size = other._size;

        bool isHead = true;

        for(size_type i = 0; i < other._bucket_count; i++) {
            //traverse thru every bucket
            //for each bucket, while there are nodes create new nodes
            HashNode* prev = nullptr;
            HashNode* curr = other._buckets[i];

            while(curr) {
                HashNode * node = new HashNode(curr->val);
                if(!prev) {
                    if(isHead) {
                        _head = node;
                        isHead = false;
                    }
                    _buckets[i] = node;
                } else {
                    prev->next = node;
                }
                prev = node;
                curr = curr->next;


            }
        }

    }

    UnorderedMap(UnorderedMap && other) {
        //move constructor
        _bucket_count = other._bucket_count;
        _hash = other._hash;
        _equal = other._equal;
        _buckets = other._buckets;
        _size = other._size;
        _head = other._head;

        other._buckets = new HashNode*[_bucket_count]{ };
        other._size = 0;
        other._head = nullptr;



    }

    UnorderedMap & operator=(const UnorderedMap & other) {
        if(this != &other) {
            this->clear();
            delete _buckets;
            _hash = other._hash;
            _equal = other._equal;
            _bucket_count = other._bucket_count;
            _buckets = new HashNode*[_bucket_count]{};
            _size = other._size;

            for(size_type i = 0; i < other._bucket_count; i++) {
                //traverse thru every bucket
                //for each bucket, while there are nodes create new nodes
                HashNode* prev = nullptr;
                HashNode* curr = other._buckets[i];
                bool isHead = true;
                while(curr) {
                    HashNode * node = new HashNode(curr->val);
                    if(!prev) {
                        if(isHead) {
                            _head = node;
                            isHead = false;
                        }
                        _buckets[i] = node;
                    } else {
                        prev->next = node;
                    }
                    prev = node;
                    curr = curr->next;

                }
            }

        }
        return *this;
    }

    UnorderedMap & operator=(UnorderedMap && other) {
        if(this != &other) {
            this->clear();
            delete _buckets;
            _move_content(other, *this);
            
            other._size = 0;
            other._buckets = new HashNode*[other._bucket_count]{};
            other._head = nullptr;
        }
        return *this;
    }

    void clear() noexcept {
        for(size_t i = 0; i < _bucket_count; i++) {
            HashNode* curr = _buckets[i];
            while(curr) {
                HashNode* node = curr;
                curr = curr->next;
                delete node;
            }
            _buckets[i] = nullptr;
            _size = 0;
        }

    }

    size_type size() const noexcept {
        return _size;
    }

    bool empty() const noexcept {
        return _size == 0;
    }

    size_type bucket_count() const noexcept {
        return _bucket_count;
    }

    iterator begin() {
        //returns an iterator pointing to the first element in the Hash Map
        //first element in our hashmap will be set to _head
        //create a new iterator using _head HashNode
        iterator start(this, _head);
        return start;
    }
    iterator end() {
        //returns an iterator pointing to the last element in the hash map
        //confused??
        iterator end(this, nullptr);
        return end;
    }

    const_iterator cbegin() const {
        const_iterator start(this, _head);
        return start;
    };
    const_iterator cend() const {
        const_iterator end(this, nullptr);
        return end;
    };

    local_iterator begin(size_type n) {
        HashNode* first = _buckets[n];
        local_iterator start(first);
        return start;
    }
    local_iterator end(size_type n) {
       local_iterator end(nullptr);
       return end; 
    }

    size_type bucket_size(size_type n) {
        //returns size of bucket at index n
        HashNode* curr = _buckets[n];
        size_type count = 0;

        while(curr) {
            count++;
            curr = curr->next;
        }
        return count;
    }

    float load_factor() const {
       //returns load_factor
       //load factor is the number of elements inside the hashmap divided by number of buckets
       return _size / static_cast<float>(_bucket_count);
    }

    size_type bucket(const Key & key) const {
        //returns index of bucket for given key
        return _bucket(key);
    }

    std::pair<iterator, bool> insert(value_type && value) {
        //inserts value into hash map
        //makes sure inserted value is not already inside list
        //uses move semantics
        bool exists = false;
        size_type bucket_index = _bucket(value);
        HashNode* insertedNode = nullptr;

        //traverse thru bucket determine if value exists already
        //if key exists, but value is diff redefine val?
        HashNode* curr = _buckets[bucket_index];
        while(curr) {
            if((curr->val).first == value.first) {
                exists = true;
                insertedNode = curr;
                if((curr->val).second != value.second) {
                    (curr->val).second = value.second;
                }
                break;
            }
            curr = curr->next;
        }

        if(!exists) {
            insertedNode = _insert_into_bucket(bucket_index, std::move(value));
        }

        iterator nodeIterator(this, insertedNode);
        std::pair<iterator, bool> insertPair(nodeIterator, !exists);
        
        return insertPair;
    }

    std::pair<iterator, bool> insert(const value_type & value) {
        bool exists = false;
        size_type bucket_index = _bucket(value.first);
        HashNode* insertedNode = nullptr;

        //traverse thru bucket determine if value exists already
        //if key exists, but value is diff redefine val?
        HashNode* curr = _buckets[bucket_index];
        while(curr) {
            if((curr->val).first == value.first) {
                exists = true;
                insertedNode = curr;
                if((curr->val).second != value.second) {
                    (curr->val).second = value.second;
                }
                break;
            }
            curr = curr->next;
        }

        if(!exists) {
            //create new node
            //insert it at bucket_index
            //redefine _head if needed

            HashNode* newNode = new HashNode(value, _buckets[bucket_index]);
            _buckets[bucket_index] = newNode;
            if(_head) {
                size_t head_index = _bucket(_head->val);
                if(bucket_index <= head_index) {
                    _head = newNode;
                }
            } else {
                _head = newNode;
            }

            insertedNode = newNode;
            _size++;
        }

        iterator nodeIterator(this, insertedNode);
        std::pair<iterator, bool> insertPair(nodeIterator, !exists);

        return insertPair;
    }

    iterator find(const Key & key) {
        //finds and returns iterator to HashNode with given key
        HashNode* node = _find(key);
        iterator target(this, node);

        return target;
    }

    T& operator[](const Key & key) {
        //finds node with given key and returns it's data
        //find bucket for given key
        //traverses bucket for key
        //if it exists then return the mapped value associated with key
        //if it does not exist, insert it into that bucket
        //and return mapped val if inserted key

        size_type bucket_index = _bucket(key);
        HashNode* curr = _buckets[bucket_index];
        while(curr) {
            if((curr->val).first == key) {
                return (curr->val).second;
            }
            curr = curr->next;
        }
        std::pair<Key, T> temp_val(key, T{});
        HashNode* newNode = _insert_into_bucket(bucket_index, std::move(temp_val));
        return newNode->val.second;
    }

    iterator erase(iterator pos) {
        //to remove element at pos, we need to find the bucket for the given item
        //then traverse that bucket using a prev and curr pointer
        // when we find the node, set prev.next to delete node's next
        //if prev is nullptr, that means the delete node is the first node
        //in that case we want to set the bucket's head to it's next node

        if(!pos._ptr) {
            return pos;
        }

        size_type bucket_index = _bucket(*pos);
        HashNode* prev = nullptr;
        HashNode* curr = _buckets[bucket_index];

        while(curr) {
            if((curr->val).first == ((pos._ptr)->val).first) {

                if(!prev) {
                    _buckets[bucket_index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                _size--;
                ++pos;
                if(curr == _head) {
                    _head = pos._ptr;
                }
                delete curr;
                return pos;
            }
            prev = curr;
            curr = curr->next;
        }
        
        return pos;
    }

    size_type erase(const Key & key) {
        //removes element with given key and returns number of elements removed
        //we first need to find the bucket index
        //then we need to traverse the bucket with curr and prev pointer
        //once we find the node, remove it and return 1
        //if we traverse thru the whole bucket w/o finding it, return 0

        size_type bucket_index = _bucket(key);
        HashNode* prev = nullptr;
        HashNode* curr = _buckets[bucket_index];

        while(curr) {
            if((curr->val).first == key) {
                if(!prev) {
                    _buckets[bucket_index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                _size--;
                delete curr;
                return 1;
            }
            prev = curr;
            curr = curr->next;

        }
        return 0;
    }

    template<typename KK, typename VV>
    friend void print_map(const UnorderedMap<KK, VV> & map, std::ostream & os);
};

template<typename K, typename V>
void print_map(const UnorderedMap<K, V> & map, std::ostream & os = std::cout) {
    using size_type = typename UnorderedMap<K, V>::size_type;
    using HashNode = typename UnorderedMap<K, V>::HashNode;

    for(size_type bucket = 0; bucket < map.bucket_count(); bucket++) {
        os << bucket << ": ";

        HashNode const * node = map._buckets[bucket];

        while(node) {
            os << "(" << node->val.first << ", " << node->val.second << ") ";
            node = node->next;
        }

        os << std::endl;
    }
}
