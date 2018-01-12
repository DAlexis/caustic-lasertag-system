#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <usbd_cdc_if.h>
#include <string.h>

#include "newlib-driver.h"
#include "fatfs.h"
#include "fatfs-utils.h"

#if defined STM32F1
# include <stm32f1xx_hal.h>
#elif defined STM32F2
# include <stm32f2xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif

#include "low-level-debug-config.h"

#define MAX_FILES_OPEN      _FS_LOCK
#define DESCRIPTOR_OFFSET   4
#define DESCRIPTOR_TO_INDEX(d)  (d - DESCRIPTOR_OFFSET)
#define INDEX_TO_DESCRIPTOR(d)  (d + DESCRIPTOR_OFFSET)

#define UNUSED_ARG(x)   ((void) (x))


//extern uint32_t __get_MSP(void);

#ifdef USE_UART_DEBUG_OUTPUT
    extern UART_HandleTypeDef huart1;
#endif
//extern uint64_t virtualTimer;

#undef errno
extern int errno;

int freeHeap = 0;

char *__env[1] = { 0 };
char **environ = __env;

FIL *fils[MAX_FILES_OPEN];

// Private functions

static void freeFil(int fil)
{
	free(fils[fil]);
	fils[fil] = NULL;
}

static char isOpened(int fil)
{
	if (fil < 0 || fil >= MAX_FILES_OPEN)
		return 0;
	if (fils[fil] == NULL)
		return 0;
	return 1;
}

// System functions

int _write(int file, char *ptr, int len);

void _exit(int status)
{
    UNUSED_ARG(status);
    while (1);
}

int _execve(char *name, char **argv, char **env)
{
    UNUSED_ARG(name); UNUSED_ARG(argv); UNUSED_ARG(env);
    errno = ENOMEM;
    return -1;
}

int _fork()
{
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st)
{
	if (file == STDIN_FILENO || file == STDOUT_FILENO || file == STDERR_FILENO)
	{
		st->st_mode = S_IFCHR;
		return 0;
	}

	int index = DESCRIPTOR_TO_INDEX(file);
	if (!isOpened(index))
	{
		return -1;
	}
	st->st_mode = S_IFREG;
	st->st_size = fils[index]->fsize;
	st->st_blksize = 512;
	st->st_nlink = 0;
	return 0;
}

int _getpid()
{
    return 1;
}
/*
int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    tv->tv_sec = virtualTimer / 1000;
    tv->tv_usec = (virtualTimer % 1000) * 1000;
    return 0;
}*/

int _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

int _kill(int pid, int sig)
{
    UNUSED_ARG(pid); UNUSED_ARG(sig);
    errno = EINVAL;
    return (-1);
}

int _link(char *old, char *new)
{
    UNUSED_ARG(old); UNUSED_ARG(new);
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    int index = DESCRIPTOR_TO_INDEX(file);
    if (!isOpened(index))
    {
        errno = EBADF;
        return -1;
    }
    int pos = dir;
    switch (ptr)
    {
    case SEEK_CUR:
        pos += fils[index]->fptr;
        break;
    case SEEK_END:
        pos += fils[index]->fsize;
        break;
    default: break;
    }

    FRESULT res = f_lseek (fils[index], pos);
    if (res != FR_OK)
    {
        errno = ENXIO; // we suppose
        return -1;
    }
    return pos;
}

caddr_t _sbrk(int incr)
{
    extern char __bss_end__;
    extern char _estack;
    char* heapLimit = &_estack - 0x200;

    //extern char isMemoryCorrupted;

    static char* current_heap_end;
    char* current_block_address;

    if (current_heap_end == 0)
    {
      current_heap_end = &__bss_end__;
    }

    current_block_address = current_heap_end;

    // Need to align heap to word boundary, else will get
    // hard faults on Cortex-M0. So we assume that heap starts on
    // word boundary, hence make sure we always add a multiple of
    // 4 to it.
    incr = (incr + 3) & (~3); // align value to 4
    if (current_heap_end + incr > heapLimit)
    {
      // Some of the libstdc++-v3 tests rely upon detecting
      // out of memory errors, so do not abort here.
    #if 0
      extern void abort (void);

      _write (1, "_sbrk: Heap and stack collision\n", 32);

      abort ();
    #else
      // Heap has overflowed
      //isMemoryCorrupted = 1;

      errno = ENOMEM;
      return (caddr_t) - 1;
    #endif
    }

    current_heap_end += incr;
    freeHeap = heapLimit - current_heap_end;

    return (caddr_t) current_block_address;
}

int _open(char *name, int flags, int perms)
{
	UNUSED(perms);
	uint8_t fatfsFlags = FA_READ;
	// Read/write bits
	if (flags & O_WRONLY)
		fatfsFlags |= FA_WRITE;

	if (flags & O_RDWR)
		fatfsFlags |= FA_WRITE | FA_READ;

	// Open existing / create new bits

	if (flags & O_EXCL)
	{
		if (flags & O_CREAT)
			fatfsFlags |= FA_CREATE_NEW;
		else
			fatfsFlags |= FA_OPEN_EXISTING;
	} else {
		fatfsFlags |= FA_OPEN_ALWAYS;
	}

	int index = -1;
	// Searching for free file descriptor
	for (int i=0; i<MAX_FILES_OPEN; i++)
	{
		if (fils[i] == NULL)
			index = i;
		break;
	}

	if (index == -1)
		return ENFILE;

	fils[index] = malloc(sizeof(FIL));
	memset(fils[index], 0, sizeof(FIL));

	FRESULT result = f_open(fils[index], name, fatfsFlags);

	if (result != FR_OK)
	{
		free(fils[index]);
	}

	if (result == FR_DISK_ERR || result == FR_NOT_READY)
		return EINTR;

	if (result != FR_OK)
		return EACCES;

	if (flags & O_APPEND)
	{
		FILINFO fi;
		if (FR_OK == f_stat(name, &fi))
		{
			f_lseek(fils[index], fi.fsize);
		}
	}
	return INDEX_TO_DESCRIPTOR(index);
}

int _close(int file)
{
	int index = DESCRIPTOR_TO_INDEX(file);
	if (isOpened(index) == 0)
		return EBADF;

	FRESULT r = f_close(fils[index]);
	freeFil(index);
	if (r != FR_OK)
		return EIO;

    return 0;
}


int _read(int file, char *ptr, int len)
{
	if (file == STDIN_FILENO)
		return 0;

	int index = DESCRIPTOR_TO_INDEX(file);
	if (isOpened(index) == 0)
	{
		errno = EBADF;
		return -1;
	}
	UINT br = 0;
	FRESULT r = FR_OK;
	if (len >= 400)
        r = f_read_huge(fils[index], (void*) ptr, (UINT) len, &br);
	else
		r = f_read(fils[index], (void*) ptr, (UINT) len, &br);
	if (r != FR_OK)
	{
		errno = EIO;
		return -1;
	}
	return br;
}

int _write(int file, char *ptr, int len)
{
	if (file == STDOUT_FILENO || file == STDERR_FILENO)
	{
		lockOutputPort();

		#ifdef USE_USB_DEBUG_OUTPUT
		    	CDC_Transmit_FS(ptr, len);
		#endif
		#ifdef USE_UART_DEBUG_OUTPUT
		        HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
		#endif

		unlockOutputPort();
		return len;
	}
	int index = DESCRIPTOR_TO_INDEX(file);
	if (isOpened(index) == 0)
	{
		errno = EBADF;
		return -1;
	}

	UINT bw = 0;
	FRESULT r = f_write(fils[index], (void*) ptr, (UINT) len, &bw);
	if (r != FR_OK)
	{
		errno = EIO;
		return -1;
	}
	return bw;
}

int _stat(const char *filepath, struct stat *st)
{
	/// Here we assume that only real files (not stdout/stdin/stderr) may be used here
	FILINFO inf;
    memset(&inf, 0, sizeof(inf));
	if (f_stat(filepath, &inf) != FR_OK)
	{
		errno = EFAULT;
		return -1;
	}
    if (st == NULL)
    {
        return 0;
    }
    memset(st, 0, sizeof(*st));
    st->st_mode = S_IFREG;
	st->st_size = inf.fsize;
	st->st_blksize = 512;
	st->st_nlink = 0;

	return 0;
}

clock_t _times(struct tms *buf)
{
    UNUSED_ARG(buf);
    return -1;
}

int _unlink(char *name)
{
	if (FR_OK == f_unlink (name))
	{
		return 0;
	}
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
    UNUSED_ARG(status);
    errno = ECHILD;
    return -1;
}



void __attribute__((weak)) lockOutputPort()
{
	// This function may be implemented anywhere
}

void __attribute__((weak)) unlockOutputPort()
{
	// This function may be implemented anywhere
}
