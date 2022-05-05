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












