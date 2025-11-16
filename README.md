# B Tree Data Structure (C++)

A B Tree Structure built to hold data keys in various nodes based on a MAGNITUDE determined by the data type stored.

## Overview
This program implements two classes and various functions to ensure the tree structure operates well and efficiently.
  - Templated BTree class holds the standard functions and objects of the B Tree.
  - Templated BTreeNode class holds format for tree nodes, including vectors for keys and children.
  - Constructor and destructor that build and delete tree objects, while also determining tree magnitude.
  - FindNode and findKey functions that scan through tree nodes and vectors to find items or insertion points in tree.
  - Insert and remove functions to add and subtract items from tree.
  - Overflow and underflow functions to manage reshaping tree when key vectors become full or too empty.
  - Search function to locate items within the tree.

## Tech Stack
  - Language: C++
  - IDE: Visual Studio


**Compilation instructions are included in comment header of main file.**
