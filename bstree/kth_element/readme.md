# Find k-th element in a bstree

* Time Complexity: O(height)

## 方法简述

为了寻找二叉搜索子树 `root` 中第 k 大的元素（在树高的线性时间内），实际上可以递归为以下过程

1. 如果 `root` 的左子树恰好有 k-1 个元素，结束递归并返回 `root`. （因为在这种情况下，依据二叉搜索树 `root` 就是第 k 大的元素）
2. 如果 k 等于或小于左子树的节点数，将本过程递归到左子树的根：找出左子树中第 k 大的元素.
3. 否则，将本过程递归到右子树：找出右子树第 (k - lnodenum - 1) 大的元素. （特别注意，这里的 k 发生了改变，因为 root 和左子树已经比第 k 大的元素小）

为了在限定的时间复杂度内完成这个任务，只需要在每个节点记录其左子树的节点数量即可（前提是记录了全树的 size）. （这是因为在上述递归中的每一个过程，我们需要的信息有：左、右子树的节点数；左子树节点数由节点本身获得，右节点的数量可以由递归过程的一个参数传递，初始的右节点数量当然可以由全树节点数减去根的左子树节点数得到）。

关键伪代码：

```
nth_subtree(root, n, total_nums) -> T // root: 递归位置；total_nums: 以 root 为根的子树的节点数
{
lnodenum = root->lnodenum_ // 节点本身记录了左子树的大小
rnodenum = total_nums - lnodenum - 1;
if (n == lnodenum + 1) return root->data_; // 递归基，左子树有 n-1 个元素

if (n <= lnodenum) return nth_subtree(root->leftchild_, n, lnodenum);
else return nth_subtree(root->rightchild_, n - (lnodenum + 1), rnodenum);
}
```
