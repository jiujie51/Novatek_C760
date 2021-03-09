#include "Type.h"
#include "DataQueue.h"
#include "cJSON.h"
#include "HwMem.h"


#define __MODULE__ MDALG
#define __DBGLVL__ 1        //ERROR mode, show err, wrn only
#define __DBGFLT__ "*"      //*=All
#include "DebugModule.h"

//static char szDataBase[256*1024];
//static int data_offset = 0;

static int isFullDataQueue(DataQueue_t *pDataQueue)
{
    if((pDataQueue->rear+1)%pDataQueue->slot_size == pDataQueue->front){
        return 0;
    }
    return -1;
}

static int isEmptyDataQueue(DataQueue_t *pDataQueue)
{
    if(pDataQueue->front == pDataQueue->rear){
        return 0;
    }
    return -1;
}

int QH_DataQueueCreate(DataQueue_t *pDataQueue, void *pDataQueueBase, unsigned int ElemSize, unsigned int ElemNum)
{
    if(pDataQueue == NULL){
        debug_msg("<QH_DataQueueCreate> the parameter pDataQueue is NULL\r\n");
        return -1;
    }
    if(pDataQueue->inited){
        debug_msg("<QH_DataQueueCreate> the data queue had inited\r\n");
        return -1;
    }
    //debug_msg("_____________DataQueue<%d:%d:%d>___________\r\n", ElemSize, ElemNum, ElemSize * (ElemNum + 1));
#if 0
    if((data_offset+ElemSize * (ElemNum + 1)) > sizeof(szDataBase)){
        debug_msg("<QH_DataQueueCreate> failed to malloc memory\r\n");
        return -1;
    }
    pDataQueue->pBase = szDataBase+data_offset;//cJSON_Malloc(ElemSize * (ElemNum + 1));
    data_offset += ElemSize * (ElemNum + 1);
#else
    pDataQueue->pBase = cJSON_Malloc(ElemSize * (ElemNum + 1));
    if(!pDataQueue->pBase){
        debug_msg("<QH_DataQueueCreate> failed to malloc memory\r\n");
        return -1;
    }
    debug_msg("<QH_DataQueueCreate> Base address<%p>\r\n", pDataQueue->pBase);
#endif
    pDataQueue->elem_size = ElemSize;
    pDataQueue->slot_size = (ElemNum + 1);
    pDataQueue->count = 0;
    pDataQueue->front = 0;
    pDataQueue->rear  = 0;
    pDataQueue->inited= 1;
    return 0;
}
int QH_DataQueueDelete(DataQueue_t *pDataQueue)
{
    if(pDataQueue == NULL){
        debug_msg("<QH_DataQueueCreate> the parameter pDataQueue is NULL\r\n");
        return -1;
    }
    if(!pDataQueue->inited){
        debug_msg("<QH_DataQueueDelete> the data queue is not inited\r\n");
        return -1;
    }
    cJSON_Free(pDataQueue->pBase);
    pDataQueue->pBase = NULL;
    pDataQueue->elem_size = 0;
    pDataQueue->slot_size = 0;
    pDataQueue->count = 0;
    pDataQueue->front = pDataQueue->rear = 0;
    pDataQueue->inited = 0;
    return 0;
}

int QH_DataQueueFlush(DataQueue_t *pDataQueue, unsigned int Number)
{
    if(pDataQueue == NULL){
        debug_msg("<QH_DataQueueFlush> the parameter pDataQueue is NULL\r\n");
        return -1;
    }
    if(!pDataQueue->inited){
        debug_msg("<QH_DataQueueFlush> the data queue is not inited\r\n");
        return -1;
    }
    if(pDataQueue->count > Number){
        pDataQueue->front = (pDataQueue->front+Number)%pDataQueue->slot_size;
        pDataQueue->count -= Number;
    }
    else{
        pDataQueue->front = pDataQueue->rear;
        pDataQueue->count = 0;
    }
    return 0;
}
int QH_DataQueueRecv(DataQueue_t *pDataQueue, void *pDataDst, unsigned int Timeout)
{
    if(pDataQueue == NULL || pDataDst == NULL){
        debug_msg("<QH_DataQueueRecv> the parameter pDataQueue or pDataDst is NULL\r\n");
        return -1;
    }
    if(!pDataQueue->inited){
        debug_msg("<QH_DataQueueRecv> the data queue is not inited\r\n");
        return -1;
    }
    if(!isEmptyDataQueue(pDataQueue)){
        //debug_msg("<QH_DataQueueRecv> the data queue is empty\r\n");
        return -1;
    }
    //hwmem_open();
	//hwmem_memcpy((UINT32)pDataDst, (UINT32)(pDataQueue->pBase+pDataQueue->front*pDataQueue->elem_size), pDataQueue->elem_size);
	//hwmem_close();
	memcpy(pDataDst, pDataQueue->pBase+pDataQueue->front*pDataQueue->elem_size, pDataQueue->elem_size);
    pDataQueue->count--;
    pDataQueue->front = (pDataQueue->front+1)%pDataQueue->slot_size;
    return 0;
}


int QH_DataQueueSend(DataQueue_t *pDataQueue, void *pDataSrc, unsigned int Timeout)
{
    if(pDataQueue == NULL){
        debug_msg("<QH_DataQueueSend> the parameter pDataQueue is NULL\r\n");
        return -1;
    }
    if(!pDataQueue->inited){
        debug_msg("<QH_DataQueueSend> the data queue is not inited\r\n");
        return -1;
    }
    if(!isFullDataQueue(pDataQueue)){
        //debug_msg("<QH_DataQueueSend> the data queue is full\r\n");
        return -1;
    }
    //hwmem_open();
	//hwmem_memcpy((UINT32)(pDataQueue->pBase+pDataQueue->rear*pDataQueue->elem_size), (UINT32)pDataSrc, pDataQueue->elem_size);
	//hwmem_close();
	memcpy(pDataQueue->pBase+pDataQueue->rear*pDataQueue->elem_size, pDataSrc, pDataQueue->elem_size);
    pDataQueue->count++;
    pDataQueue->rear = (pDataQueue->rear+1) % pDataQueue->slot_size;
    return 0;
}
int QH_DataQueueQuery(DataQueue_t *pDataQueue, unsigned int *pCurCount, unsigned int *pCurRemainedSpace)
{
    if(pDataQueue == NULL){
        debug_msg("<QH_DataQueueQuery> the parameter pDataQueue is NULL\r\n");
        return -1;
    }
    if(!pDataQueue->inited){
        debug_msg("<QH_DataQueueQuery> the data queue is not inited\r\n");
        return -1;
    }
    *pCurCount = pDataQueue->count;
    *pCurRemainedSpace = pDataQueue->slot_size - pDataQueue->count;
    return 0;
}


