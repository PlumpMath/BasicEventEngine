#define _a(x) (*s)[x]
#define _s(x) (*s)[x].s(__FILE__, __LINE__)
#define _i(x) (*s)[x].i(__FILE__, __LINE__)
#define _d(x) (*s)[x].d(__FILE__, __LINE__)
#define _p(x) (*s)[x].p(__FILE__, __LINE__)
#define _c(x,i) (*s)[x].p(i, __FILE__, __LINE__)
#define _cs(x,i) (*s)[x].p(i, __FILE__, __LINE__).s(__FILE__, __LINE__)
#define _ci(x,i) (*s)[x].p(i, __FILE__, __LINE__).i(__FILE__, __LINE__)
#define _cd(x,i) (*s)[x].p(i, __FILE__, __LINE__).d(__FILE__, __LINE__)
#define _cp(x,i) (*s)[x].p(i, __FILE__, __LINE__).p(__FILE__, __LINE__)
