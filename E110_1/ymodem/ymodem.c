/*
 * File      : ymodem.c
 * ZHOUSAN ETENG CO.,LTD
 * COPYRIGHT (C) 2016 - 2026
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-15     sea_start    the first version
*/
#include <rthw.h>
#include "ymodem.h"

#include	"dfs_posix.h"

#if 1
static const unsigned short _Ccitt_Table[256] = {
   0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
   0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
   0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
   0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
   0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
   0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
   0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
   0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
   0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
   0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
   0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
   0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
   0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
   0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
   0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
   0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
   0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
   0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
   0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
   0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
   0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
   0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
   0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
   0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
   0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
   0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
   0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
   0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
   0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
   0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
   0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
   0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

////////////////////////////////////////////////////////////////////////////////
//
// ???: usMBCRC16 ()
//
// ??:
//   - ?? CRC16 = x16+x12+x5+1
// ??: CRC
//
////////////////////////////////////////////////////////////////////////////////
static rt_uint16_t CcittCrc16(rt_uint8_t *buff, rt_uint16_t len)
{
		rt_uint16_t  crc = 0;
	  rt_uint8_t   *p = buff;

		while (len--)
        crc = _Ccitt_Table[(crc >> 8 ^ *(p ++)) & 0xffU] ^ (crc << 8);		
		return  crc;
}
#endif

static struct rym_ctx *_rym_the_ctx;

static rt_err_t _rym_rx_ind(rt_device_t dev, rt_size_t size)
{
    return rt_sem_release(&_rym_the_ctx->sem);
}

/* SOH/STX + seq + payload + crc */
#define _RYM_SOH_PKG_SZ (1 + 2 + 128  + 2)
#define _RYM_STX_PKG_SZ (1 + 2 + 1024 + 2)

static enum rym_code _rym_read_code(struct rym_ctx *ctx, rt_tick_t timeout)
{
    /* Fast path */
    if (rt_device_read(ctx->dev, 0, ctx->buf, 1) == 1)
        return (enum rym_code) *ctx->buf;

    /* Slow path */
    do {
        rt_size_t rsz;

        /* No data yet, wait for one */
        if (rt_sem_take(&ctx->sem, timeout) != RT_EOK)
            return RYM_CODE_NONE;

        /* Try to read one */
        rsz = rt_device_read(ctx->dev, 0, ctx->buf, 1);
        if (rsz == 1)
            return (enum rym_code) *ctx->buf;
    } while (1);
}

/* the caller should at least alloc _RYM_STX_PKG_SZ buffer */
static rt_size_t _rym_read_data(struct rym_ctx *ctx, rt_size_t len)
{
    /* we should already have had the code */
    rt_uint8_t *buf = ctx->buf + 1;
    rt_size_t readlen = 0;

    do
    {
        readlen += rt_device_read(ctx->dev,
                0, buf+readlen, len-readlen);
        if (readlen >= len)
            return readlen;
    } while (rt_sem_take(&ctx->sem, RYM_WAIT_CHR_TICK) == RT_EOK);

    return readlen;
}

static rt_size_t _rym_putchar(struct rym_ctx *ctx, rt_uint8_t code)
{
    rt_device_write(ctx->dev, 0, &code, sizeof(code));
	rt_thread_delay(5);
    return 1;
}

static rt_err_t _rym_do_handshake(struct rym_ctx *ctx, int tm_sec)
{
    enum rym_code code;
    rt_size_t i;

    ctx->stage = RYM_STAGE_ESTABLISHING;
    /* send C every second, so the sender could know we are waiting for it. */
    for (i = 0; i < tm_sec; i++)
    {
        _rym_putchar(ctx, RYM_CODE_C);
        code = _rym_read_code(ctx,
                RYM_CHD_INTV_TICK);
        if (code == RYM_CODE_SOH)
            break;
    }
    if (i == tm_sec)
        return -RYM_ERR_TMO;

    i = _rym_read_data(ctx, _RYM_SOH_PKG_SZ-1);
    if (i != (_RYM_SOH_PKG_SZ-1))
        return -RYM_ERR_DSZ;

    /* sanity check */
    if (ctx->buf[1] != 0 || ctx->buf[2] != 0xFF)
        return -RYM_ERR_SEQ;

    if (CcittCrc16(ctx->buf + 3, _RYM_SOH_PKG_SZ - 3))
        return -RYM_ERR_CRC;
    /* congratulations, check passed. */
    if (ctx->on_begin && ctx->on_begin(ctx, ctx->buf + 3, 128) != RYM_CODE_ACK)
        return -RYM_ERR_CAN;

    return RT_EOK;
}

static rt_err_t _rym_trans_data( struct rym_ctx *ctx, rt_size_t data_sz, enum rym_code *code)
{

    /* seq + data + crc */
    rt_size_t i = _rym_read_data(ctx, data_sz + 4);
    if (i != data_sz + 4)
        return -RYM_ERR_DSZ;

    if ((ctx->buf[1] + ctx->buf[2]) != 0xFF)
    {
        return -RYM_ERR_SEQ;
    }

    /* As we are sending C continuously, there is a chance that the
     * sender(remote) receive an C after sending the first handshake package.
     * So the sender will interpret it as NAK and re-send the package. So we
     * just ignore it and proceed. */
    if (ctx->stage == RYM_STAGE_ESTABLISHED && ctx->buf[1] == 0x00)
    {
        *code = RYM_CODE_NONE;
        return RT_EOK;
    }

    ctx->stage = RYM_STAGE_TRANSMITTING;

    /* sanity check */
    if (CcittCrc16(ctx->buf + 3, data_sz + 2))
        return -RYM_ERR_CRC;
		
    /* congratulations, check passed. */
    if (ctx->on_data)
        *code = ctx->on_data(ctx, ctx->buf + 3, data_sz);
    else
        *code = RYM_CODE_ACK;
    return RT_EOK;
}

static rt_err_t _rym_do_trans(struct rym_ctx *ctx)
{
	int err_cnt = 0;
	
    _rym_putchar(ctx, RYM_CODE_ACK);
    _rym_putchar(ctx, RYM_CODE_C);
    ctx->stage = RYM_STAGE_ESTABLISHED;
		rt_thread_delay(10);
	
    while (1)
    {
        rt_err_t err;
        enum rym_code code;
        rt_size_t data_sz, i;

        code = _rym_read_code(ctx, RYM_WAIT_PKG_TICK);
        switch (code)
        {
						case RYM_CODE_SOH:
								data_sz = 128;
								break;
						case RYM_CODE_STX:
								data_sz = 1024;
								break;
						case RYM_CODE_EOT:
								return RT_EOK;
						default:
							err_cnt++;
							
							if(err_cnt > 3)
								return -RYM_ERR_CODE;
							break;
        };
		
			err = _rym_trans_data(ctx, data_sz, &code);
			if (err != RT_EOK)
			{
				return err;
			}
        switch (code)
        {
        case RYM_CODE_CAN:
            /* the spec require multiple CAN */
            for (i = 0; i < RYM_END_SESSION_SEND_CAN_NUM; i++) {
                _rym_putchar(ctx, RYM_CODE_CAN);
            }
            return -RYM_ERR_CAN;
        case RYM_CODE_ACK:
            _rym_putchar(ctx, RYM_CODE_ACK);
		//	num++;
					 
            break;
        default:
            // wrong code
            break;
        };
    }
}

static rt_err_t _rym_do_fin(struct rym_ctx *ctx)
{
    enum rym_code code;
    rt_size_t i;
	
    ctx->stage = RYM_STAGE_FINISHING;
    /* we already got one EOT in the caller. invoke the callback if there is
     * one. */
    if (ctx->on_end)
        ctx->on_end(ctx, ctx->buf + 3, 128);

    _rym_putchar(ctx, RYM_CODE_NAK);

	code = _rym_read_code(ctx, RYM_WAIT_PKG_TICK);
	if (code != RYM_CODE_EOT)
		return -RYM_ERR_CODE;

	_rym_putchar(ctx, RYM_CODE_ACK);
	_rym_putchar(ctx, RYM_CODE_C);

	code = _rym_read_code(ctx, RYM_WAIT_PKG_TICK);		

	if(code == RYM_CODE_EOT)
	{
		_rym_putchar(ctx, RYM_CODE_ACK);
		_rym_putchar(ctx, RYM_CODE_C);

		code = _rym_read_code(ctx, RYM_WAIT_PKG_TICK);		
	}
	
    if (code != RYM_CODE_SOH)
        return -RYM_ERR_CODE;
	
    i = _rym_read_data(ctx, _RYM_SOH_PKG_SZ - 1);
    if (i != (_RYM_SOH_PKG_SZ-1))
        return -RYM_ERR_DSZ;

    /* sanity check
     *
     * TODO: multiple files transmission
     */
    if (ctx->buf[1] != 0 || ctx->buf[2] != 0xFF)
        return -RYM_ERR_SEQ;

    if (CcittCrc16(ctx->buf + 3, _RYM_SOH_PKG_SZ - 3))
        return -RYM_ERR_CRC;
    /* congratulations, check passed. */
    ctx->stage = RYM_STAGE_FINISHED;

    /* put the last ACK */
    _rym_putchar(ctx, RYM_CODE_ACK);

    return RT_EOK;
}

static rt_err_t _rym_do_recv(struct rym_ctx *ctx, int handshake_timeout)
{
    rt_err_t err;

    ctx->stage = RYM_STAGE_NONE;

    ctx->buf = rt_malloc(_RYM_STX_PKG_SZ);
    if (ctx->buf == RT_NULL)
        return -RT_ENOMEM;

    err = _rym_do_handshake(ctx, handshake_timeout);
    if (err != RT_EOK)
        return err;

    err = _rym_do_trans(ctx);
    if (err != RT_EOK)
        return err;

    return _rym_do_fin(ctx);
	 
}

rt_err_t rym_recv_on_device(
        struct rym_ctx *ctx,
        rt_device_t dev,
        rt_uint16_t oflag,
        rym_callback on_begin,
        rym_callback on_data,
        rym_callback on_end,
        int handshake_timeout)
{
    rt_err_t res;
    rt_err_t (*odev_rx_ind)(rt_device_t dev, rt_size_t size);
    rt_uint16_t odev_flag;
    int int_lvl;

    RT_ASSERT(_rym_the_ctx == 0);
    _rym_the_ctx = ctx;

    ctx->on_begin = on_begin;
    ctx->on_data  = on_data;
    ctx->on_end   = on_end;
    ctx->dev      = dev;
    rt_sem_init(&ctx->sem, "rymsem", 0, RT_IPC_FLAG_FIFO);

    odev_rx_ind = dev->rx_indicate;
    /* no data should be received before the device has been fully setted up.
     */
    int_lvl = rt_hw_interrupt_disable();
    rt_device_set_rx_indicate(dev, _rym_rx_ind);

    odev_flag = dev->flag;
    /* make sure the device don't change the content. */
    dev->flag &= ~RT_DEVICE_FLAG_STREAM;
    rt_hw_interrupt_enable(int_lvl);



    res = rt_device_open(dev, oflag);
    if (res != RT_EOK)
        goto __exit;

    res = _rym_do_recv(ctx, handshake_timeout);
	

    rt_device_close(dev);
	

__exit:
    /* no rx_ind should be called before the callback has been fully detached.
     */
    int_lvl = rt_hw_interrupt_disable();
    rt_sem_detach(&ctx->sem);

    dev->flag = odev_flag;
    rt_device_set_rx_indicate(dev, odev_rx_ind);
    rt_hw_interrupt_enable(int_lvl);

    rt_free(ctx->buf);
    _rym_the_ctx = RT_NULL;

    return res;
}
