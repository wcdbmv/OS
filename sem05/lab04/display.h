#ifndef DISPLAY_H_
#define DISPLAY_H_

void display_header(void);
void display_on_parent(const char *format, ...);
void display_on_child(int i, const char *format, ...);

#endif  // DISPLAY_H_
