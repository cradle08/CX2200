环境：MDK5.30，Keil.STM32F4xx_DFP.2.15.0
STM32Cube:  STM32Cube_FW_F4_V1.25.0


IROM1：0x8000000    0x100000
IRAM1：0x20000000   0x30000
IRAM2：0x10000000   0x10000

1  、ADC采样， ADC1用于WBC（ADC1_PA6_IN6），ADC2用于PLT（ADC1_PA5_IN5）,ADC3用于状态采集
     

2、使用32位外部SDRAM，STemWin的执行效率会比16bitSDRAM提升一倍，

3、ETH
    
    PA1     ------> ETH_REF_CLK
    PA2     ------> ETH_MDIO
    PA7     ------> ETH_CRS_DV
    PB11   ------> ETH_TX_EN
    PC1     ------> ETH_MDC
    PC4     ------> ETH_RXD0
    PC5     ------> ETH_RXD1
    PG13   ------> ETH_TXD0
    PG14   ------> ETH_TXD1 
    ETH RESET GPIO = 评估板默认使用PCF8574扩展出的IO口，这里我们改为MCU的IO口（PF8）直接控制。
		（复位：先低，100ms，拉高。IO：推挽输出、无上拉下拉)。

4、LED
LED0：PB1（拉高灭，拉低亮）
LED1：PB0

5、USART
USART1: TX_PA9, RX_PA10, 作为调试串口，并重定向到printf

6、freertos+sdram
heap_1.c 使用大数组，可申请，没释放，系统使用创建消息量、定时器等也基本不需要释放；
heap_2.c使用大数组，可申请，可释放，但是不会合并相邻空闲块，会产生碎片；
heap_4.c使用大数组，可申请，可释放，相邻区块可以合并；
heap_3.直接使用malloc和free封装；
heap_5.c允许使用多个内存堆块，可内部RAM可外部RAM，每个内存堆的起始地址和大小需用户定义；

7、CCM RAM为64k，速度比通用的要快。
uint8_t ucHeap[64*1024]  __attribute__ ((at(0x10000000)))

8、FMC
8.1、地址范围：0x60000000-0xDFFFFFFF
8.2、FMC总线分为六个区域，每个区域的大小为：4*64MB
六个区域分别为：(NOR、PSRAM、SRAM)Bank1，NAND Flash Bank2-3，SD卡-Bank4，SDRAM Bank1-2
8.3、地址线为：A0-A25，寻址范围：64M（一个bank的一个区域大小），
	此外还有A26-27两条内部地址线用于选择一个bank的四个区域。数据线为：32位。
8.4、SDRAM，（SDRAM,数据掉电消失，不能存放执行代码，RAM可以）通过直接使用的方式使用SDRAM，
	寄存器：
	FMC_SDCR1/2:控制寄存器,可配置时钟频率，突发读使能，写保护，行列地址位数，总线位数等。
		FMC_SDCLK:为HCLK（180M）的1/2或1/3。
	FMC_SDCMR1/2: 命令模式配置，存储并发送命令给SDRAM。
	FMC_SDTR:时序寄存器，配置SDRAM访问各种延时、TRP预充电、加载激活延时。
	FMC_SDRTR: 配置SDRAM自动刷新周期。
	
	SDRAM0 = 0xC000 0000 - 0xCFFF FFFF
	SDRAM1 = 0xD000 0000 - 0xDFFF FFFF

8.5、SDRAM的GPIO引脚配置，（复用FMC推挽，上拉，高速）

	FMC_D0=PD14		FMC_A0=PF0
	FMC_D1=PD15		FMC_A1=PF1
	FMC_D2=PD0		FMC_A2=PF2
	FMC_D3=PD1		FMC_A3=PF3
	FMC_D4=PE7		FMC_A4=PF4
	FMC_D5=PE8		FMC_A5=PF5
	FMC_D6=PE9		FMC_A6=PF12
	FMC_D7=PE10		FMC_A7=PF13
	FMC_D8=PE11		FMC_A8=PF14
	FMC_D9=PE12		FMC_A9=PF15
	FMC_D10=PE13		FMC_A10=PG0
	FMC_D11=PE14		FMC_A11=PG1
	FMC_D12=PE15		FMC_A12=PG2
	FMC_D13=PD8		
	FMC_D14=PD9		
	FMC_D15=PD10		

	FMC_BA0=PG4		FMC_SDCKE0=PC3，（ph2）
	FMC_BA1=PG5		FMC_SDCLK=PG8
	
	FMC_NBL0=PE0		FMC_SDNWE=PC0
	FMC_NBL1=PE1		FMC_SDNE0=PC2，（ph3）

	FMC_SDNRAS=PF11
	FMC_SDNCAS=PG15
	
9、LTDC
9.1、RGB565=R：5位（LCD_R[7:3]），G：6位（LCD_G[7:2]），B：5位（LCD_B[7:3]），最大支持1024*768分辨率。
9.2、LTDC_GCR: 全局配置参数寄存器：生成同步时序、像素时钟、使能信号。
9.3、LTDC通过逐行扫描行来显示，VSYNC为：帧帧同步信号，表示一个画面的开始，HSYNC：行同步信号（刷新一行），VDEN：数据使能信号。
9.4、需设置一些寄存器的时序，及显示宽带（省略）。
9.5、LTDC通过不断的把显存刷新到LCD上。
9.6、LTDC把显存刷到LCD上，一般有两种刷新方式：DE（大分辨率LCD）和HV（小分辨率LCD）模式，（从左到右，从上到下）
	DE莫斯：LCD_DE和LCD_CLK信号控制刷新。一个有效的LCD_DE刷新一行，周而复始。
	HV同步模式：LCD_VSYNC表示新一帧的开始，LCD_HSYNC表示新一行的开始，周而复始。
9.7、基于LTDC的图像界面开发，需要自己实现画点、线、图片，字库，控件等驱动接口，并初始化图层并分配显存。
	

10、MDA2D
10.1、一个专用的图像处理DMA，作用是把数据传输到显存中，之后LTDC再把显存中的数据刷新到LCD。
    引脚定义：
    PI9（）     LTDC_VSYNC 
    PI10（）    LTDC_HSYNC 
    PF10（）    LTDC_DE
    PG7 （）    LTDC_CLK 	
    PH9（）     LTDC_R3
    PH10 （）   LTDC_R4
    PH11 （）   LTDC_R5
    PH12 （）   LTDC_R6
    PG6 （）    LTDC_R7
    PH13（）    LTDC_G2
    PH14 （）   LTDC_G3
    PH15（）    LTDC_G4	
    PI0 （）    LTDC_G5
    PI1（）     LTDC_G6
    PI2（）     LTDC_G7
    PG11（）    LTDC_B3
    PI4（）   LTDC_B4
    PI5  （）   LTDC_B5	
    PI6 （）    LTDC_B6
    PI7 （）    LTDC_B7 
    PB5          LCD_BL（背光亮度，高亮，低黑）
    PD7	    LCD_RST(复位，低-复位，高-工作)
    PB7          LCD_PWR(电源控制，模拟或数字电源上电时序， 高-先数字后模拟，低-先模拟后数字)

XPT2046电阻触摸芯片SPI:
    PH7	= T_PEN（触摸中断，触摸IC：xpt2046）
    PH6      = T_SCK（触摸SPI_CLK）
    PI3       = T_MOSI（触摸SPI_MOSI）
    PG3      = T_MISO（触摸SPI_MISO）
    NRST	=  RESET（LCD复位和MCU的复位一致）

FT5206电容屏触摸芯片IIC:
    PH7	= T_PEN（触摸中断，触摸IC：xpt2046）
    PH6      = T_CLK
    PI3       = T_SDA（触摸SPI_MOSI）
    PG3      = 未使用
    PI8	=  RESET（LCD复位和MCU的复位一致）

10.2、STM32F429的DMA2D采用的是ARGB颜色格式，255表示不透明、0：透明。


11、 STemWin
11.1、点亮背光，没那么黑，
11.2、 移植：ltdc初始化，设置背景颜色，则启动会显示对应的颜色，但是初始化layer后，则出现规律的花屏（需要清屏），
	由于指定的外部内存块上电后的初始值，为随机值。清屏即手动初始化显存。
11.3、电容屏会自动校准，获取到的数值为坐标值。
          电阻屏需手动校准、获取到的为ADC值，需通过校准之后的线性函数，把ADC值转换成坐标值，
          STemVim通过  函数把坐标值保存在FIFO中。

11.4、GUIConf.c为内存配置文件。
          GUIConf.h为emWin系统配置、剪裁。
          LCDConf_Lin_Template.h/c : 和LCD显示相关配置，分辨率，显存、驱动、颜色转换等（直线性访问型LCD：LIN（RGB接口））。
          LCDConf_FlexColor_Template.h/c : 非直接访问型LCD：FlexColor（如：8080接口）
	（三级缓存、RGB565、单图层（多层可用于多屏显示，或是多层混合处理））
          GUI_X.c:裸机环境下时基、延时相关。
          GUI_X_OS.c:操作系统环境下，时基、延时和任务调度。
11.5、

11.6 、STemWin使用的内存分为两大块：1：STemWin系统动态内存，2：显存（和LTDC的buffer为同一个东西）

12：模拟器（STemWin）
12.1、下载模拟器：https://www.segger.com/downloads/emwin，解压，然后用vs2019打开。
12.2、在工程属性中-->连接器-->输入-->附加依赖项：legacy_stdio_definitions.lib（添加到第一个）
		     |                    |->忽略所有默认库：否
	                     |                    |->忽略特定默认库：LIBC.lib;LIBCMTD.lib（添加）
		     |->高级-->映像具有安全异常处理程序：否
         之后按“F5”编译（编译为第一级目录的：Application）。
         （官方给其他综合运用例子在sample下：包括综合例子Application下，控件使用例子：Tutorial）
12.3、GUICC_565和GUICC_M565：为调色版，GUICC_M565：表示为ARGB模式下的调色板，GUICC_565：为ABGR。

12.4、窗口管理器、每个通过事件驱动系统+回调处理的方式，进行交互。
	在WM_PAINT消息中：不能使用窗口操作函数，只是进行绘制。
12.5、定时器


12.6、STemWim API
	WM_SetHasTrans（hItem），设置透明度
	 PROGBAR_SetSkin(hProg, _CustomSkin);换肤
	GUI_DrawGraph（）;画波形图	

	图片及命名：
	input_guide.gif
	output_guide.gif
	operate_guide.gif

	显示login图片，
	bmp格式耗时：16bit，800*480，751kb，383ms，
	png耗时：24bit，800*480，194kb，2753ms
	jpg低质量耗时：24bit，800*480，29k，623ms
	jpg高质量耗时：24bit，800*480，138k， 1421ms

12.7、拼音输入法:
	GUI_StoreKeyMsg(),可存储键值。
	GUI_SendKeyMsg()


13：Demo外设控制
        ETH复位：FP8

        进仓OC：PB8 ，出仓OC: PB9 
        定量电极：(PC9)
        进出仓电极：DIR：PC8， EN：PC7， CLK：PC6_TIM3_CH1（8k）

        气压：PF7_ADC3_IN5
        气阀：PF9， 液阀：PF6
        泵：PB6_TIM4_CH1（1k）

        数字电位器：PC10_SPI3_CLK,  
	           PC11_SPI3_MISO,  
	           PC12_SPI_MOSI
                           PH8_CS


14：WBC数据采集模块
14.1、采样ADC1进行数据采集，采用半中断和全中断的方式，每一帧数据的大小为512*2=1024Byte。
14.2、a、在10秒内的采样，通过数据采集并通过网卡发送的状态下，F429的查询/采样为：10W/3W。F407_B版的查询采样比：26W/3W, 采样（2.927P/ms）
         b、在10秒内采样，不间断发送数据，F429的查询/采样/发送为：4061W/3W/3W
         c、示波器30k正弦波，1秒内，429不断的调用算法接口处理采样数据，查询/计算：96W/3W
         d、开启ADC采样，通过429不断的调用算法接口处理仿真数据，数据生成/调用算法次数为：8375/2931=2.86
         e、算法计算耗时（调用该接口calculate）：150002脉冲信息，用时263ms，即：每毫秒处理570.34个脉冲。


15：NAND Flash（STM32仅支持SLC类型的NAND，SLC坏块概率较小，MLC相对大一点）
15.1、正点原子的429的板子的nand flash型号位：MT29F4G08（8位）nandflash。Nand ID: DC909556
	可分为两个plane，每个plane有2048个block，每个block有64个page，每个page的大小：2K+64Byte。
	2*2048*64*（2K+64Byte）=553648128Byte=512MB
15.2、最小擦除单位为：block，包含64个page，每个page分为有个区域：2K（数据区）+64Byte（备用区，用于存放校验ECC）。
	ECC来实现坏块管理和磨损均衡。
15.3、nand flash地址分为三类：块地址（block address，12个地址线）、页地址（page address，6个地址线）和列地址（column address， 17个地址线）。
	这三个地址分五个周期发送，从而实现数据读取或存储的定位。
15.4、 nand flash驱动需要用到一系列命令。如：读ID，读写页，内部数据移动等。
	在写之前，需擦除（擦除的最小单位为块（128+4k）），擦除的过程为写0xFF（也就是nandflash一般初始为1，写入数据：把对应的1改为0）。

15.5、ECC（错误检查和校验）：有三种算法：汉明码（能实现1bit错误修正和2bit的粗无检测）、RS码和BCH码。
	STM32的FMC支持nand flash的硬件ECC计算，采用汉明码的计算方式，只进行ECC计算(每512个字节产生一个32bit的ECC值）。修正和检测需结合软件实现。
	一个全为0x11（0xFF）的512个字节的buf，ECC的值为：0x00。 ECC校验在使用是开启，使用后关闭，否则会有问题。
	擦涂整个nand耗时20秒。

15.6、FTL（Flash Translation Layer）闪存转换层，由于每个BLOCK都有擦除限制，所以需要一层代码实现，坏块管理和磨损均衡。
	FTL算法的实现方式（步骤）：
	1、识别和标记坏块。
	    通过在每个block的第一个page的第一个字节，和第二page的备份区的第一个字节，写入一个非0xFF，来标记一个坏块。
	    通过每次对比ECC来识别一个坏块。
	2、转换表，
	   2.1、 逻辑地址（block序号）和物理地址的转换表，该转换表为动态的。
	   2.2、 使用每个block的第一个页的备份区的前四个字节，其中第一个字节：用来做为标识坏块，第二字节：标识该block是否使用，
		第三、四个字节：为逻辑地址（block的序号）。
	3、保留区	
	     有两个作用：1：用来代替坏块，2：复写数据时，代替复写的块，提高写入速度。
15.7、Nand flash 片选脚接在FMC_NCE2, nandflash的地址空间为0x70000000，如果时FMC_NCE3(当前使用的为NCE3), 则nandflash的地址空间为：0x80000000
	FMC如果有多个总线设备时，必须确保他们分时复用。否则会出错。

15.8、Nand Flash 擦除时间为：20秒， 文件系统第一次上电初始化耗时：70秒
	  Nand 页(2k)擦除时间200us，块擦除（1.5ms）,
	


15.7、NandFlash GPIO使用（FMC_D0-FMC_D7和SDRAM共用）
	FMC_D0 = PD14
	FMC_D1 = PD15
	FMC_D2 = PD0
	FMC_D3 = PD1
	FMC_D4 = PE7
	FMC_D5 = PE8
	FMC_D6 = PE9
	FMC_D7 = PE10
	FMC_A16_CLE = PD11
	FMC_A17_ALE = PD12
	FMC_NOE = PD4	
	FMC_NWE = PD5
	FMC_NWAIT = PD6
	FMC_NCE3 = PG9

15-16:
	nor读取比nand快，nor芯片内xip结构，可以执行代码（配置为内存映射模式）
	nand比nor写入快


16: W25Q256JVEQ
	最大速率：133M,容量：256Mbit=32MB，支持SPI模式0和3，全双工MSB，数据长度8Bit，10万次擦写，20年数据，
	坏块管理？
	
	页编程时间：典型值 0.45ms，最大值 3ms
	扇区擦除时间（4KB）：典型值 50ms，最大值 400ms
	块擦除时间（32KB）：典型值 120ms，最大值 1600ms。
	块擦除时间（64KB）：典型值 150ms，最大值 2000ms。
	整个芯片擦除时间：典型值 80s，最大值 400s。
	
	0、spi nor flash数据下载算法（非FATFS方式，直接烧录方式）
		0.1、Dev_Inf.c配置外部flash信息
		0.2、Loader_Src.c功能（读写、擦涂等）
			Init(GPIO/CLK)，Write写，SectorErase擦除
		0.3、修改文件名 ***.stldr
		0.4、放到bin/ExternalLoader目录下
		0.5、如何采用直接烧录的方式，使用时如何读取？
		
	1、实际和W25Q256通信，F4需配置spi速度为多大？
		1.1、F429,SPI4在APB2上最大通信速率为45Mbit/s(180M ), 42Mbit/s(168M), 最好使用DMA方式
		
		
	2、spi复用的话，采用哪种方式（查询，终端，DMA），以及与TP、数字电位器的兼容问题？
		2.1、TP、数字电位器最大的通信速率？当前为spi的256分频->大概175k，模式0，全双工MSB，查询方式
		2.2、在读取TP、数据电位器和spiflash时需要切换，重新配置spi的clk及DMA中断方式等，由于采用cubemax配置
			 所以spi默认设置为spi4的DMA方式，在读取写入spiflash设置为DMA方式，
			 在读取TP、数字电位器时切换至普通查询方式。

		
	3、W25Q256JVEQ驱动
		3.1、最小编辑单位为页（256byte），当按照页写入时，超过页写入范围，会跳转至页首开始重新开始写入，
		3.2、最小擦除单位扇区（4k），也可按块擦除（32k、64k），发送擦除命令->需擦除的地址（擦除区域范围内地址，一般给出首地址）
	
	4、FATFS同时挂载W25Q256和NAND配置问题
		4.1、diskio.c中添加两者的驱动
		4.2、修改FATFS的配置
	
	5、USB作为从设备（U盘）接入电脑，操作W25Q256和NAND
		5.1、在cubemax中修改usb配置
		5.2、添加通过Fatfs读写W25Q256和Nand接口
	
	6、验证
	   6.1、功能正常
	   6.2、UI读取图片速度，UI切换影响
	   
			F429主频180M, FatFS_NAND:-----------
			Nand读取3个字库耗时：分别是273，332，635ms。 
			Nand读取gif耗时：
				fail_output2.gif=76ms, operate.gif=1326ms, re_input.gif=95ms
			Nand读取bmp耗时：
				login.bmp=305ms, qc_analysis.bmp=2ms, btn_empty_1=6ms, service界面下小bmps=1-2ms
				config界面下小bmps=1-2ms, countfail.bmp=95ms, re_count.bmp=15ms，counting.bmp=26ms
				confirm_take.bmp=28ms, n_sec_outin_in.bmp=29ms
			Nand读取一个数据WBCHGB_Info耗时：0-1ms
			-----------------NAND
			
			F429主频180M, RL_FlashFS_NAND:-----------
				Nand读取3个字库耗时：分别是489，596，1158ms。
				Nand读取gif耗时：
				fail_output2.gif=ms, operate.gif=614ms,783ms, re_input.gif=ms, input.gif=229ms,ok_output.gif=225ms
				Nand读取bmp耗时：
				login.bmp=382ms, qc_analysis.bmp=2ms, btn_empty_1=6ms, service界面下小bmps=1-2ms
				config界面下小bmps=1-2ms, countfail.bmp=95ms, re_count.bmp=15ms，counting.bmp=110ms
				confirm_take.bmp=28ms, n_sec_outin_in.bmp=72ms
			Nand读取一个数据WBCHGB_Info耗时：0-1ms
			-----------------NAND
	
	7、生产烧录工装软件设计以及操作流程
		工装软件：显示两个U盘（spiflash, nand）,可以拷贝数据到指定的U盘。
				  修改EPPROM中版本信息等，自动升级（将U盘中的数据拷贝到mcu flash）。
				  板子自检程序。
		上述工装流程自动化的实现

	16.8、AN5054，stm32cubeprogrammer进行安全烧录
		  UM2237, stm32cubeprogrammer，工具节度使，使用，包括命令方式
		  
16：FATFS 文件系统
      免费开源的FAT（兼容Windows的FAT）文件系统，支持多分区（物理驱动器），多文件名，长文件名，支持OS。


16：RL-FlashFS	Version6.13.6
	打开一个文件所需要的栈：512+96Bytes
	FS_LFN_CM3_L.lib:Library with long filename support for devices based on Cortex-M3, Cortex-M4, and Cortex-M7 in little endian configuration.
	
		0. NAND Flash这块经常有人咨询，这里发布一个完整的解决方案，支持擦写均衡，坏块管理，ECC和掉电保护。    
			早期的时候我们是用的自己做的NAND算法，支持滑块管理，擦写均衡，实际测试效果不够好，容易出问题，所以放弃了。
		1. 此例子仅支持MDK4.74版本，因为RTX，RL-FlashFS，RL-USB都是来自MDK4.74的安装目录，使用MDK4.74才是最佳组合。
		2. RL-FlashFS本身支持擦写均衡，坏块管理，ECC和掉电保护。其中使用掉电保护的话，请开启配置文件中的FAT Journal。
		3. 在前几年的时候，有客户反应使用RL-FlashFS写入文件多后会写入越来越慢，原因是没有正确配置，加大文件名缓冲个数即可。
		4. 当前使用的短文件名的库，使用长文件名的话请更换为长文件名的库，也在MDK的安装目录里面。
		5. RL-FlashFS是FAT兼容的文件系统，也就是说可以在window系统上面模拟U盘，提供的程序代码已经做了支持。
		6. RL-FlashFS的文件名仅支持ASCII，不支持中文，这点要特别注意。
		7. 首次格式化后使用，读速度2.3MB/S左右，写速度3.2MB/S左右，配置不同的文件系统缓冲大小，速度有区别。
		8. RL-FlashFS的函数是标准的C库函数，跟电脑端的文件系统使用方法一样。
		9. RL-FlashFS与FatFS的区别，FatFS仅是一个FAT类的文件件系统，擦写均衡，坏块管理，ECC和掉电保护都不支持。
			这些都需要用户自己去实现。
		10. UFFS，YAFFS这两款文件系统是不兼容FAT的，也就是无法在Windows端模拟U盘。
	16.2: 开发过程
		在MDK中选中、配置RTE组件。
		配置fs。
		配置硬件？
		编写用户代码。
		调试
	16.3： RL-FlashFS(FAT32内部读写)：写：1.767MB/s(偶尔0.762M/s), 读：2.556MB/s
		   RL-FlashFS(FAT16内部读写)：写：2.003MB/s(偶尔0.762M/s), 读：3.062MB/s
		   如果通过U盘(FAT32)：写：177
		   如果通过U盘(FAT16)：写：177
		   
		   Fatfs：写：177KB/s, 读：687KB/s

		   FAT12： 128MB
		   FAT16： 128MB-2GB
		   FAT32： >128MB(>2GB)

17、USB
    17.1、采用四根线，VDD、GND、D+、D-，采用差分方式传输信号。
	在USB主机：D+-外接15k电阻到地，此时均为低电平，无设备输入。
	USB高速设备：在D+上接1.5k电阻到VCC。
	USB低速设备：在D-上接1.5k电阻到VCC。由此区分设备类型。
	
	
	429支持USB OTG FS(全速: 12Mb/S, 低速：1.5Mb/S)和USB OTG HS(高速, 需外接PHY芯片)
     	OTG FS 同时支持 HNP 和 SRP，主要特性分为三类：通用特性、主机模式、从机模式。



18、WIFI
	乐鑫 ESP8266，需使用SPI透传协议（双线），ESP8266作为从机和MCU的SPI通信。
	需用到6根线，4根SPI，2根用于告知MCU当前从机接受和发送状态以实现数据流控制。



19、STM32F429 USB
	1、自带有USB OTG FS（全速）和USB OTG HS（高速），其中HS需要外接高速PHY芯片。
	2、当使用USB OTG FS时，十一款双角色控制器，同时支持从机和主机功能，符合USB2.0 OTG标准，全速：12Mb/s, 低速：1.5Mb/s
		从机支持全速， OTG FS同时支持HNP和SRP。
	3、USB OTG FS主要特征分为三类：
		通用特性：支持主机协商协议(HNP)和会话请求协议(SRP)，...
		主机模式: 
		从机模式: 
	4、


20、仪器
 	1、小孔电压位，导通前位：，导通后：



21、整机测试
	1、当前一条数据：1577字节，每个文件存储250条数，9M可存储6000条数据。



22、algo
	v0.0.5 工装和样本分析之分
	v0.0.5.1, 支持高频工装信号








