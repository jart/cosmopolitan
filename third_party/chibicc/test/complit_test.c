#include "third_party/chibicc/test/test.h"

typedef struct Tree {
  int val;
  struct Tree *lhs;
  struct Tree *rhs;
} Tree;

Tree *tree = &(Tree){1, &(Tree){2, &(Tree){3, 0, 0}, &(Tree){4, 0, 0}}, 0};

int main() {
  ASSERT(1, (int){1});
  ASSERT(2, ((int[]){0, 1, 2})[2]);
  ASSERT('a', ((struct {
                char a;
                int b;
              }){'a', 3})
                  .a);
  ASSERT(3, ({
           int x = 3;
           (int){x};
         }));
  (int){3} = 5;

  ASSERT(1, tree->val);
  ASSERT(2, tree->lhs->val);
  ASSERT(3, tree->lhs->lhs->val);
  ASSERT(4, tree->lhs->rhs->val);

  return 0;
}
