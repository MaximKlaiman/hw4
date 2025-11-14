#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

//Write a recursive (no loops allowed) routine to determine 
//if ALL paths from leaves to root are the same length.

// Helper function: returns the height of the subtree AND
// sets `balanced` to false if any subtree violates equal path lengths.
int getHeight(Node* root, bool& balanced) {
    if (!root) return 0;

    bool leftBalanced = true;
    bool rightBalanced = true;

    int leftH = getHeight(root->left, leftBalanced);
    int rightH = getHeight(root->right, rightBalanced);

    // If either subtree is already unbalanced, bubble it up
    if (!leftBalanced || !rightBalanced) {
        balanced = false;
        return 0;
    }

    // If both children exist, heights must match
    if (root->left && root->right && leftH != rightH) {
        balanced = false;
    }

    // Otherwise, one child may be missing – still fine
    // as long as the existing subtree is balanced.

    return 1 + max(leftH, rightH);
}

bool equalPaths(Node * root)
{
    if (!root) return true;

    bool balanced = true;
    getHeight(root, balanced);
    return balanced;
}

// bool equalPaths(Node * root)
// {
//     // Add your code below
//     if (root == NULL) {
//         return true;
//     }



// }

