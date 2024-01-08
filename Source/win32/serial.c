#include <Windows.h>

#include <stdio.h>
#include <string.h>

#include <zstack/types.h>

#define PORTNAME_SIZE (16)
#define NAME_LEN      (100)

unsigned int serial_open(unsigned char id) {
  HANDLE hCom;
  DCB dcb;
  int fd;
  char port[PORTNAME_SIZE];

  memset(port, 0, PORTNAME_SIZE);
  if (id <= 10) {
    snprintf(port, PORTNAME_SIZE, "COM%d", id);
  }
  else {
    snprintf(port, PORTNAME_SIZE, "\\\\.\\COM%d", id);
  }

  hCom = CreateFileA(port,
    GENERIC_READ | GENERIC_WRITE,
    0,    // don't share
    NULL, // security
    OPEN_EXISTING,
    0, // Sync
    NULL);

  if (hCom == INVALID_HANDLE_VALUE) {
    printf("open %s failed\n", port);
    return -1;
  }

  // 2. configure rx/tx buffer, 1024 -> actually 4096, why?
  SetupComm(hCom, 1024, 1024);

  GetCommState(hCom, &dcb);
  dcb.BaudRate = 115200;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  SetCommState(hCom, &dcb);

  // If not set, ReadFile return immediately with TRUE
  // Refer to wiki: windows serial
  // Unit: ms
  // Total timeout: ReadTotalTimeoutMultiplier * Count + ReadTotalTimeoutConstant
  // 115200 => interval in two bits is ? < 1ms
  COMMTIMEOUTS timeOuts;
  timeOuts.ReadIntervalTimeout = 1; // TODO: delay the read time
  timeOuts.ReadTotalTimeoutMultiplier = 1;
  timeOuts.ReadTotalTimeoutConstant = 1;
  timeOuts.WriteTotalTimeoutMultiplier = 1;
  timeOuts.WriteTotalTimeoutConstant = 1;
  SetCommTimeouts(hCom, &timeOuts);

  // Clear tx rx buffer
  PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);

  // TODO
  //fd = ((u64)hCom) & 0xFFFFFFFF; // remove warning C4311

  fd = (U32)(hCom);

  return fd;
}

unsigned int serial_open_async(unsigned char id) {
  HANDLE hCom;
  DCB dcb;
  int fd;
  char port[PORTNAME_SIZE];

  memset(port, 0, PORTNAME_SIZE);
  if (id <= 10) {
    snprintf(port, PORTNAME_SIZE, "COM%d", id);
  }
  else {
    snprintf(port, PORTNAME_SIZE, "\\\\.\\COM%d", id);
  }

  hCom = CreateFileA(port,
    GENERIC_READ | GENERIC_WRITE,
    0,    // don't share
    NULL, // security
    OPEN_EXISTING,
    FILE_FLAG_OVERLAPPED, // ASync
    NULL);

  if (hCom == INVALID_HANDLE_VALUE) {
    printf("open %s failed\n", port);
    return -1;
  }

  // 2. configure
  SetupComm(hCom, 1024, 1024);

  GetCommState(hCom, &dcb);
  dcb.BaudRate = 115200;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  SetCommState(hCom, &dcb);

  // If not set, ReadFile return immediately with TRUE
  // Refer to wiki: windows serial
  COMMTIMEOUTS timeOuts;
  timeOuts.ReadIntervalTimeout = 1; // TODO: delay the read time
  timeOuts.ReadTotalTimeoutMultiplier = 1;
  timeOuts.ReadTotalTimeoutConstant = 1;
  timeOuts.WriteTotalTimeoutMultiplier = 1;
  timeOuts.WriteTotalTimeoutConstant = 1;
  SetCommTimeouts(hCom, &timeOuts);

  PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);

  // TODO
  //fd = ((u64)hCom) & 0xFFFFFFFF; // remove warning C4311

  fd = (U32)hCom;

  return fd;
}

unsigned int serial_rxlen(unsigned int fd)
{
  HANDLE handle = (HANDLE)fd; // remove warning C4312
  DWORD dwErrorFlags;
  COMSTAT comStat;

  ClearCommError(handle, &dwErrorFlags, &comStat);

  return comStat.cbInQue;
}

unsigned int serial_read(unsigned int fd, unsigned char* buffer, unsigned int length, int timeout) {
  DWORD bytesread;
  HANDLE handle = (HANDLE)fd; // remove warning C4312
  DWORD dwErrorFlags;
  COMSTAT comStat;
  DWORD bytes2read;
  BOOL read_state;

  ClearCommError(handle, &dwErrorFlags, &comStat);
  if (!comStat.cbInQue)
    return 0;

  bytes2read = comStat.cbInQue > length ? length : comStat.cbInQue;

  read_state = ReadFile(handle, buffer, bytes2read, &bytesread, NULL);
  if (!read_state) {
    return 0;
  }

  return bytesread;
}

unsigned int serial_read_async(unsigned int fd, unsigned char* buffer, unsigned int length, int timeout) {
  HANDLE handle = (HANDLE)fd; // remove warning C4312
  DWORD dwErrorFlags;
  COMSTAT comStat;
  DWORD bytes2read;

  // If no data in rxbuf, return directly
  ClearCommError(handle, &dwErrorFlags, &comStat);
  if (!comStat.cbInQue)
    return 0;

  bytes2read = comStat.cbInQue > length ? length : comStat.cbInQue;

#if 0
  memset(&overlap, 0, sizeof(OVERLAPPED));
  overlap.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
  if (NULL == overlap.hEvent)
    return 0;

  // ASYNC: always return false
  if (FALSE == ReadFile(handle, buffer, length, &wCount, &overlap)) {
#if 0
    if (WAIT_OBJECT_0 == WaitForSingleObject(overlap.hEvent, timeout)) {
      if (TRUE == GetOverlappedResult(handle, &overlap, &wCount, FALSE)) {
        return wCount;
      }
    }
#endif

#if 1
    if (GetLastError() == ERROR_IO_PENDING) {
      // set TRUE, wait until 
      GetOverlappedResult(handle, &overlap, &wCount, FALSE);
    }
    else {
      ClearCommError(handle, &dwErrorFlags, &comStat);
      CloseHandle(overlap.hEvent);
    }
#endif
  }
#endif

  return 0;
}

unsigned int serial_write_async(unsigned int fd, unsigned char* buffer, unsigned int length) {
  unsigned int dwBytesWrite;
  COMSTAT ComStat;
  DWORD dwErrorFlags;
  BOOL bWriteStat;
  OVERLAPPED overlap;
  HANDLE handle = (HANDLE)(unsigned long long)fd; // remove warning C4312

  memset(&overlap, 0, sizeof(OVERLAPPED));

  PurgeComm(handle, PURGE_RXABORT);

  ClearCommError(handle, &dwErrorFlags, &ComStat);

  bWriteStat = WriteFile(handle, buffer, length, &dwBytesWrite, &overlap);
  if (!bWriteStat) {
    if (GetLastError() == ERROR_IO_PENDING) {
      GetOverlappedResult(handle, &overlap, &dwBytesWrite, TRUE);
    }
    else {
      printf("error write: %d\n", GetLastError());
      return -1;
    }
  }

  return dwBytesWrite;
}

unsigned int serial_write(unsigned int fd, unsigned char* buffer, unsigned int length) {
  unsigned int dwBytesWrite;
  COMSTAT ComStat;
  DWORD dwErrorFlags;
  BOOL bWriteStat;
  HANDLE handle = (HANDLE)(unsigned long long)fd; // remove warning C4312

  ClearCommError(handle, &dwErrorFlags, &ComStat);

  bWriteStat = WriteFile(handle, buffer, length, &dwBytesWrite, NULL);
  if (!bWriteStat) {
    return 0;
  }

  return dwBytesWrite;
}

void serial_port_list(void) {
  HKEY hKey;
  LPCTSTR lpSubKey = TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM\\");
  char szValueName[NAME_LEN];
  BYTE szPortName[NAME_LEN];
  LONG status;
  DWORD dwIndex = 0;
  DWORD dwSizeValueName;
  DWORD dwSizeofPortName;
  DWORD Type;

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_READ, &hKey) !=
    ERROR_SUCCESS) {
    return;
  }

  do {
    dwSizeValueName = NAME_LEN;
    dwSizeofPortName = NAME_LEN;

    status = RegEnumValueA(hKey, dwIndex++, szValueName, &dwSizeValueName, NULL,
      &Type, szPortName, &dwSizeofPortName);
    if (status == ERROR_SUCCESS) {
      printf("%s\n", (char*)szPortName);
      fflush(stdout);
    }
  } while ((status != ERROR_NO_MORE_ITEMS));

  RegCloseKey(hKey);
}
