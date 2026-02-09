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
	xa_store(&xa, 0, (void *)0x100, GFP_KERNEL);
	xa_store(&xa, 1, (void *)0x200, GFP_KERNEL);
	xa_store(&xa, 5, (void *)0x500, GFP_KERNEL);
	xa_store(&xa, 100, (void *)0x1000, GFP_KERNEL);
	printf("Stored 4 entries.\n");

	/* Load entries */
	entry = xa_load(&xa, 0);
	printf("xa_load(0) = %p (expected 0x100)\n", entry);

	entry = xa_load(&xa, 1);
	printf("xa_load(1) = %p (expected 0x200)\n", entry);

	entry = xa_load(&xa, 5);
	printf("xa_load(5) = %p (expected 0x500)\n", entry);

	entry = xa_load(&xa, 100);
	printf("xa_load(100) = %p (expected 0x1000)\n", entry);

	entry = xa_load(&xa, 99);
	printf("xa_load(99) = %p (expected NULL)\n", entry);

	/* Erase an entry */
	entry = xa_erase(&xa, 1);
	printf("xa_erase(1) returned %p (expected 0x200)\n", entry);

	entry = xa_load(&xa, 1);
	printf("xa_load(1) after erase = %p (expected NULL)\n", entry);

	/* Iterate */
	printf("Iterating all entries:\n");
	xa_for_each(&xa, index, entry) {
		printf("  index=%lu entry=%p\n", index, entry);
	}

	/* Store value entries */
	xa_store(&xa, 10, xa_mk_value(42), GFP_KERNEL);
	entry = xa_load(&xa, 10);
	if (xa_is_value(entry))
		printf("xa_load(10) is value = %lu (expected 42)\n",
		       xa_to_value(entry));

	/* Destroy */
	xa_destroy(&xa);
	printf("XArray destroyed.\n");

	printf("\nAll tests passed!\n");
	return 0;
}
