/**
 * \kernelgroup{SceIdStorage}
 * \usage{psp2kern/idstorage.h,SceIdStorageForDriver_stub}
 */

#ifndef _PSP2KERN_IDSTORAGE_H_
#define _PSP2KERN_IDSTORAGE_H_

#include <psp2kern/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCE_ID_STORAGE_LEAF_SIZE         0x200
#define SCE_ID_STORAGE_KEY_MAX           0xFFEF
#define SCE_ID_STORAGE_ATOMIC_LEAVES_MAX 32

/** Module-specific errors explicitly emitted by FW 3.60 SceIdStorage. */
typedef enum SceIdStorageErrorCode {
	SCE_ID_STORAGE_ERROR_INVALID_ARGUMENT    = 0x80230000, //!< Invalid argument, range, or required manufacturing mode.
	SCE_ID_STORAGE_ERROR_INVALID_KEY         = 0x80230001, //!< The key is greater than ::SCE_ID_STORAGE_KEY_MAX.
	SCE_ID_STORAGE_ERROR_INVALID_LEAF_COUNT  = 0x80230002, //!< Atomic leaf count is outside 1 through 32.
	SCE_ID_STORAGE_ERROR_NOT_FORMATTED       = 0x80230003, //!< IdStorage is not formatted or initialized.
	SCE_ID_STORAGE_ERROR_NO_SPACE            = 0x80230004, //!< Heap allocation or mapping-slot exhaustion.
	SCE_ID_STORAGE_ERROR_LEAF_NOT_FOUND      = 0x80230005, //!< The requested key has no mapping-table entry.
	SCE_ID_STORAGE_ERROR_LEAF_ALREADY_EXISTS = 0x80230006  //!< The requested key already has a mapping-table entry.
} SceIdStorageErrorCode;

/**
 * Read one complete IdStorage leaf.
 *
 * The caller must pass the IdStorage read-access check. The destination is a
 * kernel buffer and is written directly by SceIdStorage. The operation is
 * serialized by the internal IdStorage mutex.
 *
 * @param[in] key - Leaf key from 0 through ::SCE_ID_STORAGE_KEY_MAX.
 * @param[out] buf - Destination for exactly ::SCE_ID_STORAGE_LEAF_SIZE bytes.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceIdStorageReadLeaf(SceUInt32 key, void *buf);

/**
 * Replace one complete IdStorage leaf in the write-back cache.
 *
 * The key must already exist. This operation requires manufacturing mode, the
 * IdStorage write-access check, and writable storage. It is serialized by the
 * internal mutex, marks the cached leaf dirty, and does not guarantee
 * persistence until ::ksceIdStorageFlush succeeds.
 *
 * @param[in] key - Leaf key from 0 through ::SCE_ID_STORAGE_KEY_MAX.
 * @param[in] buf - Source containing exactly ::SCE_ID_STORAGE_LEAF_SIZE bytes.
 *
 * @return 0 on success, -1 when IdStorage is read-only, or a negative error
 *         code.
 */
int ksceIdStorageWriteLeaf(SceUInt32 key, const void *buf);

/**
 * Create several leaf mappings as one in-memory operation.
 *
 * Every key is validated before the mapping table is changed, and all new
 * mappings are allocated within one 32-entry block. The function does not
 * initialize leaf contents and does not reject duplicate keys within the input
 * array. Persistence requires ::ksceIdStorageFlush.
 *
 * This operation requires manufacturing mode, the IdStorage write-access
 * check, formatted storage, and writable storage. It holds the IdStorage mutex
 * and suspends interrupts while changing the mapping table.
 *
 * @param[in] keys - Kernel array containing @p numLeaves 16-bit keys.
 * @param[in] numLeaves - Number of keys, from 1 through
 *                        ::SCE_ID_STORAGE_ATOMIC_LEAVES_MAX.
 *
 * @return 0 on success, -1 when IdStorage is read-only, or a negative error
 *         code.
 */
int ksceIdStorageCreateAtomicLeaves(const SceUInt16 *keys, SceInt32 numLeaves);

/**
 * Create one leaf mapping.
 *
 * The function requires manufacturing mode, the IdStorage write-access check,
 * formatted storage, and writable storage. It holds the IdStorage mutex and
 * suspends interrupts while changing only the in-memory mapping table; leaf
 * contents are not initialized and persistence requires ::ksceIdStorageFlush.
 *
 * @param[in] key - Leaf key from 0 through ::SCE_ID_STORAGE_KEY_MAX.
 *
 * @return 0 on success, -1 when IdStorage is read-only, or a negative error
 *         code.
 */
int ksceIdStorageCreateLeaf(SceUInt32 key);

/**
 * Delete one leaf mapping.
 *
 * The underlying leaf sector is not erased. This operation requires
 * manufacturing mode, the IdStorage write-access check, formatted storage, and
 * writable storage. It holds the IdStorage mutex and suspends interrupts while
 * changing the mapping table. Persistence requires ::ksceIdStorageFlush.
 *
 * @param[in] key - Leaf key from 0 through ::SCE_ID_STORAGE_KEY_MAX.
 *
 * @return 0 on success, -1 when IdStorage is read-only, or a negative error
 *         code.
 */
int ksceIdStorageDeleteLeaf(SceUInt32 key);

/**
 * Acquire and release the IdStorage mutex.
 *
 * Despite its exported name, FW 3.60 does not enumerate or return a key. The
 * function only acts as a synchronization barrier.
 *
 * @return 0 on success, or the mutex-lock error.
 */
int ksceIdStorageEnumId(void);

/**
 * Persist dirty cached leaves and mapping-table changes.
 *
 * FW 3.60 also invokes this operation during the process-suspend system event
 * and when the 32-leaf cache must be recycled. It serializes write-back using
 * the internal IdStorage mutex.
 *
 * @return 0 when there is nothing to flush or all writes succeed, otherwise a
 *         negative formatting, allocation, mutex, seek, or write error.
 */
int ksceIdStorageFlush(void);

/**
 * Initialize a new IdStorage mapping table.
 *
 * FW 3.60 fills the mapping table with 0xFFFF, marks the first 32 entries with
 * reserved value 0xFFF5, and writes the table immediately. Existing leaf data
 * sectors are not erased. Manufacturing mode is required, and formatting is
 * serialized by the internal IdStorage mutex.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceIdStorageFormat(void);

/**
 * Count entries containing the unallocated mapping value 0xFFFF. The mapping
 * table scan is serialized by the internal mutex.
 *
 * @return The number of free mapping entries, or a negative error code when
 *         IdStorage is not formatted or the mutex cannot be acquired.
 */
int ksceIdStorageGetFreeLeaves(void);

/**
 * Get the fixed IdStorage leaf size.
 *
 * @return ::SCE_ID_STORAGE_LEAF_SIZE.
 */
SceSize ksceIdStorageGetLeafSize(void);

/**
 * Check for pending write-back work. This is a lock-free snapshot and may race
 * concurrent writers.
 *
 * @return SCE_TRUE when the mapping table or a cached leaf is dirty, otherwise
 *         SCE_FALSE.
 */
SceBool ksceIdStorageIsDirty(void);

/**
 * Check whether the mapping table was loaded or formatted successfully. This
 * is a lock-free snapshot.
 *
 * @return SCE_TRUE when formatted, otherwise SCE_FALSE.
 */
SceBool ksceIdStorageIsFormatted(void);

/**
 * Return the cached read-only flag.
 *
 * The FW 3.60 module never sets this flag internally and clears it after a
 * successful format. This is a lock-free snapshot.
 *
 * @return SCE_TRUE when read-only, otherwise SCE_FALSE.
 */
SceBool ksceIdStorageIsReadOnly(void);

/**
 * Read a range from an existing leaf.
 *
 * The caller must pass the IdStorage read-access check. The destination is a
 * kernel buffer. A zero-byte lookup still validates and loads the requested
 * key but does not access @p buf, which allows it to be used as an existence
 * check. The operation is serialized by the internal IdStorage mutex.
 *
 * @param[in] key - Leaf key from 0 through ::SCE_ID_STORAGE_KEY_MAX.
 * @param[in] offset - Byte offset below ::SCE_ID_STORAGE_LEAF_SIZE.
 * @param[out] buf - Destination for @p size bytes; not accessed when @p size
 *                   is zero.
 * @param[in] size - Number of bytes. <code>offset + size</code> must not exceed
 *                   ::SCE_ID_STORAGE_LEAF_SIZE.
 *
 * @return 0 on success, or a negative error code.
 */
int ksceIdStorageLookup(SceUInt32 key, SceUInt32 offset, void *buf, SceSize size);

/**
 * Tear down and reinitialize the FW 3.60 IdStorage instance.
 *
 * Manufacturing mode is required. When @p flush is nonzero the function calls
 * ::ksceIdStorageFlush first, but intentionally ignores its return value before
 * destroying and rebuilding the heap, cache, mutex, and partition state. The
 * caller must ensure no other IdStorage operation runs concurrently while the
 * mutex itself is destroyed and recreated.
 *
 * @param[in] flush - Nonzero to attempt a flush before restarting.
 *
 * @return 0 on success, or a negative product-mode or initialization error.
 */
int ksceIdStorageRestart(SceBool flush);

/**
 * Mark the IdStorage partition unformatted.
 *
 * FW 3.60 writes a zero-filled sector at each 32-leaf group boundary and then
 * clears the formatted flag. Individual seek and write failures in that loop
 * are ignored. Manufacturing mode is required, and the loop is serialized by
 * the internal IdStorage mutex.
 *
 * @return 0 after the loop, or a negative product-mode or mutex error before
 *         it begins.
 */
int ksceIdStorageUnformat(void);

/**
 * Replace a range in an existing leaf through a cached read-modify-write.
 *
 * This operation requires manufacturing mode, the IdStorage write-access
 * check, formatted storage, and writable storage. A zero-byte update still
 * loads and marks the leaf dirty. The operation is serialized by the internal
 * IdStorage mutex, and persistence requires ::ksceIdStorageFlush.
 *
 * @param[in] key - Leaf key from 0 through ::SCE_ID_STORAGE_KEY_MAX.
 * @param[in] offset - Byte offset below ::SCE_ID_STORAGE_LEAF_SIZE.
 * @param[in] buf - Source for @p size bytes; not accessed when @p size is zero.
 * @param[in] size - Number of bytes. <code>offset + size</code> must not exceed
 *                   ::SCE_ID_STORAGE_LEAF_SIZE.
 *
 * @return 0 on success, -1 when IdStorage is read-only, or a negative error
 *         code.
 */
int ksceIdStorageUpdate(SceUInt32 key, SceUInt32 offset, const void *buf, SceSize size);

#ifdef __cplusplus
}
#endif

#endif /* _PSP2KERN_IDSTORAGE_H_ */
