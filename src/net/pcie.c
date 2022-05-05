#include <stdint.h>
#include "pcie.h"

#define PCIE_ECAM_BASE 0x30000000


struct pcie_ecam *get_ecam_header(uint8_t bus, uint8_t device, 
                                  uint8_t function, uint16_t reg) {
    
    return (struct pcie_ecam_header)(PCIE_ECAM_BASE | bus << 20 | device << 15 | 
                                     function << 12 | reg << 2);
}

struct pcie_ecam *get_pcie_virtio_net(void) {

    int bus, device;

    for (bus = 0; bus < MAXBUS; bus++) {
        for (device = 0; device < MAXDEV; device++) {
            
            struct pcie_ecam_header *ecam_head = get_ecam_header(bus, device, 0, 0);

            if (ecam_head->vendor_id == 0xffff) {
                continue;
            }
            if (ecam_head->vendor_id == 0x1af4 && ecam_head->device_id == 0x1000) {
                return ecam_head;
}



void configure_pcie_bridge(struct pcie_ecam *ecam_head, uint16_t bus) {

    //struct pcie_ecam *ecam_head = get_pcie_virtio_net();
    
    // Set bit 1 of command reg so we can configure the devices MMIO address
    ecam_head->command_reg = (ecam_command_reg & SET_COMMAND_REG_MMIO);

    uint8_t subordinate = 1;
    uint32_t memory_base = 0x40000000 | ((uint32_t)subordinate << 20);
    uint32_t memory_limit = memory base + ((1 << 20) - 1);

    ecam_head->type1.memory_base = memory_base >> 16;
    ecam_head->type1.memory_limit = memory_limit >> 16;
    ecam_head->type1.prefetch_memory_base = memory_base >> 16;
    ecam_head->type1.prefetch_memory_limit = memory_limit >> 16;
    ecam_head->type1.primary_bus_no = bus;
    ecam_head->type1.secondary_bus_no = subordinate;
    ecam_head->type1.subordinate_bus_no = subordinate;
    
    subordinate += 1;
}








