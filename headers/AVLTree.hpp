#include "AVLNode.hpp"
/****************************************************************************************/
/*                                 AVLTree Prototype                                    */
/****************************************************************************************/
template <typename ITEM>
class AVLTree : public BinarySearchTree<AVLNode<ITEM>, ITEM> // a class that inheirt from BinarySearchTree
{
private:
	AVLNode <ITEM> * updateHeightAndEvaluateBalance(AVLNode <ITEM> * n);
	void rotateWithLeftChild(AVLNode <ITEM> * p);
	void rotateWithRightChild(AVLNode <ITEM> * p);
	void doubleRotateWithLeftChild(AVLNode <ITEM> * p);
	void doubleRotateWithRightChild(AVLNode <ITEM> * p);
	int min;
	int max;
public:
	AVLTree();
	AVLNode <ITEM> * insert(const ITEM & item);
	bool remove(const ITEM & item);
	void traverse(AVLNode <ITEM> * n = NULL, int depth = 0);
	void printStats();
	~AVLTree();
};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                               AVLTree Implementation                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

template <typename ITEM>  AVLNode <ITEM> * AVLTree<ITEM>::updateHeightAndEvaluateBalance(AVLNode <ITEM> * n) // a method that update the height
{
	if (n == NULL) return NULL;
	n->setHeight(MAX(HEIGHT(n->getLeft()), HEIGHT(n->getRight())) + 1);
	if (!(HEIGHT(n->getLeft()) - HEIGHT(n->getRight()) > 1 || HEIGHT(n->getRight()) - HEIGHT(n->getLeft()) > 1))
		return updateHeightAndEvaluateBalance(n->getParent());
	else return n;
}
template <typename ITEM> void AVLTree<ITEM>::rotateWithLeftChild(AVLNode <ITEM> * p)// a method that will rotate the left subtree of "p"   
{
	//This function need to be written by you.

	if (p->getParent() == NULL) {
		p->getLeft()->setRight(p);
		p->setParent(p->getLeft());
		p->setLeft(NULL);
	}
	else {
		p->getParent()->setRight(p->getLeft());
		p->getLeft()->setParent(p->getParent());
		p->getLeft()->setRight(p);
		p->setParent(p->getLeft());
		p->setLeft(NULL);
	}
	p->setHeight(MAX(HEIGHT(p->getLeft()), HEIGHT(p->getRight()) + 1));

}

template <typename ITEM> void AVLTree<ITEM>::rotateWithRightChild(AVLNode <ITEM> * p)// a method that will rotate the right subtree of "p"  
{
	//This function need to be written by you.

	if (p->getParent() == NULL) {
		p->getRight()->setLeft(p);
		p->setParent(p->getRight());
		p->setRight(NULL);
	}
	else {
		p->getParent()->setRight(p->getRight());
		p->getRight()->setParent(p->getParent());
		p->getRight()->setLeft(p);
		p->setParent(p->getRight());
		p->setRight(NULL);
	}
	p->setHeight(MAX(HEIGHT(p->getLeft()), HEIGHT(p->getRight()) + 1));

}

template <typename ITEM> void AVLTree<ITEM>::doubleRotateWithLeftChild(AVLNode <ITEM> * p)// left rotation of the right subtree of the left child of "p"
{
	//This function need to be written by you.
	if (p->getParent() == NULL) {
		p->getLeft()->getRight()->setLeft(p->getLeft());
		p->getLeft()->setParent(p->getLeft()->getRight());
		p->setRight(p->getLeft()->getRight());
		p->getLeft()->getRight()->setParent(p);
		p->getRight()->getLeft()->setRight(NULL);
	}
	else {
		p->getLeft()->getRight()->setLeft(p->getLeft());
		p->getLeft()->setParent(p->getLeft()->getRight());
		p->setLeft(p->getLeft()->getRight());
		p->getLeft()->getRight()->setParent(p);
		p->getLeft()->setRight(NULL);
	}
	rotateWithLeftChild(p);

}
template <typename ITEM> void AVLTree<ITEM>::doubleRotateWithRightChild(AVLNode <ITEM> * p)// right rotation of the left subtree of the left child of "p"
{
	//This function need to be written by you.

	if (p->getParent() == NULL) {
		p->getRight()->getLeft()->setRight(p->getRight());
		p->getRight()->setParent(p->getRight()->getLeft());
		p->setLeft(p->getRight()->getLeft());
		p->getRight()->getLeft()->setParent(p);
		p->getLeft()->getRight()->setLeft(NULL);
	}
	else {
		p->getRight()->getLeft()->setRight(p->getRight());
		p->getRight()->setParent(p->getRight()->getLeft());
		p->setRight(p->getRight()->getLeft());
		p->getRight()->getLeft()->setParent(p);
		p->getRight()->setLeft(NULL);
	}

	rotateWithRightChild(p);

}

template <typename ITEM> AVLTree<ITEM>::AVLTree() :BinarySearchTree<AVLNode<ITEM>, ITEM>() { min = 1000000; max = 0; }
template <typename ITEM> AVLNode <ITEM> * AVLTree<ITEM>::insert(const ITEM & item)// a method that insert a node into the right place using the method updateHeightAndEvaluateBalance   {
	AVLNode <ITEM> * avlNode = BinarySearchTree<AVLNode<ITEM>, ITEM>::insert(item);

	while (avlNode != NULL)
	{
		AVLNode <ITEM> * balance_node = updateHeightAndEvaluateBalance(avlNode);
		if (balance_node != NULL)
		{
			if (balance_node->getLeft() != NULL)
			{
				if (HEIGHT(balance_node->getLeft()->getLeft()) >= HEIGHT(balance_node->getLeft()->getRight()))
					rotateWithLeftChild(balance_node); // Insertion into the left sub tree of the left child
				else doubleRotateWithLeftChild(balance_node);
			}
			if (balance_node->getRight() != NULL)
			{
				if (HEIGHT(balance_node->getRight()->getRight()) >= HEIGHT(balance_node->getRight()->getLeft()))
					rotateWithRightChild(balance_node);  // Insertion into the right sub tree of the right child
				else doubleRotateWithRightChild(balance_node);
			}
		}
		avlNode = balance_node;
	}
	return avlNode;
}
template <typename ITEM> void AVLTree<ITEM>::traverse(AVLNode <ITEM> * n, int depth)// a method that will traverse untill it reach the depth
{
	if (n == NULL) n = this->root;
	depth++;
	if (ISLEAF(n))
	{
		if (max < depth) max = depth;
		if (min > depth) min = depth;
		return;
	}
	if (n->getLeft() != NULL) traverse(n->getLeft(), depth);
	if (n->getRight() != NULL) traverse(n->getRight(), depth);
}



template <typename ITEM> void AVLTree<ITEM>::printStats()// a method that prints the min and the max values 
{
	cout << "Min:" << min << endl;
	cout << "Max:" << max << endl;

}



template <typename ITEM> bool AVLTree<ITEM>::remove(const ITEM & item)// a mehtod that will remove a node  {
	return BinarySearchTree<AVLNode<ITEM>, ITEM>::remove(item);
}
template <typename ITEM> AVLTree<ITEM>::~AVLTree(){}
