#include <stdint.h>
#include "pcie.h"

#define PCIE_ECAM_BASE 0x30000000


struct pcie_ecam *get_ecam_header(uint8_t bus, uint8_t device, 
                                  uint8_t function, uint16_t reg) {
    
    return (struct pcie_ecam_header)(PCIE_ECAM_BASE | bus << 20 | device << 15 | 
                                     function << 12 | reg << 2);
}














