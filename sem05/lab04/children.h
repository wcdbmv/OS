#ifndef CHILDREN_H_
#define CHILDREN_H_

#define DEFAULT_CHILDREN_COUNT 2

void fork_children(void (*on_child)(int i), int children_count);
void wait_children(int children_count);

#endif  // CHILDREN_H_
