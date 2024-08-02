#ifndef JOJ_ERROR_CODE_H
#define JOJ_ERROR_CODE_H

typedef enum ErrorCode_enum
{
    OK = 0,
    FAIL = 1,

    // Win32Window error codes
    ERR_WIN32_WINDOW_GET_HANDLE,
    ERR_WIN32_WINDOW_REGISTRATION,
    ERR_WIN32_WINDOW_HANDLE_CREATION,

} ErrorCode;

#endif // JOJ_ERROR_CODE_H