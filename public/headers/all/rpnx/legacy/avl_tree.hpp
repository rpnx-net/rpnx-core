/*

Copyright 2018 Ryan P. Nicholl

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


 */
#ifndef RPNX_AVL_TREE_TEST
#define RPNX_AVL_TREE_TEST

#include <algorithm>
#include <assert.h>
#include <cstddef>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#define heavy_assert(x) assert(x)

namespace rpnx
{
template < typename K, typename V >
class avl_tree
{
public:
  using key_type = K const;
  using mapped_type = V;
  using value_type = std::pair< key_type, mapped_type >;

private:
  struct node
  {
    value_type val;
    node* cld[2];
    node* dir[2];
    node* par;
    size_t height;
    size_t sz;

    node(value_type v) : val(std::move(v))
    {
      cld[0] = nullptr;
      cld[1] = nullptr;
      dir[0] = nullptr;
      dir[1] = nullptr;
      par = nullptr;
      height = 1;
      sz = 1;
    }
  };

public:
  class const_iterator
  {
    friend class avl_tree< K, V >;

  private:
    node           * at;
    avl_tree<K, V> * par;

    const_iterator(node* at_arg) 
      :
      at(at_arg), par(nullptr)
       {}

  public:
  
    const_iterator()
    : at(nullptr), par(nullptr)
    {}
    
    value_type const & operator *() const
    {
      return at->val;
    }
    
    bool operator ==( const_iterator const & other )
    {
      return other.at == at;
    }
  };

private: // private member variables
  node* in_root;
  std::size_t in_sz;

private:
  static node* get_node(const_iterator const& it) { return it.at; }
  static avl_tree<K, V> * get_tree(const_iterator const &it)
  {
    return it.par;
  }

  class iterator : public const_iterator
  {
    friend class avl_tree< K, V >;

  private:
  };
  // public variables

 

  std::string stringify(node* n)
  {
    std::string s = "digraph BST {" + printt(n) + "}";
    return s;
  }

  void printt() { std::cout << stringify() << std::endl; }

  std::string node_name(node* n)
  {
    std::stringstream ss;

    ss << "node_" << n->val.first;

    return ss.str();
  }

  std::string printt(node* n)
  {
    std::stringstream ss;
    ss << node_name(n) << " [label=\"";

    int h = real_height(n);
    int b = real_balance(n);
    /*ss << "<<table border=\"0\" cellspacing=\"0\">"
                  "<tr><td port=\"port1\" border=\"1\">" << n->val.first <<
       "</td></tr>"
                  "<tr><td port=\"port2\" border=\"1\">h: " << h << "</td></tr>"
                  "<tr><td port=\"port3\" border=\"1\">b: " << b << "</td></tr>"
              "</table>>";*/
    ss << n->val.first;

    ss << "(" << (b > 0 ? "+" : "") << b << ")";
    // ss << "[" << h << "]";
    if (b != balance(n)) ss << " ERR_BAL=" << balance(n);
    if (h != real_height(n))
      ss << " ERR_H=" << height(n) << "act" << real_height(n);

    ss << "{";
    if (n->dir[0] == nullptr)
    {
      ss << "-";
    }
    else
      ss << n->dir[0]->val.first;

    ss << ",";
    if (n->dir[1] == nullptr)
      ss << "-";
    else
      ss << n->dir[1]->val.first;
    ss << "}";
    ss << "\"";

    if (b > 1 || b < -1)
      ss << ",color=red, shape=doublecircle";
    else
      ss << ", shape=circle";
    ss << "];";
    if (left(n))
    {
      ss << node_name(n) << " -> " << node_name(left(n)) << ";";
    }
    else
      ss << "null_" << n->val.first << "_l [shape=point]; " << node_name(n)
         << " -> "
         << "null_" << n->val.first << "_l; ";

    if (right(n))
      ss << node_name(n) << " -> " << node_name(right(n)) << "; ";
    else
      ss << "null_" << (n->val.first) << "_r [shape=point]; " << node_name(n)
         << " -> "
         << "null_" << (n->val.first) << "_r; ";

    std::string s = ss.str();
    if (left(n)) s += printt(left(n));
    if (right(n)) s += printt(right(n));
    return s;
  }

  int cmp(node * a, K const& b)
  {
    if (a->val.first < b) return -1;
    if (b < a->val.first) return 1;
    return 0;
  }

  int cmp(K const& a, node* b)
  {
    if (a < b->val.first) return -1;
    if (b->val.first < a) return 1;
    return 0;
  }

  int cmp(node* a, node* b)
  {
    if (a->val < b->val)
      return -1;
    else if (b->val < a->val)
      return 1;
    return 0;
  }
  ssize_t balance(node* n) { return height(right(n)) - height(left(n)); }
  ssize_t height(node* n)
  {
    if (n == nullptr) return 0;
    return n->height;
  }

  void adjust_from_child(node* n, node* c, ssize_t hd)
  {
    if (hd == 0) return;
    if (n == nullptr) return;
  }

  size_t sz(node* n) const
  {
    if (n == nullptr) return 0;
    return n->sz;
  }

  void update(node* n)
  {
    n->height = std::max(height(left(n)), height(right(n))) + 1;
    n->sz = sz(left(n)) + sz(right(n)) + 1;
  }

  ssize_t real_height(node* n)
  {
    if (n == nullptr) return 0;
    return 1 + std::max(real_height(n->cld[0]), real_height(n->cld[1]));
  }

  ssize_t real_balance(node* n)
  {
    if (n == nullptr) return 0;
    return real_height(right(n)) - real_height(left(n));
  }

  node* left(node* a) { return a->cld[0]; }

  node* right(node* a) { return a->cld[1]; }

  void left_rotate(node* a)
  {

    node* b = a->cld[1];
    assert(b != nullptr);
    node* y = b->cld[0];

    node* p = a->par;

    heavy_assert(balance(a) == real_balance(a));
    heavy_assert(balance(b) == real_balance(b));
    std::string before = stringify();

    a->cld[1] = y;
    b->cld[0] = a;

    b->par = p;
    a->par = b;
    if (y) y->par = a;
    if (p)
    {
      if (p->cld[0] == a)
        p->cld[0] = b;
      else
        p->cld[1] = b;
    }
    else
      in_root = b;

    update(a);
    update(b);

    heavy_assert(balance(a) == real_balance(a));
    heavy_assert(balance(b) == real_balance(b));
  }

  void right_rotate(node* a)
  {
    node* b = a->cld[0];

    assert(b != nullptr);

    node* y = b->cld[1];
    node* p = a->par;

    heavy_assert(balance(a) == real_balance(a));
    heavy_assert(balance(b) == real_balance(b));

    a->cld[0] = y;
    b->cld[1] = a;

    b->par = p;
    a->par = b;
    if (y) y->par = a;
    if (p)
    {
      if (p->cld[1] == a)
        p->cld[1] = b;
      else
        p->cld[0] = b;
    }
    else
      in_root = b;

    // a->bal = a->bal + 1 - std::max(0, b->bal);
    // b->bal = b->bal + 1 + std::max(0, a->bal);

    update(a);
    update(b);
    heavy_assert(balance(a) == real_balance(a));
    heavy_assert(balance(b) == real_balance(b));
  }



  bool is_sound(node* n)
  {
    if (n == nullptr) return true;

    if (left(n) && left(n)->par != n)
    {
      std::cerr << "p-violation L" << std::endl;
      return false;
    }

    if (right(n) && right(n)->par != n)

    {
      std::cerr << "p-violation R" << std::endl;
      return false;
    }
    return is_sound(left(n)) && is_sound(right(n));
  }

  bool is_valid(node* n)
  {
    if (n == nullptr) return true;

    if (left(n) && left(n)->par != n)
    {
      std::cerr << "p-violation L" << std::endl;
      return false;
    }
    if (right(n) && right(n)->par != n)

    {
      std::cerr << "p-violation R" << std::endl;
      return false;
    }

    if (left(n) && cmp(left(n), n) != -1)
    {
      std::cerr << "k-violation L" << std::endl;
      return false;
    }

    if (right(n) && cmp(right(n), n) != 1)
    {
      std::cerr << "k-violation R" << std::endl;
      return false;
    }
    if (height(n) != real_height(n))
    {
      std::cerr << "h-violation" << std::endl;
      return false;
    }
    if (balance(n) <= -2 || balance(n) >= 2)
    {
      std::cerr << "b-violation" << std::endl;
      return false;
    }
    if (balance(n) != real_balance(n))
    {

      std::cerr << "rb-violation" << std::endl;

      return false;
    }

    return is_valid(left(n)) && is_valid(right(n));
  }

  void swap_node_refs(node* a, node* b, node*& r)
  {
    if (r == a)
      r = b;
    else if (r == b)
      r = a;

    return;
  }

  void swap_positions(node* a, node* b)
  {
    using std::swap;
    heavy_assert(is_sound());

    node* ap = a->par;
    node* bp = b->par;
    node* al = a->cld[0];
    node* ar = a->cld[1];
    node* bl = b->cld[0];
    node* br = b->cld[1];

    if (ap == b) ap = nullptr;
    if (bp == a) bp = nullptr;
    if (al == b) al = nullptr;
    if (ar == b) ar = nullptr;
    if (bl == a) bl = nullptr;
    if (br == a) br = nullptr;

    swap(a->cld[0], b->cld[0]);
    swap(b->cld[1], a->cld[1]);
    swap(a->par, b->par);

    swap_node_refs(a, b, a->par);
    swap_node_refs(a, b, b->par);

    swap_node_refs(a, b, a->cld[0]);
    swap_node_refs(a, b, a->cld[1]);
    swap_node_refs(a, b, b->cld[0]);
    swap_node_refs(a, b, b->cld[1]);

    if (ap)
    {
      swap_node_refs(a, b, ap->cld[0]);
      swap_node_refs(a, b, ap->cld[1]);
    }
    else
      swap_node_refs(a, b, in_root);

    if (bp)
    {
      swap_node_refs(a, b, bp->cld[0]);
      swap_node_refs(a, b, bp->cld[1]);
    }
    else
      swap_node_refs(a, b, in_root);

    if (al) swap_node_refs(a, b, al->par);
    if (ar) swap_node_refs(a, b, ar->par);

    if (bl) swap_node_refs(a, b, bl->par);
    if (br) swap_node_refs(a, b, br->par);
  }

  bool imbalanced(node *n)
  {
    update(n);
    
    return balance(n) <= -2 || balance(n) >= 2;
  }

  void rebalance_local(node *n)
  {
    update(n);

    if (balance(n) <= -2)
    {
      if (balance(left(n)) == 1)
      {
        left_rotate(left(n));
      }
      if (balance(n) == -2) right_rotate(n);
    }

    if (balance(n) >= 2)
    {
      if (balance(right(n)) == -1)
      {
        right_rotate(right(n));
      }
      if (balance(n) == 2) left_rotate(n);
    }

    //heavy_assert(balance(n) == real_balance(n));
    //heavy_assert(balance(n) != -2 && balance(n) != 2);
  }

  void rebalance(node* n)
  {
    rebalance_local(n);

    if (n->par) rebalance(n->par);
  }

  std::pair< node*, bool > insert_node(value_type val)
  {
    node* n = in_root;
    if (n == nullptr)
    {
      node* what = new node(val);
      in_root = what;
      return {what, true};
    }

    for (;;)
    {
      assert(n != nullptr);
      int c = cmp(val.first, n);

      if (c == -1 && n->cld[0] != nullptr)
      {
        n = n->cld[0];
        continue;
      }

      if (c == 1 && n->cld[1] != nullptr)
      {
        n = n->cld[1];
        continue;
      }

      if (c == 0)
      {
        return {n, false};
      }

      if (c == -1)
      {
        node* what = new node(val);

        assert(n->cld[0] == nullptr);

        auto prev = n->dir[0];
        what->dir[0] = prev;
        if (prev) prev->dir[1] = what;
        what->dir[1] = n;
        n->dir[0] = what;

        n->cld[0] = what;
        what->par = n;
        update(what);
        update(what->par);
        rebalance(what);

        return {what, true};
      }

      if (c == 1)
      {
        node* what = new node(val);
        assert(n->cld[1] == nullptr);

        auto next = n->dir[1];
        what->dir[1] = next;
        what->dir[0] = n;

        n->dir[1] = what;
        what->dir[1] = next;

        if (next) next->dir[0] = what;

        n->cld[1] = what;
        what->par = n;

        update(what);
        update(what->par);
        rebalance(what);
        return {what, true};
      }

      assert(false);
      return {nullptr, false};
    }
  }
  
  

  void replace_child(node* n, node* c, node* nc)
  {
    if (nc) nc->par = n;
    if (n)
    {

      if (n->cld[0] == c)
      {
        n->cld[0] = nc;
      }
      else
      {
        assert(n->cld[1] == c);
        n->cld[1] = nc;
      }
    }
    else
    {
      if (in_root == c) in_root = nc;
    }
  }
  
  

  node* remove_node(key_type const& key)
  {
    node* n = in_root;
    heavy_assert(is_valid());

    while (n != nullptr)
    {
      int c = cmp(key, n);
      if (c == -1)
      {
        n = n->cld[0];
        continue;
      }
      if (c == 1)
      {
        n = n->cld[1];
        continue;
      }
      if (c == 0)
      {
        heavy_assert(is_sound());

        if (n->cld[0] != nullptr && n->cld[1] != nullptr)
        {
     
          assert(n->dir[1] != nullptr);
          swap_positions(n, n->dir[1]);
          assert(is_sound());
        }

        auto xb = n->dir[0];
        auto xa = n->dir[1];

        if (xb) xb->dir[1] = xa;
        if (xa) xa->dir[0] = xb;
        n->dir[0] = nullptr;
        n->dir[1] = nullptr;

        assert(n->cld[0] == nullptr || n->cld[1] == nullptr);

        if (left(n) == nullptr && right(n) == nullptr)
        {
          replace_child(n->par, n, nullptr);
          rebalance(n->par);
          assert(is_valid());
          n->par = nullptr;
          n->dir[0] = nullptr;
          n->dir[1] = nullptr;
          n->cld[0] = nullptr;
          n->cld[1] = nullptr;
          return n;
        }
        else if (left(n) != nullptr)
        {
          assert(right(n) == nullptr);
          auto child = n->cld[0];
          child->par = n->par;
          replace_child(n->par, n, child);
          rebalance(child);
          n->par = nullptr;
          n->dir[0] = nullptr;
          n->dir[1] = nullptr;
          n->cld[0] = nullptr;
          n->cld[1] = nullptr;
          heavy_assert(is_valid());
          return n;
        }
        else
        {
          assert(right(n) != nullptr);
          assert(left(n) == nullptr);
          auto child = n->cld[1];
          child->par = n->par;
          replace_child(n->par, n, child);
          heavy_assert(is_sound());
          rebalance(child);
          heavy_assert(is_valid());
          n->par = nullptr;
          n->dir[0] = nullptr;
          n->dir[1] = nullptr;
          n->cld[0] = nullptr;
          n->cld[1] = nullptr;
          return n;
        }
      }
    }
    heavy_assert(is_valid());
    
    return n;
  }
  
  node * last_node()
  {
    node * n = in_root;
    
    while(n)
    {
      if (n->cld[1]) n = n->cld[1];
      else break;
    }
    
    return n;
  }
  
  node * first_node()
  {
    node * n = in_root;
    while(n)
    {
      if (n->cld[0]) n = n->cld[0];
      else break;
    }
    return n;
  }
  
  node* find_host_node(key_type const & key)
  {
    node * n = in_root;
    
    if (!n) return nullptr;
    
    while (true)
    {
      int i = cmp(key, n);
      
      if (i == -1)
      {
        if (n->cld[0]) n = n->cld[0];
        else break;
      }
      else if (i == 1)
      {
        if (n->cld[1]) n = n->cld[1];
        else break;
      }
      else if (i == 0) return n;
    }
    
    return n;
  }
  
  void rotate_up(node *n)
  {
    while (n->par) 
    {
      node * p = n->par;
      int k = 0;
      
      if (n->par->cld[0] == n)
      {
        right_rotate(n->par);
        while (imbalanced(p))
        {
          rebalance_local(p);
          k++;
        }
      }
      else if (n->par->cld[1] == n)
      {
        left_rotate(p);
        while (imbalanced(p))
        {
          rebalance_local(p);
          k++;
        }
      }
      
      else std::cerr << "fail in rotate_up" << std::endl;
      
      std::cerr << "rotate-up k=" << k << std::endl; 
    }
  }
  
  

public:
  
  
  static void split(key_type const & k, avl_tree<K, V> &a, avl_tree<K, V> &b)
  {
    node * rt = a.find_host_node(k);
    
    std::cerr << "rt value=" << rt->val.first << std::endl;
    
    a.rotate_up(rt);
    
  }

  std::string to_json(node* n)
  {
    if (n == nullptr) return "null";
    std::stringstream ss;
    ss << "{\"val\":\"" << n->val.first << "\",\"left\": " << to_json(n->cld[0])
       << ",\"right\":" << to_json(n->cld[1]) << "}";
    return ss.str();
  }

  bool is_valid() { return is_valid(in_root); }

  bool is_sound() { return is_sound(in_root); }
  
  avl_tree() noexcept : in_root(0), in_sz(0) {}

  std::pair< iterator, bool > insert(value_type const& val)
  {
    auto r = insert_node(val);
    return {iterator(r.first), r.second};
  }

  size_t size() const { return sz(in_root); }

 std::string stringify()
  {
    std::string s = "digraph BST {" + printt(in_root) + "}";
    return s;
  }

  void insert(key_type k, mapped_type m)
  {
    assert(is_valid());
    insert_node(value_type{k, m});
    assert(is_valid());
  }
  
  static void join(avl_tree<K, V> &a, avl_tree<K, V> &b, avl_tree<K, V> &r)
  {
    assert(r.size() == 1);
    
    node * &rptr = r.in_root;    
    rptr->cld[0] = a.in_root;
    rptr->cld[1] = b.in_root;
    
    node * al = a.last_node();
    node * bf = b.first_node();
    
    if (al) al->dir[1] = rptr;
    if (bf) bf->dir[0] = rptr;
    
    rptr->dir[0] = al;
    rptr->dir[1] = bf;    
    
    a.in_root->par = rptr;
    b.in_root->par = rptr;
    
    
    while (r.imbalanced(r.in_root))
    {
      r.rebalance_local(r.in_root);
    }
    
   heavy_assert(r.is_valid());
    
    return;
  }
  
  void erase(key_type const & key)
  {
    node * n = remove_node (key);
    if (n) delete n;
    return;
  }
};
} // namespace rpnx

#endif
