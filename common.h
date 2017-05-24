#ifndef COMMONHEADER_H
#define COMMONHEADER_H

typedef enum {
    QResult_Success = 0,
    QResult_Failure,
    QResult_IndexOutOfRange,
    QResult_DataOutOfRange,
    QResult_IncorrectData,
    QResult_NullPointer,
    QResult_ActionUnavailable,
    QResult_NotFound,
    QResult_NotImplemented,
    QResult_UnexpectedError
} QResultStatus;


#endif // COMMONHEADER_H
