#ifndef _Mil1394_Emu_Addr
#define _Mil1394_Emu_Addr

/*基本信息定义*/
#define        TNF_PID3        0x0004	/* TO DO: set the Vendor ID for your device*/
#define        TNF_VID3        0x1057	/* TO DO: set the Device ID for your device*/
#define        TNF_PIDVID3    0x00041057	/* */

#define        TNF_PID4        0xe005	/* TO DO: set the Vendor ID for your device*/
#define        TNF_VID4        0x19aa	/* TO DO: set the Device ID for your device*/
#define        TNF_PIDVID4   0xe00519aa	/* TO DO: set the Vendor ID for your device*/

#define        TNF_PID4new        0x0008	/* TO DO: set the Vendor ID for your device*/
#define        TNF_VID4new        0x1057	/* TO DO: set the Device ID for your device*/
#define        TNF_PIDVID4new   0x00081057	/* TO DO: set the Vendor ID for your device*/

#define        PCARD                 4U                     /*最多支持卡数量*/
#define        PNODE                 4U                     /*节点最多打数量*/
#define        NODE_CNT            (PCARD*PNODE)           /*总共设计节点数量*/

#define        MGS_BUFF_SIZE       (2048U)                   /*每条消息2K 字节 512个字*/
#define        MGS_BUFF_WORDCNT    (512U)                    /*每条消息2K 字节 512个字*/

#define        MAX_RCV_CNT         (100U)                    /*接收最大条数*/

#define        BLOCK_NUM           (MAX_RCV_CNT*2)

#define        DMA_SND_SIZE        (MGS_BUFF_SIZE * 66)         /*发送64条 或 30条  缓存，第31条/65条 为事件消息*/
#define        DMA_RCV_SIZE        (MGS_BUFF_SIZE * MAX_RCV_CNT)/*接收最大   */
#define        NODE_DMA_SIZE       ((DMA_RCV_SIZE*2) +  DMA_SND_SIZE)
#define        DMA_SIZE			   (NODE_DMA_SIZE * PNODE)


#endif
