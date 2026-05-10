#ifndef _ATA_H
#define _ATA_H

#include "../include/types.h"
#include "../include/kprintf.h"

/* ATA Primary Channel Base Ports */
#define ATA_PRIMARY_IO      0x1F0
#define ATA_PRIMARY_CTRL    0x3F6
#define ATA_PRIMARY_DMA_BASE 0xC000

/* DMA Controller Ports (ISA DMA Channel 0) */
#define DMA_PORT_BASE       0x00
#define DMA_CHANNEL_0       0x00
#define DMA_CHANNEL_SELECT  0x0B
#define DMA_MODE_REGISTER   0x0B
#define DMA_CLEAR_FF        0x0C
#define DMA_MASTER_CLEAR    0x0D
#define DMA_UNMASK_ALL      0x0F

/* ATA Register Offsets */
#define ATA_REG_DATA        0x0
#define ATA_REG_ERROR       0x1
#define ATA_REG_FEATURES    0x1
#define ATA_REG_SECTOR_CNT  0x2
#define ATA_REG_SECTOR_NUM  0x3
#define ATA_REG_CYL_LOW     0x4
#define ATA_REG_CYL_HIGH    0x5
#define ATA_REG_HEAD        0x6
#define ATA_REG_COMMAND     0x7
#define ATA_REG_STATUS      0x7
#define ATA_REG_ALT_STATUS  0x206
#define ATA_REG_DEV_CTRL    0x206

/* ATA Status Bits */
#define ATA_STATUS_ERR      0x01  /* Error */
#define ATA_STATUS_DRQ      0x08  /* Data Request Ready */
#define ATA_STATUS_SRV      0x10  /* Service */
#define ATA_STATUS_DF       0x20  /* Device Fault */
#define ATA_STATUS_RDY      0x40  /* Device Ready */
#define ATA_STATUS_BSY      0x80  /* Busy */

/* ATA Commands */
#define ATA_CMD_READ_SECTORS        0x20
#define ATA_CMD_WRITE_SECTORS       0x30
#define ATA_CMD_READ_DMA           0xC8
#define ATA_CMD_WRITE_DMA          0xCA
#define ATA_CMD_IDENTIFY            0xEC
#define ATA_CMD_SET_FEATURES        0xEF
#define ATA_CMD_READ_NATIVE_MAX     0xF8  /* Query max LBA */

/* ATA Device Control */
#define ATA_CTRL_NIEN   0x02  /* Disable IRQ */
#define ATA_CTRL_SRST   0x04  /* Software Reset */

/* Sector Size */
#define ATA_SECTOR_SIZE 512

/* Max sectors per operation */
#define ATA_MAX_SECTORS_PER_OP 256

/* Timeouts (in microseconds) */
#define ATA_TIMEOUT_READY       100000  /* 100ms */
#define ATA_TIMEOUT_DRQ         100000
#define ATA_TIMEOUT_IRQ         5000000 /* 5s */
#define ATA_RETRY_COUNT         3

/* Error Codes */
typedef enum {
    ATA_ERR_OK = 0,
    ATA_ERR_NOT_PRESENT = -1,
    ATA_ERR_BUSY_TIMEOUT = -2,
    ATA_ERR_DRQ_TIMEOUT = -3,
    ATA_ERR_DEVICE_ERROR = -4,
    ATA_ERR_INVALID_PARAMS = -5,
    ATA_ERR_DMA_FAILED = -6,
} ata_error_t;

/* PRD (Physical Region Descriptor) for DMA */
typedef struct __attribute__((packed)) {
    uint32_t physical_addr;  /* Physical buffer address */
    uint16_t byte_count;     /* Bytes to transfer (0 = 64KB) */
    uint16_t flags;          /* Bit 7 = End of Table (EOT) */
} ata_prd_entry_t;

/* ATA Device Structure */
typedef struct {
    uint8_t primary;        /* 1 = Primary channel, 0 = Secondary */
    uint8_t slave;          /* 1 = Slave device, 0 = Master */
    uint16_t io_base;       /* I/O base address */
    uint16_t ctrl_base;     /* Control base address */
    uint16_t dma_base;      /* DMA base address */
    uint32_t present;       /* Device present flag */
    uint32_t dma_capable;   /* DMA support available */
    uint32_t max_lba;       /* Maximum LBA */
    uint32_t total_sectors; /* Total sector count */
    uint32_t last_error;    /* Last error code */
    uint32_t operations;    /* Operation counter for diagnostics */
    uint32_t errors;        /* Error counter */
} ata_device_t;

/* Function Prototypes */
void ata_init(void);
void ata_write_command(uint8_t channel, uint8_t slave, uint8_t reg, uint8_t data);
uint8_t ata_read_command(uint8_t channel, uint8_t slave, uint8_t reg);
void ata_wait_ready(uint8_t channel, uint8_t slave);
uint8_t ata_identify_device(uint8_t channel, uint8_t slave);

/* PIO Operations (backward compatible) */
int ata_read_sector(uint32_t lba, uint8_t *buffer);
int ata_write_sector(uint32_t lba, const uint8_t *buffer);

/* Multi-sector Operations */
int ata_read_sectors(uint32_t lba, uint16_t count, uint8_t *buffer);
int ata_write_sectors(uint32_t lba, uint16_t count, const uint8_t *buffer);

/* DMA Operations */
int ata_dma_read_sectors(uint32_t lba, uint16_t count, uint8_t *buffer);
int ata_dma_write_sectors(uint32_t lba, uint16_t count, const uint8_t *buffer);

/* Diagnostics */
void ata_get_device_info(void);
int ata_test_read_write(void);

#endif /* _ATA_H */
