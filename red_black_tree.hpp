#ifndef RED_BLACK_TREE_HPP
#define RED_BLACK_TREE_HPP

#include <memory>

namespace rbt{

    template<class KeyType,class ValueType>
    class RedBlackTree{

        class Node:public std::enable_shared_from_this<Node>{

            using NodePtr=std::shared_ptr<Node>;
            using ParentPtr=std::weak_ptr<Node>;

        public:


            const KeyType key;
            ValueType value;

            bool color;

            NodePtr leftChild,rightChild;
            ParentPtr parent;

            void swap(NodePtr other){
                leftChild->swap(other->leftChild);
                rightChild->swap(other->rightChild);
                parent->swap(other->parent);
            }

            Node(){

            }

            Node(const KeyType &key,ValueType &value,bool color)
            :key(key),
            value(value),
            color(color),
            parent(),
            leftChild(),
            rightChild(){
            }

            NodePtr getThis(){
                return Node::shared_from_this();
            }

            bool isLeft(){
                auto temp=getThis();
                return temp==temp->parent.lock()->leftChild;
            }

            void rotate(std::shared_ptr<Node> &rootTemp){
                auto now=getThis();
                if(now->parent.expired()){
                    return;
                }

                auto parentTemp=now->parent.lock();

                if(parentTemp->parent.expired()){
                    now->parent=parentTemp->parent;
                }
                else{
                    rootTemp=now;
                }

                if(now==now->parent.lock()->leftChild){
                    parentTemp->leftChild=now->rightChild;
                    now->rightChild=parentTemp;
                }
                else{
                    parentTemp->rightChild=now->leftChild;
                    now->leftChild=parentTemp;
                }

                parentTemp->parent=now;
            }

            std::shared_ptr<Node> getBroNode(){
                if(isLeft()){
                    return parent->rightChild;
                }
                else{
                    return parent->leftChild;
                }
            }

            void changeColor(){
                color^=true;
            }

            bool getColor(){
                return color;
            }

            bool isRed(){
                return !color;
            }

            void setColorBlack(){
                color=true;
            }

        };

        std::shared_ptr<Node> root;

        bool checkSharedPtrNull(std::shared_ptr<Node> &ptr){
            return ptr== nullptr;
        }

        bool checkWeakPtrNull(std::weak_ptr<Node> &ptr){
            return ptr.expired();
        }

        size_t treeSize;

        void insertFix(std::shared_ptr<Node> now){
            while(!checkSharedPtrNull(now) &&
                !checkWeakPtrNull(now->parent)&&
                now->parent.lock()->isRed()){
                auto parent=now->parent.lock();
                if(parent->isLeft()){
                    auto uncle=parent->parent.lock()->rightChild;
                    if(uncle&&uncle->isRed()){

                        parent->changeColor();
                        uncle->changeColor();
                        parent->parent.lock()->changeColor();

                        now=parent->parent.lock();
                    }
                    else{
                        if(!now->isLeft()){
                            now->rotate(root);
                            now.swap(parent);
                        }
                        parent->parent.lock()->changeColor();
                        parent->changeColor();
                        parent->rotate(root);
                    }
                }
                else{
                    auto uncle=parent->parent.lock()->leftChild;
                    if(uncle&&uncle->isRed()){
                        parent->changeColor();
                        uncle->changeColor();
                        parent->parent.lock()->changeColor();

                        now=parent->parent.lock();
                    }
                    else{
                        if(now->isLeft()){
                            now->rotate(root);
                            now.swap(parent);
                        }
                        parent->parent.lock()->changeColor();
                        parent->changeColor();
                        parent->rotate(root);
                    }
                }
            }
            root->setColorBlack();
        }

        void insertNode(const KeyType &key,ValueType &value){
            std::shared_ptr<Node> last;
            auto now=root;
            while(true){
                if(!now){

                    ++treeSize;

                    auto temp=std::make_shared<Node>(key,value,false);
                    temp->parent=last;
                    if(key<last->key){
                        last->leftChild=temp;
                    }else{
                        last->rightChild=temp;
                    }

                    insertFix(temp);

                    return;
                }

                last=now;

                if(key<now->key){
                    now=now->leftChild;
                }
                else if(now->key<key){
                    now=now->rightChild;
                }
                else{
                    now->value=value;
                    return;
                }
            }
        }

        ValueType queryNode(const KeyType &key){
            auto now=root;
            while(true){
                if(!now){
                    ValueType tempResult;
                    return tempResult;
                }
                if(key<now->key){
                    now=now->leftChild;
                }
                else if(now->key<key){
                    now=now->rightChild;
                }
                else{
                    return now->value;
                }
            }
        }

        bool containsKey(const KeyType &key){
            auto now=root;
            while(true){
                if(!now){
                    return false;
                }
                if(key<now->key){
                    now=now->leftChild;
                }
                else if(now->key<key){
                    now=now->rightChild;
                }
                else{
                    return true;
                }
            }
        }

        auto findNode(const KeyType &key)-> decltype(root){
            auto now=root;
            while(true){
                if(!now){
                    return std::make_shared<Node>(nullptr);
                }
                if(key<now->key){
                    now=now->leftChild;
                }
                else if(now->key<key){
                    now=now->rightChild;
                }
                else{
                    return std::make_shared<Node>(now);
                }
            }
        }

        auto findPreNode(std::shared_ptr<Node> node)->decltype(root){
            if(!node->leftChild){

                if(!node->parent){
                    return node;
                }

                auto temp=node;
                while(temp->parent){
                    if(!temp->isLeft()){
                        return temp->parent;
                    }
                }

                return node;
            }
            node=node->leftChild;
            while(node->rightChild){
                node=node->rightChild;
            }
            return node;
        }

        auto findSufNode(std::shared_ptr<Node> node)-> decltype(root){
            if(!node->rightChild){
                if(!node->parent){
                    return node;
                }

                auto temp=node;
                while(temp->parent){
                    if(temp->isLeft()){
                        return temp->parent;
                    }
                }

                return node;
            }

            node=node->rightChild;
            while(node->leftChild){
                node=node->leftChild;
            }

            return node;
        }

        void fixAfterRemove(std::shared_ptr<Node> node){
            while(node->parent&&!node->isRed()){
                if(node->isLeft()){
                    auto broNode=node->getBroNode();
                    if(broNode->isRed()){
                        broNode->rotate();
                        broNode->changeColor();
                        node->parent->changeColor();
                    }
                    else if((!broNode->leftChild||!broNode->leftChild->isRed())
                    &&(!broNode->rightChild||!broNode->rightChild->isRed())){
                        broNode->changeColor();
                        node=node->parent;
                    }
                    else{
                        if(broNode->leftChild&&broNode->leftChild->isRed()&&
                                (!broNode->rightChild||!broNode->rightChild->isRed())){
                            broNode->leftChild->changeColor();
                            broNode->changeColor();
                            broNode->leftChild->rotate();
                            broNode=node->parent->rightChild;
                        }
                        if(node->parent->isRed()){
                            broNode->changeColor();
                        }
                        node->parent->setColorBlack();
                        broNode->rightChild->setColorBlack();
                        broNode->rotate();

                        node=root;
                    }
                }
                else{
                    auto broNode=node->getBroNode();
                    if(broNode->isRed()){
                        broNode->rotate();
                        broNode->changeColor();
                        node->parent->changeColor();
                    }
                    else if((!broNode->leftChild||!broNode->leftChild->isRed())
                            &&(!broNode->rightChild||!broNode->rightChild->isRed())){
                        broNode->changeColor();
                        node=node->parent;
                    }
                    else{
                        if(broNode->rightChild&&broNode->rightChild->isRed()&&
                           (!broNode->leftChild||!broNode->leftChild->isRed())){
                            broNode->rightChild->changeColor();
                            broNode->changeColor();
                            broNode->rightChild->rotate();
                            broNode=node->parent->leftChild;
                        }
                        if(node->parent->isRed()){
                            broNode->changeColor();
                        }
                        node->parent->setColorBlack();
                        broNode->leftChild->setColorBlack();
                        broNode->rotate();

                        node=root;
                    }
                }
            }

            node->setColorBlack();
        }

        void removeNode(const KeyType &key){
            auto node= findNode(key);
            if(!node){
                return;
            }

            --treeSize;

            if(node->leftChild&&node->rightChild){
                node->swap(findPreNode(node));
            }

            if(node==root&&!node->leftChild&&!node->rightChild){
                root= nullptr;
                return;
            }

            auto temp=node->leftChild;
            if(!temp){
                temp=node->rightChild;
            }
            if(!temp){
                if(!node->isRed()){
                    fixAfterRemove(node);
                }

                if(node->isLeft()){
                    node->parent->leftChild= nullptr;
                }
                else{
                    node->parent->rightChild= nullptr;
                }
            }else{
                if(node->parent== nullptr){
                    root=temp;
                }
                else{
                    if(node->isLeft()){
                        node->parent->leftChild=temp;
                    }
                    else{
                        node->parent->rightChild=temp;
                    }

                    if(!node->isRed()){
                        fixAfterRemove(temp);
                    }
                }
            }
        }

    public:

        RedBlackTree():
        treeSize(0),
        root(){
            root= nullptr;
        }

        void insert(const KeyType &key,ValueType value){
            if(!root){
                root=std::make_shared<Node>(key,value,true);
                return;
            }
            else{
                insertNode(key,value);
            }
        }

        ValueType query(const KeyType &key){
            return queryNode(key);
        }

        bool contains(const KeyType &key){
            return containsKey(key);
        }

        size_t size(){
            return treeSize;
        }

        void remove(const KeyType &key){
            removeNode(key);
        }

        void clear(){
            root= nullptr;
        }

    };
}

#endif