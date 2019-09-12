#ifndef Sqlite3Config_h
#define Sqlite3Config_h

#ifdef __APPLE__
# include <TargetConditionals.h>
#endif

#if defined(__ANDROID__)

    // Inspired by AOSP platform_external_sqlite/dist/Android.bp at
    // tag: android-6.0.1_r81

    #define NDEBUG                                      1

    #define HAVE_USLEEP                                 1
    #define HAVE_STRCHRNUL                              0
    #define HAVE_MALLOC_H                               1
    #define HAVE_MALLOC_USABLE_SIZE                     1

    #define USE_PREAD64                                 1

    #define fdatasync                                   fdatasync

    #define SQLITE_HAVE_ISNAN                           1
    #define SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT           1048576
    #define SQLITE_THREADSAFE                           2
    #define SQLITE_TEMP_STORE                           3
    #define SQLITE_POWERSAFE_OVERWRITE                  1
    #define SQLITE_DEFAULT_FILE_FORMAT                  4
    #define SQLITE_DEFAULT_AUTOVACUUM                   1
    #define SQLITE_ENABLE_MEMORY_MANAGEMENT             1
    #define SQLITE_ENABLE_FTS3                          1
    #define SQLITE_ENABLE_FTS3_BACKWARDS                1
    #define SQLITE_ENABLE_FTS4                          1
    #define SQLITE_OMIT_BUILTIN_TEST                    1
    #define SQLITE_OMIT_COMPILEOPTION_DIAGS             1
    #define SQLITE_OMIT_LOAD_EXTENSION                  1
    #define SQLITE_DEFAULT_FILE_PERMISSIONS             0600
    #define SQLITE_SECURE_DELETE                        1
    #define SQLITE_ENABLE_BATCH_ATOMIC_WRITE            1

#elif defined (__APPLE__)

    #define SQLITE_BUG_COMPATIBLE_20160819              1
    #define SQLITE_DEFAULT_CACHE_SIZE                   128
    #define SQLITE_DEFAULT_CKPTFULLFSYNC                1
    #define SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT           32768
    #define SQLITE_DEFAULT_PAGE_SIZE                    4096
    #define SQLITE_DEFAULT_SYNCHRONOUS                  2
    #define SQLITE_DEFAULT_WAL_SYNCHRONOUS              1
    #define SQLITE_ENABLE_API_ARMOR                     1
    #define SQLITE_ENABLE_COLUMN_METADATA               1
    #define SQLITE_ENABLE_DBSTAT_VTAB                   1
    #define SQLITE_ENABLE_FTS3                          1
    #define SQLITE_ENABLE_FTS3_PARENTHESIS              1
    #define SQLITE_ENABLE_FTS3_TOKENIZER                1
    #define SQLITE_ENABLE_FTS4                          1
    #define SQLITE_ENABLE_FTS5                          1
    #define SQLITE_ENABLE_JSON1                         1
    #define SQLITE_ENABLE_LOCKING_STYLE                 1
    #define SQLITE_ENABLE_PREUPDATE_HOOK                1
    #define SQLITE_ENABLE_RTREE                         1
    #define SQLITE_ENABLE_SESSION                       1
    #define SQLITE_ENABLE_SNAPSHOT                      1
    #define SQLITE_ENABLE_SQLLOG                        1
    #define SQLITE_ENABLE_UNKNOWN_SQL_FUNCTION          1
    #define SQLITE_ENABLE_UPDATE_DELETE_LIMIT           1
    #define SQLITE_HAS_CODEC_RESTRICTED                 1
    #define SQLITE_HAVE_ISNAN                           1
    #define SQLITE_MAX_LENGTH                           2147483645
    #define SQLITE_MAX_MMAP_SIZE                        20971520
    #define SQLITE_MAX_VARIABLE_NUMBER                  500000
    #define SQLITE_OMIT_AUTORESET                       1
    #define SQLITE_OMIT_LOAD_EXTENSION                  1
    #define SQLITE_STMTJRNL_SPILL                       131072
    #define SQLITE_SUBSTR_COMPATIBILITY                 1
    #define SQLITE_THREADSAFE                           2
    #define SQLITE_USE_URI                              1

#else

    #error "Unsupported target"

#endif

#endif // Sqlite3Config_h
