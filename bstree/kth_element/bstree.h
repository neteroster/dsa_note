#pragma once
#include <initializer_list>
#include <utility>
#include <type_traits>
#include <iterator>
#include <vector> // this is for some constructor and test function.
#include <string>
#include <iostream>

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
			size_type lnodenum_;

			node(size_type lnodenum, node* leftchild = nullptr, node* rightchild = nullptr)
				: data_(value_type()), leftchild_(leftchild), rightchild_(rightchild), lnodenum_(lnodenum) {}
			node(size_type lnodenum, value_type data, node* leftchild = nullptr, node* rightchild = nullptr)
				: data_(data), leftchild_(leftchild), rightchild_(rightchild), lnodenum_(lnodenum) {}
		};
		using node_ptr = node*;
		using const_node_ptr = const node*;


		bstree()
			: root_(nullptr), size_(0) {}
		bstree(const bstree& rhs) : size_(rhs.size_) { root_ = assign_subtree(rhs.root_); }
		bstree(bstree&& rhs) : root_(rhs.root_), size_(rhs.size_) { rhs.size_ = 0; rhs.root_ = nullptr; }
		bstree(std::initializer_list<value_type> rhs) : size_(rhs.size())
		{
			initlist_assignment(rhs); // O(nlogn) if unsorted.
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
			initlist_assignment(rhs);

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
		void print() { print_subtree("", root_, false); }

		const value_type& nth(size_type n)
		{
			return nth_subtree(root_, n, size_);
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


		// O(nlogn)
		void initlist_assignment(std::initializer_list<value_type> rhs) { assign_by_itr(rhs.begin(), rhs.end()); }

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


		void print_subtree(const std::string& prefix, const node_ptr root, bool is_left)
		{
			if (root == nullptr) return;

			std::cout << prefix;
			std::cout << (is_left ? "(L)|---" : "(R)|---");

			std::cout << root->data_ << std::endl;

			print_subtree(prefix + (is_left ? "|   " : "    "), root->leftchild_, true);
			print_subtree(prefix + (is_left ? "|   " : "    "), root->rightchild_, false);
		}

		value_type& nth_subtree(node_ptr root, size_type n, size_type total_nums)
		{
			size_type lnodenum = root->lnodenum_;
			size_type rnodenum = total_nums - lnodenum - 1;
			if (n == lnodenum + 1) return root->data_;

			if (n <= lnodenum) return nth_subtree(root->leftchild_, n, lnodenum);
			else return nth_subtree(root->rightchild_, n - (lnodenum + 1), rnodenum);
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
		clear_subtree(root->rightchild_);
		delete root; root = nullptr;
	}

	template<typename T>
	typename bstree<T>::node_ptr bstree<T>::assign_subtree(node_ptr root)
	{
		if (root == nullptr) return nullptr;

		node_ptr lchild = assign_subtree(root->leftchild_);
		node_ptr rchild = assign_subtree(root->rightchild_);

		return new node(root->lnodenum_, root->data_, lchild, rchild);
	}

	template<typename T>
	bool bstree<T>::insert_subtree(node_ptr& root, const_reference x)
	{
		if (root == nullptr)
		{
			root = new node(0, x);
			return true;
		}

		if (root->data_ == x) return false;

		if (x < root->data_)
		{
			if (insert_subtree(root->leftchild_, x)) { ++(root->lnodenum_); return true; }
		}
		else
			return insert_subtree(root->rightchild_, x);
	}


	template<typename T>
	bool bstree<T>::insert_subtree(node_ptr& root, value_type&& x)
	{
		if (root == nullptr)
		{
			root = new node(0, std::move(x));
			return true;
		}

		if (root->data_ == x) return false;

		if (x < root->data_)
		{
			if (insert_subtree(root->leftchild_, std::move(x))) { ++(root->lnodenum_); return true; }
		}
		else
			return insert_subtree(root->rightchild_, std::move(x));
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

		if (x < root->data_)
		{
			if (erase_subtree(root->leftchild_, x)) { --(root->lnodenum_); return true; }
		}
		else
			return erase_subtree(root->rightchild_, x);
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
