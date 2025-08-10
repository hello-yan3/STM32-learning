/**
  ******************************************************************************
  * @file    protocol.c
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   野火PID调试助手通讯协议解析
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./protocol/protocol.h"
#include <string.h>
#include "./pid/bsp_pid.h"
#include "./tim/bsp_basic_tim.h"


/* 数据 环形队列 存放结构体 */
struct prot_frame_parser_t
{
    uint8_t *recv_ptr;    // 接收数据指针，指向保存的数据
    uint16_t r_oft;       // 读偏移
    uint16_t w_oft;       // 写偏移 
    uint16_t frame_len;   // 帧长度
    uint16_t found_frame_head;   // 帧头
};
// 定义结构体：parser
static struct prot_frame_parser_t    parser;  
// 定义数据接收缓冲区
static uint8_t recv_buf[PROT_FRAME_LEN_RECV];

/**
  * @brief 计算校验和
  * @param init：初始值
  * @param ptr：需要计算的数据
  * @param len：需要计算的长度
  * @retval 校验和
  */
static uint8_t check_sum(uint8_t init, uint8_t *ptr, uint8_t len)
{
  uint8_t sum = init;
  
  while(len--)
  {
    sum += *ptr;
    ptr++;
  }
  
  return sum;
}

/**
 * @brief   获取接收到的数据帧的帧类型（帧命令）
 * @param   *frame:  数据帧
 * @param   head_oft: 帧头的偏移位置
 * @return  帧长度.
 */
static uint8_t get_frame_type(uint8_t *frame, uint16_t head_oft)
{
    return (frame[(head_oft + CMD_INDEX_VAL) % PROT_FRAME_LEN_RECV] & 0xFF);
}

/**
 * @brief   获取接收到的数据帧的帧长度
 * @param   *buf:  数据缓冲区. 数据缓冲区.parser.recv_ptr = recv_buf;
 * @param   head_oft: 帧头的偏移位置, parser.r_oft读偏移
 * @return  帧长度.
 */
static uint16_t get_frame_len(uint8_t *frame, uint16_t head_oft)
{
    return ((frame[(head_oft + LEN_INDEX_VAL + 0) % PROT_FRAME_LEN_RECV] <<  0) |
            (frame[(head_oft + LEN_INDEX_VAL + 1) % PROT_FRAME_LEN_RECV] <<  8) |
            (frame[(head_oft + LEN_INDEX_VAL + 2) % PROT_FRAME_LEN_RECV] << 16) |
            (frame[(head_oft + LEN_INDEX_VAL + 3) % PROT_FRAME_LEN_RECV] << 24));    // 合成帧长度
}

/**
 * @brief   获取接收到的数据帧的校验和数据
 * @param   *frame:  数据缓冲区.
 * @param   head_oft: 帧头的偏移位置
 * @param   head_oft: 帧长
 * @return  帧长度.
 */
static uint8_t get_frame_checksum(uint8_t *frame, uint16_t head_oft, uint16_t frame_len)
{
    return (frame[(head_oft + frame_len - 1) % PROT_FRAME_LEN_RECV]);
}

/**
 * @brief   查找帧头
 * @param   *buf:  数据缓冲区.parser.recv_ptr = recv_buf;
 * @param   ring_buf_len: 缓冲区大小 PROT_FRAME_LEN_RECV
 * @param   start: 起始位置，parser.r_oft读偏移
 * @param   len: 需要查找的长度，need_to_parse_len新接收到的数据的长度
 * @return  -1：没有找到帧头，其他值：帧头的位置.
 */
static int32_t recvbuf_find_header(uint8_t *buf, uint16_t ring_buf_len, uint16_t start, uint16_t len)
{
    uint16_t i = 0;

    for (i = 0; i < (len - 3); i++)
    {
        if (((buf[(start + i + 0) % ring_buf_len] <<  0) |
             (buf[(start + i + 1) % ring_buf_len] <<  8) |
             (buf[(start + i + 2) % ring_buf_len] << 16) |
             (buf[(start + i + 3) % ring_buf_len] << 24)) == FRAME_HEADER)
        {
            return ((start + i) % ring_buf_len);
        }
    }
    return -1;
}

/**
 * @brief   计算未解析的数据长度
 * @param   frame_len:  现在已经接收到的数据帧长度
 * @param   ring_buf_len: 缓冲区大小 PROT_FRAME_LEN_RECV
 * @param   start: 起始位置，parser.r_oft读偏移
 * @param   end: 结束位置，parser.w_oft写偏移
 * @return  未解析的数据长度 
 */
static int32_t recvbuf_get_len_to_parse(uint16_t frame_len, uint16_t ring_buf_len,uint16_t start, uint16_t end)
{   
    uint16_t unparsed_data_len = 0;   // 未解析的数据帧长度
 
    if (start <= end)
        unparsed_data_len = end - start;
    else
        unparsed_data_len = ring_buf_len - start + end;

    /* frame_len初始值为0，
       在找到帧头，读取帧长度位后，确定帧长度，
       如果帧长度比现在已经接收到的数据帧长度大，说明数据没有接收全
       此时直接返回0，返回空指令
    */
    if (frame_len > unparsed_data_len)
        return 0;
    else
        return unparsed_data_len;
}

/**
 * @brief   接收数据写入缓冲区
 * @param   *buf:  数据缓冲区.
 * @param   ring_buf_len: 缓冲区大小
 * @param   w_oft: 写偏移
 * @param   *data: 需要写入的数据
 * @param   *data_len: 需要写入数据的长度
 * @return  void.
 */
static void recvbuf_put_data(uint8_t *buf, uint16_t ring_buf_len, uint16_t w_oft,
        uint8_t *data, uint16_t data_len)
{
    if ((w_oft + data_len) > ring_buf_len)               // 超过缓冲区尾
    {
        uint16_t data_len_part = ring_buf_len - w_oft;     // 缓冲区剩余长度

        /* 数据分两段写入缓冲区*/
        memcpy(buf + w_oft, data, data_len_part);                         // 写入缓冲区尾
        memcpy(buf, data + data_len_part, data_len - data_len_part);      // 写入缓冲区头
    }
    else
        memcpy(buf + w_oft, data, data_len);    // 数据写入缓冲区
}


/**********************************************
	此函数为数据接收解析函数：

	由于中断每次仅可接收一个字节数据
	因此需要通过帧头以及校验和判断一帧数据什么时候结束

**********************************************/
/**
 * @brief   查询帧类型（命令）
 * @param   *data:  存放接收到的数据帧的缓冲区
 * @param   data_len: 帧数据的大小
 * @return  帧类型（命令）.
 */
static uint8_t protocol_frame_parse(uint8_t *data, uint16_t *data_len)
{
    uint8_t frame_type = CMD_NONE;    // 帧指令; 空指令
    uint16_t need_to_parse_len = 0;   // 新接收到的数据的长度
    int16_t header_oft = -1;          // 偏移地址
    uint8_t checksum = 0;             // 校验和
    
		/****************** 以下为从原始数据缓冲区中获取数据帧帧头及数据帧长度的内容 ******************/
	  // 计算未解析的数据长度，parser.frame_len此时未更新值为0
    need_to_parse_len = recvbuf_get_len_to_parse(parser.frame_len, PROT_FRAME_LEN_RECV, parser.r_oft, parser.w_oft);    // 得到未解析的数据长度
    if (need_to_parse_len < 9)     // 新接收到的数据的长度<9，肯定还不能同时找到帧头和帧长度
			return frame_type;  // 帧指令; CMD_NONE: 空指令

    /* 还未找到帧头，需要进行查找 */
    if (0 == parser.found_frame_head)
    {
			  // 查找帧头
        header_oft = recvbuf_find_header(parser.recv_ptr, PROT_FRAME_LEN_RECV, parser.r_oft, need_to_parse_len);
        if (0 <= header_oft)  // 找到帧头
        {
            /* 已找到帧头*/
            parser.found_frame_head = 1;  // 已找到帧头
            parser.r_oft = header_oft;    // 读偏移 = 帧头位置
          
            /* 确认是否可以计算帧长*/
						// 数据帧的长度<9，肯定不是一帧完整数据
            if (recvbuf_get_len_to_parse(parser.frame_len, PROT_FRAME_LEN_RECV, parser.r_oft, parser.w_oft) < 9)
                return frame_type;  // 帧指令; CMD_NONE: 空指令
        }
        else    // 未找到帧头
        {
            /* 未解析的数据中依然未找到帧头，丢掉此次解析过的所有数据 */
					  /* -3原因: 同步头为四字节，可能存在未解析的数据中后三个字节刚好为同步头前三个字节的情况，
            因此查找同步头时，后三个字节将不会被丢弃 */
            parser.r_oft = ((parser.r_oft + need_to_parse_len - 3) % PROT_FRAME_LEN_RECV);
            return frame_type;  // 帧指令; CMD_NONE: 空指令
        }
    }
    
    /* 获得帧长，并确定是否可以进行数据解析*/
	  /* parser.frame_len未更新值为0 */
    if (0 == parser.frame_len) 
    {
			  // 得到帧长度，(即读取接收数据的帧长度位)
        parser.frame_len = get_frame_len(parser.recv_ptr, parser.r_oft);   
				// 新接收到的数据的长度 < 帧长度
        if(need_to_parse_len < parser.frame_len)
            return frame_type;
    }

		/****************** 以下为数据帧校验及将原始数据缓冲区中的数据帧拷贝至解析数据帧的内容 ******************/
    /* 帧头位置确认，且未解析的数据超过帧长，可以计算校验和*/
    if ((parser.frame_len + parser.r_oft - PROT_FRAME_LEN_CHECKSUM) > PROT_FRAME_LEN_RECV)
    {
        /* 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头 */
        checksum = check_sum(checksum, parser.recv_ptr + parser.r_oft, 
                PROT_FRAME_LEN_RECV - parser.r_oft);
        checksum = check_sum(checksum, parser.recv_ptr, parser.frame_len -
                PROT_FRAME_LEN_CHECKSUM + parser.r_oft - PROT_FRAME_LEN_RECV);
    }
    else 
    {
        /* 数据帧可以一次性取完*/
        checksum = check_sum(checksum, parser.recv_ptr + parser.r_oft, parser.frame_len - PROT_FRAME_LEN_CHECKSUM);
    }

    if (checksum == get_frame_checksum(parser.recv_ptr, parser.r_oft, parser.frame_len))
    {
        /* 校验成功，拷贝整帧数据到接收数据缓冲区 */
        if ((parser.r_oft + parser.frame_len) > PROT_FRAME_LEN_RECV) 
        {
            /* 数据帧被分为两部分，一部分在缓冲区尾，一部分在缓冲区头*/
            uint16_t data_len_part = PROT_FRAME_LEN_RECV - parser.r_oft;
            memcpy(data, parser.recv_ptr + parser.r_oft, data_len_part);
            memcpy(data + data_len_part, parser.recv_ptr, parser.frame_len - data_len_part);
        }
        else 
        {
            /* 数据帧可以一次性取完*/
            memcpy(data, parser.recv_ptr + parser.r_oft, parser.frame_len);
        }
				// 获得数据帧长度
        *data_len = parser.frame_len;
				// 获得帧类型
        frame_type = get_frame_type(parser.recv_ptr, parser.r_oft);

        /* 读偏移更新，已经读到的数据从缓冲区中清除 */
        parser.r_oft = (parser.r_oft + parser.frame_len) % PROT_FRAME_LEN_RECV;  // 读偏移更新
    }
    else  // 校验错误
    {
        /* 校验错误，说明之前找到的帧头只是偶然出现的废数据*/
        parser.r_oft = (parser.r_oft + 1) % PROT_FRAME_LEN_RECV;     // 读偏移向后移动一个数据
    }
    parser.frame_len = 0;         // 帧长度清零
    parser.found_frame_head = 0;  // 帧头清零
 
    return frame_type;
}

/*****************************用户调用函数****************************/

/**
 * @brief   初始化接收协议
 * @param   void
 * @return  初始化结果.
 */
uint8_t protocol_init(void)
{
	  // 结构体清空
    memset(&parser, 0, sizeof(struct prot_frame_parser_t));
    
    /* 初始化分配数据接收与解析缓冲区*/
    parser.recv_ptr = recv_buf;
   
    return 0;
}

/**
 * @brief   接收到的数据写入缓冲区，并修改写指针偏移
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
void protocol_data_recv(uint8_t *data, uint16_t data_len)
{
  	// 接收到的数据写入缓冲区		参数分别为: 缓冲区, 缓冲区长度, 数据写偏移, 待写入数据, 待写入数据长度
    recvbuf_put_data(parser.recv_ptr, PROT_FRAME_LEN_RECV, parser.w_oft, data, data_len);    // 接收数据
	  // 计算新的写偏移
    parser.w_oft = (parser.w_oft + data_len) % PROT_FRAME_LEN_RECV;                          // 计算写偏移
}

/**
 * @brief   接收的数据处理
 * @param   void
 * @return  -1：没有找到一个正确的命令.
 */
int8_t receiving_process(void)
{
  uint8_t frame_data[128];         // 要能放下最长的帧
  uint16_t frame_len = 0;          // 帧长度
  uint8_t cmd_type = CMD_NONE;     // 命令类型
  
  while(1)
  {
		// 查询帧类型（命令）
		// 查询后会讲数据帧放入frame_data, 数据帧长度放入frame_len, 返回值为命令类型
    cmd_type = protocol_frame_parse(frame_data, &frame_len);
    switch (cmd_type)
    {
      case CMD_NONE:
      {
        return -1;
      }

      case SET_P_I_D_CMD:
      {
        uint32_t temp0 = COMPOUND_32BIT(&frame_data[13]);
        uint32_t temp1 = COMPOUND_32BIT(&frame_data[17]);
        uint32_t temp2 = COMPOUND_32BIT(&frame_data[21]);
        
        float p_temp, i_temp, d_temp;
        
        p_temp = *(float *)&temp0;
        i_temp = *(float *)&temp1;
        d_temp = *(float *)&temp2;
        
        set_p_i_d(p_temp, i_temp, d_temp);    // 设置 P I D
      }
      break;

      case SET_TARGET_CMD:
      {
        int actual_temp = COMPOUND_32BIT(&frame_data[13]);    // 得到数据
        
        set_point = (actual_temp);    // 设置目标值
				pid.target_val = set_point;
      }
      break;
      
      case START_CMD:
      {
//        set_motor_enable();              // 启动电机
      }
      break;
      
      case STOP_CMD:
      {
//        set_motor_disable();              // 停止电机
      }
      break;
      
      case RESET_CMD:
      {
        HAL_NVIC_SystemReset();          // 复位系统
      }
      break;
      
      case SET_PERIOD_CMD:
      {
        uint32_t temp = COMPOUND_32BIT(&frame_data[13]);     // 周期数
        SET_BASIC_TIM_PERIOD(temp);                          // 设置定时器周期1~1000ms
      }
      break;

      default: 
        return -1;
    }
  }
}

/**
  * @brief 设置上位机的值(就是数据发送至上位机)
  * @param cmd： 命令
  * @param ch:   曲线通道
  * @param data：参数指针
  * @param num： 参数个数
  * @retval 无
  */
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num)
{
  uint8_t sum = 0;    // 校验和
  num *= 4;           // 一个参数 4 个字节
  
  static packet_head_t set_packet;
  
  set_packet.head = FRAME_HEADER;     // 包头 0x59485A53
  set_packet.len  = 0x0B + num;       // 包长, 0x0B为包头(4bytes)+通道地址(1bytes)+包长度(4bytes)+命令(1bytes)+校验和(1bytes)=0x0B
  set_packet.ch   = ch;               // 设置通道
  set_packet.cmd  = cmd;              // 设置命令
  
  sum = check_sum(0, (uint8_t *)&set_packet, sizeof(set_packet));       // 计算包头校验和
  sum = check_sum(sum, (uint8_t *)data, num);                           // 计算参数校验和
  
	// 串口发送数据
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&set_packet, sizeof(set_packet), 0xFFFFF);    // 发送数据头
  HAL_UART_Transmit(&UartHandle, (uint8_t *)data, num, 0xFFFFF);                          // 发送参数
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&sum, sizeof(sum), 0xFFFFF);                  // 发送校验和
}

/**********************************************************************************************/
