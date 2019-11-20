#ifndef COLOR_H_
#define COLOR_H_

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define COLOR(color, string) ANSI_COLOR_##color string ANSI_COLOR_RESET
#define COLOR_RED(string) COLOR(RED, string)
#define COLOR_GREEN(string) COLOR(GREEN, string)
#define COLOR_YELLOW(string) COLOR(YELLOW, string)
#define COLOR_BLUE(string) COLOR(BLUE, string)
#define COLOR_MAGENTA(string) COLOR(MAGENTA, string)
#define COLOR_CYAN(string) COLOR(CYAN, string)

#endif  // COLOR_H_
