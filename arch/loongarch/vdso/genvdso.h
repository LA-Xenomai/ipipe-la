/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 Loongson Technology Corporation Limited
 * Author: Huacai Chen <chenhuacai@loongson.cn>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

static inline bool FUNC(patch_vdso)(const char *path, void *vdso)
{
	const ELF(Ehdr) *ehdr = vdso;
	void *shdrs;
	ELF(Shdr) *shdr;
	uint16_t sh_count, sh_entsize, i;

	shdrs = vdso + FUNC(swap_uint)(ehdr->e_shoff);
	sh_count = swap_uint16(ehdr->e_shnum);
	sh_entsize = swap_uint16(ehdr->e_shentsize);

	shdr = shdrs + (sh_entsize * swap_uint16(ehdr->e_shstrndx));

	for (i = 0; i < sh_count; i++) {
		shdr = shdrs + (i * sh_entsize);

		/*
		 * Ensure there are no relocation sections - ld.so does not
		 * relocate the VDSO so if there are relocations things will
		 * break.
		 */
		switch (swap_uint32(shdr->sh_type)) {
		case SHT_REL:
		case SHT_RELA:
			fprintf(stderr,
				"%s: '%s' contains relocation sections\n",
				program_name, path);
			return false;
		}
	}

	return true;
}

static inline bool FUNC(get_symbols)(const char *path, void *vdso)
{
	const ELF(Ehdr) *ehdr = vdso;
	void *shdrs, *symtab;
	ELF(Shdr) *shdr;
	const ELF(Sym) *sym;
	char *strtab, *name;
	uint16_t sh_count, sh_entsize, st_count, st_entsize, i, j;
	uint64_t offset;
	uint32_t flags;

	shdrs = vdso + FUNC(swap_uint)(ehdr->e_shoff);
	sh_count = swap_uint16(ehdr->e_shnum);
	sh_entsize = swap_uint16(ehdr->e_shentsize);

	for (i = 0; i < sh_count; i++) {
		shdr = shdrs + (i * sh_entsize);

		if (swap_uint32(shdr->sh_type) == SHT_SYMTAB)
			break;
	}

	if (i == sh_count) {
		fprintf(stderr, "%s: '%s' has no symbol table\n", program_name,
			path);
		return false;
	}

	/* Get flags */
	flags = swap_uint32(ehdr->e_flags);
	switch (flags) {
	case EF_LARCH_ABI_LP32:
		elf_abi = ABI_LP32;
		break;
	case EF_LARCH_ABI_LPX32:
		elf_abi = ABI_LPX32;
		break;
	case EF_LARCH_ABI_LP64:
	default:
		elf_abi = ABI_LP64;
		break;
	}

	/* Get symbol table. */
	symtab = vdso + FUNC(swap_uint)(shdr->sh_offset);
	st_entsize = FUNC(swap_uint)(shdr->sh_entsize);
	st_count = FUNC(swap_uint)(shdr->sh_size) / st_entsize;

	/* Get string table. */
	shdr = shdrs + (swap_uint32(shdr->sh_link) * sh_entsize);
	strtab = vdso + FUNC(swap_uint)(shdr->sh_offset);

	/* Write offsets for symbols needed by the kernel. */
	for (i = 0; vdso_symbols[i].name; i++) {
		if (!(vdso_symbols[i].abis & elf_abi))
			continue;

		for (j = 0; j < st_count; j++) {
			sym = symtab + (j * st_entsize);
			name = strtab + swap_uint32(sym->st_name);

			if (!strcmp(name, vdso_symbols[i].name)) {
				offset = FUNC(swap_uint)(sym->st_value);

				fprintf(out_file,
					"\t.%s = 0x%" PRIx64 ",\n",
					vdso_symbols[i].offset_name, offset);
				break;
			}
		}

		if (j == st_count) {
			fprintf(stderr,
				"%s: '%s' is missing required symbol '%s'\n",
				program_name, path, vdso_symbols[i].name);
			return false;
		}
	}

	return true;
}
