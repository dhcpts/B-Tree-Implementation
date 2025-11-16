/*
@filename: BTree - Driver Main

@author: Doc Holloway
@date: 11/7/2025

@description: This main drive holds the comparison function that is pointed to as a parameter in the template class for the B Tree. 
The main program creates a B-Tree, and inserts enough non-repeated values into the tree to make it overflow three times. After this, it searches
for one value not in the tree, and one in the tree. Lastly, it deletes values from the tree until it shrinks back down to only the root.
The main uses exception handling blocks to ensure that cases of duplicate item insertion, item not found, and other exceptions are handled.

Compilation instructions:

Using Ubuntu 22.04:
	g++ -c BTreeDriverMain.cpp -o main.o
	g++ main.o -o BTreeTest
	./BTreeTest
Using Visual Studio:
	Run local Windows Debugger
*/

#include "BTreeTemplateClass.h"

/*
Compare function used as pointer parameter in tree construction. Function returns -1, 0, or 1 based on the comparison
of two items.

@param[in]: Two TYPE items to be compared.
@return: -1,0, or 1 based on the comparison of the inputs.
*/
template <typename TYPE>
int compare(const TYPE& item1, const TYPE& item2)
{
	if (item1 < item2)
		return -1;
	if (item1 == item2)
		return 0;
	return 1;
}

/*
Main function constructs a B-Tree and uses driver commands to fill the tree until it overflows three times, test for various values, and
lastly shrink the tree back down to only the root via deletions. Various try/catch blocks are present to handle exceptions such as duplicateItem,
itemNotFound, etc.

@param[in]: Nothing. Main creates tree object and manipulates it itself.
@return: Text output into the output window stating which tests passed and what exceptions occurred.
*/
int main(int argc, char* argv[])
{
	BTree<int> testTree(compare);

	srand(static_cast<unsigned>(time(nullptr)));

	vector<int> insertList;

	try
	{
		for (int i = 0; i < 400; i++)
		{
			try
			{
				int randomValue = 1 + (rand() % 800);
				testTree.insert(randomValue);
				insertList.push_back(randomValue);
			}
			catch (DuplicateItemException& e)
			{
				cout << e.toString() << endl;
			}
		}

		if (testTree.nodeCounter() == 5)
		{
			cout << "Passed insertion tests" << endl;
		}
		else
		{
			cout << "Failed: Tree has not properly overflowed" << endl;
		}

		try
		{
			testTree.search(801);
		}
		catch (ItemNotFoundException& e)
		{
			cout << e.toString() << endl;
		}

		int searchResult = testTree.search(insertList.back());
		if (searchResult == insertList.back())
		{
			cout << "Search passed" << endl;
		}

		for (int i = insertList.size(); i > 30; i--)
		{
			try
			{
				testTree.remove(insertList.back());
				insertList.pop_back();
			}
			catch (ItemNotFoundException& e)
			{
				cout << e.toString() << endl;
			}
		}

		if (testTree.nodeCounter() == 1)
		{
			cout << "Removal tests passed" << endl;
		}
		else
		{
			cout << "Failed: Tree not shrunk down to root" << endl;
		}

		cout << "Total Key Count:" << testTree.count() << endl;
	}
	catch (TreeEmptyException& e)
	{
		cout << e.toString() << endl;
	}
	catch (Exception& e)
	{
		cout << e.toString() << endl;
	}
	catch (...)
	{
		cout << "Unknown error" << endl;
	}


	return 0;
}