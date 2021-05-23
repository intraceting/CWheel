/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "scsi.h"

const char* good_scsi_type2string(uint8_t type)
{
    switch(type)
    {
        case TYPE_DISK:
            return "Direct-Access";
        case TYPE_TAPE:
            return "Sequential-Access";
        case 0x02:
            return "Printer";
        case TYPE_PROCESSOR:
            return "Processor";
        case TYPE_WORM:
            return "Write-once";
        case TYPE_ROM:
            return "CD-ROM";
        case TYPE_SCANNER:
            return "Scanner";
        case TYPE_MEDIUM_CHANGER:
            return "Medium Changer";
        case TYPE_ENCLOSURE:
            return "Enclosure";
        default:
            return "Reserved";
    }
}

int good_scsi_sgioctl(int fd,struct sg_io_hdr *hdr)
{
    assert(fd >= 0 && hdr != NULL);

    assert(hdr->interface_id == 'S');
    assert(hdr->dxfer_direction == SG_DXFER_NONE || hdr->dxfer_direction == SG_DXFER_TO_DEV || 
            hdr->dxfer_direction == SG_DXFER_FROM_DEV || hdr->dxfer_direction == SG_DXFER_TO_FROM_DEV);
    assert(hdr->cmdp != NULL && hdr->cmd_len > 0);
    assert((hdr->dxferp != NULL && hdr->dxfer_len > 0) ||(hdr->dxferp == NULL && hdr->dxfer_len <= 0));
    assert((hdr->sbp != NULL && hdr->mx_sb_len > 0) ||(hdr->sbp == NULL && hdr->mx_sb_len <= 0));

    return ioctl(fd,SG_IO,hdr);
}

int good_scsi_sgioctl2(int fd, int direction,
                       uint8_t *cdb, uint8_t cdblen,
                       uint8_t *transfer, uint32_t transferlen,
                       uint32_t timeout, good_scsi_io_stat *stat)
{
    struct sg_io_hdr hdr = {0};
    int chk;

    assert(fd >= 0 && cdb != NULL && cdblen > 0);
    assert((transfer != NULL && transferlen > 0) ||
           (transfer == NULL && transferlen <= 0));
    assert(stat != NULL);

    /*clear*/
    memset(stat,0,sizeof(*stat));

    hdr.interface_id = 'S';
    hdr.dxfer_direction = direction;
    hdr.cmdp = cdb;
    hdr.cmd_len = cdblen;
    hdr.dxferp = transfer;
    hdr.dxfer_len = transferlen;
    hdr.sbp = stat->sense;
    hdr.mx_sb_len = sizeof(stat->sense);
    hdr.timeout = timeout;

    chk = good_scsi_sgioctl(fd,&hdr);
    if(chk != 0)
        return -1;

    stat->status = hdr.status;
    stat->host_status = hdr.host_status;
    stat->driver_status = hdr.driver_status;
    stat->senselen = hdr.sb_len_wr;
    stat->resid = hdr.resid;

    return 0;
}

uint8_t good_scsi_sense_key(uint8_t *sense)
{
    assert(sense != NULL);

    return sense[2] & 0xf;
}

uint8_t good_scsi_sense_code(uint8_t *sense)
{
    assert(sense != NULL);

    return sense[12];
}

uint8_t good_scsi_sense_qualifier(uint8_t *sense)
{
    assert(sense != NULL);

    return sense[13];
}

int good_scsi_test(int fd, uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[6] = {0};
    
    cdb[0] = 0x00; /*00H is TEST UNIT READY*/

    return good_scsi_sgioctl2(fd,SG_DXFER_NONE,cdb,6,NULL,0,timeout,stat);
}

int good_mtx_request_sense(int fd,uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t cdb[6] = {0};
    
    cdb[0] = 0x03; /*03H is Request Sense*/

    return good_scsi_sgioctl2(fd,SG_DXFER_NONE,cdb,6,NULL,0,timeout,stat);
}

int good_scsi_inquiry(int fd,int vpd, uint8_t vid,
                      uint8_t *transfer, uint32_t transferlen,
                      uint32_t timeout,good_scsi_io_stat *stat)
{
    uint8_t cdb[6] = {0};
  
    cdb[0] = 0x12; /*12H is INQUIRY*/
    cdb[1] = (vpd ? 0x01 : 0x00); /* Enable Vital Product Data */
    cdb[2] = (vpd ? vid : 0x00); /* Return PAGE CODE */
    cdb[4] = transferlen;

    return good_scsi_sgioctl2(fd,SG_DXFER_FROM_DEV,cdb,6,transfer,transferlen,timeout,stat);
}

int good_scsi_inquiry_standard(int fd, uint8_t *type, char vendor[8], char product[16],
                               uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t tmp[255] = {0};
    int chk;

    chk = good_scsi_inquiry(fd,0,0x00,tmp,255,timeout,stat);
    if(chk != 0)
        return -1;

    if(stat->status != GOOD)
        return -1;

    /* TYPE，VENDOR，PRODUCT。*/
    if(type)
        *type = tmp[0] & 0x1f;
    if(vendor)
        memcpy(vendor, tmp + 8, 8);
    if(product)
        memcpy(product, tmp + 16, 16);

    /* 去掉两端的空格。 */
    if(vendor)
        good_strtrim(vendor,isspace,2);
    if(product)
        good_strtrim(product,isspace,2);

    return 0;
}

int good_scsi_inquiry_serial(int fd, uint8_t *type, char serial[255],
                             uint32_t timeout, good_scsi_io_stat *stat)
{
    uint8_t tmp[255] = {0};
    int chk;

    chk = good_scsi_inquiry(fd,1,0x80,tmp,255,timeout,stat);
    if(chk != 0)
        return -1;

    if(stat->status != GOOD)
        return -1;

    /* TYPE，SERIAL。*/
    if(type)
        *type = tmp[0] & 0x1f;
    if(serial)
        memcpy(serial, tmp + 4, tmp[3]);

    /* 去掉两端的空格。 */
    if(serial)
        good_strtrim(serial,isspace,2);

    return 0;
}
