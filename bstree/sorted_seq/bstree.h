#pragma once
#include <initializer_list>
#include <utility>
#include <type_traits>
#include <iterator>
#include <vector> // this is for some constructor and test function.
#include <algorithm>

namespace DSAC
{

	template <typename T>
	class bstree
	{
	public:
		using value_type = T;
		using reference = T&;
		using const_reference = const T&;
		using size_type = unsigned long int;

		struct node
		{
			value_type data_;
			node* leftchild_;
			node* rightchild_;
			bool trueright_;
			node(bool trueright = true, node* leftchild = nullptr, node* rightchild = nullptr)
				: data_(value_type()), leftchild_(leftchild), rightchild_(rightchild), trueright_(trueright) {}
			node(value_type data, bool trueright = true, node* leftchild = nullptr, node* rightchild = nullptr)
				: data_(data), leftchild_(leftchild), rightchild_(rightchild), trueright_(trueright) {}
		};
		using node_ptr = node*;
		using const_node_ptr = const node*;


		bstree()
			: root_(nullptr), size_(0) {}
		bstree(const bstree& rhs) : size_(rhs.size_) { root_ = assign_subtree(rhs.root_); }
		bstree(bstree&& rhs) : root_(rhs.root_), size_(rhs.size_) { rhs.size_ = 0; rhs.root_ = nullptr; }
		bstree(std::initializer_list<value_type> rhs) : size_(rhs.size())
		{
			if (std::is_sorted(rhs.begin(), rhs.end())) // O(n)
				ordered_initlist_assignment(rhs); // O(n) if sorted
			else
				initlist_assignment(rhs); // O(nlogn) if unsorted.
		}

		// for performance test
		bstree(const std::vector<value_type>& v) : size_(v.size())
		{
			root_ = assign_by_ordered_itr(root_, v.begin(), v.end());
		}

		~bstree() { clear_subtree(root_); }

		bstree& operator=(const bstree& rhs)
		{
			if (this == &rhs) return *this;

			clear_subtree(root_);
			root_ = assign_subtree(rhs.root_);
			return *this;
		}
		bstree& operator=(bstree&& rhs)
		{
			if (this == &rhs) return *this;

			clear_subtree(root_);

			root_ = rhs.root_;
			size_ = rhs.size_;
			rhs.root_ = nullptr;
			rhs.size_ = 0;

			return *this;
		}
		bstree& operator=(std::initializer_list<T> rhs)
		{
			size_ = rhs.size();

			if (std::is_sorted(rhs.begin(), rhs.end())) // O(n)
				ordered_initlist_assignment(rhs); // O(n) if sorted
			else
				initlist_assignment(rhs); // O(nlogn) if unsorted.

			return *this;
		}

		// for performance test
		bstree& operator=(const std::vector<value_type>& v)
		{
			size_ = v.size();
			root_ = assign_by_ordered_itr(root_, v.begin(), v.end());
			return *this;
		}

		size_type size() const { return size_; }
		bool empty() const { return size_ == 0; }

		bool contains(const_reference x) { return contains_subtree(root_, x); }

		const value_type& min() const;
		value_type& min();
		const value_type& max() const;
		value_type& max();

		bool insert(const_reference x) { bool flag = insert_subtree(root_, x); if (flag) ++size_; return flag; }
		bool insert(value_type&& x) { bool flag = insert_subtree(root_, std::move(x)); if (flag) ++size_; return flag; }
		bool erase(const_reference x) { bool flag = erase_subtree(root_, x); if (flag) --size_; return flag; }

		// return inorder and post-order traversal sequences of this tree. 
		// for test only
		std::pair<std::vector<value_type>, std::vector<value_type>>
			serialize() const
		{
			std::vector<value_type> inorder_seq; inorder_seq.reserve(size_);
			std::vector<value_type> postorder_seq; postorder_seq.reserve(size_);

			inorder_traversal(root_, inorder_seq);
			postorder_traversal(root_, postorder_seq);

			return std::make_pair(inorder_seq, postorder_seq);
		}

		void update_rnode()
		{
			node_ptr prev = nullptr;
			update_rnode_subtree(root_, prev);
		}

		std::vector<value_type> sorted()
		{
			std::vector<value_type> ret;
			node_ptr p = root_;
			while (p->leftchild_ != nullptr) p = p->leftchild_; // go down through left-most path.
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

	private:
		node_ptr root_;
		size_type size_;

		void clear_subtree(node_ptr& root);

		node_ptr assign_subtree(node_ptr root);

		bool insert_subtree(node_ptr& root, const_reference x);
		bool insert_subtree(node_ptr& root, value_type&& x);

		bool erase_subtree(node_ptr& root, const_reference x);

		bool contains_subtree(const_node_ptr root, const_reference x) const;

		// O(nlogn) for unsorted.
		template <typename ITR, typename = typename std::enable_if<!std::is_arithmetic<ITR>::value, bool>::type>
		void assign_by_itr(ITR first, ITR last)
		{
			for (auto p = first; p != last; ++p) insert_subtree(root_, *p);
		}

		// O(n) for sorted, iff. vector.
		template <typename ITR, typename = typename std::enable_if<!std::is_arithmetic<ITR>::value, bool>::type>
		node_ptr assign_by_ordered_itr(node_ptr root, ITR first, ITR last)
		{
			if (first == last) return nullptr;
			size_type n = std::distance(first, last); // O(1) for vector.

			ITR mid = first + n / 2; // O(1) for vector.
			node_ptr lsubtree = assign_by_ordered_itr(root, first, mid);
			node_ptr rsubtree = assign_by_ordered_itr(root, mid + 1, last);
			return new node(*mid, true, lsubtree, rsubtree);
		}

		// O(nlogn)
		void initlist_assignment(std::initializer_list<value_type> rhs) { assign_by_itr(rhs.begin(), rhs.end()); }

		void ordered_initlist_assignment(std::initializer_list<value_type> rhs)
		{
			std::vector<value_type> v(rhs); // O(n)
			root_ = assign_by_ordered_itr(root_, v.begin(), v.end()); // O(n)
			// total: O(n) + O(n) = O(n)
		}

		// for test only
		void inorder_traversal(const_node_ptr root, std::vector<value_type>& v) const
		{
			if (root == nullptr) return;

			inorder_traversal(root->leftchild_, v);
			v.push_back(root->data_);
			inorder_traversal(root->rightchild_, v);
		}

		// for test only
		void postorder_traversal(const_node_ptr root, std::vector<value_type>& v) const
		{
			if (root == nullptr) return;

			postorder_traversal(root->leftchild_, v);
			postorder_traversal(root->rightchild_, v);
			v.push_back(root->data_);
		}
		void update_rnode_subtree(node_ptr root, node_ptr& prev)
		{
			if (root == nullptr) return;

			update_rnode_subtree(root->leftchild_, prev);
			if (prev != nullptr && prev->rightchild_ == nullptr)
			{
				prev->trueright_ = false;
				prev->rightchild_ = root;
			}
			prev = root;
			update_rnode_subtree(root->rightchild_, prev);
		}

	};

	template<typename T>
	const typename bstree<T>::value_type& bstree<T>::min() const
	{
		const_node_ptr p = root_;
		while (p->leftchild_ != nullptr) p = p->leftchild_;
		return p->data_;
	}

	template<typename T>
	typename bstree<T>::value_type& bstree<T>::min()
	{
		node_ptr p = root_;
		while (p->leftchild_ != nullptr) p = p->leftchild_;
		return p->data_;
	}

	template<typename T>
	const typename bstree<T>::value_type& bstree<T>::max() const
	{
		const_node_ptr p = root_;
		while (p->rightchild_ != nullptr) p = p->rightchild_;
		return p->data_;
	}

	template<typename T>
	typename bstree<T>::value_type& bstree<T>::max()
	{
		node_ptr p = root_;
		while (p->rightchild_ != nullptr) p = p->rightchild_;
		return p->data_;
	}

	template<typename T>
	void bstree<T>::clear_subtree(node_ptr& root)
	{
		if (root == nullptr) return;

		clear_subtree(root->leftchild_);
		if (root->trueright_) clear_subtree(root->rightchild_);
		delete root; root = nullptr;
	}

	template<typename T>
	typename bstree<T>::node_ptr bstree<T>::assign_subtree(node_ptr root)
	{
		if (root == nullptr) return nullptr;

		node_ptr lchild = assign_subtree(root->leftchild_);
		node_ptr rchild = assign_subtree(root->rightchild_);

		return new node(root->data_, true, lchild, rchild);
	}

	template<typename T>
	bool bstree<T>::insert_subtree(node_ptr& root, const_reference x)
	{
		if (root == nullptr)
		{
			root = new node(x);
			return true;
		}

		if (root->data_ == x) return false;

		return x < root->data_ ?
			insert_subtree(root->leftchild_, x)
			:
			insert_subtree(root->rightchild_, x);
	}


	template<typename T>
	bool bstree<T>::insert_subtree(node_ptr& root, value_type&& x)
	{
		if (root == nullptr)
		{
			root = new node(std::move(x));
			return true;
		}
		if (root->data_ == x) return false;

		return x < root->data_ ?
			insert_subtree(root->leftchild_, std::move(x))
			:
			insert_subtree(root->rightchild_, std::move(x));
	}

	template<typename T>
	bool bstree<T>::erase_subtree(node_ptr& root, const_reference x)
	{
		if (root == nullptr) return false;

		if (root->data_ == x && root->leftchild_ != nullptr && root->rightchild_ != nullptr)
		{
			// find min element in right subtree
			node_ptr p = root->rightchild_;
			node_ptr* pref = &(root->rightchild_);
			// walk down through the left-most path to the end.
			while (p->leftchild_ != nullptr) { pref = &(p->leftchild_); p = p->leftchild_; }

			std::swap(root->data_, p->data_);
			*pref = p->rightchild_;
			delete p;

			return true;
		}

		if (root->data_ == x && (root->leftchild_ == nullptr || root->rightchild_ == nullptr))
		{
			node_ptr to_delete = root;

			root =
				(root->leftchild_ == nullptr) ?
				root->rightchild_
				:
				root->leftchild_;

			delete to_delete;
			return true;
		}

		return x < root->data_ ?
			erase_subtree(root->leftchild_, x)
			:
			erase_subtree(root->rightchild_, x);
	}

	template<typename T>
	bool bstree<T>::contains_subtree(const_node_ptr root, const_reference x) const
	{
		if (root == nullptr) return false;
		if (root->data_ == x) return true;

		return x < root->data_ ?
			contains_subtree(root->leftchild_, x)
			:
			contains_subtree(root->rightchild_, x);
	}


}
