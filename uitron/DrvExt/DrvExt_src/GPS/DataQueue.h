#ifndef _DATA_QUEUE_H_
#define _DATA_QUEUE_H_

typedef struct _DataQueue{
    unsigned int inited;
    unsigned int front;
    unsigned int rear;
    unsigned int elem_size;
    unsigned int slot_size;
    unsigned int count;
    void *pBase;
}DataQueue_t;

extern int QH_DataQueueCreate(DataQueue_t *pDataQueue, void *pDataQueueBase, unsigned int ElemSize, unsigned int ElemNum);
extern int QH_DataQueueDelete(DataQueue_t *pDataQueue);
extern int QH_DataQueueFlush(DataQueue_t *pDataQueue, unsigned int num);
extern int QH_DataQueueRecv(DataQueue_t *pDataQueue, void *pDataDst, unsigned int Timeout);
extern int QH_DataQueueSend(DataQueue_t *pDataQueue, void *pDataSrc, unsigned int Timeout);
extern int QH_DataQueueQuery(DataQueue_t *pDataQueue, unsigned int *pCurCount, unsigned int *pCurRemainedSpace);

#endif