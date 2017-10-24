#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <usbd_cdc_if.h>

#include "newlib-driver.h"
#include "fatfs.h"

#if defined STM32F1
# include <stm32f1xx_hal.h>
#elif defined STM32F2
# include <stm32f2xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif

#include "low-level-debug-config.h"

#define MAX_FILES_OPEN      4
#define DESCRIPTOR_OFFSET   4
#define DESCRIPTOR_TO_INDEX(d)  (d - DESCRIPTOR_OFFSET)
#define INDEX_TO_DESCRIPTOR(d)  (d + DESCRIPTOR_OFFSET)


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

int _write(int file, char *ptr, int len);

void _exit(int status)
{
    while (1);
}

int _execve(char *name, char **argv, char **env)
{
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
    st->st_mode = S_IFCHR;
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
    errno = EINVAL;
    return (-1);
}

int _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
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

#define FLAG_WRITE    0b1
#define FLAG_WRITE    0b1

int _open(char *name, int flags, int perms)
{
	UNUSED(perms);
	uint8_t fatfsFlags = FA_READ;
	// Read/write bits
	if (flags & O_WRONLY)
	{
		fatfsFlags |= FA_WRITE;
	}
	if (flags & O_RDWR)
	{
		fatfsFlags |= FA_WRITE | FA_READ;
	}

	// Open existing / create new bits

	if (flags & O_EXCL)
	{
		if (flags & O_CREAT)
		{
			fatfsFlags |= FA_CREATE_NEW;
		}
		else
		{
			fatfsFlags |= FA_OPEN_EXISTING;
		}
	}
	else
	{
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

	FRESULT result = f_open(fils[index], name, FA_OPEN_EXISTING | FA_READ);

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
		/// @todo seek to file end
	}
	//ENOENT
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
	FRESULT r = f_read(fils[index], (void*) ptr, (UINT) len, &br);
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
    st->st_mode = S_IFCHR;
    return 0;
}

clock_t _times(struct tms *buf)
{
    return -1;
}

int _unlink(char *name)
{
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
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
