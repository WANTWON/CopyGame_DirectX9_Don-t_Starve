#include "stdafx.h"
#include "..\Public\Behavior_Tree.h"

/*class Selector : public CompositeNode {
public:
virtual bool run() override {
for (Node* child : getChildren()) {  // The generic Selector implementation
if (child->run())  // If one child succeeds, the entire operation run() succeeds.  Failure only results if all children fail.
return true;
}
return false;  // All children failed so the entire run() operation fails.
}
};

class RandomSelector : public CompositeNode {  // RandomSelector operates as a Selector, but in a random order instead of from first child to last child.
public:
virtual bool run() override {
for (Node* child : childrenShuffled()) {  // The order is shuffled
if (child->run())
return true;
}
return false;
}
};

class Sequence : public CompositeNode {
public:
virtual bool run() override {
for (Node* child : getChildren()) {  // The generic Sequence implementation.
if (!child->run())  // If one child fails, then enter operation run() fails.  Success only results if all children succeed.
return false;
}
return true;  // All children suceeded, so the entire run() operation succeeds.
}
};

class Root : public Node {
private:
Node* child;
friend class BehaviourTree;
void setChild (Node* newChild) {child = newChild;}
virtual bool run() override {return child->run();}
};
private:
Root* root;
public:
BehaviourTree() : root(new Root) {}
void setRootChild (Node* rootChild) const {root->setChild (rootChild);}
bool run() const {return root->run();}*/