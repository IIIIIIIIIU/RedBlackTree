#include <iostream>

#include "red_black_tree.hpp"

int main() {

    rbt::RedBlackTree<int,int> tree;
    tree.insert(10,10);
    tree.insert(100,100);
    printf("%d\n",tree.query(10));
    printf("%d\n",tree.query(100));
    printf("%d\n",tree.contains(0));
    return 0;
}
