#include "protocol.h"
PDU *mkPDU(unsigned int uiMsgLen)
{
    int uiPDULen = sizeof(PDU) + uiMsgLen;
    PDU *pdu = (PDU*)malloc(uiPDULen);

    if(pdu == nullptr)exit(EXIT_FAILURE);

    memset(pdu,0,uiPDULen);//初始化所有字节（全部设为0）

    pdu->uiPDULen = uiPDULen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}
