#ifndef _MIL1394_EMU_XT_
#define _MIL1394_EMU_XT_

/* Includes */

#define _LINUX1394_PXI
/*#define MIL1394_EMU_XT_EXPORTS*/

/*#define    NIVISA1394_PXI
#define    CVI_1394_PXI		*/

#if defined NIVISA1394_PXI

  #define NIVISA_PXI          /* to include PXI VISA Attributes */
 
  #include <visa.h>
    #ifdef MIL1394_EMU_XT_EXPORTS
      #define MIL1394_EMU_XT_API __declspec(dllexport)
    /* 32位系统   ViBusAddress为32位  64位系统建议调试*/
    #else
      #define MIL1394_EMU_XT_API __declspec(dllimport)
    #endif

    typedef   ViUInt32 TNFU32;
    typedef   ViInt32  TNFI32;
    typedef   TNFU32   AddrMem;

#elif defined _LINUX1394_PXI

    #include <sys/fcntl.h>
    #include <sys/types.h>

    #if defined(MIL1394_EMU_XT_EXPORTS)
     #define MIL1394_EMU_XT_API __attribute__((visibility("default")))
    #else
     #define MIL1394_EMU_XT_API
    #endif

    typedef int64_t     TNFI64;
    typedef u_int64_t   TNFU64;
    typedef int32_t     TNFI32;
    typedef u_int32_t   TNFU32;

    #if defined __x86_64__
        typedef TNFI64   AddrMem;
    #elif defined __i386__
        typedef TNFI32   AddrMem;
    #elif defined __aarch64__
        typedef TNFI64   AddrMem;
    #endif

#elif defined _Vxworks1394_PXI

    #define MIL1394_EMU_XT_API
    #include <vxWorks.h>

    typedef  UINT64 TNFU64;
    typedef  UINT32 TNFU32;
    typedef  INT32  TNFI32;

    #ifndef _WRS_CONFIG_LP64
     typedef TNFU32   AddrMem;
    #else
     typedef TNFU64   AddrMem;
    #endif
    
    #if (_BYTE_ORDER != _LITTLE_ENDIAN)
      #define CPU_BigToLittle_
    #endif /* _BYTE_ORDER */


#else

  #define _Vxworks1394_PXI
 
  #include <vxWorks.h>
  #define MIL1394_EMU_XT_API

  typedef  UINT64 TNFU64;
  typedef  UINT32 TNFU32;
  typedef  INT32  TNFI32;
  
 #ifndef _WRS_CONFIG_LP64
  typedef TNFU32   AddrMem;
 #else
  typedef TNFU64   AddrMem;
 #endif
  
  #if (_BYTE_ORDER != _LITTLE_ENDIAN)
    #define CPU_BigToLittle_
  #endif /* _BYTE_ORDER */

#endif



/*This class is exported from the TNF_CRB_DLL.dll*/
#if defined __cplusplus
extern "C"{
#endif
#define		SoftDrv_CRB_VER					"V4.5.X_V2022.07.18\0" 


#define		LOGIC_CRB_VER1					0x20141106 /*定版20141106*/
#define		LOGIC_CRB_VER2					0x20151128 /*定版20151203*/
#define		LOGIC_CRB_VER3					0x20180813 /*定版20181206，20181128，20180917*/
#define		LOGIC_CRB_VER4					0x20190710 /*定版20190710，20200310*/
#define		LOGIC_CRB_VER5					0x20200514 /*定版20200627*/
#define		LOGIC_CRB_VER6					0x20200904 /*定版20201117*/
#define		LOGIC_CRB_VER7					0x20210803 /*定版0x20210804  0x20210805 0x20210806 更改 心跳方式，仿真卡可以选择几周期加几 ，或者按时间增加心跳 */
#define		LOGIC_CRB_VER8					0xFFFFFFFF 
/*
	LOGIC_CRB_VER7（0x20210803）
		当逻辑版本的值>= VER7时,默认配置心跳参数HeartBeatStep为周期/步长（1周期增加1填写：0x0101），
		                        支持配置心跳参数HeartBeatStep为时间（15ms增加1填写：15000）;
		当 VER6< 时逻辑版本的值 < VER7时，不支持选择，四节点卡心跳只能按时间增加。
	LOGIC_CRB_VER6（0x20200904）
		当逻辑版本的值>= VER6时,配置心跳参数HeartBeatStep为时间（15ms增加1填写：15000）;
		当逻辑版本的值<  VER6时,配置心跳参数HeartBeatStep为周期/步长（1周期增加1填写：0x0101）。
	LOGIC_CRB_VER5（0x20200514）
		当逻辑版本的值< VER5时 节点ID字段 中BUSID 6位可写；
		当逻辑版本的值> VER5时 软件加载节点ID。
	LOGIC_CRB_VER4（0x20190710）
		当逻辑版本的值< VER4时，更新数据接口参数ID保留；
		当逻辑版本的值>= VER4时，更新数据接口参数ID有实际意义，配置软件VPC是否取反。
	LOGIC_CRB_VER3（0x20180813）
		当逻辑版本的值< VER3时，板卡更新数据有极小概率出现错包情况。建议升级逻辑。
	LOGIC_CRB_VER2（0x20151128）
		当逻辑版本的值< VER2时所有消息的心跳步长统一配置，不能控制单条消息。
	LOGIC_CRB_VER1（0x20141106）
		版本过低，建议升级。
*/
        /*Return Value*/
#define   OK                               0
#define   READ_SOLT_NUM_FAIL              -100	/*获取插槽号失败*/
#define   NODE_INIT_FAIL                  -101	/*节点初始化失败*/
#define   SND_EVENT_LEN_ERR               -102	/*发送长度错误*/
#define   SND_EVENT_ID_ERR                -103	/*发送事件消息ID错误*/
#define   SND_EVENT_FIFO_FULL             -104	/*发送事件消息队列满*/
#define   SND_EVENT_SPACE_LACK            -105	/*发送事件空间不足*/
#define   LLC_SPEED_SEL_ERROR             -106  /*链路层速率设置错误*/
#define   CHANNAL_CONFIG_ERROR            -107  /*通道配置错误*/
#define   PHY_ID_ERR                      -108  /*id超出总线节点数*/
#define   PHY_PORT_ERR                    -109  /*port数超出访问范围*/
#define   PHY_PAGE_ERR                    -110  /*page数超出访问范围*/
#define   PHY_REG_ERR                     -111  /*寄存器超出范围*/
#define   PHY_TYPE_ERR                    -112  /*类型超出范围，只能是1or5*/
#define   PER_LIMIT_ERR                   -113  /*周期或次数超出范围 0-65535*/
#define   CFG_Tx_NUM_Err                  -114	/*消息序号超出发送配置条数*/
#define   SEND_DMA_ERR                    -115	/*发送消息DMA错误*/
#define   LIMIT_ERR                       -116	/*周期或步长越界应在0-255之间*/
#define   SEND_DMA_BUSY                   -117  /*上次消息未发送，新消息不更新*/
#define   TNF_FIFI_IS_FULL                -118	/*队列发送满了*/
#define   TNF_FIFI_NOT_START              -119	/*队列发送线程未启动*/
#define   DMAaddr_mallcoFaile             -120	/*队列发送线程未启动*/
#define   LLC_TIMEOUT                     -121	/*链路层访问超时*/
#define   PHY_OFFSET_ERR                  -122	/*物理层读出地址异常*/
#define   NO_THIS_MSGID                   -123	/*当前消息缓存区 无消息ID消息*/
#define   NO_NEW_MSGID                    -124	/*有该消息ID消息， 消息已取过无新消息*/
#define   SND_HEADER_ERR                  -125	/*1394头不含 A0 错误的1394头*/
#define   ZONE_NOT_ENOUGH                 -126	/*输入数组空间不足*/
#define   HANDLE_NULL	                  -127	/*设备句柄不合法*/




#define   NET_MSGID_SPILL                 -301  /*网络管理消息ID超出范围*/
#define   ACK_MSGID_SPILL                 -302  /*上网请求消息ID超出范围*/
#define   INPUT_BUFF_NULL                 -303  /*输入缓存区为空*/
#define   RCV_LENGTH_SPILL                -304  /*接收数据长度大于输入长度*/
#define   MSG_IS_LOST                     -305  /*启动接收的DMA数量为0错误    */
#define   MSG_MAYBE_LOST                  -306  /*消息可能有丢失  双链表中有一条链表已近存满*/

#if  1
        /*Return Value*/
        enum Speed_TNF{         S100M = 0U,
                                S200M,
                                S400M};
/*
*       1：总线复位计数 2：STOF包发送计数 3：STOF包接收计数 4：发送消息计数
*		5：接收消息计数 6：接收HCRC错误消息计数 7：接收MSGID错误消息计数
*		8：接收DCRC错误消息计数 9：接收VPC错误消息计数 10：发送事件消息计数
*		11：发送事件消息错误计数 12：接收事件消息计数 13：接收事件数据CRC错误消息计数
*		14：接收事件数据VPC错误消息计数 15 接收STOFVPC错误计数 16 接收STOF数据CRC错误
*		17：接收异步流软件VPC错误计数 18 接收事件消息软件VPC错误计数 19 主题ID错误计数
*/
	enum TNF_Msg_CNT{
		BusResetCNT = 1U,
		STOF_SendCNT,
		STOF_RecvCNT,
		Asyn_SendCNT,
		Asyn_RecvCNT,
		ERR_Asyn_HCRCCNT,
		ERR_Asyn_MsgIDCNT,
		ERR_Asyn_DCRCCNT,
		ERR_Asyn_VPCCNT,
		Event_SendCNT,
		Event_SendErrCNT,
		Event_RecvCNT,
		ERR_Event_DCRCCNT,
		ERR_Event_VPCCNT,
		ERR_STOF_VPCCNT,
		ERR_STOF_DCRCCNT,
		ERR_Asyn_SVPCCNT,
		ERR_Event_SVPCCNT,
		ERR_TopicIDCNT
	};
#endif

	typedef struct
	{
		TNFU32 RsrcManager;
		TNFU32 vi;                    /*设备句柄*/
		TNFU32 viMem;                 /*内存申请*/

		AddrMem vimemAddress;          /*内存地址偏移*/
		TNFU32 NodeDMASendPhyAdd;     /*内存地址偏移-》发送*/
		TNFU32 NodeDMARcvPhyAdd0;     /*内存地址偏移-》接收*/
		TNFU32 NodeDMARcvPhyAdd1;     /*内存地址偏移-》接收*/
		AddrMem NodeDMASendUserAdd;
		AddrMem NodeDMARcvUserAdd0;
		AddrMem NodeDMARcvUserAdd1;

		TNFU32 card;                  /*板卡索引*/
		TNFU32 node;                  /*节点索引*/
		TNFU32 CardLogic_Ver;         /*板卡逻辑版本*/
		TNFU32 Card_type;

		AddrMem NodeBaseCommon;        /*Bar1 公共寄存器地址 */
		AddrMem NodeBase1;             /*Bar1 节点偏移地址 */
		AddrMem NodeBase2;             /*Bar2 节点偏移地址 */
		TNFU32 SndEventMsgAddr;       /*发送事件消息偏移地址*/

		TNFU32 Pointstate;
		TNFU32 UpPoint;
		TNFU32 DownPoint;
		TNFU32 CFG_Tx_NUM;            /*发送消息条数*/
		TNFU32 DMA_Rx_NUM;            /*接收消息DMA缓存大小*/
		TNFU32 nodeState;             /*当前节点是否正常, 0 正常*/
	}_TNF_Node_Struct;


#ifdef CPU_BigToLittle_
	typedef  struct
	{
		TNFU32	 PacketFlag     :1;   /*第一个字 1：接收标志 0：自己发送的消息  */
		TNFU32	 LenERR         :1;   /*第一个字 错误标志 -  与配置接收长度不一致*/
		TNFU32	 VPCERR         :1;   /*第一个字 错误标志 -  垂直奇偶校验错误*/
		TNFU32	 CRCERR         :1;   /*第一个字 错误标志 -  循环冗余校验错误*/


		TNFU32	 Speed          :2;   /*第一个字 消息包速率 0 S100，1S200，2S400*/
		TNFU32	 SVPCERR        :1;   /*第一个字 消息软件VPC错误*/
		TNFU32	 STOFLIMITErr   :1;   /*第一个字 消息类型为STOF时，STOF包的周期超出门限范围*/
		TNFU32	 ser1           :6;   /*第一个字 保留*/
		TNFU32	 MessageTYPE    :2;   /*第一个字 0busreset 1stof 2异步-流消息 3事件*/

		TNFU32	 RTC            :16;  /*第一个字 16位相对STOF的时间标签*/

		/*TNFU32	 StateFlag;   */      /*第一个字 消息状态字 高32位     */
		TNFU32	 LRTCH;               /*第二个字 64为时间标签 高32位     */
		TNFU32	 LRTCL;               /*第三个字 64为时间标签 低32位     */

		TNFU32 	Headerlength    :16;  /*第四个字4 */
		TNFU32 	Headerser       :2;   /*第四个字4 */
		TNFU32 	HeaderChannel   :6;   /*第四个字4 */
		TNFU32 	Headerflag      :8;   /*第四个字4 */

		/*TNFU32 	Header1394;   */    /*第四个字 4   如果不关心可以注释掉1394 头的格式，使用此变量*/
		TNFU32 	MessageID;            /*第五个字5*/
	}_MsgState;
#else
	typedef  struct
	{
		TNFU32  RTC            :16;  /*第一个字 16位相对STOF的时间标签*/

		TNFU32  MessageTYPE    :2;   /*第一个字 0busreset 1stof 2异步-流消息 3事件*/
		TNFU32  ser1           :6;   /*第一个字 保留*/

		TNFU32  STOFLIMITErr   :1;   /*第一个字 消息类型为STOF时，STOF包的周期超出门限范围*/
		TNFU32  SVPCERR        :1;   /*第一个字 消息软件VPC错误*/
		TNFU32  Speed          :2;   /*第一个字 消息包速率 0 S100，1S200，2S400*/

		TNFU32  CRCERR         :1;   /*第一个字 错误标志 -  循环冗余校验错误*/
		TNFU32  VPCERR         :1;   /*第一个字 错误标志 -  垂直奇偶校验错误*/
		TNFU32  LenERR         :1;   /*第一个字 错误标志 -  与配置接收长度不一致*/
		TNFU32  PacketFlag     :1;   /*第一个字 1：接收标志 0：自己发送的消息  */

		/*TNFU32  StateFlag;   */      /*第一个字 消息状态字 高32位     */
		TNFU32  LRTCH;               /*第二个字 64为时间标签 高32位     */
		TNFU32  LRTCL;               /*第三个字 64为时间标签 低32位     */

		TNFU32  Headerflag      :8;   /*第四个字4 */
		TNFU32  HeaderChannel   :6;   /*第四个字4 */
		TNFU32  Headerser       :2;   /*第四个字4 */
		TNFU32  Headerlength    :16;  /*第四个字4 */
		/*TNFU32  Header1394;     */    /*第四个字 4   如果不关心可以注释掉1394 头的格式，使用此变量*/
		TNFU32  MessageID;            /*第五个字5*/
	}_MsgState;
#endif



	typedef  struct
	{
#ifdef CPU_BigToLittle_
		/*TNFU32  Headerlength  :16;*/  /*第四个字4*/
		/*TNFU32  Headerser     :2; */  /*第四个字4*/
		/*TNFU32  HeaderChannel :6; */  /*第四个字4*/
		/*TNFU32  Headerflag    :8; */  /*第四个字4*/
#else
		/*TNFU32  Headerflag    :8; */  /*第四个字4*/
		/*TNFU32  HeaderChannel :6; */  /*第四个字4*/
		/*TNFU32  Headerser     :2; */  /*第四个字4*/
		/*TNFU32  Headerlength  :16;*/  /*第四个字4*/
#endif
		TNFU32  Header1394;           /*第四个字4 如果不关心可以注释掉1394 头的格式，使用此变量*/

		TNFU32  STOFPayload0;
		TNFU32  STOFPayload1;        /*5,4位代表余度分支，配置错误导致某些产品进入故障模式*/
		TNFU32  STOFPayload2;
		TNFU32  STOFPayload3;
		TNFU32  STOFPayload4;
		TNFU32  STOFPayload5;
		TNFU32  STOFPayload6;
		TNFU32  STOFPayload7;
		TNFU32  STOFPayload8;
		TNFU32  STOFVPC;             /*14*/
	}_MsgSTOF;
	typedef  struct
	{
#ifdef CPU_BigToLittle_
		/*TNFU32  Headerlength  :16;*/  /*第四个字4*/
		/*TNFU32  Headerser     :2; */  /*第四个字4*/
		/*TNFU32  HeaderChannel :6; */  /*第四个字4*/
		/*TNFU32  Headerflag    :8; */  /*第四个字4*/
#else
		/*TNFU32  Headerflag    :8; */  /*第四个字4*/
		/*TNFU32  HeaderChannel :6; */  /*第四个字4*/
		/*TNFU32  Headerser     :2; */  /*第四个字4*/
		/*TNFU32  Headerlength  :16;*/  /*第四个字4*/
#endif
		TNFU32 	Header1394;            /*第四个字4   如果不关心可以注释掉1394 头的格式，使用此变量*/

		TNFU32 	MessageID;            /*第五个字5*/
		TNFU32 	Security;             /*第六个字6*/

#ifdef CPU_BigToLittle_
		/*TNFU32 	serv         :16;*/   /*第七个7*/
		/*TNFU32 	busID        :10;*/   /*第七个7*/
		/*TNFU32 	NodeID       :6; */   /*第七个字7*/
#else
		/*TNFU32 	NodeID       :6; */   /*第七个字7*/
		/*TNFU32 	busID        :10;*/   /*第七个7*/
		/*TNFU32 	serv         :16;*/   /*第七个7*/
#endif
		TNFU32 	NodeID;				        /*第七个7*/

#ifdef CPU_BigToLittle_
		/*TNFU32 	payloadLen   :24;*/   /*8*/
		/*TNFU32 	payloadLen   :24;*/   /*8*/
#else
		/*TNFU32 	payloadLen   :24;*/   /*8*/
		/*TNFU32 	Priority     :8;*/    /*8*/
#endif
		TNFU32 	payloadLen;           /*8*/

		TNFU32 	healthStat;           /*9*/
		TNFU32 	heartBeat;            /*10*/
		TNFU32 	msgData[502];         /*11 -   offset = msgData[payloadLen/4 - 2]     vpc = msgData[payloadLen/4 + 1]*/
	}_MsgAsyn;

	typedef  struct RcvMsgList
	{
		/*TNFU32* 	        previous;*/ /*上一条消息位置 labview时转换使用*/
		/*TNFU32* 	        next;   */  /*下一条消息位置 labview时转换使用*/
		struct RcvMsgList* 	previous; /*上一条消息位置*/
		struct RcvMsgList* 	next;     /*下一条消息位置*/
#ifdef NIVISA1394_PXI
		TNFU32              msgstateAddr;/*消息状态      3个字*/
		TNFU32              msgAddr;     /*1394消息格式       */
#else
		_MsgState*          msgstateAddr;/*消息状态      3个字*/
		_MsgAsyn*           msgAddr;     /*1394消息格式       */
#endif
		TNFU32              listID;
	}_RcvMsgList;

	typedef struct
	{
		TNFU32 Header1394;        /*1394头*/
		TNFU32 MessageID;         /*消息ID*/

		TNFU32 HeartBeatStyle;    /*2 自动更新-时间方式 ； 1 自动更新-步进方式 ；0 软件填写*/
		TNFU32 HeartBeatEnable;   /*1--使能 0--自动更新停止*/
		TNFU32 HeartBeatStep;     /*步进方式：实例0x0201 两周期加一 0x0101 一周期加一 ,0x0102一周期加2 ；时间方式：时间 0-655350us 精度10us    */
		TNFU32 HeartBeatinitValue;/*自动更新时心跳初始值 0 - 65535*/

		TNFU32 HCRCErrorEnable;	  /*头CRC故障是否注入1使能，0停止*/
		TNFU32 DCRCErrorEnable;	  /*数据CRC故障是否注入1使能，0停止*/
		TNFU32 VPC_ErrorEnable;	  /*单包VPC错误使能，0--VPC错误使能 1--VPC错误不使能*/
		TNFU32 VPC_ErrorBit;      /*32位，VPC错误取反位   */
		TNFU32 ErrMode;           /*错误模式	 1-无限次,0-有限次*/
		TNFU32 ErrNum;            /*错误数量 */

		TNFU32 SoftVPCenable;     /*0，不使能，1软件VPC使能（影响所有消息按 '亦或'），3软件VPC使能（影响所有消息按 '亦或取反'）*/
		TNFU32 nodeIdEnable;      /*节点ID统一加载 或者 软件自定义  0自主加载  1.软件加载 加载值从 更新数据时写入*/
	} _TNF_ASYNCCFG_Struct;

	typedef struct
	{
		TNFU32 STOFPeriod;          /*STOF 周期*/

		TNFU32 SysCntType;          /*SysCntType 1自动增加 ，0软件填写 */
		TNFU32 STOFPayload4;        /*Systemcnt 自动增加初始值  SysCntType 变为1时有效一次*/

		TNFU32 HCRCErrorEnable;	    /*头CRC故障是否注入1使能，0停止*/
		TNFU32 DCRCErrorEnable;	    /*数据CRC故障是否注入1使能，0停止*/
		TNFU32 VPC_ErrorEnable;	    /*单包VPC错误使能，0--VPC错误使能 1--VPC错误不使能*/
		TNFU32 VPC_ErrorBit;        /*32位，VPC错误取反位   */
		TNFU32 ErrMode;             /*错误模式	 1-无限次,0-有限次*/
		TNFU32 ErrNum;              /*错误数量    0-65535*/

		TNFU32 STOFPRD_ErrorEnable; /*周期错误使能 1使能，0停止*/
		TNFU32 STOFPRD_Error_Period;/*周期错误 的错误周期时间 us */
		TNFU32 STOFPRD_Error_NUM;   /*周期错误 的错误周期个数   0 - 65535 */
	} _TNF_STOFCFG_Struct;

	typedef struct
	{
		TNFU32    Phy_ID;
		TNFU32    Port;
		TNFU32    CMD;               /*5—打开端口  1—断开端口     0,1,2,3,4,5,6*/
	}_PHY_Remote_Com_;

	typedef struct
	{
		TNFU32    Phy_ID;
		TNFU32    type;              /*1-base寄存器  5-page寄存器*/
		TNFU32    page;              /*0-7*/
		TNFU32    portNum;           /*0-15*/
		TNFU32    reg;               /*0-7*/
	}_PHY_Remote_Call_;

	typedef struct
	{
		TNFU32    net_msg0_ID;       /*wdtmsg id*/
		TNFU32    net_msg1_ID;       /*生命状态*/
		TNFU32    net_msg2_ID;       /*上网请求*/
	}_Filter_netMsgID_Struct;

#ifdef NIVISA1394_PXI
	#ifdef MIL1394_EMU_XT_EXPORTS
		typedef  _TNF_Node_Struct* P_TNF_Node;
	#else
		typedef  void* P_TNF_Node;
	#endif
#else
	typedef  _TNF_Node_Struct* P_TNF_Node;
#endif

	/*读写接口*/
	MIL1394_EMU_XT_API TNFI32 TNF_W_5643Reg(P_TNF_Node pTNF, TNFU32 offset, TNFU32 value);
	MIL1394_EMU_XT_API TNFI32 TNF_R_5643Reg(P_TNF_Node pTNF, TNFU32 offset, TNFU32 *value);
	MIL1394_EMU_XT_API TNFI32 TNF_W_CFGReg(P_TNF_Node pTNF, TNFU32 offset, TNFU32 value);
	MIL1394_EMU_XT_API TNFI32 TNF_R_CFGReg(P_TNF_Node pTNF, TNFU32 offset, TNFU32 *value);
	MIL1394_EMU_XT_API TNFI32 TNF_W_LLC(P_TNF_Node pTNF, TNFU32 offset,TNFU32 value);
	MIL1394_EMU_XT_API TNFI32 TNF_R_LLC(P_TNF_Node pTNF, TNFU32 offset, TNFU32 *value);
	MIL1394_EMU_XT_API TNFI32 TNF_W_PHY(P_TNF_Node pTNF, TNFU32 offset, TNFU32 value);
	MIL1394_EMU_XT_API TNFI32 TNF_R_PHY(P_TNF_Node pTNF, TNFU32 offset, TNFU32 *value);

#ifdef NIVISA1394_PXI
	MIL1394_EMU_XT_API TNFI32 Mil1394_Struct(P_TNF_Node pTNF,_TNF_Node_Struct* pTNFstruct);
	MIL1394_EMU_XT_API TNFU32 MSG_RECV_MemAlloc_viMem(P_TNF_Node pTNF);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_listTOmsg(P_TNF_Node pTNF,_RcvMsgList** msglist,_MsgState* msgState,TNFU32 msg[]);
#elif defined _Vxworks1394_PXI
	MIL1394_EMU_XT_API TNFI32 Mil1394_init(TNFU32 Card,AddrMem BAR1_base_add,AddrMem BAR2_base_add,TNFU32 *NodeCnt);
#endif

	/*设备接口*/
	MIL1394_EMU_XT_API char* Mil1394_getDrv_Version();
	MIL1394_EMU_XT_API TNFI32 Mil1394_XT_Found(TNFU32 *CardnumInstrs,TNFU32 NodeCnt[],char Descriptor[]);
	MIL1394_EMU_XT_API P_TNF_Node Mil1394_XT_OPEN(char Descriptor[],TNFU32 *CardIndex,TNFU32 node);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Close(P_TNF_Node pTNF);
	MIL1394_EMU_XT_API TNFI32 Mil1394_RESET(P_TNF_Node pTNF);
	MIL1394_EMU_XT_API TNFI32 Mil1394_RCV_STOF_ENABLE(P_TNF_Node pTNF,TNFU32 enable);/*使能接收STOF時，作为RN模式 */

	/*接收发送配置*/
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_LoadCfg(P_TNF_Node pTNF,TNFU32 CFG_Tx_NUM,TNFU32 CFG_Rx_NUM,TNFU32 RCV_Channel,TNFU32 pCFG_TX[],TNFU32 pCFG_RX[]);
	/*  接收接口- 接收STOF 、异步流消息、记录总线复位时间》*/
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_STOF_limitPer(P_TNF_Node pTNF,TNFU32 value);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_ASYNC_CFG(    P_TNF_Node pTNF,TNFU32 MessageID[], TNFU32 MessageLen[], TNFU32 RCV_Channel, TNFU32 RX_Count);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_ASYNC_CHANNEL(P_TNF_Node pTNF,TNFU32 RCV_Channel);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_BM_ENABLE(    P_TNF_Node pTNF,TNFU32 enable );

	MIL1394_EMU_XT_API TNFI32 MSG_RECV_Ctrl(       P_TNF_Node pTNF,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_list(       P_TNF_Node pTNF,TNFU32 *cnt,_RcvMsgList** msglist,_RcvMsgList** msglastlist);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_Packet_DATA(P_TNF_Node pTNF,TNFU32 buff[],TNFU32 *SizeOfBuff);/*字数*/
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_Packet_STOF(P_TNF_Node pTNF,             _MsgSTOF* Msg_STOF,_MsgState *stateMsg);
	MIL1394_EMU_XT_API TNFI32 MSG_RECV_Packet_Asyn(P_TNF_Node pTNF,TNFU32 MsgID,_MsgAsyn* Msg_Asyn,_MsgState *stateMsg);

	MIL1394_EMU_XT_API TNFI32 MSG_RECV_LRTC_Enable(P_TNF_Node pTNF,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 BusReset_RECV_Enable(P_TNF_Node pTNF, TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 SelfID_RECV_DATA(P_TNF_Node pTNF,TNFU32 selfid[],TNFU32 sizeof_selfid);/*selfid的字个数需要输入，必须大于要输出的自标识包个数，或者直接输入65*/

	/* 接收部分 配置接口*/
	MIL1394_EMU_XT_API TNFI32 BM_ACK_Cfg(P_TNF_Node pTNF,TNFU32 ackId);
	MIL1394_EMU_XT_API TNFI32 BM_ACK_Enable(P_TNF_Node pTNF,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 BM_STOF_Enable(P_TNF_Node pTNF,TNFU32 enable);/*在接收STOF时，使能后不在缓存区存放STOF消息 */
	MIL1394_EMU_XT_API TNFI32 BM_NetMsgID_Cfg(P_TNF_Node pTNF,_Filter_netMsgID_Struct* netMsgId);
	MIL1394_EMU_XT_API TNFI32 BM_NetMsgID_Enable(P_TNF_Node pTNF,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 BM_TopicID_Filt_Set(P_TNF_Node pTNF,TNFU32 TopicIDNum,TNFU32 TopicID[]);
	MIL1394_EMU_XT_API TNFI32 BM_TopicID_Filt_Enable(P_TNF_Node pTNF,TNFU32 enable);

	MIL1394_EMU_XT_API TNFI32 MSG_STOF_Period(P_TNF_Node pTNF,TNFU32 STOFPeriod);/*1.发送STOF周期时间，2.接收STOF 校验时间*/

	/* STOF 发送接口 -》*/
	MIL1394_EMU_XT_API TNFI32 MSG_STOF_SEND_DATA_Set(P_TNF_Node pTNF,_MsgSTOF *pstof);
	MIL1394_EMU_XT_API TNFI32 MSG_STOF_SEND_CFG(     P_TNF_Node pTNF,_TNF_STOFCFG_Struct *config);
	MIL1394_EMU_XT_API TNFI32 MSG_STOF_SEND_Ctrl(    P_TNF_Node pTNF,TNFU32 enable,TNFU32 STOFPeriod,TNFU32 style, TNFU32 count);
	MIL1394_EMU_XT_API TNFI32 MSG_STOF_SEND_ERR_Ctrl(P_TNF_Node pTNF,TNFU32 enable);

	/* 异步流消息 发送接口 -》*/
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_SEND_DATA_Set(P_TNF_Node pTNF,TNFU32 SN,TNFU32 TimeOffset,TNFU32 pdataBuf[]);
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_SEND_CFG(     P_TNF_Node pTNF,TNFU32 SN,_TNF_ASYNCCFG_Struct *config);
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_SEND_Ctrl(    P_TNF_Node pTNF,TNFU32 SN,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_SEND_ALLCtrl( P_TNF_Node pTNF,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_SEND_ERR_Ctrl(P_TNF_Node pTNF,TNFU32 enable);
	MIL1394_EMU_XT_API TNFI32 MSG_ASYNC_SEND_mode(    P_TNF_Node pTNF, TNFU32 ctrl_cmd, TNFU32 SelfPeriod);

	/* 事件消息发送接口*/
	MIL1394_EMU_XT_API TNFI32 MSG_EVENT_SEND(P_TNF_Node pTNF,TNFU32 SendOffset,TNFU32 pdataBuf[]);

	/*其他接口*/
	MIL1394_EMU_XT_API TNFI32 Mil1394_MSG_Cnt_Get(    P_TNF_Node pTNF, TNFU32 type, TNFU32 *pdata );

	MIL1394_EMU_XT_API TNFI32 Mil1394_SEND_Speed_Set( P_TNF_Node pTNF, TNFU32 SpeedSel);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Port_Speed_Set( P_TNF_Node pTNF, TNFU32 SpeedSel);
	MIL1394_EMU_XT_API TNFI32 Mil1394_SEND_RTC_ENABLE(P_TNF_Node pTNF, TNFU32 enable);

	MIL1394_EMU_XT_API TNFI32 Mil1394_BusReset(       P_TNF_Node pTNF, TNFU32 type);
	MIL1394_EMU_XT_API TNFI32 Mil1394_BusReset_Cycle( P_TNF_Node pTNF, TNFU32 enable,TNFU32 Period,TNFU32 type);

	MIL1394_EMU_XT_API TNFI32 Mil1394_RootNode_Set(  P_TNF_Node pTNF, TNFU32 phyId);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Phy_RemoteCall(P_TNF_Node pTNF,_PHY_Remote_Call_* command,TNFU32* remdata);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Phy_RemoteCom( P_TNF_Node pTNF,_PHY_Remote_Com_* Data);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Port_Dis(      P_TNF_Node pTNF,TNFU32 port, TNFU32 disenable);
	MIL1394_EMU_XT_API TNFI32 Mil1394_NodeNum_Get(P_TNF_Node pTNF,TNFU32 * nodeCnt,TNFU32 * PHYID); 

	/*同型号板卡同步接口测试 */
	MIL1394_EMU_XT_API TNFI32 Mil1394_Synch_NodeMode( P_TNF_Node pTNF, TNFU32 type);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Synch_STOF_Ctrl(P_TNF_Node pTNF,TNFU32 enable ,TNFU32 agoffset);
	MIL1394_EMU_XT_API TNFI32 Mil1394_Synch_LRTC_Ctrl(P_TNF_Node pTNF,TNFU32 enable);

#ifdef CPU_BigToLittle_
      #define CPU_LE_32(val)   (((val&0xff)  << 24) |((val & 0xff00) << 8) | ((val & 0xff0000) >> 8) | ((val & 0xff000000)>>24))
#else
      #define CPU_LE_32(val)	val
#endif

#if defined __cplusplus
}
#endif

#endif
