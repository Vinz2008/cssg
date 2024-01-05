#ifdef _WIN32
#define mkdir(folder, mode) mkdir(folder)
#include <io.h>
#define F_OK 0
#define access _access
#define stat _stat
#define SOCKET_TYPE unsigned int
#endif