#include "rpnx/experimental/avl_tree.hpp"


int main()
{
  rpnx::avl_tree< int, int > a, b, c, d;

  std::mt19937 r;

  for (size_t i = 1; i < 6; i++)
  {
    a.insert(i, i);
  }

  for (size_t i = 14; i < 31; i++)
  {
    b.insert(i, i);
  }
  
  c.insert(10, 10);
  
  rpnx::avl_tree<int, int>::join(a, b, c);

  

  std::cout << "c=" << c.stringify() << std::endl << std::endl;
  
  rpnx::avl_tree<int, int>::split(26, c, b);
  std::cout << c.stringify() << std::endl;
}
