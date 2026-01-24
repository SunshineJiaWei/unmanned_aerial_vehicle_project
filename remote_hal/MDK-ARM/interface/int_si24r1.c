#include "int_si24r1.h"


uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x06, 0x03, 0x02, 0x01}; // 定义一个静态发送地址

static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t rx_byte = 0;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &rx_byte, 1, 1000);
	return rx_byte;
}


/********************************************************
函数功能：写寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
uint8_t int_si24r1_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	CS_LOW();
	status = SPI_RW(reg);
	SPI_RW(value);
	CS_HIGH();

	return (status);
}

/********************************************************
函数功能：写寄存器的值（多字节）
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t int_si24r1_write_buf(uint8_t reg, const uint8_t *pBuf, uint8_t size)
{
	uint8_t status, byte_ctr;

	CS_LOW();
	status = SPI_RW(reg);
	for (byte_ctr = 0; byte_ctr < size; byte_ctr++)
	{
		SPI_RW(*pBuf++);
	}
	CS_HIGH();

	return (status);
}

/********************************************************
函数功能：读取寄存器的值（单字节）
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
uint8_t int_si24r1_read_reg(uint8_t reg)
{
	uint8_t value;

	CS_LOW();
	SPI_RW(reg);
	value = SPI_RW(0);
	CS_HIGH();

	return (value);
}

/********************************************************
函数功能：读取寄存器的值（多字节）
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
uint8_t int_si24r1_read_buf(uint8_t reg, uint8_t *pBuf, uint8_t size)
{
	uint8_t status, byte_ctr;

	CS_LOW();
	status = SPI_RW(reg);
	for (byte_ctr = 0; byte_ctr < size; byte_ctr++)
	{
		pBuf[byte_ctr] = SPI_RW(0); // 读取数据，低字节在前
	}
	CS_HIGH();

	return (status);
}

/********************************************************
函数功能：SI24R1接收模式初始化
入口参数：无
返回  值：无
*********************************************************/
void int_si24r1_rx_mode(void)
{
	CE_LOW();
	int_si24r1_write_buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 接收设备接收通道0使用和发送设备相同的发送地址
	int_si24r1_write_reg(WRITE_REG + EN_AA, 0x01);							// 使能接收通道0自动应答
	int_si24r1_write_reg(WRITE_REG + EN_RXADDR, 0x01);						// 使能接收通道0
	int_si24r1_write_reg(WRITE_REG + RF_CH, TX_RX_CHANNEL);					// 选择射频通道0x40
	int_si24r1_write_reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);				// 接收通道0选择和发送通道相同有效数据宽度
	int_si24r1_write_reg(WRITE_REG + RF_SETUP, 0x0f);						// 数据传输率1Mbps，发射功率4dBm
	int_si24r1_write_reg(WRITE_REG + CONFIG, 0x0f);							// CRC使能，16位CRC校验，上电，接收模式
	int_si24r1_write_reg(WRITE_REG + STATUS, 0xff);							// 清除所有的中断标志位
	CE_HIGH();																// 拉高CE启动接收设备
}

/********************************************************
函数功能：SI24R1发送模式初始化
入口参数：无
返回  值：无
*********************************************************/
void int_si24r1_tx_mode(void)
{
	CE_LOW();
	int_si24r1_write_buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);	// 写入发送地址
	int_si24r1_write_buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 为了应答接收设备，接收通道0地址和发送地址相同

	int_si24r1_write_reg(WRITE_REG + EN_AA, 0x01);			// 使能接收通道0自动应答
	int_si24r1_write_reg(WRITE_REG + EN_RXADDR, 0x01);		// 使能接收通道0
	int_si24r1_write_reg(WRITE_REG + SETUP_RETR, 0x0a);		// 自动重发延时等待250us+86us，自动重发10次
	int_si24r1_write_reg(WRITE_REG + RF_CH, TX_RX_CHANNEL); // 选择射频通道0x40
	int_si24r1_write_reg(WRITE_REG + RF_SETUP, 0x0f);		// 数据传输率2Mbps，发射功率7dBm
	int_si24r1_write_reg(WRITE_REG + CONFIG, 0x0e);			// CRC使能，16位CRC校验，上电
	CE_HIGH();												// CE = 1;
}

/********************************************************
函数功能：读取接收数据
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
		  1:没有接收到数据
*********************************************************/
uint8_t int_si24r1_rx_packet(uint8_t *rxbuf)
{
	uint8_t state;
	state = int_si24r1_read_reg(STATUS);			 // 读取状态寄存器的值
	int_si24r1_write_reg(WRITE_REG + STATUS, state); // 清除RX_DS中断标志

	if (state & RX_DR) // 接收到数据
	{
		int_si24r1_read_buf(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH); // 读取数据
		int_si24r1_write_reg(FLUSH_RX, 0xff);					 // 清除RX FIFO寄存器
		return 0;
	}
	return 1; // 没收到任何数据
}

/********************************************************
函数功能：发送一个数据包
入口参数：txbuf:要发送的数据
返回  值：
		  0x00:发送成功
		  0x01:发送失败
*********************************************************/
uint8_t int_si24r1_tx_packet(uint8_t *txbuf)
{
	uint8_t state;
	CE_LOW();												  // CE拉低，使能SI24R1配置
	int_si24r1_write_buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // 写数据到TX FIFO,32个字节
	CE_HIGH();												  // CE置高，使能发送

	// sampleCode：51中使用中断判断是否发送完成
	// while (IRQ == 1)
	// 	;											 // 等待发送完成


	state = int_si24r1_read_reg(STATUS);			 // 读取状态寄存器的值
	while ((!(state & TX_DS)) && (!(state & MAX_RT)))
	{
		state = int_si24r1_read_reg(STATUS);
		vTaskDelay(1); // 短暂让出CPU，防止通讯失败时，导致程序一直卡在这
	}

	int_si24r1_write_reg(WRITE_REG + STATUS, state); // 清除TX_DS或MAX_RT中断标志
	if (state & MAX_RT)								 // 达到最大重发次数
	{
		int_si24r1_write_reg(FLUSH_TX, 0xff); // 清除TX FIFO寄存器
		return 1;
	}
	if (state & TX_DS) // 发送完成
	{
		return 0;
	}
	return 1; // 发送失败
}

static uint8_t si24r1_rx_buf[5] = {0};

/**
 * @brief SI24R1初始化检查
 * 
 * @return uint8_t 
 * 			0: 初始化成功
 * 			1: 初始化失败
 */
static uint8_t int_si24r1_check(void)
{
	// 经过测试，需要先读取一次，spi才能正常通信
	int_si24r1_read_buf(READ_REG + TX_ADDR, si24r1_rx_buf, TX_ADR_WIDTH);

	int_si24r1_write_buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);

	int_si24r1_read_buf(READ_REG + TX_ADDR, si24r1_rx_buf, TX_ADR_WIDTH);

	for (uint8_t i = 0; i < TX_ADR_WIDTH; i++)
	{
		if (si24r1_rx_buf[i] != TX_ADDRESS[i])
		{
			return 1;
		}
	}
	return 0;
}

void int_si24r1_init(void)
{
	HAL_Delay(200); // 芯片上电延迟
	while (int_si24r1_check())
	{
		HAL_Delay(10);
	}

	// 默认接收模式
	int_si24r1_rx_mode();
	DEBUG_PRINTF("SI24R1 Init Success\n");
}
