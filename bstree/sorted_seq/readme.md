# return a sorted sequence of elements in a BST.

* Complexity: ~O(num of nodes)

## 方法简述

为了高效输出一棵二叉搜索树的升序元素序列，我们需要充分利用原本左右节点为空的节点。具体来说，过程分两步进行

1. 把右孩子为空的节点的右孩子指针改为该节点中序遍历序列的下一个节点的指针。（为什么是中序遍历：因为只有中序遍历序列满足先小后中最后大，这与二叉搜索树的特征匹配）
2. 利用右孩子的信息输出树的升序元素序列。

需要注意，因为我们修改了某些节点的右孩子（那些没有右孩子的节点），因此我们需要在每个节点增加一个数据 `trueright_` 来区分右孩子是真正的右孩子还是中序遍历序列的下一个节点。在第一个操作之前，所有节点 `trueright_` 值都为 `true`，第一步操作将更新节点的 `trueright_`，这将导致第二步对节点的不同操作。

### 步骤 1

这个过程只需要通过一个简单的中序遍历完成。需要注意：因为我们要把右孩子为空节点的右孩子指针改为中序遍历序列的**下一**节点，我们需要把这**右孩子指针的引用**在递归过程中传递，以便下一个节点能够把这指针引用改为自己（此时自己就是原节点中序遍历的下一个节点）。

关键代码：

```cpp
void update_rnode_subtree(node_ptr root, node_ptr& prev) // prev: 中序遍历序列中前一个节点的引用
{
	if (root == nullptr) return;

	update_rnode_subtree(root->leftchild_, prev);
	if (prev != nullptr && prev->rightchild_ == nullptr) // 如果前一个节点不空且没有右孩子
	{
    // 把前一节点的右孩子指针指向当前节点，也就是中序遍历序列的下一个节点。
		prev->trueright_ = false;
		prev->rightchild_ = root;
	}
	prev = root; // 更新 prev 指针
	update_rnode_subtree(root->rightchild_, prev);
}
```

### 步骤 2

完成了步骤 1，我们开始考虑如何输出升序序列。经过对一些树结构的分析和实验，可以发现下面操作总能完成这个任务：

1. 首先从树根开始沿左侧链下行到最底端（直到没有左孩子的那个节点）。记这个节点为 `p`.
2. 在序列中插入 `p`.
3. 循环地（直到 `p` 右孩子指针为空）做
   - 如果 `p` 有真正的右孩子，则：赋值 `p` 为那真正的右孩子，然后让 `p` 从当前位置沿左侧链下行到底端，在序列中插入 `p`.
   - 否则，赋值 `p` 为那假的右孩子，然后在序列中插入 `p`.

关键代码

```cpp
std::vector<value_type> sorted()
{
	std::vector<value_type> ret; // 将要返回的升序序列
	node_ptr p = root_;
	while (p->leftchild_ != nullptr) p = p->leftchild_; // 沿着左侧链下行
	ret.push_back(p->data_);
	while (p->rightchild_ != nullptr)
	{
		if (p->trueright_)
		{
			p = p->rightchild_;
			while (p->leftchild_ != nullptr) p = p->leftchild_;
			ret.push_back(p->data_);
		}
		else
		{
			p = p->rightchild_;
			ret.push_back(p->data_);
		}
	}
	return ret;
}

```
