/*
@filename: BTree - Template Class Header File

@author: Doc Holloway
@date: 11/7/2025

@description: This header file contains the entire implementation of the B-Tree in a template class, as well as the classes needed for 
exception handling. Using a node class inside of the tree class, the implementation allows full access and modification of the B-Tree, while
accomadating conditions such as overflow and underflow.

Compilation instructions:

Using Ubuntu 22.04:
    g++ -c BTreeDriverMain.cpp -o main.o
    g++ main.o -o BTreeTest
    ./BTreeTest
Using Visual Studio:
    Run local Windows Debugger
*/
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <random>
#include <exception>
#include <ctime>

using namespace std;
using std::string;
using std::stringstream;


extern int MAGNITUDE = 0;

/*
General exception class serves as template for specific exceptions that may occur within the program. Holds protected variables for exception info and
the output string for an exception.

@param[in]: Error message and number from throw line later in program.
@return: Text output stating the error that has occurred.
*/
class Exception
{
protected:
    string message;
    int errorNumber;
public:
    Exception(int errNo, string msg) : errorNumber(errNo), message(msg) {}
    virtual string toString()
    {
        stringstream sstream;
        sstream << "Exception: " << errorNumber << " ERROR: " << message;
        return sstream.str();
    }
};
/*
Duplicate item error class inherits from the exception class and identifies the exception performed was that an insertion failed due to an item already
existing.

@param[in]: Inherited info on error from Exception class.
@return: Text output stating duplicateitem error has occurred.
*/
class DuplicateItemException : public Exception
{
public:
    DuplicateItemException(int eNo, string msg) : Exception(eNo, msg) {}
    string toString()
    {
        stringstream sstream;
        sstream << "DuplicatItemException: " << errorNumber << " ERROR: " << message;
        return sstream.str();
    }
};
/*
Empty tree exception class inherits from exception class and reports an error occurred due to the tree being empty.

@param[in]: Inherited info on error from Exception class.
@return: Text output stating empty tree error has occurred.
*/
class TreeEmptyException : public Exception
{
public:
    TreeEmptyException(int eNo, string msg) : Exception(eNo, msg) {}
    string toString()
    {
        stringstream sstream;
        sstream << "TreeEmptyException: " << errorNumber << " ERROR: " << message;
        return sstream.str();
    }
};
/*
Item not found exception inherits from the general exception class and reports an error for a failed search/deletion stating the item was not found in the tree.

@param[in]: Inherited info on error from Exception class.
@return: Text output stating no item found error has occurred.
*/
class ItemNotFoundException : public Exception
{
public:
    ItemNotFoundException(int eNo, string msg) : Exception(eNo, msg) {}
    string toString()
    {
        stringstream sstream;

        sstream << "ItemNotFoundException: " << errorNumber << " ERROR: " << message;
        return sstream.str();
    }
};

/*
Massive B-Tree class holds all of the functions needed to manipulate and access the tree. Uses a constructor to set up the B-Tree object, 
and holds functions tied to insertion, deletion, search, count, and more. All functions are public besides findNode, which is used by
functions to locate a desired node to modify. Holds a node class inside of itself to define and construct nodes as needed.

@param[in]: A pointer to the comparison function located in main.
@return: A B-Tree structure that is accessible and modifiable by its functions.
*/
template <typename DATA_TYPE>
class BTree
{
    /*
    B-Tree node class holds the structure needed to assemble a B-Tree node, including vectors for keys, children, and a pointer to the parent.
    Also contains a helper function for finding the insertion point in a keyVector.

    @param[in]: Nothing. Constructor creates a default condition for the node with empty vectors and nullptr parent.
    @return: A B-Tree node that can be dereferenced and searched through via findKey.
    */
    class BTreeNode
    {
    public:
        vector<DATA_TYPE> keyVector;
        vector<BTreeNode*> childrenVector;
        BTreeNode* parent; 

        BTreeNode()
        {
            parent = nullptr;
        }
        /*
        findKey searches for an item, or a slot in the keyVector where item should be inserted. If a matching item is found, it returns
        -1. Otherwise, it returns the index where a new key should be inserted into the keyVector.

        @param[in]: An item of type DATA_TYPE to search for.
        @return: -1 if a matching item is found, or the insertion index if not found.
        */
        int findKey(const DATA_TYPE& item)
        {
            int keyIndex = 0;
            int maxKeys = keyVector.size();


            for (keyIndex; keyIndex < maxKeys; keyIndex++)
            {
                if (keyVector[keyIndex] == item)
                {
                    return -1;
                }
                else if (keyVector[keyIndex] > item)
                {
                        return keyIndex;
                }
                else if (keyIndex == keyVector.size() - 1)
                {
                        return keyVector.size();
                }
            }
        }
    };

    BTreeNode* root;
    int nodeCount;
    int totalKeyCount;
    //Pointer definition for compare function in main.
    int (*compare)(const DATA_TYPE& item1, const DATA_TYPE& item2);

    BTreeNode* findNode(BTreeNode* startNode, const DATA_TYPE& item);

    /*
    Post order delete is used by destructor to travel down to leaves, and slowly delete all the nodes in the tree from the bottom up,
    and then delete the called node at the end. For the destructor, this will be the root.

    @param[in]: A BtreeNode to delete, along with all it's children (Always the root).
    @return: Nothing. Purges the tree of all items.
    */
    void postOrderDelete(BTreeNode* node)
    {
        if (!node)
            return;

        for (int i = 0; i < node->childrenVector.size(); i++)
        {
            if (node->childrenVector[i] != nullptr)
            {
                postOrderDelete(node->childrenVector[i]);
            }
        }
        delete node;
    }

public:
    BTree(int (*cmp)(const DATA_TYPE& item1, const DATA_TYPE& item2));
    ~BTree();

    void insert(const DATA_TYPE& item);
    void resolveOverflow(BTreeNode* overNode);
    void remove(const DATA_TYPE& item);
    void resolveUnderflow(BTreeNode* underNode);
    void leftBorrow(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex);
    void rightBorrow(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex);
    void leftMerge(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex);
    void rightMerge(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex);
    void underflowAddresses(BTreeNode* sibling, BTreeNode* underNode, int resolveCase);
    int search(const DATA_TYPE& item);

    //Count function takes no parameter, and only returns the total amount of keys in the tree.
    int count()
    {
        return totalKeyCount;
    }

    //NodeCounter function takes no parameter, and returns number of nodes in tree. Used for driver testing.
    int nodeCounter()
    {
        return nodeCount;
    }
};

/*
BTree constructor takes compare function pointer as parameter, and creates B-Tree object with empty root, and empty tree conditions.
It also determines the magnitude based on the size of the data type being inserted.

@param[in]: Pointer to the compare function in main.
@return: An empty B-Tree object.
*/
template <typename DATA_TYPE>
BTree<DATA_TYPE>::BTree(int (*cmp)(const DATA_TYPE& item1, const DATA_TYPE& item2))
{
    compare = cmp;
    MAGNITUDE = 512/sizeof(DATA_TYPE);
    nodeCount = 0;
    totalKeyCount = 0;
    root = nullptr;
}

/*
BTree destructor calls on postOrderDelete at the root to clean up all allocated memory in the tree, destroying it.

@param[in]: Nothing.
@return: An empty B-Tree object with all nodes deleted.
*/
template <typename DATA_TYPE>
BTree<DATA_TYPE>::~BTree()
{
    postOrderDelete(root);
}

/*
Insert function first checks for empty tree conditions, and creates a root if necessary. Otherwise, it uses findNode and findKey to locate
the point of insertion for the new key, throwing an exception if the item already exists. It then inserts the key, and checks if an
overflow has occurred.

@param[in]: An item to be inserted into the tree.
@return: The B-Tree with the new key inserted.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::insert(const DATA_TYPE& item)
{
    if (nodeCount == 0)
    {
        root = new BTreeNode();
        root->keyVector.push_back(item);
        nodeCount++;
        totalKeyCount++;
        return;
    }

    BTreeNode* insertNode = findNode(root, item);

    int checkDuplicate = insertNode->findKey(item);
    if (checkDuplicate == -1)
    {
        DuplicateItemException exception(__LINE__, "Duplicate item detected. Unable to insert");
        throw exception;
    }

    insertNode->keyVector.insert(insertNode->keyVector.begin() + checkDuplicate, item);
    
    if(insertNode->keyVector.size() > MAGNITUDE - 1)
    {
        resolveOverflow(insertNode);
    }

    totalKeyCount++;
}

/*
Resolve overflow function handles an overflow after an insertion by splitting the overflowed node with a newly created sibling, and moving
one key up to the parent, if it exists. If there is no parent, it moves a key up to a newly created root. The function also calls recursively
if the parent overflows.

@param[in]: A node that has overflowed with keys.
@return: The B-Tree with 1-2 new nodes based on the overflow cased, with all nodes no longer full.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::resolveOverflow(BTreeNode* overNode)
{
    BTreeNode* sibling = new BTreeNode();

    if (overNode->parent == nullptr)
    {
        BTreeNode* parent = new BTreeNode();
        overNode->parent = parent;
        parent->childrenVector.push_back(overNode);
        sibling->parent = parent;
        parent->childrenVector.push_back(sibling);
        root = parent;

        int keyMidpoint = overNode->keyVector.size() / 2;
        if (overNode->keyVector.size() % 2 == 0) { keyMidpoint--; }
        
        sibling->keyVector.assign(overNode->keyVector.begin() + (keyMidpoint + 1), overNode->keyVector.end());
        overNode->keyVector.erase(overNode->keyVector.begin() + (keyMidpoint + 1), overNode->keyVector.end());


        parent->keyVector.push_back(overNode->keyVector[keyMidpoint]);
        overNode->keyVector.pop_back();

        if (!overNode->childrenVector.empty())
        {
            sibling->childrenVector.assign(overNode->childrenVector.begin() + (keyMidpoint + 1), overNode->childrenVector.end());
            overNode->childrenVector.erase(overNode->childrenVector.begin() + (keyMidpoint + 1), overNode->childrenVector.end());

            for (int i = 0; i < sibling->childrenVector.size(); i++)
            {
                BTreeNode* child = sibling->childrenVector[i];
                if (child != nullptr)
                {
                    child->parent = sibling;
                }
            }
        }

        nodeCount += 2;
    }
    else
    {
        BTreeNode* parent = overNode->parent;
        int nodeIndex = 0;
        for (int i = 0; parent->childrenVector[i] != overNode; i++)
        {
            nodeIndex = i + 1;
        }
        parent->childrenVector.insert(parent->childrenVector.begin() + (nodeIndex + 1), sibling);
        sibling->parent = parent;

        int keyMidpoint = overNode->keyVector.size() / 2;
        if (overNode->keyVector.size() % 2 == 0) { keyMidpoint--; }

        sibling->keyVector.assign(overNode->keyVector.begin() + (keyMidpoint + 1), overNode->keyVector.end());
        overNode->keyVector.erase(overNode->keyVector.begin() + (keyMidpoint + 1), overNode->keyVector.end());


        parent->keyVector.insert(parent->keyVector.begin() + nodeIndex, overNode->keyVector[keyMidpoint]);
        overNode->keyVector.pop_back();

        if (!overNode->childrenVector.empty())
        {
            sibling->childrenVector.assign(overNode->childrenVector.begin() + (keyMidpoint + 1), overNode->childrenVector.end());
            overNode->childrenVector.erase(overNode->childrenVector.begin() + (keyMidpoint + 1), overNode->childrenVector.end());

            for (int i = 0; i < sibling->childrenVector.size(); i++)
            {
                BTreeNode* child = sibling->childrenVector[i];
                if (child != nullptr)
                {
                    child->parent = sibling;
                }
            }
        }

        nodeCount += 1;

        if (parent->keyVector.size() > MAGNITUDE - 1)
        {
            resolveOverflow(parent);
        }
    }
}

/*
Remove function first checks to make sure the tree isn't empty, then uses findNode and findKey to locate the deleted key. If the key does
not exist, an exception will be thrown. It then finds the index in the vector to delete from, and follows either a leaf deletion or a delete
by copy algorithm depending on the location of the node being deleted from. In both cases, a leaf node is checked for underflow, and
resolveUnderflow is called if underflow has occurred.

@param[in]: An item to be deleted from the tree.
@return: The B-Tree without the deleted key.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::remove(const DATA_TYPE& item)
{
    if (nodeCount == 0 || root->keyVector.size() == 0)
    {
        TreeEmptyException exception(__LINE__, "Tree is Empty. Unable to delete");
        throw exception;
    }

    BTreeNode* deleteNode = findNode(root, item);

    if (deleteNode->findKey(item) != -1)
    {
        ItemNotFoundException exception(__LINE__, "Item to be deleted not found");
        throw exception;
    }

    int deleteIndex = 0;

    for (int i = 0; i < deleteNode->keyVector.size(); i++)
    {
        deleteIndex = i;

        if (deleteNode->keyVector[i] == item)
        {
            break;
        }
    }

    if (deleteNode->childrenVector.empty())
    {
        deleteNode->keyVector.erase(deleteNode->keyVector.begin() + deleteIndex);

        if (deleteNode->keyVector.size() < (MAGNITUDE - 1) / 2)
        {
            resolveUnderflow(deleteNode);
        }
    }
    else
    {
        BTreeNode* predecessorNode = deleteNode->childrenVector[deleteIndex];

        while (!predecessorNode->childrenVector.empty())
        {
            predecessorNode = predecessorNode->childrenVector.back();
        }

        DATA_TYPE pred = predecessorNode->keyVector.back();
        predecessorNode->keyVector.pop_back();
        deleteNode->keyVector.erase(deleteNode->keyVector.begin() + deleteIndex);
        deleteNode->keyVector.insert(deleteNode->keyVector.begin() + deleteIndex, pred);

        if (predecessorNode->keyVector.size() < (MAGNITUDE - 1) / 2) 
        { 
            resolveUnderflow(predecessorNode); 
        }
    }

    totalKeyCount--;
}

/*
Resolve underflow function resolves underflow conditions. First, it identifies which child the underflowed node is to its parent, and then
follows a series of prioritized conditionals to determine if a borrow or a merge is necessary from the left or right sibling.

@param[in]: A node that has underflow.
@return: A restructured B-Tree based on the condition deduced by the conditionals.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::resolveUnderflow(BTreeNode* underNode)
{
    if (nodeCount == 1)
    {
        return;
    }

    BTreeNode* parent = underNode->parent;
    BTreeNode* leftSibling;
    BTreeNode* rightSibling;

    int underIndex = 0;

    for (int i = 0; i < parent->childrenVector.size(); i++)
    {
        underIndex = i;
        if (parent->childrenVector[i] == underNode)
        {
            break;
        }
    }

    if (underIndex != 0)
    {
        leftSibling = parent->childrenVector[underIndex - 1];
        if(leftSibling->keyVector.size() > (MAGNITUDE - 1) / 2)
        {
            leftBorrow(parent, leftSibling, underNode, underIndex);
            return;
        }
        else if (underNode == parent->childrenVector.back())
        {
            if (leftSibling->keyVector.size() == (MAGNITUDE - 1) / 2)
            {
                leftMerge(parent, leftSibling, underNode, underIndex);

                return;
            }
        }
        else
        {
            rightSibling = parent->childrenVector[underIndex + 1];
            if (rightSibling->keyVector.size() > (MAGNITUDE - 1) / 2)
            {
                rightBorrow(parent, rightSibling, underNode, underIndex);
                return;
            }
            else if (leftSibling->keyVector.size() == (MAGNITUDE - 1) / 2)
            {
                leftMerge(parent, leftSibling, underNode, underIndex);

                return;
            }
            else if (rightSibling->keyVector.size() == (MAGNITUDE - 1) / 2)
            {
                rightMerge(parent, rightSibling, underNode, underIndex);

                return;
            }
        }
    }
    else
    {
        rightSibling = parent->childrenVector[1];
        if (rightSibling->keyVector.size() > (MAGNITUDE - 1) / 2)
        {
            rightBorrow(parent, rightSibling, underNode, underIndex);
            return;
        }
        else if (rightSibling->keyVector.size() == (MAGNITUDE - 1) / 2)
        {
            rightMerge(parent, rightSibling, underNode, underIndex);
            return;
        }
    }
}

/*
Left borrow carries out the algorithm for resolving underflow by borrowing from the left sibling. It first pulls the separator between
sibling and the afflicted node down to the node, then pulls the last value of the sibling up to the parent. It also resolves addresses if
necessary.

@param[in]: Parent, leftsibling, and underflowed node as well as the childindex of the underflowed node in parent's vector.
@return: The B-Tree modified by the restructuring of the underflow condition.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::leftBorrow(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex)
{
    DATA_TYPE temp1 = sibling->keyVector.back();
    DATA_TYPE temp2 = parent->keyVector[underIndex - 1];
    underNode->keyVector.insert(underNode->keyVector.begin(), temp2);
    parent->keyVector.at(underIndex - 1) = temp1;
    sibling->keyVector.pop_back();
    if (!underNode->childrenVector.empty())
    {
        underflowAddresses(sibling, underNode, 1);
    }
}

/*
Right borrow carries out the algorithm for resolving underflow by borrowing from the right sibling. It first pulls the separator between
sibling and the afflicted node down to the node, then pulls the first value of the sibling up to the parent. It also resolves addresses if
necessary.

@param[in]: Parent, rightsibling, and underflowed node as well as the childindex of the underflowed node in parent's vector.
@return: The B-Tree modified by the restructuring of the underflow condition.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::rightBorrow(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex)
{
    DATA_TYPE temp1 = sibling->keyVector.front();
    DATA_TYPE temp2 = parent->keyVector[underIndex];
    underNode->keyVector.push_back(temp2);
    parent->keyVector.at(underIndex) = temp1;
    sibling->keyVector.erase(sibling->keyVector.begin());
    if (!underNode->childrenVector.empty())
    {
        underflowAddresses(sibling, underNode, 3);
    }
}

/*
Left merge carries out the algorithm for resolving underflow by merging with the left sibling. It first pulls the separator between
sibling and the afflicted node down to the sibling, then appends the leftSibling with all the keys, and children if needed, of the underflowed
node. It then checks to see if the root needs to be reset, or if a recursive call is needed for an underflowed parent.

@param[in]: Parent, leftsibling, and underflowed node as well as the childindex of the underflowed node in parent's vector.
@return: The B-Tree modified by the restructuring of the underflow condition.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::leftMerge(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex)
{
    DATA_TYPE temp1 = parent->keyVector[underIndex - 1];
    sibling->keyVector.push_back(temp1);
    parent->keyVector.erase(parent->keyVector.begin() + (underIndex - 1));
    sibling->keyVector.insert(sibling->keyVector.end(), underNode->keyVector.begin(), underNode->keyVector.end());
    parent->childrenVector.erase(parent->childrenVector.begin() + underIndex);

    if (!underNode->childrenVector.empty())
    {
        underflowAddresses(sibling, underNode, 2);
    }

    delete underNode;
    nodeCount--;

    if (parent == root)
    {
        if (parent->keyVector.size() == 0)
        {
            root = sibling;
            delete parent;
            root->parent = nullptr;
            nodeCount--;
        }
    }
    else
    {
        if (parent->keyVector.size() < (MAGNITUDE - 1) / 2)
        {
            resolveUnderflow(parent);
        }
    }
}

/*
Right merge carries out the algorithm for resolving underflow by merging with the right sibling. It first pulls the separator between
sibling and the afflicted node down to the sibling, then appends the rightSibling's beginning with all the keys, and children if needed, of the underflowed
node. It then checks to see if the root needs to be reset, or if a recursive call is needed for an underflowed parent.

@param[in]: Parent, rightSibling, and underflowed node as well as the childindex of the underflowed node in parent's vector.
@return: The B-Tree modified by the restructuring of the underflow condition.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::rightMerge(BTreeNode* parent, BTreeNode* sibling, BTreeNode* underNode, int underIndex)
{
    DATA_TYPE temp1 = parent->keyVector[underIndex];
    sibling->keyVector.insert(sibling->keyVector.begin(), temp1);
    parent->keyVector.erase(parent->keyVector.begin() + (underIndex));
    sibling->keyVector.insert(sibling->keyVector.begin(), underNode->keyVector.begin(), underNode->keyVector.end());
    parent->childrenVector.erase(parent->childrenVector.begin() + underIndex);

    if (!underNode->childrenVector.empty())
    {
        underflowAddresses(sibling, underNode, 4);
    }

    delete underNode;
    nodeCount--;

    if (parent == root)
    {
        if (parent->keyVector.size() == 0)
        {
            root = sibling;
            delete parent;
            root->parent = nullptr;
            nodeCount--;
        }
    }
    else
    {
        if (parent->keyVector.size() < (MAGNITUDE - 1) / 2)
        {
            resolveUnderflow(parent);
        }
    }
}

/*
Underflow addresses fixes any addresses of nodes called into resolveUnderflow, and follows the same process as the keys based
on the case determined in resolveOverflow. A switch statement is used to enumerate the cases.

@param[in]: The sibling node, underflowed node, and the case of the underflow.
@return: The nodes restructured in their children addresses as needed for the underflow case.
*/
template <typename DATA_TYPE>
void BTree<DATA_TYPE>::underflowAddresses(BTreeNode* sibling, BTreeNode* underNode, int resolveCase)
{
    BTreeNode* child;
    switch (resolveCase)
    {
        case 1:
            child = sibling->childrenVector.back();
            underNode->childrenVector.insert(underNode->childrenVector.begin(), sibling->childrenVector.back());
            sibling->childrenVector.pop_back();
            child->parent = underNode;
            break;
        case 2:
            for (int i = 0; i < underNode->childrenVector.size(); i++)
            {
                child = underNode->childrenVector[i];
                child->parent = sibling;
            }
            sibling->childrenVector.insert(sibling->childrenVector.end(), underNode->childrenVector.begin(), underNode->childrenVector.end());
            underNode->childrenVector.erase(underNode->childrenVector.begin(), underNode->childrenVector.end());
            break;
        case 3:
            child = sibling->childrenVector.front();
            underNode->childrenVector.push_back(sibling->childrenVector.front());
            sibling->childrenVector.erase(sibling->childrenVector.begin());
            child->parent = underNode;
            break;
        case 4:
            for (int i = 0; i < underNode->childrenVector.size(); i++)
            {
                child = underNode->childrenVector[i];
                child->parent = sibling;
            }
            sibling->childrenVector.insert(sibling->childrenVector.begin(), underNode->childrenVector.begin(), underNode->childrenVector.end());
            underNode->childrenVector.erase(underNode->childrenVector.begin(), underNode->childrenVector.end());
            break;
        default:
            break;
    }
}

/*
Search uses findNode to locate a node where an item should be, and then loops across that node's vector until it hits the end
or finds the searched item. If not found, and exception is thrown

@param[in]: An item to be searched for.
@return: The item searched for.
*/
template <typename DATA_TYPE>
int BTree<DATA_TYPE>::search(const DATA_TYPE& item)
{
    BTreeNode* searchNode = findNode(root, item);
    for (int i = 0; i < searchNode->keyVector.size(); i++)
    {
        if (searchNode->keyVector[i] == item)
        {
            return searchNode->keyVector[i];
        }
    }

    throw ItemNotFoundException(__LINE__, "Item was not found");
}

/*
FindNode navigates through the B-Tree, and finds the node where an operation should happen. If the inputted node is a leaf,
or contains the item parameter, it returns that node. Otherwise, it uses checks to travel down child pointers if they exist
further down the tree, but also returns the current node if those pointers don't exist.

@param[in]: The startNode to search at, usually root, and an item to compare with items in node keyVectors.
@return: The node where a B-Tree operation should occur.
*/
template <typename DATA_TYPE>
typename BTree<DATA_TYPE>::BTreeNode* BTree<DATA_TYPE>::findNode(BTreeNode* startNode, const DATA_TYPE& item)
{
    int i = 0;

    if (startNode->childrenVector.empty())
    {
        return startNode;
    }

    while (i < startNode->keyVector.size())
    {
        if (startNode->keyVector[i] == item)
        {
            return startNode;
        }
        else if (startNode->keyVector[i] > item)
        {
            if (startNode->childrenVector[i] != nullptr)
            {
                BTreeNode* returnNode = findNode(startNode->childrenVector[i], item);
                return returnNode;
            }
            else
            {
                return startNode;
            }
        }
        i++;
    }
    if (startNode->keyVector.back() < item)
    {
        if (startNode->childrenVector.back() != nullptr)
        {
            BTreeNode* returnNode = findNode(startNode->childrenVector.back(), item);
            return returnNode;
        }
        else
        {
            return startNode;
        }
    }
}