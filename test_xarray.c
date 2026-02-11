#include <linux/xarray.h>
#include <stdio.h>

int main(void)
{
	struct xarray xa;
	void *entry;
	unsigned long index;
	int ret;

	/* Initialize */
	xa_init(&xa);
	printf("XArray initialized.\n");

	/* Store entries */
	xa_store(&xa, 0x1, xa_mk_value(1), GFP_KERNEL);
	xa_store(&xa, 0xffffffffffffffff, xa_mk_value(2), GFP_KERNEL);
	printf("Stored 2 entries.\n");
	xa_dump(&xa);
	/* Load entries */
	entry = xa_load(&xa, 0x1);
	printf("xa_load(0x1) = %p (expected 0x1)\n", (void *)xa_to_value(entry));

	entry = xa_load(&xa, 0xffffffffffffffff);
	printf("xa_load(0xffffffffffffffff) = %p (expected 0x2)\n", (void *)xa_to_value(entry));

	/* Erase an entry */
	entry = xa_erase(&xa, 0xffffffffffffffff);
	printf("xa_erase(0xffffffffffffffff) returned %p (expected 0x2)\n", (void *)xa_to_value(entry));

	entry = xa_load(&xa, 0xffffffffffffffff);
	printf("xa_load(0xffffffffffffffff) after erase = %p (expected NULL)\n", (void *)xa_to_value(entry));

	/* Iterate */
	printf("Iterating all entries:\n");
	xa_for_each(&xa, index, entry) {
		printf("  index=%lu entry=%p\n", index, entry);
	}

	/* Store value entries */

	/* Destroy */
	xa_destroy(&xa);
	printf("XArray destroyed.\n");

	printf("\nAll tests passed!\n");
	return 0;
}
