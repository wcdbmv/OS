#ifndef CHILDREN_H_
#define CHILDREN_H_

int fork_children(int children_count, void (*on_child)(int id));
void wait_children(int children_count);

#endif  // CHILDREN_H_
