/************************ (C) COPYLEFT 2018 Merafour *************************
* File Name          : merafour_queue_hid.h
* Author             : Merafour
* Last Modified Date : 01/21/2024
* Signature          : 生命不过是一场寂寞的独角戏
* Description        : 环形缓存.
********************************************************************************
* https://merafour.blog.163.com
* merafour@163.com
* https://github.com/merafour
******************************************************************************/
#ifndef _MERAFOUR_QUEUE_HID_H_
#define _MERAFOUR_QUEUE_HID_H_

#include <stdint.h>

#define  MQUEUE_HID_SIZE       32
struct mQueue_hid_t{
    uint8_t wPtr;
    uint8_t rPtr;
    uint8_t Datas[MQUEUE_HID_SIZE][64];
};
#define mQueue_hid_next(index,SIZE) {(index) = (index)+1; if((index)>=(SIZE)) (index)=0;}
/*_____________________________________________________________ API write ________________________________________________________________*/
static __inline void mQueue_hid_init(struct mQueue_hid_t* const _mq)
{
    __asm("CPSID  I"); // ARM 指令
    _mq->rPtr = 0;
    _mq->wPtr = 0;
    __asm("CPSIE  I");
}
// 判断缓存是否为空
/**
 * |_____r_____| r==w, empty
 * |_____w_____|
*/
static __inline uint8_t mQueue_hid_is_empty(const struct mQueue_hid_t* const _mq)
{
    const uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    if(rPtr == wPtr) return 1;  /* empty */
    return 0;
}
/**
 * |_____r_____| r==next(w), full
 * |____w______|
*/
static inline uint8_t mQueue_hid_is_full(const struct mQueue_hid_t* const _mq)
{
    const uint8_t rIndex = _mq->rPtr;
    // next(w)
    uint8_t wNext = _mq->wPtr+1;
    if(wNext>=MQUEUE_HID_SIZE) wNext=0;
    if(rIndex==wNext) return 1;   // full
    return 0;
}
// 获取缓存中数据长度
/**
 * w>=r |___r_______| w=next(r+size)
 *      |_____w_____| size=w-r
 * w<r  |_______r___| w=next(r+size)
 *      |_____w_____| size=(w+MQUEUE_HID_SIZE)-r
*/
static __inline uint8_t mQueue_hid_get_size(const struct mQueue_hid_t* const _mq)
{
    const uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    if(wPtr>=rPtr) return (wPtr-rPtr);
    else // wPtr<rPtr
    {
        return ((uint16_t)wPtr+MQUEUE_HID_SIZE-rPtr);
    }
}
/**
 * |_____r_____| r==w, empty
 * |_____w_____|
*/
static __inline void mQueue_hid_delete(struct mQueue_hid_t* const _mq, const uint8_t _size)
{
    uint8_t index;
    uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    for(index=0; index<_size; index++)
    {
        if(wPtr == rPtr) break;
        // next(r)
        rPtr++;
        if(rPtr>=MQUEUE_HID_SIZE) rPtr=0;
    }
	__asm("CPSID  I");
	_mq->rPtr = rPtr;
	__asm("CPSIE  I");
}
/**
 * |_____r_____| r==next(w), full
 * |____w______|
 * datas[w]=data, w=next(w)
*/
static __inline uint8_t mQueue_hid_write(struct mQueue_hid_t* const _mq, const uint8_t Datas[64])
{
    const uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    uint8_t wNext = wPtr+1;
    uint8_t ii;
    if(wNext>=MQUEUE_HID_SIZE) wNext=0;
	// r==next(w), full
    if(wNext != rPtr)
    {
        for(ii=0; ii<64; ii++) _mq->Datas[wPtr][ii] = Datas[ii];
        __asm("CPSID  I");
        _mq->wPtr = wNext;
        __asm("CPSIE  I");
        return 1;
    }
    return 0;
}
/**
 * |____r______| r==w, empty
 * |_____w_____|
 * data=datas[r], r=next(r)
*/
static __inline uint8_t mQueue_hid_read(struct mQueue_hid_t* const _mq, uint8_t Datas[64])
{
    uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    uint8_t ii;
    // r==w, empty
    if(rPtr != wPtr)
    {
        for(ii=0; ii<64; ii++) Datas[ii] = _mq->Datas[rPtr][ii];
        rPtr  = rPtr+1;
        if(rPtr>=MQUEUE_HID_SIZE) rPtr=0;
        __asm("CPSID  I");
        _mq->rPtr = rPtr;
        __asm("CPSIE  I");
        return 1;
    }
    return 0;
}
/**
 * |____r______| r==w, empty
 * |_____w_____|
 * data=datas[r],
*/
static __inline uint8_t mQueue_hid_peek(const struct mQueue_hid_t* const _mq, uint8_t Datas[64])
{
    uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    uint8_t ii;
    // r==w, empty
    if(rPtr != wPtr)
    {
        for(ii=0; ii<64; ii++) Datas[ii] = _mq->Datas[rPtr][ii];
        return 1;
    }
    return 0;
}
static __inline uint8_t mQueue_hid_peeks(const struct mQueue_hid_t* const _mq, uint8_t Datas[64], const uint8_t Size)
{
    uint8_t rPtr = _mq->rPtr;
    const uint8_t wPtr = _mq->wPtr;
    uint8_t ii;
    uint8_t cnt=0;
    for(cnt=0; cnt<Size; cnt++)
    {
        // r==w, empty
        if(wPtr == rPtr) break;
        for(ii=0; ii<64; ii++) Datas[cnt*64+ii] = _mq->Datas[rPtr][ii];
        rPtr  = rPtr+1;
        if(rPtr>=MQUEUE_HID_SIZE) rPtr=0;
    }
    return cnt;
}

#endif //_MERAFOUR_QUEUE_HID_H_
