#ifndef COMMON_H_
#define COMMON_H_

#define DEFAULT_CHILDREN_COUNT 2

void fork_children(void (*on_child)(int i), int children_count);
void wait_children(void (*prewait)(int i), int children_count);

#endif  // COMMON_H_
