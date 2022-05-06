#ifndef PCIE_H
#define PCIE_H

#include <stdint.h>

#define PCIE_ECAM_BASE 0x30000000

#define MAXBUS 256
#define MAXDEV 32
#define SET_COMMAND_REG_MMIO 0xfffe


struct pcie_ecam {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command_reg;
    uint16_t status_reg;
    uint8_t revision_id;
    uint8_t prog_if;
    union {
        uint16_t class_code;
        struct {
            uint8_t class_subcode;
            uint8_t class_basecode;
        };
    };
    uint8_t cacheline_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    union {
        struct {
            uint32_t bar[6];
            uint32_t cardbus_cis_pointer;
            uint16_t sub_vendor_id;
            uint16_t sub_device_id;
            uint32_t expansion_rom_addr;
            uint8_t  capes_pointer;
            uint8_t  reserved0[3];
            uint32_t reserved1;
            uint8_t  interrupt_line;
            uint8_t  interrupt_pin;
            uint8_t  min_gnt;
            uint8_t  max_lat;
        } type0;
        struct {
            uint32_t bar[2];
            uint8_t  primary_bus_no;
            uint8_t  secondary_bus_no;
            uint8_t  subordinate_bus_no;
            uint8_t  secondary_latency_timer;
            uint8_t  io_base;
            uint8_t  io_limit;
            uint16_t secondary_status;
            uint16_t memory_base;
            uint16_t memory_limit;
            uint16_t prefetch_memory_base;
            uint16_t prefetch_memory_limit;
            uint32_t prefetch_base_upper;
            uint32_t prefetch_limit_upper;
            uint16_t io_base_upper;
            uint16_t io_limit_upper;
            uint8_t  capes_pointer;
            uint8_t  reserved0[3];
            uint32_t expansion_rom_addr;
            uint8_t  interrupt_line;
            uint8_t  interrupt_pin;
            uint16_t bridge_control;
        } type1;
        struct {
            uint32_t reserved0[9];
            uint8_t  capes_pointer;
            uint8_t  reserved1[3];
            uint32_t reserved2;
            uint8_t  interrupt_line;
            uint8_t  interrupt_pin;
            uint8_t  reserved3[2];
        } common;
    };
};

struct pcie_ecam *get_ecam_header(uint8_t bus, uint8_t device, uint8_t function, uint16_t reg);
struct pcie_ecam *get_pcie_virtio_net(void);
void configure_pcie_bridge(struct pcie_ecam *ecam_head, uint16_t bus);
void configure_pcie_capes(struct pcie_ecam *ecam_head, uint8_t bus, uint8_t device);
void configure_pcie_bar(struct pcie_ecam *ecam_head);
void pcie_init(void);

#endif
