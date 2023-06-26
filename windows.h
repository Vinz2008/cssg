#ifdef _WIN32
#define mkdir(folder, mode) mkdir(folder)
#include <io.h>
#define F_OK 0
#define access _access
#endif