#include "./OLED/oled.h"
#include "stm32f1xx.h"
#include "./systick/bsp_SysTick.h"
// ע����������������.c�ļ������ᱨ��
// ԭ����ǣ�.h�ļ����Ƽ����峣��
// ����.c���壬.h extern
#include "./OLED/oledfont.h"  	 
#include "./OLED/bmp.h"  	 

u8 OLED_GRAM[144][8];


/**************���ų�ʼ��***************/
void OLED_SPI_Init(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	/*����GPIO����ʱ��*/
	SCL_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOG_CLK_ENABLE()
	SDA_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOD_CLK_ENABLE()
	RES_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOD_CLK_ENABLE()
	DC_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOD_CLK_ENABLE()
	CS_GPIO_CLK_ENABLE();   // __HAL_RCC_GPIOD_CLK_ENABLE()
	
	/*�������ŵ��������Ϊ�������*/
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  
	/*��������Ϊ����ģʽ*/
	GPIO_InitStruct.Pull  = GPIO_PULLUP;
	/*������������Ϊ���� */   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStruct.Pin = SCL_PIN;	
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	HAL_GPIO_Init(SCL_GPIO_PORT, &GPIO_InitStruct);	
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStruct.Pin = SDA_PIN;	
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	HAL_GPIO_Init(SDA_GPIO_PORT, &GPIO_InitStruct);	
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStruct.Pin = RES_PIN;	
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	HAL_GPIO_Init(RES_GPIO_PORT, &GPIO_InitStruct);	
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStruct.Pin = DC_PIN;	
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	HAL_GPIO_Init(DC_GPIO_PORT, &GPIO_InitStruct);	
	
	/*ѡ��Ҫ���Ƶ�GPIO����*/															   
	GPIO_InitStruct.Pin = CS_PIN;	
	/*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
	HAL_GPIO_Init(CS_GPIO_PORT, &GPIO_InitStruct);	
	
	OLED_SCLK_Set();
	OLED_SDIN_Set();
	OLED_RES_Set();
	OLED_DC_Set();
	OLED_CS_Set();
}

/**************OLED��������***************/
//����һ���ֽ�
//dat:Ҫ���͵�����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else
	  OLED_DC_Clr();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set(); 
}

//����OLED��ʾ 
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x14,OLED_CMD);//������ɱ�
	OLED_WR_Byte(0xAF,OLED_CMD);//������Ļ
}

//�ر�OLED��ʾ 
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//��ɱ�ʹ��
	OLED_WR_Byte(0x10,OLED_CMD);//�رյ�ɱ�
	OLED_WR_Byte(0xAE,OLED_CMD);//�ر���Ļ
}

//�����Դ浽OLED	
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   OLED_WR_Byte(0xb0+i,OLED_CMD); //��������ʼ��ַ
	   OLED_WR_Byte(0x00,OLED_CMD);   //���õ�����ʼ��ַ
	   OLED_WR_Byte(0x10,OLED_CMD);   //���ø�����ʼ��ַ
	   for(n=0;n<128;n++)
		 OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
  }
}

//��������
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//�����������
			}
  }
	OLED_Refresh();//������ʾ
}

//���Ժ���
void OLED_ColorTurn(u8 i)  
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);//������ʾ
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);//��ɫ��ʾ
		}
}

//��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//������ʾ
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//��ת��ʾ
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

//���� 
//x:0~127
//y:0~63
void OLED_DrawPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_GRAM[x][i]|=n;
}

//���һ����
//x:0~127
//y:0~63
void OLED_ClearPoint(u8 x,u8 y)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	OLED_GRAM[x][i]|=n;
	OLED_GRAM[x][i]=~OLED_GRAM[x][i];
}

//����
//x1,y1:�������
//x2,y2:��������
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2)
{
	u8 i,k,k1,k2,y0;
	if((x1<0)||(x2>128)||(y1<0)||(y2>64)||(x1>x2)||(y1>y2))return;
	if(x1==x2)    //������
	{
			for(i=0;i<(y2-y1);i++)
			{
				OLED_DrawPoint(x1,y1+i);
			}
  }
	else if(y1==y2)   //������
	{
			for(i=0;i<(x2-x1);i++)
			{
				OLED_DrawPoint(x1+i,y1);
			}
  }
	else      //��б��
	{
		k1=y2-y1;
		k2=x2-x1;
		k=k1*10/k2;
		for(i=0;i<(x2-x1);i++)
			{
			  OLED_DrawPoint(x1+i,y1+i*k/10);
			}
	}
}

//x,y:Բ������
//r:Բ�İ뾶
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b);
        OLED_DrawPoint(x - a, y - b);
        OLED_DrawPoint(x - a, y + b);
        OLED_DrawPoint(x + a, y + b);
 
        OLED_DrawPoint(x + b, y + a);
        OLED_DrawPoint(x + b, y - a);
        OLED_DrawPoint(x - b, y - a);
        OLED_DrawPoint(x - b, y + a);
        
        a++;
        num = (a * a + b * b) - r*r;//���㻭�ĵ���Բ�ĵľ���
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//char�ַ�
//size1:ѡ������ 6x8/6x12/8x16/12x24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1)
{
	u8 i,m,temp,size2,chr1;
	u8 y0=y;
	size2=(size1/8+((size1%8)?1:0))*(size1/2);  //�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1=chr-' ';  //����ƫ�ƺ��ֵ
	for(i=0;i<size2;i++)
	{
		if(size1==12)
        {temp=asc2_1206[chr1][i];} //����1206����
		else if(size1==16)
        {temp=asc2_1608[chr1][i];} //����1608����
		else if(size1==24)
        {temp=asc2_2412[chr1][i];} //����2412����
		else return;
				for(m=0;m<8;m++)           //д������
				{
					if(temp&0x80)OLED_DrawPoint(x,y);
					else OLED_ClearPoint(x,y);
					temp<<=1;
					y++;
					if((y-y0)==size1)
					{
						y=y0;
						x++;
						break;
          }
				}
  }
}

//��ʾ�ַ���
//x,y:�������  
//size1:�����С 
//*chr:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1)
{
	while((*chr>=' ')&&(*chr<='~'))//�ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x,y,*chr,size1);
		x+=size1/2;
		if(x>128-size1)  //����
		{
			x=0;
			y+=2;
    }
		chr++;
  }
}

//��˷�������m,n�����m^n
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

//��ʾ����
//x,y :�������
//num :Ҫ��ʾ������
//len :���ֵ�λ��
//size:�����С
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1)
{
	u8 t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/OLED_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				OLED_ShowChar(x+(size1/2)*t,y,'0',size1);
      }
			else 
			{
			  OLED_ShowChar(x+(size1/2)*t,y,temp+'0',size1);
			}
  }
}

//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//size:���ִ�С
//ȡģ��ʽ ����ʽ
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1)
{
	u8 i,m,n=0,temp,chr1;
	u8 x0=x,y0=y;
	u8 size3=size1/8;
	while(size3--)
	{
		chr1=num*size1/8+n;
		n++;
			for(i=0;i<size1;i++)
			{
				if(size1==16)
						{temp=Hzk1[chr1][i];}//����16*16����
				else if(size1==24)
						{temp=Hzk2[chr1][i];}//����24*24����
				else if(size1==32)       
						{temp=Hzk3[chr1][i];}//����32*32����
				else if(size1==64)
						{temp=Hzk4[chr1][i];}//����64*64����
				else return;
							
						for(m=0;m<8;m++)
							{
								if(temp&0x01)OLED_DrawPoint(x,y);
								else OLED_ClearPoint(x,y);
								temp>>=1;
								y++;
							}
							x++;
							if((x-x0)==size1)
							{x=x0;y0=y0+8;}
							y=y0;
			 }
	}
}

//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
void OLED_ScrollDisplay(u8 num,u8 space)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16); //д��һ�����ֱ�����OLED_GRAM[][]������
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //��ʾ���
				 {
					for(i=0;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=0;i<144;i++)   //ʵ������
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//x0,y0���������
//x1,y1���յ�����
//BMP[]��Ҫд���ͼƬ����
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	u32 j=0;
	u8 x=0,y=0;
	if(y%8==0)y=0;
	else y+=1;
	for(y=y0;y<y1;y++)
	 {
		 OLED_WR_BP(x0,y);
		 for(x=x0;x<x1;x++)
		 {
			 OLED_WR_Byte(BMP[j],OLED_DATA);
			 j++;
     }
	 }
}

//����д�����ݵ���ʼλ��
void OLED_WR_BP(u8 x,u8 y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);//��������ʼ��ַ
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD);
}


//OLED�ĳ�ʼ��
void OLED_Init(void)
{
	OLED_SPI_Init();    // �˿ڳ�ʼ��
	
	OLED_RES_Clr();     // OLED��λ
	Delay_ms(200);
	OLED_RES_Set();
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel       �ر�OLED���
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address   ���õ��е�ַ
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address  ���ø��е�ַ
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)  ������ʼ�ߵ�ַ����ӳ��RAM��ʾ��ʼ��
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register     ���öԱȶȿ��ƼĴ���
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness  ����SEG�����������
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display         ����������ʾ
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)      ���ö�·������(1��64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)   ������ʾƫ����λӳ��RAM������(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset   ��ƫ��
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency   ������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec         ���÷�Ƶ�ȣ�����ʱ��Ϊ100֡/��
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period      ����Ԥ�����
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock      ��Ԥ�������Ϊ15��ʱ�ӣ����ŵ�����Ϊ1��ʱ��
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration       ����COM����Ӳ������
	OLED_WR_Byte(0x12,OLED_CMD); 
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level      ����VCOMȡ��ѡ�񼶱�
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)  ����ҳ��Ѱַģʽ(0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);// 
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable    ���õ�ɱ�����/����
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable          ����(0x10)����
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)   ����������ʾ��(0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7)    ���÷�ת��ʾ��(0xa6/A7)
	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_Clear();
}

// OLEDӦ�þ���
void OLED_Show(void)
{
	OLED_Init();
	
	/*************** ���¾�Ϊ�������� ***************/
	
	// ��Ļ��ʾ���ã���ֱ��ע��
	OLED_ColorTurn(0);  //0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	OLED_Refresh();

	OLED_ShowPicture(0,0,128,64,BMP1);   // ��ʾͼƬ���������0,0��ͼƬ��С128,64��ͼƬ����BMP
	OLED_Refresh();   // ������ʾ
	Delay_ms(3000);

	OLED_Clear();     // ����
	//�������x,y�����ֶ�Ӧ��ţ����ִ�С
	OLED_ShowChinese(0,0,0,16);  //��
	OLED_ShowChinese(18,0,1,16); //��
	OLED_ShowChinese(36,0,2,16); //԰
	OLED_ShowChinese(54,0,3,16); //��
	OLED_ShowChinese(72,0,4,16); //��
	OLED_ShowChinese(90,0,5,16); //��
	OLED_ShowChinese(108,0,6,16);//��
	//�������x,y���ַ�����ʼ��ַ�������С
	OLED_ShowString(8,16,"ZHONGJINGYUAN",16);
	OLED_ShowString(20,32,"2014/05/01",16);
	OLED_ShowString(0,48,"ASCII:",16);  
	OLED_ShowString(63,48,"CODE:",16);
	//�������x,y��Ҫ��ʾ���ַ��������С
	OLED_ShowChar(48,48,' ',16);//��ʾASCII�ַ�	   
	//�������x,y��Ҫ��ʾ�����֣�����λ���������С
	OLED_ShowNum(103,48,(u8)' ',3,16);//��ʾ����
	OLED_Refresh();
	Delay_ms(3000);   // ������ʾ
	
	OLED_Clear();     // ����
	OLED_ShowChinese(0,0,0,16);  //16*16 ��
	OLED_ShowChinese(16,0,0,24); //24*24 ��
	OLED_ShowChinese(24,20,0,32);//32*32 ��
	OLED_ShowChinese(64,0,0,64); //64*64 ��
	OLED_Refresh();
	Delay_ms(3000);
	
	OLED_Clear();
	OLED_ShowString(0,0,"ABC",8);//6*8 ��ABC��
	OLED_ShowString(0,8,"ABC",12);//6*12 ��ABC��
	OLED_ShowString(0,20,"ABC",16);//8*16 ��ABC��
	OLED_ShowString(0,36,"ABC",24);//12*24 ��ABC��
	OLED_Refresh();
	Delay_ms(3000);
	
	// ��ʾ���ֵĸ�����ÿһ����ʾ�ļ����������ʾ
	// �˺���������ѭ����
	OLED_ScrollDisplay(11,4);
}
