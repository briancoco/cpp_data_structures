#pragma once

#include <functional> // std::less
#include <iostream>
#include <queue> // std::queue
#include <utility> // std::pair

template <typename K, typename V, typename Comparator = std::less<K>>
class BinarySearchTree
{
  public:
    using key_type        = K;
    using value_type      = V;
    using key_compare     = Comparator;
    using pair            = std::pair<key_type, value_type>;
    using pointer         = pair*;
    using const_pointer   = const pair*;
    using reference       = pair&;
    using const_reference = const pair&;
    using difference_type = ptrdiff_t;
    using size_type       = size_t;

  private:
    struct BinaryNode
    {
        pair element;
        BinaryNode *left;
        BinaryNode *right;

        BinaryNode( const_reference theElement, BinaryNode *lt, BinaryNode *rt )
          : element{ theElement }, left{ lt }, right{ rt } { }
        
        BinaryNode( pair && theElement, BinaryNode *lt, BinaryNode *rt )
          : element{ std::move( theElement ) }, left{ lt }, right{ rt } { }
    };

    using node           = BinaryNode;
    using node_ptr       = node*;
    using const_node_ptr = const node*;

    node_ptr _root;
    size_type _size;
    key_compare comp;

    node_ptr copyConstruct(const node_ptr node) {
        //base case
        if (!node) {
            return nullptr;
        }
        node_ptr newNode = new BinaryNode(node->element, nullptr, nullptr);
        newNode->left = copyConstruct(node->left);
        newNode->right = copyConstruct(node->right);

        return newNode;
    }

    void deleteTree(node_ptr node) {
        if(!node) {
            return;
        }
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

    

  public:
    BinarySearchTree() {
        _root = nullptr;
        _size = 0;

    }
    BinarySearchTree( const BinarySearchTree & rhs ) {
        //copy constructor
        _root = copyConstruct(rhs._root);
        _size = rhs._size;
        
    }
    BinarySearchTree( BinarySearchTree && rhs ) {
        // move constructor
        _root = rhs._root;
        _size = rhs._size;
        
        rhs._root = nullptr;
        rhs._size = 0;
    }
    ~BinarySearchTree() {
        // destructor
        // traverse through the binary search tree deleting all allocated nodes
        //we need to do this using depth-first search in a post-order manner
        //this means we traverse the tree in the order left->right->curr
        //this will make it so that we deallocate all children nodes before parent.
        deleteTree(_root);
    }

    const_reference min() const { return min( _root )->element; }
    const_reference max() const { return max( _root )->element; }
    const_reference root() const {
        // returns const reference to the pair contained in the _root node
        return _root->element;
    }
    bool contains( const key_type & x ) const { return contains( x, _root ); }
    value_type & find( const key_type & key ) { return find( key, _root )->element.second; }
    const value_type & find( const key_type & key ) const { return find( key, _root )->element.second; }
    bool empty() const {
        //returns true if tree is empty and false otherwise
        return _size == 0;
    }
    size_type size() const {
        // returns size of the tree
        return _size;
    }

    void clear() {
        clear(_root);
        _size = 0;
    }
    void insert( const_reference x ) { insert( x, _root ); }
    void insert( pair && x ) { insert( std::move( x ), _root ); }
    void erase( const key_type & x ) { erase(x, _root); _size--;}

    BinarySearchTree & operator=( const BinarySearchTree & rhs ) {
        //copy assignment operator
        //deallocates current tree, and creates a copy of rhs
        //1. make sure current tree and tree to be copied are not the same
        //2. deallocate current tree
        //3. copy other tree
        //4. return ref to current tree

        if(this != &rhs) {
            deleteTree(_root);
            _root = this->copyConstruct(rhs._root);
            _size = rhs._size;
        }
        return *this;
    }
    BinarySearchTree & operator=( BinarySearchTree && rhs ) {
        // move assignment operator
        //first make sure two trees aren't the same
        //then deallocate current tree
        //then move data members of rhs to curr tree
        if(this != &rhs) {
            deleteTree(_root);
            _root = rhs._root;
            _size = rhs._size;

            rhs._root = nullptr;
            rhs._size = 0;
        }

        return *this;
    }

  private:

    

    void insert( const_reference x, node_ptr & t ) {
        // recursively traverse thru the BST
        //at each node, determine if the curr val to be inserted is less than or
        //greater than curr node
        //then traverse left/right accordingly
        //if the curr node is equal to the new node's key then simply update that node's value
        //and return

        if(!t) {
            _root = new BinaryNode(x, nullptr, nullptr);
            _size++;
        } else if (comp((t->element).first, x.first)) {
            if(!(t->right)) {
                node_ptr newNode = new BinaryNode(x, nullptr, nullptr);
                t->right = newNode;
                _size++;
            } else {
                insert(x, t->right);
            }
        } else if (comp(x.first, (t->element).first)){
            if(!(t->left)) {
                node_ptr newNode = new BinaryNode(x, nullptr, nullptr);
                t->left = newNode;
                _size++;
            } else {
                insert(x, t->left);
            }
        } else if((t->element).first == x.first) {
            (t->element).second = x.second;
        }
       
       //insertNode(x, t);

    }

    void insert( pair && x, node_ptr & t ) {
        if(!t) {
            _root = new BinaryNode(std::move(x), nullptr, nullptr);
            _size++;
        }  else if (comp((t->element).first, x.first)) {
            if(!(t->right)) {
                t->right = new BinaryNode(std::move(x), nullptr, nullptr);;
                _size++;
            } else {
                insert(std::move(x), t->right);
            }
        } else if (comp(x.first, (t->element).first)){
            if(!(t->left)) {
                t->left = new BinaryNode(std::move(x), nullptr, nullptr);
                _size++;
            } else {
                insert(std::move(x), t->left);
            }
        } else if((t->element).first == x.first) {
            t->element = std::move(x);
        }
       

        //insertNode(std::move(x), t);
    }

    void erase( const key_type & x, node_ptr & t ) {
        //erases the node with value x
        //when erasing we have 3 cases
        //1. if the node we're erasing has right children
            //then we want to set the leftmost right node to fill in the place
            //of the node we're removing
        //2. if the node we're erasing has only left children
            //then set rightmost left children to be filler node
        //3. node we're erasing has no children, we can erase w/o any extra work

        //traverse thru our BST, each time checking t->element.first against x
        //if it equals x, then we need to determine which case this node falls in
        //and remove accordingly

        //otherwise determine if the curr node is less/greater than x and traverse tree accordingly

        if(!t) {
            return;
        } else if(this->comp((t->element).first, x)) {
            erase(x, t->right);
        } else if(this->comp(x, (t->element).first)){
            erase(x, t->left);
        } else if((t->element).first == x) {
            if(t->right && t->left) {
                pair element = min(t->right)->element;
                erase(element.first, t);
                t->element = element;
            } else {
                node_ptr node = t;
                t = t->left ? t->left : t->right;
                delete node;
            }
        }
        // else if(this->comp((t->element).first, x)) {
        //     erase(x, t->right);
        // } else if(this->comp(x, (t->element).first)){
        //     erase(x, t->left);
        // }

    }

    const_node_ptr min( const_node_ptr t ) const {
        if(t->left) {
            return min(t->left);
        }
        return t;
    }

    const_node_ptr max( const_node_ptr t ) const {
        if(t->right) {
            return max(t->right);
        }
        return t;
    }

    bool contains( const key_type & x, const_node_ptr t ) const {
        // determines if there is a node with the key x in the tree
        if(!t) {
            return false;
        } else if((t->element).first == x) {
            return true;
        } else if(this->comp((t->element).first, x)) {
            return this->contains(x, t->right);
        } else {
            return this->contains(x, t->left);
        }

    }
    node_ptr find( const key_type & key, node_ptr t ) {
        //traverses bst to find node with given key and returns node
        //this can be done recursivly
        //base case: if t is nullptr, then return nullptr
        if(!t) {
            return nullptr;
        } else if(this->comp((t->element).first), key) {
            return find(key, t->right);
        } else if (this->comp(key, (t->element).first)){
            return find(key, t->left);
        } else if((t->element).first == key) {
            return t;
        }

    }
    const_node_ptr find( const key_type & key, const_node_ptr t ) const {
        if(!t) {
            return nullptr;
        } else if(this->comp((t->element).first, key)) {
            return find(key, t->right);
        } else if (this->comp(key, (t->element).first)){
            return find(key, t->left);
        } else if((t->element).first == key) {
            return t;
        } 

    }

    void clear( node_ptr & t ) {
        this->deleteTree(t);
        _root = nullptr;
    }
    
    node_ptr clone ( const_node_ptr t ) const {
        if (!t) {
            return nullptr;
        }
        node_ptr newNode = new BinaryNode(t->element, nullptr, nullptr);
        newNode->left = copyConstruct(t->left);
        newNode->right = copyConstruct(t->right);

        return newNode;
    }

  public:
    template <typename KK, typename VV, typename CC>
    friend void printLevelByLevel( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out );

    template <typename KK, typename VV, typename CC>
    friend std::ostream& printNode(std::ostream& o, const typename BinarySearchTree<KK, VV, CC>::node& bn);

    template <typename KK, typename VV, typename CC>
    friend void printTree( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out );

    template <typename KK, typename VV, typename CC>
    friend void printTree(typename BinarySearchTree<KK, VV, CC>::const_node_ptr t, std::ostream & out, unsigned depth );

    template <typename KK, typename VV, typename CC>
    friend void vizTree(
        typename BinarySearchTree<KK, VV, CC>::const_node_ptr node, 
        std::ostream & out,
        typename BinarySearchTree<KK, VV, CC>::const_node_ptr prev
    );

    template <typename KK, typename VV, typename CC>
    friend void vizTree(
        const BinarySearchTree<KK, VV, CC> & bst, 
        std::ostream & out
    );
};

template <typename KK, typename VV, typename CC>
std::ostream& printNode(std::ostream & o, const typename BinarySearchTree<KK, VV, CC>::node & bn) {
    return o << '(' << bn.element.first << ", " << bn.element.second << ')';
}

template <typename KK, typename VV, typename CC>
void printLevelByLevel( const BinarySearchTree<KK, VV, CC>& bst, std::ostream & out = std::cout ) {
    using node = typename BinarySearchTree<KK, VV, CC>::node;
    using node_ptr = typename BinarySearchTree<KK, VV, CC>::node_ptr;
    using const_node_ptr = typename BinarySearchTree<KK, VV, CC>::const_node_ptr;
    
    //given a BST, use a breath first traversal algorithm to print out the tree level by level
    //we will use the STL queue class to implement our queue
    //our queue will start with the root of the tree
    //each time we want to run a for loop which runs for the curr size of the queue
    //each iteration, we want to pop from the queue, check if the node we popped has any children node and add those to the queue
    //and then print out the curr node's (key, value) followed by a space
    //continue while our queue is non-empty

    if(!bst._root) return;

    std::queue<node_ptr> q;
    q.push(bst._root);
    bool validNodes = true;
    while(!q.empty() && validNodes) {
        validNodes = false;
        size_t queueSize = q.size();
        for(size_t i = 0; i < queueSize; i++) {
            node_ptr node = q.front();
            q.pop();
            if(node) {
                if(node->left) {
                    q.push(node->left);
                    validNodes = true;
                } else {
                    q.push(nullptr);
                }

                if(node->right) {
                    q.push(node->right);
                    validNodes = true;
                } else {
                    q.push(nullptr);
                }
                out << '(' << (node->element).first << ", " << (node->element).second << ") ";
            } else {
                q.push(nullptr);
                q.push(nullptr);
                out << "null ";
            }

        }
        out << std::endl;
    }



}

template <typename KK, typename VV, typename CC>
void printTree( const BinarySearchTree<KK, VV, CC> & bst, std::ostream & out = std::cout ) { printTree<KK, VV, CC>(bst._root, out ); }

template <typename KK, typename VV, typename CC>
void printTree(typename BinarySearchTree<KK, VV, CC>::const_node_ptr t, std::ostream & out, unsigned depth = 0 ) {
    if (t != nullptr) {
        printTree<KK, VV, CC>(t->right, out, depth + 1);
        for (unsigned i = 0; i < depth; ++i)
            out << '\t';
        printNode<KK, VV, CC>(out, *t) << '\n';
        printTree<KK, VV, CC>(t->left, out, depth + 1);
    }
}

template <typename KK, typename VV, typename CC>
void vizTree(
    typename BinarySearchTree<KK, VV, CC>::const_node_ptr node, 
    std::ostream & out,
    typename BinarySearchTree<KK, VV, CC>::const_node_ptr prev = nullptr
) {
    if(node) {
        std::hash<KK> khash{};

        out << "\t" "node_" << (uint32_t) khash(node->element.first)
            << "[label=\"" << node->element.first 
            << " [" << node->element.second << "]\"];" << std::endl;
        
        if(prev)
            out << "\tnode_" << (uint32_t) khash(prev->element.first) <<  " -> ";
        else
            out << "\t";
        
        out << "node_" << (uint32_t) khash(node->element.first) << ";" << std::endl;
    
        vizTree<KK, VV, CC>(node->left, out, node);
        vizTree<KK, VV, CC>(node->right, out, node);
    }
}

template <typename KK, typename VV, typename CC>
void vizTree(
    const BinarySearchTree<KK, VV, CC> & bst, 
    std::ostream & out = std::cout
) {
    out << "digraph Tree {" << std::endl;
    vizTree<KK, VV, CC>(bst._root, out);
    out << "}" << std::endl;
}
