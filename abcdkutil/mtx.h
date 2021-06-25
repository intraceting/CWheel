/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#ifndef ABCDKUTIL_MTX_H
#define ABCDKUTIL_MTX_H

#include "general.h"
#include "scsi.h"
#include "tree.h"

/**
 * 元件类型。
*/
enum _abcdk_mtx_element_type
{
    /** Medium transport element.*/
    ABCDK_MXT_ELEMENT_CHANGER = 1,
#define ABCDK_MXT_ELEMENT_CHANGER ABCDK_MXT_ELEMENT_CHANGER

    /** Storage element.*/
    ABCDK_MXT_ELEMENT_STORAGE = 2,
#define ABCDK_MXT_ELEMENT_STORAGE ABCDK_MXT_ELEMENT_STORAGE

    /** Import / Export Element. */
    ABCDK_MXT_ELEMENT_IE_PORT = 3,
#define ABCDK_MXT_ELEMENT_IE_PORT ABCDK_MXT_ELEMENT_IE_PORT

    /** Data transfer element (drives). */
    ABCDK_MXT_ELEMENT_DXFER = 4
#define ABCDK_MXT_ELEMENT_DXFER ABCDK_MXT_ELEMENT_DXFER
#define ABCDK_MXT_ELEMENT_DRIVER ABCDK_MXT_ELEMENT_DXFER
};

/**
 * 元件的字段。
*/
enum _abcdk_mtx_element_field
{
    /** 地址字段索引。*/
    ABCDK_MTX_ELEMENT_ADDR = 0,
#define ABCDK_MTX_ELEMENT_ADDR ABCDK_MTX_ELEMENT_ADDR

    /** 类型字段索引。*/
    ABCDK_MTX_ELEMENT_TYPE = 1,
#define ABCDK_MTX_ELEMENT_TYPE ABCDK_MTX_ELEMENT_TYPE

    /** 介质有无字段索引。*/
    ABCDK_MTX_ELEMENT_ISFULL = 2,
#define ABCDK_MTX_ELEMENT_ISFULL ABCDK_MTX_ELEMENT_ISFULL

    /** 条码字段索引。*/
    ABCDK_MTX_ELEMENT_BARCODE = 3,
#define ABCDK_MTX_ELEMENT_BARCODE ABCDK_MTX_ELEMENT_BARCODE

    /** DVCID字段索引。*/
    ABCDK_MTX_ELEMENT_DVCID = 4
#define ABCDK_MTX_ELEMENT_DVCID ABCDK_MTX_ELEMENT_DVCID
};

/**
 * 初始化设备元件状态。
 * 
 * 可能会花费较长的间。
 * 
 * @param address 开始地址。
 * @param count 数量，为0时忽略address，并初始化所有元件状态。
 * 
 * @return 0 成功，-1 失败。
 *  
*/
int abcdk_mtx_inventory(int fd, uint16_t address, uint16_t count,
                       uint32_t timeout, abcdk_scsi_io_stat *stat);

/**
 * 移动介质。
 * 
 * cdb = 0xA5
 * 
 * @param t 机械臂地址
 * @param src 源槽位地址
 * @param dst 目标槽位地址
 * 
 * @return 0 成功，-1 失败。
 * 
 * @warning  SENSE key = 0x06 设备仓门被打开过，需要重新盘点介质。
 */
int abcdk_mtx_move_medium(int fd, uint16_t t, uint16_t src, uint16_t dst,
                         uint32_t timeout, abcdk_scsi_io_stat *stat);

/**
 * 限制介质是否允许能被移动到出入仓位。
 * 
 * 不影响介质的导入。
 * 
 * cdb = 0x1E
 * 
 * @param disable 0 允许，!0 不允许。
 *
 * @return 0 成功，-1 失败。 
*/
int abcdk_mtx_prevent_medium_removal(int fd, int disable,
                                    uint32_t timeout, abcdk_scsi_io_stat *stat);

/**
 * 查询设备信息。
 * 
 * cdb = 0x1A
 * 
 * @return 0 成功，-1 失败。 
*/
int abcdk_mtx_mode_sense(int fd, uint8_t pctrl, uint8_t pcode, uint8_t spcode,
                        uint8_t *transfer, uint8_t transferlen,
                        uint32_t timeout, abcdk_scsi_io_stat *stat);

/**
 * 查询设备元件状态。
 * 
 * cdb = 0xB8
 * 
 * @param transferlen 返回数据的最大长度。2MB是支持的最大长度，原因未知。
 * 
 * @return 0 成功，-1 失败。 
*/
int abcdk_mtx_read_element_status(int fd, uint8_t type, uint16_t address, uint16_t count,
                                 uint8_t *transfer, uint32_t transferlen,
                                 uint32_t timeout, abcdk_scsi_io_stat *stat);

/**
 * 分析设备元件状态，构造结构化数据。
 * 
 * @see abcdk_mtx_read_element_status
*/
void abcdk_mtx_parse_element_status(abcdk_tree_t *father,const uint8_t *element,uint16_t count);

/**
 * 查询设备所有元件状态。
 * 
 * @return 0 成功，-1 失败。 
 * 
 * @see abcdk_mtx_read_element_status
 * @see abcdk_mtx_parse_element_status
*/
int abcdk_mtx_inquiry_element_status(abcdk_tree_t *father,int fd,uint32_t timeout, abcdk_scsi_io_stat *stat);


#endif //ABCDKUTIL_MTX_H
