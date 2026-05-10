#include "pe_loader.h"
#include "pe_structs.h"
#include <heap.h>
#include <string.h>
#include <kprintf.h>

int pe_load_image(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, pe_loaded_image_t *out)
{
    if (!file_data || !info || !out || !info->valid) return -1;
    if (info->size_of_image == 0 || info->size_of_image > (16U * 1024U * 1024U)) return -1;

    memset(out, 0, sizeof(*out));

    uint8_t *mem = (uint8_t *)kmalloc(info->size_of_image);
    if (!mem) return -1;

    memset(mem, 0, info->size_of_image);

    uint32_t hdr_copy = info->size_of_headers;
    if (hdr_copy > file_size) hdr_copy = file_size;
    memcpy(mem, file_data, hdr_copy);

    for (uint16_t i = 0; i < info->section_count; i++) {
        const pe_section_info_t *s = &info->sections[i];
        if (s->raw_size == 0) continue;
        if (s->raw_offset + s->raw_size > file_size) {
            kfree(mem);
            return -1;
        }
        if (s->rva + s->raw_size > info->size_of_image) {
            kfree(mem);
            return -1;
        }
        memcpy(mem + s->rva, file_data + s->raw_offset, s->raw_size);
    }

    out->image_base = mem;
    out->mapped_size = info->size_of_image;
    out->actual_load_base = (uint32_t)(uintptr_t)mem;
    out->entry_point = (uint64_t)(uintptr_t)(mem + (uint32_t)(info->entry_point - info->image_base));
    out->loaded = 1;
    out->relocs_applied = 0;
    return 0;
}

int pe_apply_relocations(const uint8_t *file_data, uint32_t file_size, const pe_image_info_t *info, pe_loaded_image_t *img)
{
    if (!file_data || !info || !img || !img->image_base) return -1;
    if (!info->has_relocs) return 0;

    uint32_t reloc_rva = info->data_dirs[PE_DIR_ENTRY_BASERELOC].rva;
    uint32_t reloc_size = info->data_dirs[PE_DIR_ENTRY_BASERELOC].size;
    if (reloc_rva == 0 || reloc_size == 0) return 0;

    uint32_t reloc_off = 0;
    if (pe_rva_to_offset(info, reloc_rva, &reloc_off) != 0) return -1;
    if (reloc_off > file_size || reloc_size > file_size - reloc_off) return -1;

    uint64_t delta = (uint64_t)(uintptr_t)img->image_base - info->image_base;
    if (delta == 0) {
        img->relocs_applied = 1;
        return 0;
    }

    uint32_t pos = reloc_off;
    uint32_t end = reloc_off + reloc_size;

    while (pos + sizeof(pe_base_reloc_block_t) <= end) {
        const pe_base_reloc_block_t *block = (const pe_base_reloc_block_t *)(file_data + pos);
        if (block->block_size < sizeof(pe_base_reloc_block_t)) break;

        uint32_t entries_count = (block->block_size - sizeof(pe_base_reloc_block_t)) / 2;
        uint32_t entries_end = pos + block->block_size;
        pos += sizeof(pe_base_reloc_block_t);

        for (uint32_t e = 0; e < entries_count && pos + 2 <= entries_end; e++) {
            uint16_t entry = *(const uint16_t *)(file_data + pos);
            pos += 2;

            uint16_t type = (entry >> 12) & 0xF;
            uint16_t offset = entry & 0xFFF;

            if (type == PE_REL_BASED_ABSOLUTE) continue;

            uint64_t *patch_addr = (uint64_t *)(img->image_base + block->page_rva + offset);

            switch (type) {
                case PE_REL_BASED_DIR64:
                    *patch_addr += delta;
                    break;
                case PE_REL_BASED_HIGHLOW:
                    *(uint32_t *)patch_addr += (uint32_t)delta;
                    break;
                case PE_REL_BASED_HIGH:
                    *(uint16_t *)patch_addr += (uint16_t)(delta >> 16);
                    break;
                case PE_REL_BASED_LOW:
                    *(uint16_t *)patch_addr += (uint16_t)delta;
                    break;
                default:
                    break;
            }
        }
    }

    img->relocs_applied = 1;
    return 0;
}

int pe_resolve_imports(pe_loaded_image_t *img, const pe_image_info_t *info, const uint8_t *file_data, uint32_t file_size)
{
    (void)img;
    (void)info;
    (void)file_data;
    (void)file_size;
    return 0;
}

void pe_unload_image(pe_loaded_image_t *img)
{
    if (!img) return;
    if (img->image_base) kfree(img->image_base);
    img->image_base = NULL;
    img->entry_point = 0;
    img->mapped_size = 0;
    img->loaded = 0;
    img->relocs_applied = 0;
}
