/**
  ******************************************************************************
  * @file    protocol.c
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   Ұ��PID��������ͨѶЭ�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./protocol/protocol.h"
#include <string.h>
#include "./pid/bsp_pid.h"
#include "./tim/bsp_basic_tim.h"


/* ���� ���ζ��� ��Žṹ�� */
struct prot_frame_parser_t
{
    uint8_t *recv_ptr;    // ��������ָ�룬ָ�򱣴������
    uint16_t r_oft;       // ��ƫ��
    uint16_t w_oft;       // дƫ�� 
    uint16_t frame_len;   // ֡����
    uint16_t found_frame_head;   // ֡ͷ
};
// ����ṹ�壺parser
static struct prot_frame_parser_t    parser;  
// �������ݽ��ջ�����
static uint8_t recv_buf[PROT_FRAME_LEN_RECV];

/**
  * @brief ����У���
  * @param init����ʼֵ
  * @param ptr����Ҫ���������
  * @param len����Ҫ����ĳ���
  * @retval У���
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
 * @brief   ��ȡ���յ�������֡��֡���ͣ�֡���
 * @param   *frame:  ����֡
 * @param   head_oft: ֡ͷ��ƫ��λ��
 * @return  ֡����.
 */
static uint8_t get_frame_type(uint8_t *frame, uint16_t head_oft)
{
    return (frame[(head_oft + CMD_INDEX_VAL) % PROT_FRAME_LEN_RECV] & 0xFF);
}

/**
 * @brief   ��ȡ���յ�������֡��֡����
 * @param   *buf:  ���ݻ�����. ���ݻ�����.parser.recv_ptr = recv_buf;
 * @param   head_oft: ֡ͷ��ƫ��λ��, parser.r_oft��ƫ��
 * @return  ֡����.
 */
static uint16_t get_frame_len(uint8_t *frame, uint16_t head_oft)
{
    return ((frame[(head_oft + LEN_INDEX_VAL + 0) % PROT_FRAME_LEN_RECV] <<  0) |
            (frame[(head_oft + LEN_INDEX_VAL + 1) % PROT_FRAME_LEN_RECV] <<  8) |
            (frame[(head_oft + LEN_INDEX_VAL + 2) % PROT_FRAME_LEN_RECV] << 16) |
            (frame[(head_oft + LEN_INDEX_VAL + 3) % PROT_FRAME_LEN_RECV] << 24));    // �ϳ�֡����
}

/**
 * @brief   ��ȡ���յ�������֡��У�������
 * @param   *frame:  ���ݻ�����.
 * @param   head_oft: ֡ͷ��ƫ��λ��
 * @param   head_oft: ֡��
 * @return  ֡����.
 */
static uint8_t get_frame_checksum(uint8_t *frame, uint16_t head_oft, uint16_t frame_len)
{
    return (frame[(head_oft + frame_len - 1) % PROT_FRAME_LEN_RECV]);
}

/**
 * @brief   ����֡ͷ
 * @param   *buf:  ���ݻ�����.parser.recv_ptr = recv_buf;
 * @param   ring_buf_len: ��������С PROT_FRAME_LEN_RECV
 * @param   start: ��ʼλ�ã�parser.r_oft��ƫ��
 * @param   len: ��Ҫ���ҵĳ��ȣ�need_to_parse_len�½��յ������ݵĳ���
 * @return  -1��û���ҵ�֡ͷ������ֵ��֡ͷ��λ��.
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
 * @brief   ����δ���������ݳ���
 * @param   frame_len:  �����Ѿ����յ�������֡����
 * @param   ring_buf_len: ��������С PROT_FRAME_LEN_RECV
 * @param   start: ��ʼλ�ã�parser.r_oft��ƫ��
 * @param   end: ����λ�ã�parser.w_oftдƫ��
 * @return  δ���������ݳ��� 
 */
static int32_t recvbuf_get_len_to_parse(uint16_t frame_len, uint16_t ring_buf_len,uint16_t start, uint16_t end)
{   
    uint16_t unparsed_data_len = 0;   // δ����������֡����
 
    if (start <= end)
        unparsed_data_len = end - start;
    else
        unparsed_data_len = ring_buf_len - start + end;

    /* frame_len��ʼֵΪ0��
       ���ҵ�֡ͷ����ȡ֡����λ��ȷ��֡���ȣ�
       ���֡���ȱ������Ѿ����յ�������֡���ȴ�˵������û�н���ȫ
       ��ʱֱ�ӷ���0�����ؿ�ָ��
    */
    if (frame_len > unparsed_data_len)
        return 0;
    else
        return unparsed_data_len;
}

/**
 * @brief   ��������д�뻺����
 * @param   *buf:  ���ݻ�����.
 * @param   ring_buf_len: ��������С
 * @param   w_oft: дƫ��
 * @param   *data: ��Ҫд�������
 * @param   *data_len: ��Ҫд�����ݵĳ���
 * @return  void.
 */
static void recvbuf_put_data(uint8_t *buf, uint16_t ring_buf_len, uint16_t w_oft,
        uint8_t *data, uint16_t data_len)
{
    if ((w_oft + data_len) > ring_buf_len)               // ����������β
    {
        uint16_t data_len_part = ring_buf_len - w_oft;     // ������ʣ�೤��

        /* ���ݷ�����д�뻺����*/
        memcpy(buf + w_oft, data, data_len_part);                         // д�뻺����β
        memcpy(buf, data + data_len_part, data_len - data_len_part);      // д�뻺����ͷ
    }
    else
        memcpy(buf + w_oft, data, data_len);    // ����д�뻺����
}


/**********************************************
	�˺���Ϊ���ݽ��ս���������

	�����ж�ÿ�ν��ɽ���һ���ֽ�����
	�����Ҫͨ��֡ͷ�Լ�У����ж�һ֡����ʲôʱ�����

**********************************************/
/**
 * @brief   ��ѯ֡���ͣ����
 * @param   *data:  ��Ž��յ�������֡�Ļ�����
 * @param   data_len: ֡���ݵĴ�С
 * @return  ֡���ͣ����.
 */
static uint8_t protocol_frame_parse(uint8_t *data, uint16_t *data_len)
{
    uint8_t frame_type = CMD_NONE;    // ָ֡��; ��ָ��
    uint16_t need_to_parse_len = 0;   // �½��յ������ݵĳ���
    int16_t header_oft = -1;          // ƫ�Ƶ�ַ
    uint8_t checksum = 0;             // У���
    
		/****************** ����Ϊ��ԭʼ���ݻ������л�ȡ����֡֡ͷ������֡���ȵ����� ******************/
	  // ����δ���������ݳ��ȣ�parser.frame_len��ʱδ����ֵΪ0
    need_to_parse_len = recvbuf_get_len_to_parse(parser.frame_len, PROT_FRAME_LEN_RECV, parser.r_oft, parser.w_oft);    // �õ�δ���������ݳ���
    if (need_to_parse_len < 9)     // �½��յ������ݵĳ���<9���϶�������ͬʱ�ҵ�֡ͷ��֡����
			return frame_type;  // ָ֡��; CMD_NONE: ��ָ��

    /* ��δ�ҵ�֡ͷ����Ҫ���в��� */
    if (0 == parser.found_frame_head)
    {
			  // ����֡ͷ
        header_oft = recvbuf_find_header(parser.recv_ptr, PROT_FRAME_LEN_RECV, parser.r_oft, need_to_parse_len);
        if (0 <= header_oft)  // �ҵ�֡ͷ
        {
            /* ���ҵ�֡ͷ*/
            parser.found_frame_head = 1;  // ���ҵ�֡ͷ
            parser.r_oft = header_oft;    // ��ƫ�� = ֡ͷλ��
          
            /* ȷ���Ƿ���Լ���֡��*/
						// ����֡�ĳ���<9���϶�����һ֡��������
            if (recvbuf_get_len_to_parse(parser.frame_len, PROT_FRAME_LEN_RECV, parser.r_oft, parser.w_oft) < 9)
                return frame_type;  // ָ֡��; CMD_NONE: ��ָ��
        }
        else    // δ�ҵ�֡ͷ
        {
            /* δ��������������Ȼδ�ҵ�֡ͷ�������˴ν��������������� */
					  /* -3ԭ��: ͬ��ͷΪ���ֽڣ����ܴ���δ�����������к������ֽڸպ�Ϊͬ��ͷǰ�����ֽڵ������
            ��˲���ͬ��ͷʱ���������ֽڽ����ᱻ���� */
            parser.r_oft = ((parser.r_oft + need_to_parse_len - 3) % PROT_FRAME_LEN_RECV);
            return frame_type;  // ָ֡��; CMD_NONE: ��ָ��
        }
    }
    
    /* ���֡������ȷ���Ƿ���Խ������ݽ���*/
	  /* parser.frame_lenδ����ֵΪ0 */
    if (0 == parser.frame_len) 
    {
			  // �õ�֡���ȣ�(����ȡ�������ݵ�֡����λ)
        parser.frame_len = get_frame_len(parser.recv_ptr, parser.r_oft);   
				// �½��յ������ݵĳ��� < ֡����
        if(need_to_parse_len < parser.frame_len)
            return frame_type;
    }

		/****************** ����Ϊ����֡У�鼰��ԭʼ���ݻ������е�����֡��������������֡������ ******************/
    /* ֡ͷλ��ȷ�ϣ���δ���������ݳ���֡�������Լ���У���*/
    if ((parser.frame_len + parser.r_oft - PROT_FRAME_LEN_CHECKSUM) > PROT_FRAME_LEN_RECV)
    {
        /* ����֡����Ϊ�����֣�һ�����ڻ�����β��һ�����ڻ�����ͷ */
        checksum = check_sum(checksum, parser.recv_ptr + parser.r_oft, 
                PROT_FRAME_LEN_RECV - parser.r_oft);
        checksum = check_sum(checksum, parser.recv_ptr, parser.frame_len -
                PROT_FRAME_LEN_CHECKSUM + parser.r_oft - PROT_FRAME_LEN_RECV);
    }
    else 
    {
        /* ����֡����һ����ȡ��*/
        checksum = check_sum(checksum, parser.recv_ptr + parser.r_oft, parser.frame_len - PROT_FRAME_LEN_CHECKSUM);
    }

    if (checksum == get_frame_checksum(parser.recv_ptr, parser.r_oft, parser.frame_len))
    {
        /* У��ɹ���������֡���ݵ��������ݻ����� */
        if ((parser.r_oft + parser.frame_len) > PROT_FRAME_LEN_RECV) 
        {
            /* ����֡����Ϊ�����֣�һ�����ڻ�����β��һ�����ڻ�����ͷ*/
            uint16_t data_len_part = PROT_FRAME_LEN_RECV - parser.r_oft;
            memcpy(data, parser.recv_ptr + parser.r_oft, data_len_part);
            memcpy(data + data_len_part, parser.recv_ptr, parser.frame_len - data_len_part);
        }
        else 
        {
            /* ����֡����һ����ȡ��*/
            memcpy(data, parser.recv_ptr + parser.r_oft, parser.frame_len);
        }
				// �������֡����
        *data_len = parser.frame_len;
				// ���֡����
        frame_type = get_frame_type(parser.recv_ptr, parser.r_oft);

        /* ��ƫ�Ƹ��£��Ѿ����������ݴӻ���������� */
        parser.r_oft = (parser.r_oft + parser.frame_len) % PROT_FRAME_LEN_RECV;  // ��ƫ�Ƹ���
    }
    else  // У�����
    {
        /* У�����˵��֮ǰ�ҵ���֡ͷֻ��żȻ���ֵķ�����*/
        parser.r_oft = (parser.r_oft + 1) % PROT_FRAME_LEN_RECV;     // ��ƫ������ƶ�һ������
    }
    parser.frame_len = 0;         // ֡��������
    parser.found_frame_head = 0;  // ֡ͷ����
 
    return frame_type;
}

/*****************************�û����ú���****************************/

/**
 * @brief   ��ʼ������Э��
 * @param   void
 * @return  ��ʼ�����.
 */
uint8_t protocol_init(void)
{
	  // �ṹ�����
    memset(&parser, 0, sizeof(struct prot_frame_parser_t));
    
    /* ��ʼ���������ݽ��������������*/
    parser.recv_ptr = recv_buf;
   
    return 0;
}

/**
 * @brief   ���յ�������д�뻺���������޸�дָ��ƫ��
 * @param   *data:  Ҫ��������ݵ�����.
 * @param   data_len: ���ݵĴ�С
 * @return  void.
 */
void protocol_data_recv(uint8_t *data, uint16_t data_len)
{
  	// ���յ�������д�뻺����		�����ֱ�Ϊ: ������, ����������, ����дƫ��, ��д������, ��д�����ݳ���
    recvbuf_put_data(parser.recv_ptr, PROT_FRAME_LEN_RECV, parser.w_oft, data, data_len);    // ��������
	  // �����µ�дƫ��
    parser.w_oft = (parser.w_oft + data_len) % PROT_FRAME_LEN_RECV;                          // ����дƫ��
}

/**
 * @brief   ���յ����ݴ���
 * @param   void
 * @return  -1��û���ҵ�һ����ȷ������.
 */
int8_t receiving_process(void)
{
  uint8_t frame_data[128];         // Ҫ�ܷ������֡
  uint16_t frame_len = 0;          // ֡����
  uint8_t cmd_type = CMD_NONE;     // ��������
  
  while(1)
  {
		// ��ѯ֡���ͣ����
		// ��ѯ��ὲ����֡����frame_data, ����֡���ȷ���frame_len, ����ֵΪ��������
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
        
        set_p_i_d(p_temp, i_temp, d_temp);    // ���� P I D
      }
      break;

      case SET_TARGET_CMD:
      {
        int actual_temp = COMPOUND_32BIT(&frame_data[13]);    // �õ�����
        
        set_point = (actual_temp);    // ����Ŀ��ֵ
				pid.target_val = set_point;
      }
      break;
      
      case START_CMD:
      {
//        set_motor_enable();              // �������
      }
      break;
      
      case STOP_CMD:
      {
//        set_motor_disable();              // ֹͣ���
      }
      break;
      
      case RESET_CMD:
      {
        HAL_NVIC_SystemReset();          // ��λϵͳ
      }
      break;
      
      case SET_PERIOD_CMD:
      {
        uint32_t temp = COMPOUND_32BIT(&frame_data[13]);     // ������
        SET_BASIC_TIM_PERIOD(temp);                          // ���ö�ʱ������1~1000ms
      }
      break;

      default: 
        return -1;
    }
  }
}

/**
  * @brief ������λ����ֵ(�������ݷ�������λ��)
  * @param cmd�� ����
  * @param ch:   ����ͨ��
  * @param data������ָ��
  * @param num�� ��������
  * @retval ��
  */
void set_computer_value(uint8_t cmd, uint8_t ch, void *data, uint8_t num)
{
  uint8_t sum = 0;    // У���
  num *= 4;           // һ������ 4 ���ֽ�
  
  static packet_head_t set_packet;
  
  set_packet.head = FRAME_HEADER;     // ��ͷ 0x59485A53
  set_packet.len  = 0x0B + num;       // ����, 0x0BΪ��ͷ(4bytes)+ͨ����ַ(1bytes)+������(4bytes)+����(1bytes)+У���(1bytes)=0x0B
  set_packet.ch   = ch;               // ����ͨ��
  set_packet.cmd  = cmd;              // ��������
  
  sum = check_sum(0, (uint8_t *)&set_packet, sizeof(set_packet));       // �����ͷУ���
  sum = check_sum(sum, (uint8_t *)data, num);                           // �������У���
  
	// ���ڷ�������
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&set_packet, sizeof(set_packet), 0xFFFFF);    // ��������ͷ
  HAL_UART_Transmit(&UartHandle, (uint8_t *)data, num, 0xFFFFF);                          // ���Ͳ���
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&sum, sizeof(sum), 0xFFFFF);                  // ����У���
}

/**********************************************************************************************/
