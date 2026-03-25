#ifndef BIOS_BOOT_INFO_H
#define BIOS_BOOT_INFO_H

// 
// structures sent from BIOS bootloader with relevant information
//

#include "inttype.h"

typedef enum __attribute__((packed)){
    USABLE_RAM   = 1,
    RESERVED     = 2,
    ACPI_RECLAIM = 3,
    ACPI_NVS     = 4,
    BAD_MEMORY   = 5,
} MMapType;

typedef struct {
    u64 base_addr;
    u64 length;
    MMapType type;
    u32 acpi_ext;
} BootMMapEntry;

typedef struct {
    u64 entries[512];
} Pml4Table;
typedef struct {
    u64 entries[512];
} PdptTable;
typedef struct {
    u64 entries[512];
} PdTable;
typedef struct {
    u64 entries[512];
} PtTable;

typedef struct {
    BootMMapEntry* boot_memmap_addr;
    u64 boot_memmap_entries;
    Pml4Table* pml4_table;
    PdptTable* pdpt_table;
    PdTable* pd_table;
    PtTable* pt_table;
} BiosBootInfo;

#endif
