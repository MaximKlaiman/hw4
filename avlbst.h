#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key,Value>* x);
    void rotateRight(AVLNode<Key,Value>* x);
    void updateBalanceFromChildren(AVLNode<Key,Value>* node);
    void rebalanceAt(AVLNode<Key,Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // Empty tree: create root
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key,Value>(new_item.first, new_item.second, NULL);
        return;
    }

    // Standard BST insert, but with AVLNode*
    AVLNode<Key,Value>* curr = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key,Value>* parent = NULL;

    while(curr != NULL) {
        parent = curr;
        if(new_item.first == curr->getKey()) {
            // Overwrite value, no structural change => no rebalancing needed
            curr->setValue(new_item.second);
            return;
        }
        else if(new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        }
        else {
            curr = curr->getRight();
        }
    }

    AVLNode<Key,Value>* newNode =
        new AVLNode<Key,Value>(new_item.first, new_item.second, parent);

    if(new_item.first < parent->getKey()) parent->setLeft(newNode);
    else parent->setRight(newNode);

    // Rebalance going up from the parent
    AVLNode<Key,Value>* node = parent;
    while(node != NULL) {
        updateBalanceFromChildren(node);
        int8_t bal = node->getBalance();

        if(bal < -1 || bal > 1) {
            // Fix the first unbalanced node and then we're done (AVL property)
            rebalanceAt(node);
            break;
        }
        node = node->getParent();
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // Find node to remove
    Node<Key,Value>* temp = this->internalFind(key);
    if(temp == NULL) return;

    AVLNode<Key,Value>* node = static_cast<AVLNode<Key,Value>*>(temp);

    // If it has 2 children, swap with predecessor first
    if(node->getLeft() != NULL && node->getRight() != NULL) {
        Node<Key,Value>* predNode =
            BinarySearchTree<Key,Value>::predecessor(node);
        AVLNode<Key,Value>* pred = static_cast<AVLNode<Key,Value>*>(predNode);
        nodeSwap(node, pred);
        // Now 'node' has at most one child
    }

    AVLNode<Key,Value>* parent = node->getParent();
    AVLNode<Key,Value>* child =
        (node->getLeft() != NULL) ? node->getLeft() : node->getRight();

    if(child != NULL) {
        child->setParent(parent);
    }

    if(parent == NULL) {
        // Removing the root
        this->root_ = child;
    }
    else if(node == parent->getLeft()) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete node;

    // Rebalance going up from parent
    AVLNode<Key,Value>* cur = parent;
    while(cur != NULL) {
        updateBalanceFromChildren(cur);
        int8_t bal = cur->getBalance();

        if(bal < -1 || bal > 1) {
            AVLNode<Key,Value>* oldParent = cur->getParent();
            rebalanceAt(cur);
            // After fixing at 'cur', continue from its former parent
            cur = oldParent;
        }
        else {
            cur = cur->getParent();
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getRight();
    if(y == NULL) return;

    AVLNode<Key,Value>* p = x->getParent();
    AVLNode<Key,Value>* B = y->getLeft();

    // Link y with x's parent
    y->setParent(p);
    if(p == NULL) {
        this->root_ = y;
    }
    else if(x == p->getLeft()) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }

    // Put x as left child of y
    y->setLeft(x);
    x->setParent(y);

    // B becomes right child of x
    x->setRight(B);
    if(B != NULL) {
        B->setParent(x);
    }

    // Update balances locally
    updateBalanceFromChildren(x);
    updateBalanceFromChildren(y);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* x)
{
    if(x == NULL) return;
    AVLNode<Key,Value>* y = x->getLeft();
    if(y == NULL) return;

    AVLNode<Key,Value>* p = x->getParent();
    AVLNode<Key,Value>* B = y->getRight();

    // Link y with x's parent
    y->setParent(p);
    if(p == NULL) {
        this->root_ = y;
    }
    else if(x == p->getLeft()) {
        p->setLeft(y);
    }
    else {
        p->setRight(y);
    }

    // Put x as right child of y
    y->setRight(x);
    x->setParent(y);

    // B becomes left child of x
    x->setLeft(B);
    if(B != NULL) {
        B->setParent(x);
    }

    // Update balances locally
    updateBalanceFromChildren(x);
    updateBalanceFromChildren(y);
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceFromChildren(AVLNode<Key,Value>* node)
{
    if(node == NULL) return;
    int leftH  = height<Key,Value>(node->getLeft());
    int rightH = height<Key,Value>(node->getRight());
    // Convention: balance = height(right) - height(left)
    node->setBalance(static_cast<int8_t>(rightH - leftH));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceAt(AVLNode<Key,Value>* node)
{
    if(node == NULL) return;

    // Ensure this node's balance is up to date
    updateBalanceFromChildren(node);
    int8_t bal = node->getBalance();

    if(bal == -2) {
        // Left-heavy
        AVLNode<Key,Value>* left = node->getLeft();
        updateBalanceFromChildren(left);

        if(left->getBalance() <= 0) {
            // Left-Left case
            rotateRight(node);
        }
        else {
            // Left-Right case
            rotateLeft(left);
            rotateRight(node);
        }
    }
    else if(bal == 2) {
        // Right-heavy
        AVLNode<Key,Value>* right = node->getRight();
        updateBalanceFromChildren(right);

        if(right->getBalance() >= 0) {
            // Right-Right case
            rotateLeft(node);
        }
        else {
            // Right-Left case
            rotateRight(right);
            rotateLeft(node);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif
