#ifndef _NTRTL_H
#define _NTRTL_H

#include <ntldr.h>

// Defines for forwarded ntdll functions

#define RtlExitUserProcess ExitProcess
#define RtlExitUserThread ExitThread

// Linked lists

FORCEINLINE VOID InitializeListHead(
    __out PLIST_ENTRY ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

__checkReturn FORCEINLINE BOOLEAN IsListEmpty(
    __in PLIST_ENTRY ListHead
    )
{
    return ListHead->Flink == ListHead;
}

FORCEINLINE BOOLEAN RemoveEntryList(
    __in PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;

    return Flink == Blink;
}

FORCEINLINE PLIST_ENTRY RemoveHeadList(
    __inout PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;

    return Entry;
}

FORCEINLINE PLIST_ENTRY RemoveTailList(
    __inout PLIST_ENTRY ListHead
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;

    return Entry;
}

FORCEINLINE VOID InsertTailList(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

FORCEINLINE VOID InsertHeadList(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY Entry
    )
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}

FORCEINLINE VOID AppendTailList(
    __inout PLIST_ENTRY ListHead,
    __inout PLIST_ENTRY ListToAppend
    )
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
}

FORCEINLINE PSINGLE_LIST_ENTRY PopEntryList(
    __inout PSINGLE_LIST_ENTRY ListHead
    )
{
    PSINGLE_LIST_ENTRY FirstEntry;

    FirstEntry = ListHead->Next;

    if (FirstEntry)
        ListHead->Next = FirstEntry->Next;

    return FirstEntry;
}

FORCEINLINE VOID PushEntryList(
    __inout PSINGLE_LIST_ENTRY ListHead,
    __inout PSINGLE_LIST_ENTRY Entry
    )
{
    Entry->Next = ListHead->Next;
    ListHead->Next = Entry;
}

// AVL and splay trees

typedef enum _TABLE_SEARCH_RESULT
{
    TableEmptyTree,
    TableFoundNode,
    TableInsertAsLeft,
    TableInsertAsRight
} TABLE_SEARCH_RESULT;

typedef enum _RTL_GENERIC_COMPARE_RESULTS
{
    GenericLessThan,
    GenericGreaterThan,
    GenericEqual
} RTL_GENERIC_COMPARE_RESULTS;

struct _RTL_AVL_TABLE;

typedef RTL_GENERIC_COMPARE_RESULTS (NTAPI *PRTL_AVL_COMPARE_ROUTINE)(
    __in struct _RTL_AVL_TABLE *Table,
    __in PVOID FirstStruct,
    __in PVOID SecondStruct
    );

typedef PVOID (NTAPI *PRTL_AVL_ALLOCATE_ROUTINE)(
    __in struct _RTL_AVL_TABLE *Table,
    __in CLONG ByteSize
    );

typedef VOID (NTAPI *PRTL_AVL_FREE_ROUTINE)(
    __in struct _RTL_AVL_TABLE *Table,
    __in __post_invalid PVOID Buffer
    );

typedef NTSTATUS (NTAPI *PRTL_AVL_MATCH_FUNCTION)(
    __in struct _RTL_AVL_TABLE *Table,
    __in PVOID UserData,
    __in PVOID MatchData
    );

typedef struct _RTL_BALANCED_LINKS
{
    struct _RTL_BALANCED_LINKS *Parent;
    struct _RTL_BALANCED_LINKS *LeftChild;
    struct _RTL_BALANCED_LINKS *RightChild;
    CHAR Balance;
    UCHAR Reserved[3];
} RTL_BALANCED_LINKS, *PRTL_BALANCED_LINKS;

typedef struct _RTL_AVL_TABLE
{
    RTL_BALANCED_LINKS BalancedRoot;
    PVOID OrderedPointer;
    ULONG WhichOrderedElement;
    ULONG NumberGenericTableElements;
    ULONG DepthOfTree;
    PRTL_BALANCED_LINKS RestartKey;
    ULONG DeleteCount;
    PRTL_AVL_COMPARE_ROUTINE CompareRoutine;
    PRTL_AVL_ALLOCATE_ROUTINE AllocateRoutine;
    PRTL_AVL_FREE_ROUTINE FreeRoutine;
    PVOID TableContext;
} RTL_AVL_TABLE, *PRTL_AVL_TABLE;

NTSYSAPI
VOID
NTAPI
RtlInitializeGenericTableAvl(
    __out PRTL_AVL_TABLE Table,
    __in PRTL_AVL_COMPARE_ROUTINE CompareRoutine,
    __in PRTL_AVL_ALLOCATE_ROUTINE AllocateRoutine,
    __in PRTL_AVL_FREE_ROUTINE FreeRoutine,
    __in_opt PVOID TableContext
    );

NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTableAvl(
    __in PRTL_AVL_TABLE Table,
    __in_bcount(BufferSize) PVOID Buffer,
    __in CLONG BufferSize,
    __out_opt PBOOLEAN NewElement
    );

NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTableFullAvl(
    __in PRTL_AVL_TABLE Table,
    __in_bcount(BufferSize) PVOID Buffer,
    __in CLONG BufferSize,
    __out_opt PBOOLEAN NewElement,
    __in PVOID NodeOrParent,
    __in TABLE_SEARCH_RESULT SearchResult
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlDeleteElementGenericTableAvl(
    __in PRTL_AVL_TABLE Table,
    __in PVOID Buffer
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTableAvl(
    __in PRTL_AVL_TABLE Table,
    __in PVOID Buffer
    );

NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTableFullAvl(
    __in PRTL_AVL_TABLE Table,
    __in PVOID Buffer,
    __out PVOID *NodeOrParent,
    __out TABLE_SEARCH_RESULT *SearchResult
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTableAvl(
    __in PRTL_AVL_TABLE Table,
    __in BOOLEAN Restart
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTableWithoutSplayingAvl(
    __in PRTL_AVL_TABLE Table,
    __inout PVOID *RestartKey
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlLookupFirstMatchingElementGenericTableAvl(
    __in PRTL_AVL_TABLE Table,
    __in PVOID Buffer,
    __out PVOID *RestartKey
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTableLikeADirectory(
    __in PRTL_AVL_TABLE Table,
    __in_opt PRTL_AVL_MATCH_FUNCTION MatchFunction,
    __in_opt PVOID MatchData,
    __in ULONG NextFlag,
    __inout PVOID *RestartKey,
    __inout PULONG DeleteCount,
    __in PVOID Buffer
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlGetElementGenericTableAvl(
    __in PRTL_AVL_TABLE Table,
    __in ULONG I
    );

NTSYSAPI
ULONG
NTAPI
RtlNumberGenericTableElementsAvl(
    __in PRTL_AVL_TABLE Table
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlIsGenericTableEmptyAvl(
    __in PRTL_AVL_TABLE Table
    );

typedef struct _RTL_SPLAY_LINKS
{
    struct _RTL_SPLAY_LINKS *Parent;
    struct _RTL_SPLAY_LINKS *LeftChild;
    struct _RTL_SPLAY_LINKS *RightChild;
} RTL_SPLAY_LINKS, *PRTL_SPLAY_LINKS;

#define RtlInitializeSplayLinks(Links) \
    { \
        PRTL_SPLAY_LINKS _SplayLinks; \
        _SplayLinks = (PRTL_SPLAY_LINKS)(Links); \
        _SplayLinks->Parent = _SplayLinks; \
        _SplayLinks->LeftChild = NULL; \
        _SplayLinks->RightChild = NULL; \
    }

#define RtlParent(Links) ((PRTL_SPLAY_LINKS)(Links)->Parent)
#define RtlLeftChild(Links) ((PRTL_SPLAY_LINKS)(Links)->LeftChild)
#define RtlRightChild(Links) ((PRTL_SPLAY_LINKS)(Links)->RightChild)
#define RtlIsRoot(Links) ((RtlParent(Links) == (PRTL_SPLAY_LINKS)(Links)))
#define RtlIsLeftChild(Links) ((RtlLeftChild(RtlParent(Links)) == (PRTL_SPLAY_LINKS)(Links)))
#define RtlIsRightChild(Links) ((RtlRightChild(RtlParent(Links)) == (PRTL_SPLAY_LINKS)(Links)))

#define RtlInsertAsLeftChild(ParentLinks, ChildLinks) \
    { \
        PRTL_SPLAY_LINKS _SplayParent; \
        PRTL_SPLAY_LINKS _SplayChild; \
        _SplayParent = (PRTL_SPLAY_LINKS)(ParentLinks); \
        _SplayChild = (PRTL_SPLAY_LINKS)(ChildLinks); \
        _SplayParent->LeftChild = _SplayChild; \
        _SplayChild->Parent = _SplayParent; \
    }

#define RtlInsertAsRightChild(ParentLinks, ChildLinks) \
    { \
        PRTL_SPLAY_LINKS _SplayParent; \
        PRTL_SPLAY_LINKS _SplayChild; \
        _SplayParent = (PRTL_SPLAY_LINKS)(ParentLinks); \
        _SplayChild = (PRTL_SPLAY_LINKS)(ChildLinks); \
        _SplayParent->RightChild = _SplayChild; \
        _SplayChild->Parent = _SplayParent; \
    }

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlSplay(
    __inout PRTL_SPLAY_LINKS Links
    );

NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlDelete(
    __in PRTL_SPLAY_LINKS Links
    );

NTSYSAPI
VOID
NTAPI
RtlDeleteNoSplay(
    __in PRTL_SPLAY_LINKS Links,
    __inout PRTL_SPLAY_LINKS *Root
    );

__checkReturn
NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlSubtreeSuccessor(
    __in PRTL_SPLAY_LINKS Links
    );

__checkReturn
NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlSubtreePredecessor(
    __in PRTL_SPLAY_LINKS Links
    );

__checkReturn
NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlRealSuccessor(
    __in PRTL_SPLAY_LINKS Links
    );

__checkReturn
NTSYSAPI
PRTL_SPLAY_LINKS
NTAPI
RtlRealPredecessor(
    __in PRTL_SPLAY_LINKS Links
    );

struct _RTL_GENERIC_TABLE;

typedef RTL_GENERIC_COMPARE_RESULTS (NTAPI *PRTL_GENERIC_COMPARE_ROUTINE)(
    __in struct _RTL_GENERIC_TABLE *Table,
    __in PVOID FirstStruct,
    __in PVOID SecondStruct
    );

typedef PVOID (NTAPI *PRTL_GENERIC_ALLOCATE_ROUTINE)(
    __in struct _RTL_GENERIC_TABLE *Table,
    __in CLONG ByteSize
    );

typedef VOID (NTAPI *PRTL_GENERIC_FREE_ROUTINE)(
    __in struct _RTL_GENERIC_TABLE *Table,
    __in __post_invalid PVOID Buffer
    );

typedef struct _RTL_GENERIC_TABLE
{
    PRTL_SPLAY_LINKS TableRoot;
    LIST_ENTRY InsertOrderList;
    PLIST_ENTRY OrderedPointer;
    ULONG WhichOrderedElement;
    ULONG NumberGenericTableElements;
    PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine;
    PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine;
    PRTL_GENERIC_FREE_ROUTINE FreeRoutine;
    PVOID TableContext;
} RTL_GENERIC_TABLE, *PRTL_GENERIC_TABLE;

NTSYSAPI
VOID
NTAPI
RtlInitializeGenericTable(
    __out PRTL_GENERIC_TABLE Table,
    __in PRTL_GENERIC_COMPARE_ROUTINE CompareRoutine,
    __in PRTL_GENERIC_ALLOCATE_ROUTINE AllocateRoutine,
    __in PRTL_GENERIC_FREE_ROUTINE FreeRoutine,
    __in_opt PVOID TableContext
    );

NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTable(
    __in PRTL_GENERIC_TABLE Table,
    __in_bcount(BufferSize) PVOID Buffer,
    __in CLONG BufferSize,
    __out_opt PBOOLEAN NewElement
    );

NTSYSAPI
PVOID
NTAPI
RtlInsertElementGenericTableFull(
    __in PRTL_GENERIC_TABLE Table,
    __in_bcount(BufferSize) PVOID Buffer,
    __in CLONG BufferSize,
    __out_opt PBOOLEAN NewElement,
    __in PVOID NodeOrParent,
    __in TABLE_SEARCH_RESULT SearchResult
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlDeleteElementGenericTable(
    __in PRTL_GENERIC_TABLE Table,
    __in PVOID Buffer
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTable(
    __in PRTL_GENERIC_TABLE Table,
    __in PVOID Buffer
    );

NTSYSAPI
PVOID
NTAPI
RtlLookupElementGenericTableFull(
    __in PRTL_GENERIC_TABLE Table,
    __in PVOID Buffer,
    __out PVOID *NodeOrParent,
    __out TABLE_SEARCH_RESULT *SearchResult
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTable(
    __in PRTL_GENERIC_TABLE Table,
    __in BOOLEAN Restart
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlEnumerateGenericTableWithoutSplaying(
    __in PRTL_GENERIC_TABLE Table,
    __inout PVOID *RestartKey
    );

__checkReturn
NTSYSAPI
PVOID
NTAPI
RtlGetElementGenericTable(
    __in PRTL_GENERIC_TABLE Table,
    __in ULONG I
    );

NTSYSAPI
ULONG
NTAPI
RtlNumberGenericTableElements(
    __in PRTL_GENERIC_TABLE Table
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlIsGenericTableEmpty(
    __in PRTL_GENERIC_TABLE Table
    );

// Hash tables

// begin_ntddk

#define RTL_HASH_ALLOCATED_HEADER 0x00000001
#define RTL_HASH_RESERVED_SIGNATURE 0

typedef struct _RTL_DYNAMIC_HASH_TABLE_ENTRY
{
    LIST_ENTRY Linkage;
    ULONG_PTR Signature;
} RTL_DYNAMIC_HASH_TABLE_ENTRY, *PRTL_DYNAMIC_HASH_TABLE_ENTRY;

#define HASH_ENTRY_KEY(x) ((x)->Signature)

typedef struct _RTL_DYNAMIC_HASH_TABLE_CONTEXT
{
    PLIST_ENTRY ChainHead;
    PLIST_ENTRY PrevLinkage;
    ULONG_PTR Signature;
} RTL_DYNAMIC_HASH_TABLE_CONTEXT, *PRTL_DYNAMIC_HASH_TABLE_CONTEXT;

typedef struct _RTL_DYNAMIC_HASH_TABLE_ENUMERATOR
{
    RTL_DYNAMIC_HASH_TABLE_ENTRY HashEntry;
    PLIST_ENTRY ChainHead;
    ULONG BucketIndex;
} RTL_DYNAMIC_HASH_TABLE_ENUMERATOR, *PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR;

typedef struct _RTL_DYNAMIC_HASH_TABLE
{
    // Entries initialized at creation.
    ULONG Flags;
    ULONG Shift;

    // Entries used in bucket computation.
    ULONG TableSize;
    ULONG Pivot;
    ULONG DivisorMask;

    // Counters.
    ULONG NumEntries;
    ULONG NonEmptyBuckets;
    ULONG NumEnumerators;

    // The directory. This field is for internal use only.
    PVOID Directory;
} RTL_DYNAMIC_HASH_TABLE, *PRTL_DYNAMIC_HASH_TABLE;

#if (PHNT_VERSION >= PHNT_WIN7)

FORCEINLINE
VOID
RtlInitHashTableContext(
    __inout PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    )
{
    Context->ChainHead = NULL;
    Context->PrevLinkage = NULL;
}

FORCEINLINE
VOID
RtlInitHashTableContextFromEnumerator(
    __inout PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context,
    __in PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    )
{
    Context->ChainHead = Enumerator->ChainHead;
    Context->PrevLinkage = Enumerator->HashEntry.Linkage.Blink;
}

FORCEINLINE
VOID
RtlReleaseHashTableContext(
    __inout PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    )
{
    UNREFERENCED_PARAMETER(Context);
    return;
}

FORCEINLINE
ULONG
RtlTotalBucketsHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->TableSize;
}

FORCEINLINE
ULONG
RtlNonEmptyBucketsHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->NonEmptyBuckets;
}

FORCEINLINE
ULONG
RtlEmptyBucketsHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->TableSize - HashTable->NonEmptyBuckets;
}

FORCEINLINE
ULONG
RtlTotalEntriesHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->NumEntries;
}

FORCEINLINE
ULONG
RtlActiveEnumeratorsHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    )
{
    return HashTable->NumEnumerators;
}

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlCreateHashTable(
    __deref_inout_opt PRTL_DYNAMIC_HASH_TABLE *HashTable,
    __in ULONG Shift,
    __in __reserved ULONG Flags
    );

NTSYSAPI
VOID
NTAPI
RtlDeleteHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlInsertEntryHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __in PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry,
    __in ULONG_PTR Signature,
    __inout_opt PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlRemoveEntryHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __in PRTL_DYNAMIC_HASH_TABLE_ENTRY Entry,
    __inout_opt PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    );

__checkReturn
NTSYSAPI
PRTL_DYNAMIC_HASH_TABLE_ENTRY
NTAPI
RtlLookupEntryHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __in ULONG_PTR Signature,
    __out_opt PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    );

__checkReturn
NTSYSAPI
PRTL_DYNAMIC_HASH_TABLE_ENTRY
NTAPI
RtlGetNextEntryHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __in PRTL_DYNAMIC_HASH_TABLE_CONTEXT Context
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlInitEnumerationHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __out PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    );

__checkReturn
NTSYSAPI
PRTL_DYNAMIC_HASH_TABLE_ENTRY
NTAPI
RtlEnumerateEntryHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __inout PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    );

NTSYSAPI
VOID
NTAPI
RtlEndEnumerationHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __inout PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlInitWeakEnumerationHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __out PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    );

__checkReturn
NTSYSAPI
PRTL_DYNAMIC_HASH_TABLE_ENTRY
NTAPI
RtlWeaklyEnumerateEntryHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __inout PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    );

NTSYSAPI
VOID
NTAPI
RtlEndWeakEnumerationHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable,
    __inout PRTL_DYNAMIC_HASH_TABLE_ENUMERATOR Enumerator
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlExpandHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlContractHashTable(
    __in PRTL_DYNAMIC_HASH_TABLE HashTable
    );

#endif

// end_ntddk

// Critical sections

NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeCriticalSection(
    __out PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeCriticalSectionAndSpinCount(
    __inout PRTL_CRITICAL_SECTION CriticalSection,
    __in ULONG SpinCount
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteCriticalSection(
    __inout PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlEnterCriticalSection(
    __inout PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlLeaveCriticalSection(
    __inout PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
LOGICAL
NTAPI
RtlTryEnterCriticalSection(
    __inout PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
LOGICAL
NTAPI
RtlIsCriticalSectionLocked(
    __in PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
LOGICAL
NTAPI
RtlIsCriticalSectionLockedByThread(
    __in PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
ULONG
NTAPI
RtlGetCriticalSectionRecursionCount(
    __in PRTL_CRITICAL_SECTION CriticalSection
    );

NTSYSAPI
ULONG
NTAPI
RtlSetCriticalSectionSpinCount(
    __inout PRTL_CRITICAL_SECTION CriticalSection,
    __in ULONG SpinCount
    );

// Slim reader-writer locks and condition variables

#if (PHNT_VERSION >= PHNT_VISTA)

// winbase:InitializeSRWLock
NTSYSAPI
VOID
NTAPI
RtlInitializeSRWLock(
    __out PRTL_SRWLOCK SRWLock
    );

// winbase:AcquireSRWLockExclusive
NTSYSAPI
VOID
NTAPI
RtlAcquireSRWLockExclusive(
    __inout PRTL_SRWLOCK SRWLock
    );

// winbase:AcquireSRWLockShared
NTSYSAPI
VOID
NTAPI
RtlAcquireSRWLockShared(
    __inout PRTL_SRWLOCK SRWLock
    );

// winbase:ReleaseSRWLockExclusive
NTSYSAPI
VOID
NTAPI
RtlReleaseSRWLockExclusive(
    __inout PRTL_SRWLOCK SRWLock
    );

// winbase:ReleaseSRWLockShared
NTSYSAPI
VOID
NTAPI
RtlReleaseSRWLockShared(
    __inout PRTL_SRWLOCK SRWLock
    );

// winbase:TryAcquireSRWLockExclusive
NTSYSAPI
BOOLEAN
NTAPI
RtlTryAcquireSRWLockExclusive(
    __inout PRTL_SRWLOCK SRWLock
    );

// winbase:TryAcquireSRWLockShared
NTSYSAPI
BOOLEAN
NTAPI
RtlTryAcquireSRWLockShared(
    __inout PRTL_SRWLOCK SRWLock
    );

// winbase:InitializeConditionVariable
NTSYSAPI
VOID
NTAPI
RtlInitializeConditionVariable(
    __out PRTL_CONDITION_VARIABLE ConditionVariable
    );

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlSleepConditionVariableCS(
    __inout PRTL_CONDITION_VARIABLE ConditionVariable,
    __inout PRTL_CRITICAL_SECTION CriticalSection,
    __in_opt PLARGE_INTEGER Timeout
    );

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlSleepConditionVariableSRW(
    __inout PRTL_CONDITION_VARIABLE ConditionVariable,
    __inout PRTL_SRWLOCK SRWLock,
    __in_opt PLARGE_INTEGER Timeout,
    __in ULONG Flags
    );

// winbase:WakeConditionVariable
NTSYSAPI
VOID
NTAPI
RtlWakeConditionVariable(
    __inout PRTL_CONDITION_VARIABLE ConditionVariable
    );

// winbase:WakeAllConditionVariable
NTSYSAPI
VOID
NTAPI
RtlWakeAllConditionVariable(
    __inout PRTL_CONDITION_VARIABLE ConditionVariable
    );

#endif

// Strings

#ifndef PHNT_NO_INLINE_INIT_STRING
FORCEINLINE VOID RtlInitString(
    __out PSTRING DestinationString,
    __in_opt PSTR SourceString
    )
{
    if (SourceString)
        DestinationString->MaximumLength = DestinationString->Length = (USHORT)strlen(SourceString);
    else
        DestinationString->MaximumLength = DestinationString->Length = 0;

    DestinationString->Buffer = SourceString;
}
#else
NTSYSAPI
VOID
NTAPI
RtlInitString(
    __out PSTRING DestinationString,
    __in_opt PSTR SourceString
    );
#endif

#ifndef PHNT_NO_INLINE_INIT_STRING
FORCEINLINE VOID RtlInitAnsiString(
    __out PANSI_STRING DestinationString,
    __in_opt PSTR SourceString
    )
{
    if (SourceString)
        DestinationString->MaximumLength = DestinationString->Length = (USHORT)strlen(SourceString);
    else
        DestinationString->MaximumLength = DestinationString->Length = 0;

    DestinationString->Buffer = SourceString;
}
#else
NTSYSAPI
VOID
NTAPI
RtlInitAnsiString(
    __out PANSI_STRING DestinationString,
    __in_opt PSTR SourceString
    );
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlInitAnsiStringEx(
    __out PANSI_STRING DestinationString,
    __in_opt PSTR SourceString
    );

NTSYSAPI
VOID
NTAPI
RtlFreeAnsiString(
    __in PANSI_STRING AnsiString
    );

NTSYSAPI
VOID
NTAPI
RtlFreeOemString(
    __in POEM_STRING OemString
    );

NTSYSAPI
VOID
NTAPI
RtlCopyString(
    __in PSTRING DestinationString,
    __in_opt PSTRING SourceString
    );

NTSYSAPI
CHAR
NTAPI
RtlUpperChar(
    __in CHAR Character
    );

NTSYSAPI
LONG
NTAPI
RtlCompareString(
    __in PSTRING String1,
    __in PSTRING String2,
    __in BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualString(
    __in PSTRING String1,
    __in PSTRING String2,
    __in BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixString(
    __in PSTRING String1,
    __in PSTRING String2,
    __in BOOLEAN CaseInSensitive
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendStringToString(
    __in PSTRING Destination,
    __in PSTRING Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendAsciizToString(
    __in PSTRING Destination,
    __in_opt PSTR Source
    );

NTSYSAPI
VOID
NTAPI
RtlUpperString(
    __in PSTRING DestinationString,
    __in PSTRING SourceString
    );

#ifndef PHNT_NO_INLINE_INIT_STRING
FORCEINLINE VOID RtlInitUnicodeString(
    __out PUNICODE_STRING DestinationString,
    __in_opt PWSTR SourceString
    )
{
    // MaximumLength should really be Length + sizeof(WCHAR), but it doesn't matter too much.
    if (SourceString)
        DestinationString->MaximumLength = DestinationString->Length = (USHORT)(wcslen(SourceString) * sizeof(WCHAR));
    else
        DestinationString->MaximumLength = DestinationString->Length = 0;

    DestinationString->Buffer = SourceString;
}
#else
NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    __out PUNICODE_STRING DestinationString,
    __in_opt PWSTR SourceString
    );
#endif

NTSYSAPI
NTSTATUS
NTAPI
RtlInitUnicodeStringEx(
    __out PUNICODE_STRING DestinationString,
    __in_opt PWSTR SourceString
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeString(
    __out PUNICODE_STRING DestinationString,
    __in PWSTR SourceString
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlCreateUnicodeStringFromAsciiz(
    __out PUNICODE_STRING DestinationString,
    __in PSTR SourceString
    );

NTSYSAPI
VOID
NTAPI
RtlFreeUnicodeString(
    __in PUNICODE_STRING UnicodeString
    );

#define RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE (0x00000001)
#define RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING (0x00000002)

NTSYSAPI
NTSTATUS
NTAPI
RtlDuplicateUnicodeString(
    __in ULONG Flags,
    __in PUNICODE_STRING StringIn,
    __out PUNICODE_STRING StringOut
    );

NTSYSAPI
VOID
NTAPI
RtlCopyUnicodeString(
    __in PUNICODE_STRING DestinationString,
    __in PUNICODE_STRING SourceString
    );

NTSYSAPI
WCHAR
NTAPI
RtlUpcaseUnicodeChar(
    __in WCHAR SourceCharacter
    );

NTSYSAPI
WCHAR
NTAPI
RtlDowncaseUnicodeChar(
    __in WCHAR SourceCharacter
    );

NTSYSAPI
LONG
NTAPI
RtlCompareUnicodeString(
    __in PUNICODE_STRING String1,
    __in PUNICODE_STRING String2,
    __in BOOLEAN CaseInSensitive
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    __in PUNICODE_STRING String1,
    __in PUNICODE_STRING String2,
    __in BOOLEAN CaseInSensitive
    );

#define HASH_STRING_ALGORITHM_DEFAULT 0
#define HASH_STRING_ALGORITHM_X65599 1
#define HASH_STRING_ALGORITHM_INVALID 0xffffffff

NTSYSAPI
NTSTATUS
NTAPI
RtlHashUnicodeString(
    __in PUNICODE_STRING String,
    __in BOOLEAN CaseInSensitive,
    __in ULONG HashAlgorithm,
    __out PULONG HashValue
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlValidateUnicodeString(
    __in ULONG Flags,
    __in PUNICODE_STRING String
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlPrefixUnicodeString(
    __in PUNICODE_STRING String1,
    __in PUNICODE_STRING String2,
    __in BOOLEAN CaseInSensitive
    );

#define RTL_FIND_CHAR_IN_UNICODE_STRING_START_AT_END 0x00000001
#define RTL_FIND_CHAR_IN_UNICODE_STRING_COMPLEMENT_CHAR_SET 0x00000002
#define RTL_FIND_CHAR_IN_UNICODE_STRING_CASE_INSENSITIVE 0x00000004

NTSYSAPI
NTSTATUS
NTAPI
RtlFindCharInUnicodeString(
    __in ULONG Flags,
    __in PUNICODE_STRING StringToSearch,
    __in PUNICODE_STRING CharSet,
    __out PUSHORT NonInclusivePrefixLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString(
    __in PUNICODE_STRING Destination,
    __in PUNICODE_STRING Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAppendUnicodeToString(
    __in PUNICODE_STRING Destination,
    __in_opt PWSTR Source
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    __inout PUNICODE_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDowncaseUnicodeString(
    __inout PUNICODE_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
VOID
NTAPI
RtlEraseUnicodeString(
    __inout PUNICODE_STRING String
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    __inout PUNICODE_STRING DestinationString,
    __in PANSI_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    __inout PANSI_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
WCHAR
NTAPI
RtlAnsiCharToUnicodeChar(
    __inout PUCHAR *SourceCharacter
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToAnsiString(
    __inout PANSI_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlOemStringToUnicodeString(
    __inout PUNICODE_STRING DestinationString,
    __in POEM_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToOemString(
    __inout POEM_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToOemString(
    __inout POEM_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlOemStringToCountedUnicodeString(
    __inout PUNICODE_STRING DestinationString,
    __in POEM_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeStringToCountedOemString(
    __inout POEM_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeStringToCountedOemString(
    __inout POEM_STRING DestinationString,
    __in PUNICODE_STRING SourceString,
    __in BOOLEAN AllocateDestinationString
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeN(
    __out PWSTR UnicodeString,
    __in ULONG MaxBytesInUnicodeString,
    __out_opt PULONG BytesInUnicodeString,
    __in PSTR MultiByteString,
    __in ULONG BytesInMultiByteString
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlMultiByteToUnicodeSize(
    __out PULONG BytesInUnicodeString,
    __in PSTR MultiByteString,
    __in ULONG BytesInMultiByteString
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    __out PSTR MultiByteString,
    __in ULONG MaxBytesInMultiByteString,
    __out_opt PULONG BytesInMultiByteString,
    __in PWSTR UnicodeString,
    __in ULONG BytesInUnicodeString
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteSize(
    __out PULONG BytesInMultiByteString,
    __in PWSTR UnicodeString,
    __in ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToMultiByteN(
    __out_bcount_part(MaxBytesInMultiByteString, *BytesInMultiByteString) PSTR MultiByteString,
    __in ULONG MaxBytesInMultiByteString,
    __out_opt PULONG BytesInMultiByteString,
    __in_bcount(BytesInUnicodeString) PWSTR UnicodeString,
    __in ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlOemToUnicodeN(
    __out_bcount_part(MaxBytesInUnicodeString, *BytesInUnicodeString) PWSTR UnicodeString,
    __in ULONG MaxBytesInUnicodeString,
    __out_opt PULONG BytesInUnicodeString,
    __in_bcount(BytesInOemString) PSTR OemString,
    __in ULONG BytesInOemString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToOemN(
    __out_bcount_part(MaxBytesInOemString, *BytesInOemString) PSTR OemString,
    __in ULONG MaxBytesInOemString,
    __out_opt PULONG BytesInOemString,
    __in_bcount(BytesInUnicodeString) PWSTR UnicodeString,
    __in ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpcaseUnicodeToOemN(
    __out_bcount_part(MaxBytesInOemString, *BytesInOemString) PSTR OemString,
    __in ULONG MaxBytesInOemString,
    __out_opt PULONG BytesInOemString,
    __in_bcount(BytesInUnicodeString) PWSTR UnicodeString,
    __in ULONG BytesInUnicodeString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlConsoleMultiByteToUnicodeN(
    __out_bcount_part(MaxBytesInUnicodeString, *BytesInUnicodeString) PWSTR UnicodeString,
    __in ULONG MaxBytesInUnicodeString,
    __out_opt PULONG BytesInUnicodeString,
    __in_bcount(BytesInMultiByteString) PSTR MultiByteString,
    __in ULONG BytesInMultiByteString,
    __out PULONG pdwSpecialChar
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlIsTextUnicode(
    __in PVOID Buffer,
    __in ULONG Size,
    __inout_opt PULONG Result
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualDomainName(
    __in PUNICODE_STRING String1,
    __in PUNICODE_STRING String2
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlEqualComputerName(
    __in PUNICODE_STRING String1,
    __in PUNICODE_STRING String2
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDnsHostNameToComputerName(
    __out PUNICODE_STRING ComputerNameString,
    __in PCUNICODE_STRING DnsHostNameString,
    __in BOOLEAN AllocateComputerNameString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlStringFromGUID(
    __in PGUID Guid,
    __out PUNICODE_STRING GuidString
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGUIDFromString(
    __in PUNICODE_STRING GuidString,
    __out PGUID Guid
    );

#if (PHNT_VERSION >= PHNT_VISTA)
// ntifs
NTSYSAPI
NTSTATUS
NTAPI
RtlNormalizeString(
    __in ULONG NormForm,
    __in PCWSTR SourceString,
    __in LONG SourceStringLength,
    __out_ecount_part(*DestinationStringLength, *DestinationStringLength) PWSTR DestinationString,
    __inout PLONG DestinationStringLength
    );
#endif

#if (PHNT_VERSION >= PHNT_VISTA)
// ntifs
NTSYSAPI
NTSTATUS
NTAPI
RtlIsNormalizedString(
    __in ULONG NormForm,
    __in PCWSTR SourceString,
    __in LONG SourceStringLength,
    __out PBOOLEAN Normalized
    );
#endif

#if (PHNT_VERSION >= PHNT_WIN7)
// ntifs:FsRtlIsNameInExpression
NTSYSAPI
BOOLEAN
NTAPI
RtlIsNameInExpression(
    __in PUNICODE_STRING Expression,
    __in PUNICODE_STRING Name,
    __in BOOLEAN IgnoreCase,
    __in_opt PWCH UpcaseTable
    );
#endif

// PEB

NTSYSAPI
VOID
NTAPI
RtlAcquirePebLock();

NTSYSAPI
VOID
NTAPI
RtlReleasePebLock();

NTSYSAPI
NTSTATUS
NTAPI
RtlAllocateFromPeb(
    __in ULONG Size,
    __out PVOID *Block
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlFreeToPeb(
    __in PVOID Block,
    __in ULONG Size
    );

// Processes

#define DOS_MAX_COMPONENT_LENGTH 255
#define DOS_MAX_PATH_LENGTH (DOS_MAX_COMPONENT_LENGTH + 5)

typedef struct _CURDIR
{
    UNICODE_STRING DosPath;
    HANDLE Handle;
} CURDIR, *PCURDIR;

#define RTL_USER_PROC_CURDIR_CLOSE 0x00000002
#define RTL_USER_PROC_CURDIR_INHERIT 0x00000003

typedef struct _RTL_DRIVE_LETTER_CURDIR
{
    USHORT Flags;
    USHORT Length;
    ULONG TimeStamp;
    STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

typedef struct _RTL_USER_PROCESS_PARAMETERS
{
    ULONG MaximumLength;
    ULONG Length;

    ULONG Flags;
    ULONG DebugFlags;

    HANDLE ConsoleHandle;
    ULONG ConsoleFlags;
    HANDLE StandardInput;
    HANDLE StandardOutput;
    HANDLE StandardError;

    CURDIR CurrentDirectory;
    UNICODE_STRING DllPath;
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    PVOID Environment;

    ULONG StartingX;
    ULONG StartingY;
    ULONG CountX;
    ULONG CountY;
    ULONG CountCharsX;
    ULONG CountCharsY;
    ULONG FillAttribute;

    ULONG WindowFlags;
    ULONG ShowWindowFlags;
    UNICODE_STRING WindowTitle;
    UNICODE_STRING DesktopInfo;
    UNICODE_STRING ShellInfo;
    UNICODE_STRING RuntimeData;
    RTL_DRIVE_LETTER_CURDIR CurrentDirectories[RTL_MAX_DRIVE_LETTERS];

    ULONG EnvironmentSize;
    ULONG EnvironmentVersion;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

#define RTL_USER_PROC_PARAMS_NORMALIZED 0x00000001
#define RTL_USER_PROC_PROFILE_USER 0x00000002
#define RTL_USER_PROC_PROFILE_KERNEL 0x00000004
#define RTL_USER_PROC_PROFILE_SERVER 0x00000008
#define RTL_USER_PROC_RESERVE_1MB 0x00000020
#define RTL_USER_PROC_RESERVE_16MB 0x00000040
#define RTL_USER_PROC_CASE_SENSITIVE 0x00000080
#define RTL_USER_PROC_DISABLE_HEAP_DECOMMIT 0x00000100
#define RTL_USER_PROC_DLL_REDIRECTION_LOCAL 0x00001000
#define RTL_USER_PROC_APP_MANIFEST_PRESENT 0x00002000
#define RTL_USER_PROC_IMAGE_KEY_MISSING 0x00004000
#define RTL_USER_PROC_OPTIN_PROCESS 0x00020000

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateProcessParameters(
    __out PRTL_USER_PROCESS_PARAMETERS *ProcessParameters,
    __in PUNICODE_STRING ImagePathName,
    __in_opt PUNICODE_STRING DllPath,
    __in_opt PUNICODE_STRING CurrentDirectory,
    __in_opt PUNICODE_STRING CommandLine,
    __in_opt PVOID Environment,
    __in_opt PUNICODE_STRING WindowTitle,
    __in_opt PUNICODE_STRING DesktopInfo,
    __in_opt PUNICODE_STRING ShellInfo,
    __in_opt PUNICODE_STRING RuntimeData
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDestroyProcessParameters(
    __in __post_invalid PRTL_USER_PROCESS_PARAMETERS ProcessParameters
    );

NTSYSAPI
PRTL_USER_PROCESS_PARAMETERS
NTAPI
RtlNormalizeProcessParams(
    __inout PRTL_USER_PROCESS_PARAMETERS ProcessParameters
    );

NTSYSAPI
PRTL_USER_PROCESS_PARAMETERS
NTAPI
RtlDeNormalizeProcessParams(
    __inout PRTL_USER_PROCESS_PARAMETERS ProcessParameters
    );

typedef struct _RTL_USER_PROCESS_INFORMATION
{
    ULONG Length;
    HANDLE Process;
    HANDLE Thread;
    CLIENT_ID ClientId;
    SECTION_IMAGE_INFORMATION ImageInformation;
} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateUserProcess(
    __in PUNICODE_STRING NtImagePathName,
    __in ULONG Attributes,
    __in PRTL_USER_PROCESS_PARAMETERS ProcessParameters,
    __in_opt PSECURITY_DESCRIPTOR ProcessSecurityDescriptor,
    __in_opt PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    __in_opt HANDLE ParentProcess,
    __in BOOLEAN InheritHandles,
    __in_opt HANDLE DebugPort,
    __in_opt HANDLE ExceptionPort,
    __out PRTL_USER_PROCESS_INFORMATION ProcessInformation
    );

NTSYSAPI
NTSTATUS
STDAPIVCALLTYPE
RtlSetProcessIsCritical(
    __in BOOLEAN NewValue,
    __out_opt PBOOLEAN OldValue,
    __in BOOLEAN CheckFlag
    );

NTSYSAPI
NTSTATUS
STDAPIVCALLTYPE
RtlSetThreadIsCritical(
    __in BOOLEAN NewValue,
    __out_opt PBOOLEAN OldValue,
    __in BOOLEAN CheckFlag
    );

// Threads

typedef NTSTATUS (NTAPI *PUSER_THREAD_START_ROUTINE)(
    __in PVOID ThreadParameter
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateUserThread(
    __in HANDLE Process,
    __in_opt PSECURITY_DESCRIPTOR ThreadSecurityDescriptor,
    __in BOOLEAN CreateSuspended,
    __in_opt ULONG ZeroBits,
    __in_opt SIZE_T MaximumStackSize,
    __in_opt SIZE_T CommittedStackSize,
    __in PUSER_THREAD_START_ROUTINE StartAddress,
    __in_opt PVOID Parameter,
    __out_opt PHANDLE Thread,
    __out_opt PCLIENT_ID ClientId
    );

NTSYSAPI
VOID
NTAPI
RtlInitializeContext(
    __in HANDLE Process,
    __out PCONTEXT Context,
    __in_opt PVOID Parameter,
    __in_opt PVOID InitialPc,
    __in_opt PVOID InitialSp
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlRemoteCall(
    __in HANDLE Process,
    __in HANDLE Thread,
    __in PVOID CallSite,
    __in ULONG ArgumentCount,
    __in_opt PULONG_PTR Arguments,
    __in BOOLEAN PassContext,
    __in BOOLEAN AlreadySuspended
    );

#ifdef _M_X64
// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64GetThreadContext(
    __in HANDLE ThreadHandle,
    __inout PWOW64_CONTEXT ThreadContext
    );
#endif

#ifdef _M_X64
// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlWow64SetThreadContext(
    __in HANDLE ThreadHandle,
    __in PWOW64_CONTEXT ThreadContext
    );
#endif

// Images

NTSYSAPI
PVOID
NTAPI
RtlPcToFileHeader(
    __in PVOID PcValue,
    __out PVOID *BaseOfImage
    );

NTSYSAPI
PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader(
    __in PVOID Base
    );

#define RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK 0x00000001

NTSYSAPI
NTSTATUS
NTAPI
RtlImageNtHeaderEx(
    __in ULONG Flags,
    __in PVOID Base,
    __in ULONG64 Size,
    __out PIMAGE_NT_HEADERS *OutHeaders
    );

NTSYSAPI
PVOID
NTAPI
RtlAddressInSectionTable(
    __in PIMAGE_NT_HEADERS NtHeaders,
    __in PVOID BaseOfImage,
    __in ULONG VirtualAddress
    );

NTSYSAPI
PIMAGE_SECTION_HEADER
NTAPI
RtlSectionTableFromVirtualAddress(
    __in PIMAGE_NT_HEADERS NtHeaders,
    __in PVOID BaseOfImage,
    __in ULONG VirtualAddress
    );

NTSYSAPI
PVOID
NTAPI
RtlImageDirectoryEntryToData(
    __in PVOID BaseOfImage,
    __in BOOLEAN MappedAsImage,
    __in USHORT DirectoryEntry,
    __out PULONG Size
    );

NTSYSAPI
PIMAGE_SECTION_HEADER
NTAPI
RtlImageRvaToSection(
    __in PIMAGE_NT_HEADERS NtHeaders,
    __in PVOID Base,
    __in ULONG Rva
    );

NTSYSAPI
PVOID
NTAPI
RtlImageRvaToVa(
    __in PIMAGE_NT_HEADERS NtHeaders,
    __in PVOID Base,
    __in ULONG Rva,
    __inout_opt PIMAGE_SECTION_HEADER *LastRvaSection
    );

// Environment

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateEnvironment(
    __in BOOLEAN CloneCurrentEnvironment,
    __out PVOID *Environment
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDestroyEnvironment(
    __in PVOID Environment
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetCurrentEnvironment(
    __in PVOID Environment,
    __out_opt PVOID *PreviousEnvironment
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetEnvironmentVariable(
    __in_opt PVOID *Environment,
    __in PUNICODE_STRING Name,
    __in PUNICODE_STRING Value
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryEnvironmentVariable_U(
    __in PVOID Environment,
    __in PUNICODE_STRING Name,
    __out PUNICODE_STRING Value
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlExpandEnvironmentStrings_U(
    __in_opt PVOID Environment,
    __in PUNICODE_STRING Source,
    __out PUNICODE_STRING Destination,
    __out_opt PULONG ReturnedLength
    );

// Current directory and paths

typedef struct _RTLP_CURDIR_REF *PRTLP_CURDIR_REF;

typedef struct _RTL_RELATIVE_NAME_U
{
    UNICODE_STRING RelativeName;
    HANDLE ContainingDirectory;
    PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, *PRTL_RELATIVE_NAME_U;

typedef enum _RTL_PATH_TYPE
{
    RtlPathTypeUnknown,
    RtlPathTypeUncAbsolute,
    RtlPathTypeDriveAbsolute,
    RtlPathTypeDriveRelative,
    RtlPathTypeRooted,
    RtlPathTypeRelative,
    RtlPathTypeLocalDevice,
    RtlPathTypeRootLocalDevice
} RTL_PATH_TYPE;

NTSYSAPI
RTL_PATH_TYPE
NTAPI
RtlDetermineDosPathNameType_U(
    __in PWSTR DosFileName
    );

#if (PHNT_VERSION >= PHNT_WS03)
NTSYSAPI
RTL_PATH_TYPE
NTAPI
RtlDetermineDosPathNameType_Ustr(
    __in PUNICODE_STRING String
    );
#endif

NTSYSAPI
ULONG
NTAPI
RtlIsDosDeviceName_U(
    __in PWSTR DosFileName
    );

NTSYSAPI
ULONG
NTAPI
RtlGetFullPathName_U(
    __in PWSTR FileName,
    __in ULONG BufferLength,
    __out_bcount(BufferLength) PWSTR Buffer,
    __out_opt PWSTR *FilePart
    );

// missing:RtlGetFullPathName_UEx

#if (PHNT_VERSION >= PHNT_WS03)
NTSYSAPI
NTSTATUS
NTAPI
RtlGetFullPathName_UstrEx(
    __in PUNICODE_STRING FileName,
    __inout PUNICODE_STRING StaticString,
    __out_opt PUNICODE_STRING DynamicString,
    __out_opt PUNICODE_STRING *StringUsed,
    __out_opt SIZE_T *FilePartPrefixCch,
    __out_opt PBOOLEAN NameInvalid,
    __out RTL_PATH_TYPE *InputPathType,
    __out_opt SIZE_T *BytesRequired
    );
#endif

NTSYSAPI
ULONG
NTAPI
RtlGetCurrentDirectory_U(
    __in ULONG BufferLength,
    __out_bcount(BufferLength) PWSTR Buffer
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetCurrentDirectory_U(
    __in PUNICODE_STRING PathName
    );

NTSYSAPI
ULONG
NTAPI
RtlGetLongestNtPathLength();

NTSYSAPI
BOOLEAN
NTAPI
RtlDosPathNameToNtPathName_U(
    __in PWSTR DosFileName,
    __out PUNICODE_STRING NtFileName,
    __out_opt PWSTR *FilePart,
    __out_opt PRTL_RELATIVE_NAME_U RelativeName
    );

#if (PHNT_VERSION >= PHNT_WS03)
NTSYSAPI
NTSTATUS
NTAPI
RtlDosPathNameToNtPathName_U_WithStatus(
    __in PWSTR DosFileName,
    __out PUNICODE_STRING NtFileName,
    __out_opt PWSTR *FilePart,
    __out_opt PRTL_RELATIVE_NAME_U RelativeName
    );
#endif

#if (PHNT_VERSION >= PHNT_WS03)
NTSYSAPI
BOOLEAN
NTAPI
RtlDosPathNameToRelativeNtPathName_U(
    __in PWSTR DosFileName,
    __out PUNICODE_STRING NtFileName,
    __out_opt PWSTR *FilePart,
    __out_opt PRTL_RELATIVE_NAME_U RelativeName
    );
#endif

#if (PHNT_VERSION >= PHNT_WS03)
NTSYSAPI
NTSTATUS
NTAPI
RtlDosPathNameToRelativeNtPathName_U_WithStatus(
    __in PWSTR DosFileName,
    __out PUNICODE_STRING NtFileName,
    __out_opt PWSTR *FilePart,
    __out_opt PRTL_RELATIVE_NAME_U RelativeName
    );
#endif

#if (PHNT_VERSION >= PHNT_WS03)
NTSYSAPI
VOID
NTAPI
RtlReleaseRelativeName(
    __inout PRTL_RELATIVE_NAME_U RelativeName
    );
#endif

NTSYSAPI
ULONG
NTAPI
RtlDosSearchPath_U(
    __in PWSTR Path,
    __in PWSTR FileName,
    __in_opt PWSTR Extension,
    __in ULONG BufferLength,
    __out_bcount(BufferLength) PWSTR Buffer,
    __out_opt PWSTR *FilePart
    );

#define RTL_DOS_SEARCH_PATH_FLAG_APPLY_ISOLATION_REDIRECTION 0x00000001
#define RTL_DOS_SEARCH_PATH_FLAG_DISALLOW_DOT_RELATIVE_PATH_SEARCH 0x00000002
#define RTL_DOS_SEARCH_PATH_FLAG_APPLY_DEFAULT_EXTENSION_WHEN_NOT_RELATIVE_PATH_EVEN_IF_FILE_HAS_EXTENSION 0x00000004)

NTSYSAPI
NTSTATUS
NTAPI
RtlDosSearchPath_Ustr(
    __in ULONG Flags,
    __in PUNICODE_STRING Path,
    __in PUNICODE_STRING FileName,
    __in_opt PUNICODE_STRING DefaultExtension,
    __out_opt PUNICODE_STRING StaticString,
    __out_opt PUNICODE_STRING DynamicString,
    __out_opt PCUNICODE_STRING *FullFileNameOut,
    __out_opt SIZE_T *FilePartPrefixCch,
    __out_opt SIZE_T *BytesRequired
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlDoesFileExists_U(
    __in PWSTR FileName
    );

// Heaps

typedef struct _RTL_HEAP_ENTRY
{
    SIZE_T Size;
    USHORT Flags;
    USHORT AllocatorBackTraceIndex;
    union
    {
        struct
        {
            SIZE_T Settable;
            ULONG Tag;
        } s1;
        struct
        {
            SIZE_T CommittedSize;
            PVOID FirstBlock;
        } s2;
    } u;
} RTL_HEAP_ENTRY, *PRTL_HEAP_ENTRY;

#define RTL_HEAP_BUSY (USHORT)0x0001
#define RTL_HEAP_SEGMENT (USHORT)0x0002
#define RTL_HEAP_SETTABLE_VALUE (USHORT)0x0010
#define RTL_HEAP_SETTABLE_FLAG1 (USHORT)0x0020
#define RTL_HEAP_SETTABLE_FLAG2 (USHORT)0x0040
#define RTL_HEAP_SETTABLE_FLAG3 (USHORT)0x0080
#define RTL_HEAP_SETTABLE_FLAGS (USHORT)0x00e0
#define RTL_HEAP_UNCOMMITTED_RANGE (USHORT)0x0100
#define RTL_HEAP_PROTECTED_ENTRY (USHORT)0x0200

typedef struct _RTL_HEAP_TAG
{
    ULONG NumberOfAllocations;
    ULONG NumberOfFrees;
    SIZE_T BytesAllocated;
    USHORT TagIndex;
    USHORT CreatorBackTraceIndex;
    WCHAR TagName[24];
} RTL_HEAP_TAG, *PRTL_HEAP_TAG;

typedef struct _RTL_HEAP_INFORMATION
{
    PVOID BaseAddress;
    ULONG Flags;
    USHORT EntryOverhead;
    USHORT CreatorBackTraceIndex;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    ULONG NumberOfTags;
    ULONG NumberOfEntries;
    ULONG NumberOfPseudoTags;
    ULONG PseudoTagGranularity;
    ULONG Reserved[5];
    PRTL_HEAP_TAG Tags;
    PRTL_HEAP_ENTRY Entries;
} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

typedef struct _RTL_PROCESS_HEAPS
{
    ULONG NumberOfHeaps;
    RTL_HEAP_INFORMATION Heaps[1];
} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS;

typedef NTSTATUS (NTAPI *PRTL_HEAP_COMMIT_ROUTINE)(
    __in PVOID Base,
    __inout PVOID *CommitAddress,
    __inout PSIZE_T CommitSize
    );

typedef struct _RTL_HEAP_PARAMETERS
{
    ULONG Length;
    SIZE_T SegmentReserve;
    SIZE_T SegmentCommit;
    SIZE_T DeCommitFreeBlockThreshold;
    SIZE_T DeCommitTotalFreeThreshold;
    SIZE_T MaximumAllocationSize;
    SIZE_T VirtualMemoryThreshold;
    SIZE_T InitialCommit;
    SIZE_T InitialReserve;
    PRTL_HEAP_COMMIT_ROUTINE CommitRoutine;
    SIZE_T Reserved[2];
} RTL_HEAP_PARAMETERS, *PRTL_HEAP_PARAMETERS;

#define HEAP_SETTABLE_USER_VALUE 0x00000100
#define HEAP_SETTABLE_USER_FLAG1 0x00000200
#define HEAP_SETTABLE_USER_FLAG2 0x00000400
#define HEAP_SETTABLE_USER_FLAG3 0x00000800
#define HEAP_SETTABLE_USER_FLAGS 0x00000e00

#define HEAP_CLASS_0 0x00000000 // Process heap
#define HEAP_CLASS_1 0x00001000 // Private heap
#define HEAP_CLASS_2 0x00002000 // Kernel heap
#define HEAP_CLASS_3 0x00003000 // GDI heap
#define HEAP_CLASS_4 0x00004000 // User heap
#define HEAP_CLASS_5 0x00005000 // Console heap
#define HEAP_CLASS_6 0x00006000 // User desktop heap
#define HEAP_CLASS_7 0x00007000 // CSR shared heap
#define HEAP_CLASS_8 0x00008000 // CSR port heap
#define HEAP_CLASS_MASK 0x0000f000

NTSYSAPI
PVOID
NTAPI
RtlCreateHeap(
    __in ULONG Flags,
    __in_opt PVOID HeapBase,
    __in_opt SIZE_T ReserveSize,
    __in_opt SIZE_T CommitSize,
    __in_opt PVOID Lock,
    __in_opt PRTL_HEAP_PARAMETERS Parameters
    );

NTSYSAPI
PVOID
NTAPI
RtlDestroyHeap(
    __in __post_invalid PVOID HeapHandle
    );

NTSYSAPI
PVOID
NTAPI
RtlAllocateHeap(
    __in PVOID HeapHandle,
    __in_opt ULONG Flags,
    __in SIZE_T Size
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHeap(
    __in PVOID HeapHandle,
    __in_opt ULONG Flags,
    __in __post_invalid PVOID BaseAddress
    );

NTSYSAPI
SIZE_T
NTAPI
RtlSizeHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID BaseAddress
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlZeroHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags
    );

NTSYSAPI
VOID
NTAPI
RtlProtectHeap(
    __in PVOID HeapHandle,
    __in BOOLEAN MakeReadOnly
    );

#define RtlProcessHeap() (NtCurrentPeb()->ProcessHeap)

NTSYSAPI
BOOLEAN
NTAPI
RtlLockHeap(
    __in PVOID HeapHandle
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlUnlockHeap(
    __in PVOID HeapHandle
    );

NTSYSAPI
PVOID
NTAPI
RtlReAllocateHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID BaseAddress,
    __in SIZE_T Size
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlGetUserInfoHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID BaseAddress,
    __out_opt PVOID *UserValue,
    __out_opt PULONG UserFlags
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlSetUserValueHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID BaseAddress,
    __in PVOID UserValue
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlSetUserFlagsHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID BaseAddress,
    __in ULONG UserFlagsReset,
    __in ULONG UserFlagsSet
    );

typedef struct _RTL_HEAP_TAG_INFO
{
    ULONG NumberOfAllocations;
    ULONG NumberOfFrees;
    SIZE_T BytesAllocated;
} RTL_HEAP_TAG_INFO, *PRTL_HEAP_TAG_INFO;

#define RTL_HEAP_MAKE_TAG HEAP_MAKE_TAG_FLAGS

NTSYSAPI
ULONG
NTAPI
RtlCreateTagHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in_opt PWSTR TagPrefix,
    __in PWSTR TagNames
    );

NTSYSAPI
PWSTR
NTAPI
RtlQueryTagHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in USHORT TagIndex,
    __in BOOLEAN ResetCounters,
    __out_opt PRTL_HEAP_TAG_INFO TagInfo
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlExtendHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID Base,
    __in SIZE_T Size
    );

NTSYSAPI
SIZE_T
NTAPI
RtlCompactHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlValidateHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in PVOID BaseAddress
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlValidateProcessHeaps();

NTSYSAPI
ULONG
NTAPI
RtlGetProcessHeaps(
    __in ULONG NumberOfHeaps,
    __out PVOID *ProcessHeaps
    );

typedef NTSTATUS (NTAPI *PRTL_ENUM_HEAPS_ROUTINE)(
    __in PVOID HeapHandle,
    __in PVOID Parameter
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlEnumProcessHeaps(
    __in PRTL_ENUM_HEAPS_ROUTINE EnumRoutine,
    __in PVOID Parameter
    );

typedef struct _RTL_HEAP_USAGE_ENTRY
{
    struct _RTL_HEAP_USAGE_ENTRY *Next;
    PVOID Address;
    SIZE_T Size;
    USHORT AllocatorBackTraceIndex;
    USHORT TagIndex;
} RTL_HEAP_USAGE_ENTRY, *PRTL_HEAP_USAGE_ENTRY;

typedef struct _RTL_HEAP_USAGE
{
    ULONG Length;
    SIZE_T BytesAllocated;
    SIZE_T BytesCommitted;
    SIZE_T BytesReserved;
    SIZE_T BytesReservedMaximum;
    PRTL_HEAP_USAGE_ENTRY Entries;
    PRTL_HEAP_USAGE_ENTRY AddedEntries;
    PRTL_HEAP_USAGE_ENTRY RemovedEntries;
    ULONG_PTR Reserved[8];
} RTL_HEAP_USAGE, *PRTL_HEAP_USAGE;

#define HEAP_USAGE_ALLOCATED_BLOCKS HEAP_REALLOC_IN_PLACE_ONLY
#define HEAP_USAGE_FREE_BUFFER HEAP_ZERO_MEMORY

NTSYSAPI
NTSTATUS
NTAPI
RtlUsageHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __inout PRTL_HEAP_USAGE Usage
    );

typedef struct _RTL_HEAP_WALK_ENTRY
{
    PVOID DataAddress;
    SIZE_T DataSize;
    UCHAR OverheadBytes;
    UCHAR SegmentIndex;
    USHORT Flags;
    union
    {
        struct
        {
            SIZE_T Settable;
            USHORT TagIndex;
            USHORT AllocatorBackTraceIndex;
            ULONG Reserved[2];
        } Block;
        struct
        {
            ULONG CommittedSize;
            ULONG UnCommittedSize;
            PVOID FirstEntry;
            PVOID LastEntry;
        } Segment;
    };
} RTL_HEAP_WALK_ENTRY, *PRTL_HEAP_WALK_ENTRY;

NTSYSAPI
NTSTATUS
NTAPI
RtlWalkHeap(
    __in PVOID HeapHandle,
    __inout PRTL_HEAP_WALK_ENTRY Entry
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryHeapInformation(
    __in PVOID HeapHandle,
    __in HEAP_INFORMATION_CLASS HeapInformationClass,
    __out_opt PVOID HeapInformation,
    __in_opt SIZE_T HeapInformationLength,
    __out_opt PSIZE_T ReturnLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetHeapInformation(
    __in PVOID HeapHandle,
    __in HEAP_INFORMATION_CLASS HeapInformationClass,
    __in_opt PVOID HeapInformation,
    __in_opt SIZE_T HeapInformationLength
    );

NTSYSAPI
ULONG
NTAPI
RtlMultipleAllocateHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in SIZE_T Size,
    __in ULONG Count,
    __out PVOID *Array
    );

NTSYSAPI
ULONG
NTAPI
RtlMultipleFreeHeap(
    __in PVOID HeapHandle,
    __in ULONG Flags,
    __in ULONG Count,
    __in PVOID *Array
    );

// Transactions

#if (PHNT_VERSION >= PHNT_VISTA)
// rev
NTSYSAPI
HANDLE
NTAPI
RtlGetCurrentTransaction();
#endif

#if (PHNT_VERSION >= PHNT_VISTA)
// rev
NTSYSAPI
BOOLEAN
NTAPI
RtlSetCurrentTransaction(
    __in HANDLE TransactionHandle
    );
#endif

// LUIDs

FORCEINLINE BOOLEAN RtlIsEqualLuid(
    __in PLUID L1,
    __in PLUID L2
    )
{
    return L1->LowPart == L2->LowPart &&
        L1->HighPart == L2->HighPart;
}

FORCEINLINE BOOLEAN RtlIsZeroLuid(
    __in PLUID L1
    )
{
    return (L1->LowPart | L1->HighPart) == 0;
}

FORCEINLINE LUID RtlConvertLongToLuid(
    __in LONG Long
    )
{
    LUID tempLuid;
    LARGE_INTEGER tempLi;

    tempLi.QuadPart = Long;
    tempLuid.LowPart = tempLi.LowPart;
    tempLuid.HighPart = tempLi.HighPart;

    return tempLuid;
}

FORCEINLINE LUID RtlConvertUlongToLuid(
    __in ULONG Ulong
    )
{
    LUID tempLuid;

    tempLuid.LowPart = Ulong;
    tempLuid.HighPart = 0;

    return tempLuid;
}

// Debugging

typedef struct RTL_PROCESS_BACKTRACES *PRTL_PROCESS_BACKTRACES;
typedef struct RTL_PROCESS_LOCKS *PRTL_PROCESS_LOCKS;

typedef struct _RTL_DEBUG_INFORMATION
{
    HANDLE SectionHandleClient;
    PVOID ViewBaseClient;
    PVOID ViewBaseTarget;
    ULONG_PTR ViewBaseDelta;
    HANDLE EventPairClient;
    HANDLE EventPairTarget;
    HANDLE TargetProcessId;
    HANDLE TargetThreadHandle;
    ULONG Flags;
    SIZE_T OffsetFree;
    SIZE_T CommitSize;
    SIZE_T ViewSize;
    PRTL_PROCESS_MODULES Modules;
    PRTL_PROCESS_BACKTRACES BackTraces;
    PRTL_PROCESS_HEAPS Heaps;
    PRTL_PROCESS_LOCKS Locks;
    PVOID SpecificHeap;
    HANDLE TargetProcessHandle;
    PVOID Reserved[6];
} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

NTSYSCALLAPI
PRTL_DEBUG_INFORMATION
NTAPI
RtlCreateQueryDebugBuffer(
    __in_opt ULONG MaximumCommit,
    __in BOOLEAN UseEventPair
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlDestroyQueryDebugBuffer(
    __in PRTL_DEBUG_INFORMATION Buffer
    );

#define RTL_QUERY_PROCESS_MODULES 0x00000001
#define RTL_QUERY_PROCESS_BACKTRACES 0x00000002
#define RTL_QUERY_PROCESS_HEAP_SUMMARY 0x00000004
#define RTL_QUERY_PROCESS_HEAP_TAGS 0x00000008
#define RTL_QUERY_PROCESS_HEAP_ENTRIES 0x00000010
#define RTL_QUERY_PROCESS_LOCKS 0x00000020
#define RTL_QUERY_PROCESS_MODULES32 0x00000040
#define RTL_QUERY_PROCESS_NONINVASIVE 0x80000000

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlQueryProcessDebugInformation(
    __in HANDLE UniqueProcessId,
    __in ULONG Flags,
    __inout PRTL_DEBUG_INFORMATION Buffer
    );

// Messages

NTSYSCALLAPI
NTSTATUS
NTAPI
RtlFindMessage(
    __in PVOID DllHandle,
    __in ULONG MessageTableId,
    __in ULONG MessageLanguageId,
    __in ULONG MessageId,
    __out PMESSAGE_RESOURCE_ENTRY *MessageEntry
    );

// Errors

NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
    __in NTSTATUS Status
    );

NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosErrorNoTeb(
    __in NTSTATUS Status
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetLastNtStatus();

NTSYSAPI
LONG
NTAPI
RtlGetLastWin32Error();

NTSYSAPI
VOID
NTAPI
RtlSetLastWin32ErrorAndNtStatusFromNtStatus(
    __in NTSTATUS Status
    );

NTSYSAPI
VOID
NTAPI
RtlSetLastWin32Error(
    __in LONG Win32Error
    );

NTSYSAPI
VOID
NTAPI
RtlRestoreLastWin32Error(
    __in LONG Win32Error
    );

#define RTL_ERRORMODE_NOGPFAULTERRORBOX 0x0020
#define RTL_ERRORMODE_NOOPENFILEERRORBOX 0x0040

NTSYSAPI
ULONG
NTAPI
RtlGetThreadErrorMode();

NTSYSAPI
NTSTATUS
NTAPI
RtlSetThreadErrorMode(
    __in ULONG NewMode,
    __out_opt PULONG OldMode
    );

// Vectored Exception Handlers

NTSYSAPI
PVOID
NTAPI
RtlAddVectoredExceptionHandler(
    __in ULONG First,
    __in PVECTORED_EXCEPTION_HANDLER Handler
    );

NTSYSAPI
ULONG
NTAPI
RtlRemoveVectoredExceptionHandler(
    __in PVOID Handle
    );

NTSYSAPI
PVOID
NTAPI
RtlAddVectoredContinueHandler(
    __in ULONG First,
    __in PVECTORED_EXCEPTION_HANDLER Handler
    );

NTSYSAPI
ULONG
NTAPI
RtlRemoveVectoredContinueHandler(
    __in PVOID Handle
    );

// Random

NTSYSAPI
ULONG
NTAPI
RtlUniform(
    __inout PULONG Seed
    );

NTSYSAPI
ULONG
NTAPI
RtlRandom(
    __inout PULONG Seed
    );

NTSYSAPI
ULONG
NTAPI
RtlRandomEx(
    __inout PULONG Seed
    );

// IPv4/6 conversion

struct in_addr;
struct in6_addr;

NTSYSAPI
PWSTR
NTAPI
RtlIpv4AddressToStringW(
    __in struct in_addr *Addr,
    __out_ecount(16) PWSTR S
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4AddressToStringExW(
    __in struct in_addr *Address,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    __inout PULONG AddressStringLength
    );

NTSYSAPI
PWSTR
NTAPI
RtlIpv6AddressToStringW(
    __in struct in6_addr *Addr,
    __out_ecount(65) PWSTR S
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6AddressToStringExW(
    __in struct in6_addr *Address,
    __in ULONG ScopeId,
    __in USHORT Port,
    __out_ecount_part(*AddressStringLength, *AddressStringLength) PWSTR AddressString,
    __inout PULONG AddressStringLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressW(
    __in PWSTR S,
    __in BOOLEAN Strict,
    __out PWSTR *Terminator,
    __out struct in_addr *Addr
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv4StringToAddressExW(
    __in PWSTR AddressString,
    __in BOOLEAN Strict,
    __out struct in_addr *Address,
    __out PUSHORT Port
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressW(
    __in PWSTR S,
    __out PWSTR *Terminator,
    __out struct in6_addr *Addr
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlIpv6StringToAddressExW(
    __in PWSTR AddressString,
    __out struct in6_addr *Address,
    __out PULONG ScopeId,
    __out PUSHORT Port
    );

#define RtlIpv4AddressToString RtlIpv4AddressToStringW
#define RtlIpv4AddressToStringEx RtlIpv4AddressToStringExW
#define RtlIpv6AddressToString RtlIpv6AddressToStringW
#define RtlIpv6AddressToStringEx RtlIpv6AddressToStringExW
#define RtlIpv4StringToAddress RtlIpv4StringToAddressW
#define RtlIpv4StringToAddressEx RtlIpv4StringToAddressExW
#define RtlIpv6StringToAddress RtlIpv6StringToAddressW
#define RtlIpv6StringToAddressEx RtlIpv6StringToAddressExW

// Time

typedef struct _TIME_FIELDS
{
    CSHORT Year; // 1601...
    CSHORT Month; // 1..12
    CSHORT Day; // 1..31
    CSHORT Hour; // 0..23
    CSHORT Minute; // 0..59
    CSHORT Second; // 0..59
    CSHORT Milliseconds; // 0..999
    CSHORT Weekday; // 0..6 = Sunday..Saturday
} TIME_FIELDS, *PTIME_FIELDS;

NTSYSAPI
BOOLEAN
NTAPI
RtlCutoverTimeToSystemTime(
    __in PTIME_FIELDS CutoverTime,
    __out PLARGE_INTEGER SystemTime,
    __in PLARGE_INTEGER CurrentSystemTime,
    __in BOOLEAN ThisYear
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSystemTimeToLocalTime(
    __in PLARGE_INTEGER SystemTime,
    __out PLARGE_INTEGER LocalTime
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlLocalTimeToSystemTime(
    __in PLARGE_INTEGER LocalTime,
    __out PLARGE_INTEGER SystemTime
    );

NTSYSAPI
VOID
NTAPI
RtlTimeToElapsedTimeFields(
    __in PLARGE_INTEGER Time,
    __out PTIME_FIELDS TimeFields
    );

NTSYSAPI
VOID
NTAPI
RtlTimeToTimeFields(
    __in PLARGE_INTEGER Time,
    __out PTIME_FIELDS TimeFields
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeFieldsToTime(
    __in PTIME_FIELDS TimeFields, // Weekday is ignored
    __out PLARGE_INTEGER Time
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeToSecondsSince1980(
    __in PLARGE_INTEGER Time,
    __out PULONG ElapsedSeconds
    );

NTSYSAPI
VOID
NTAPI
RtlSecondsSince1980ToTime(
    __in ULONG ElapsedSeconds,
    __out PLARGE_INTEGER Time
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeToSecondsSince1970(
    __in PLARGE_INTEGER Time,
    __out PULONG ElapsedSeconds
    );

NTSYSAPI
VOID
NTAPI
RtlSecondsSince1970ToTime(
    __in ULONG ElapsedSeconds,
    __out PLARGE_INTEGER Time
    );

// Time zones

typedef struct _RTL_TIME_ZONE_INFORMATION
{
    LONG Bias;
    WCHAR StandardName[32];
    TIME_FIELDS StandardStart;
    LONG StandardBias;
    WCHAR DaylightName[32];
    TIME_FIELDS DaylightStart;
    LONG DaylightBias;
} RTL_TIME_ZONE_INFORMATION, *PRTL_TIME_ZONE_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryTimeZoneInformation(
    __out PRTL_TIME_ZONE_INFORMATION TimeZoneInformation
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetTimeZoneInformation(
    __in PRTL_TIME_ZONE_INFORMATION TimeZoneInformation
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetActiveTimeBias(
    __in LONG ActiveBias
    );

// Bitmaps

typedef struct _RTL_BITMAP
{
    ULONG SizeOfBitMap;
    PULONG Buffer;
} RTL_BITMAP, *PRTL_BITMAP;

NTSYSAPI
VOID
NTAPI
RtlInitializeBitMap(
    __out PRTL_BITMAP BitMapHeader,
    __in PULONG BitMapBuffer,
    __in ULONG SizeOfBitMap
    );

NTSYSAPI
VOID
NTAPI
RtlClearBit(
    __in PRTL_BITMAP BitMapHeader,
    __in_range(<, BitMapHeader->SizeOfBitMap) ULONG BitNumber
    );

NTSYSAPI
VOID
NTAPI
RtlSetBit(
    __in PRTL_BITMAP BitMapHeader,
    __in_range(<, BitMapHeader->SizeOfBitMap) ULONG BitNumber
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlTestBit(
    __in PRTL_BITMAP BitMapHeader,
    __in_range(<, BitMapHeader->SizeOfBitMap) ULONG BitNumber
    );

NTSYSAPI
VOID
NTAPI
RtlClearAllBits(
    __in PRTL_BITMAP BitMapHeader
    );

NTSYSAPI
VOID
NTAPI
RtlSetAllBits(
    __in PRTL_BITMAP BitMapHeader
    );

__success(return != -1)
__checkReturn
NTSYSAPI
ULONG
NTAPI
RtlFindClearBits(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG NumberToFind,
    __in ULONG HintIndex
    );

__success(return != -1)
__checkReturn
NTSYSAPI
ULONG
NTAPI
RtlFindSetBits(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG NumberToFind,
    __in ULONG HintIndex
    );

__success(return != -1)
NTSYSAPI
ULONG
NTAPI
RtlFindClearBitsAndSet(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG NumberToFind,
    __in ULONG HintIndex
    );

__success(return != -1)
NTSYSAPI
ULONG
NTAPI
RtlFindSetBitsAndClear(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG NumberToFind,
    __in ULONG HintIndex
    );

NTSYSAPI
VOID
NTAPI
RtlClearBits(
    __in PRTL_BITMAP BitMapHeader,
    __in_range(0, BitMapHeader->SizeOfBitMap - NumberToClear) ULONG StartingIndex,
    __in_range(0, BitMapHeader->SizeOfBitMap - StartingIndex) ULONG NumberToClear
    );

NTSYSAPI
VOID
NTAPI
RtlSetBits(
    __in PRTL_BITMAP BitMapHeader,
    __in_range(0, BitMapHeader->SizeOfBitMap - NumberToSet) ULONG StartingIndex,
    __in_range(0, BitMapHeader->SizeOfBitMap - StartingIndex) ULONG NumberToSet
    );

typedef struct _RTL_BITMAP_RUN
{
    ULONG StartingIndex;
    ULONG NumberOfBits;
} RTL_BITMAP_RUN, *PRTL_BITMAP_RUN;

NTSYSAPI
ULONG
NTAPI
RtlFindClearRuns(
    __in PRTL_BITMAP BitMapHeader,
    __out_ecount_part(SizeOfRunArray, return) PRTL_BITMAP_RUN RunArray,
    __in_range(>, 0) ULONG SizeOfRunArray,
    __in BOOLEAN LocateLongestRuns
    );

NTSYSAPI
ULONG
NTAPI
RtlFindLongestRunClear(
    __in PRTL_BITMAP BitMapHeader,
    __out PULONG StartingIndex
    );

NTSYSAPI
ULONG
NTAPI
RtlFindFirstRunClear(
    __in PRTL_BITMAP BitMapHeader,
    __out PULONG StartingIndex
    );

__checkReturn
FORCEINLINE
BOOLEAN
RtlCheckBit(
    __in PRTL_BITMAP BitMapHeader,
    __in_range(<, BitMapHeader->SizeOfBitMap) ULONG BitPosition
    )
{
#ifdef _M_IX86
    return (((PLONG)BitMapHeader->Buffer)[BitPosition / 32] >> (BitPosition % 32)) & 0x1;
#else
    return BitTest64((LONG64 const *)BitMapHeader->Buffer, (LONG64)BitPosition);
#endif
}

NTSYSAPI
ULONG
NTAPI
RtlNumberOfClearBits(
    __in PRTL_BITMAP BitMapHeader
    );

NTSYSAPI
ULONG
NTAPI
RtlNumberOfSetBits(
    __in PRTL_BITMAP BitMapHeader
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlAreBitsClear(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG StartingIndex,
    __in ULONG Length
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlAreBitsSet(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG StartingIndex,
    __in ULONG Length
    );

NTSYSAPI
ULONG
NTAPI
RtlFindNextForwardRunClear(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG FromIndex,
    __out PULONG StartingRunIndex
    );

NTSYSAPI
ULONG
NTAPI
RtlFindLastBackwardRunClear(
    __in PRTL_BITMAP BitMapHeader,
    __in ULONG FromIndex,
    __out PULONG StartingRunIndex
    );

// Handle tables

typedef struct _RTL_HANDLE_TABLE_ENTRY
{
    union
    {
        ULONG Flags; // allocated entries have the low bit set
        struct _RTL_HANDLE_TABLE_ENTRY *NextFree;
    };
} RTL_HANDLE_TABLE_ENTRY, *PRTL_HANDLE_TABLE_ENTRY;

#define RTL_HANDLE_ALLOCATED (USHORT)0x0001

typedef struct _RTL_HANDLE_TABLE
{
    ULONG MaximumNumberOfHandles;
    ULONG SizeOfHandleTableEntry;
    ULONG Reserved[2];
    PRTL_HANDLE_TABLE_ENTRY FreeHandles;
    PRTL_HANDLE_TABLE_ENTRY CommittedHandles;
    PRTL_HANDLE_TABLE_ENTRY UnCommittedHandles;
    PRTL_HANDLE_TABLE_ENTRY MaxReservedHandles;
} RTL_HANDLE_TABLE, *PRTL_HANDLE_TABLE;

NTSYSAPI
VOID
NTAPI
RtlInitializeHandleTable(
    __in ULONG MaximumNumberOfHandles,
    __in ULONG SizeOfHandleTableEntry,
    __out PRTL_HANDLE_TABLE HandleTable
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDestroyHandleTable(
    __inout PRTL_HANDLE_TABLE HandleTable
    );

NTSYSAPI
PRTL_HANDLE_TABLE_ENTRY
NTAPI
RtlAllocateHandle(
    __in PRTL_HANDLE_TABLE HandleTable,
    __out_opt PULONG HandleIndex
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHandle(
    __in PRTL_HANDLE_TABLE HandleTable,
    __in PRTL_HANDLE_TABLE_ENTRY Handle
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlIsValidHandle(
    __in PRTL_HANDLE_TABLE HandleTable,
    __in PRTL_HANDLE_TABLE_ENTRY Handle
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlIsValidIndexHandle(
    __in PRTL_HANDLE_TABLE HandleTable,
    __in ULONG HandleIndex,
    __out PRTL_HANDLE_TABLE_ENTRY *Handle
    );

// Atom tables

#define RTL_ATOM_MAXIMUM_INTEGER_ATOM (RTL_ATOM)0xc000
#define RTL_ATOM_INVALID_ATOM (RTL_ATOM)0x0000
#define RTL_ATOM_TABLE_DEFAULT_NUMBER_OF_BUCKETS 37
#define RTL_ATOM_MAXIMUM_NAME_LENGTH 255
#define RTL_ATOM_PINNED 0x01

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateAtomTable(
    __in ULONG NumberOfBuckets,
    __out PVOID *AtomTableHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDestroyAtomTable(
    __in __post_invalid PVOID AtomTableHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlEmptyAtomTable(
    __in PVOID AtomTableHandle,
    __in BOOLEAN IncludePinnedAtoms
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAtomToAtomTable(
    __in PVOID AtomTableHandle,
    __in PWSTR AtomName,
    __inout_opt PRTL_ATOM Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlLookupAtomInAtomTable(
    __in PVOID AtomTableHandle,
    __in PWSTR AtomName,
    __out_opt PRTL_ATOM Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteAtomFromAtomTable(
    __in PVOID AtomTableHandle,
    __in RTL_ATOM Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlPinAtomInAtomTable(
    __in PVOID AtomTableHandle,
    __in RTL_ATOM Atom
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryAtomInAtomTable(
    __in PVOID AtomTableHandle,
    __in RTL_ATOM Atom,
    __out_opt PULONG AtomUsage,
    __out_opt PULONG AtomFlags,
    __inout_bcount_part_opt(*AtomNameLength, *AtomNameLength) PWSTR AtomName,
    __inout_opt PULONG AtomNameLength
    );

// SIDs

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlValidSid(
    __in PSID Sid
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlEqualSid(
    __in PSID Sid1,
    __in PSID Sid2
    );

NTSYSAPI
ULONG
NTAPI
RtlLengthRequiredSid(
    __in ULONG SubAuthorityCount
    );

NTSYSAPI
PVOID
NTAPI
RtlFreeSid(
    __in __post_invalid PSID Sid
    );

__checkReturn
NTSYSAPI
NTSTATUS
NTAPI
RtlAllocateAndInitializeSid(
    __in PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    __in UCHAR SubAuthorityCount,
    __in ULONG SubAuthority0,
    __in ULONG SubAuthority1,
    __in ULONG SubAuthority2,
    __in ULONG SubAuthority3,
    __in ULONG SubAuthority4,
    __in ULONG SubAuthority5,
    __in ULONG SubAuthority6,
    __in ULONG SubAuthority7,
    __deref_out PSID *Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlInitializeSid(
    __out PSID Sid,
    __in PSID_IDENTIFIER_AUTHORITY IdentifierAuthority,
    __in UCHAR SubAuthorityCount
    );

NTSYSAPI
PSID_IDENTIFIER_AUTHORITY
NTAPI
RtlIdentifierAuthoritySid(
    __in PSID Sid
    );

NTSYSAPI
PULONG
NTAPI
RtlSubAuthoritySid(
    __in PSID Sid,
    __in ULONG SubAuthority
    );

NTSYSAPI
PUCHAR
NTAPI
RtlSubAuthorityCountSid(
    __in PSID Sid
    );

NTSYSAPI
ULONG
NTAPI
RtlLengthSid(
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCopySid(
    __in ULONG DestinationSidLength,
    __in_bcount(DestinationSidLength) PSID DestinationSid,
    __in PSID SourceSid
    );

#if (PHNT_VERSION >= PHNT_VISTA)
NTSYSAPI
NTSTATUS
NTAPI
RtlCreateServiceSid(
    __in PUNICODE_STRING ServiceName,
    __out_bcount(*ServiceSidLength) PSID ServiceSid,
    __inout PULONG ServiceSidLength
    );
#endif

#if (PHNT_VERSION >= PHNT_VISTA)
// rev unsure
NTSYSAPI
NTSTATUS
NTAPI
RtlSidDominates(
    __in PSID Sid1,
    __in PSID Sid2,
    __out PBOOLEAN Result
    );
#endif

#if (PHNT_VERSION >= PHNT_VISTA)
// rev unsure
NTSYSAPI
NTSTATUS
NTAPI
RtlSidEqualLevel(
    __in PSID Sid1,
    __in PSID Sid2,
    __out PBOOLEAN Result
    );
#endif

#if (PHNT_VERSION >= PHNT_VISTA)
// rev unsure
NTSYSAPI
NTSTATUS
NTAPI
RtlSidIsHigherLevel(
    __in PSID Sid1,
    __in PSID Sid2,
    __out PBOOLEAN Result
    );
#endif

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSAPI
NTSTATUS
NTAPI
RtlCreateVirtualAccountSid(
    __in PUNICODE_STRING Name,
    __in ULONG BaseSubAuthority,
    __out_bcount(*SidLength) PSID Sid,
    __inout PULONG SidLength
    );
#endif

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSAPI
NTSTATUS
NTAPI
RtlReplaceSidInSd(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in PSID OldSid,
    __in PSID NewSid,
    __out ULONG *NumChanges
    );
#endif

#define MAX_UNICODE_STACK_BUFFER_LENGTH 256

NTSYSAPI
NTSTATUS
NTAPI
RtlConvertSidToUnicodeString(
    __inout PUNICODE_STRING UnicodeString,
    __in PSID Sid,
    __in BOOLEAN AllocateDestinationString
    );

// LUIDs

#define RtlEqualLuid(L1, L2) \
    (((L1)->LowPart == (L2)->LowPart) && \
    ((L1)->HighPart == (L2)->HighPart))

#define RtlIsZeroLuid(L1) ((BOOLEAN)(((L1)->LowPart | (L1)->HighPart) == 0))

NTSYSAPI
VOID
NTAPI
RtlCopyLuid(
    __out PLUID DestinationLuid,
    __in PLUID SourceLuid
    );

// Security Descriptors

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor(
    __out PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in ULONG Revision
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlValidSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor
    );

NTSYSAPI
ULONG
NTAPI
RtlLengthSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor
    );

__checkReturn
NTSYSAPI
BOOLEAN
NTAPI
RtlValidRelativeSecurityDescriptor(
    __in_bcount(SecurityDescriptorLength) PSECURITY_DESCRIPTOR SecurityDescriptorInput,
    __in ULONG SecurityDescriptorLength,
    __in SECURITY_INFORMATION RequiredInformation
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetControlSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PSECURITY_DESCRIPTOR_CONTROL Control,
    __out PULONG Revision
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetControlSecurityDescriptor(
     __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
     __in SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
     __in SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet
     );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetAttributesSecurityDescriptor(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in SECURITY_DESCRIPTOR_CONTROL Control,
    __out PULONG Revision
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlGetSecurityDescriptorRMControl(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PUCHAR RMControl
    );

NTSYSAPI
VOID
NTAPI
RtlSetSecurityDescriptorRMControl(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PUCHAR RMControl
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in BOOLEAN DaclPresent,
    __in_opt PACL Dacl,
    __in_opt BOOLEAN DaclDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetDaclSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PBOOLEAN DaclPresent,
    __out PACL *Dacl,
    __out PBOOLEAN DaclDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetSaclSecurityDescriptor(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in BOOLEAN SaclPresent,
    __in_opt PACL Sacl,
    __in_opt BOOLEAN SaclDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetSaclSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PBOOLEAN SaclPresent,
    __out PACL *Sacl,
    __out PBOOLEAN SaclDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetSaclSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PBOOLEAN SaclPresent,
    __out PACL *Sacl,
    __out PBOOLEAN SaclDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetOwnerSecurityDescriptor(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID Owner,
    __in_opt BOOLEAN OwnerDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetOwnerSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PSID *Owner,
    __out PBOOLEAN OwnerDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetGroupSecurityDescriptor(
    __inout PSECURITY_DESCRIPTOR SecurityDescriptor,
    __in_opt PSID Group,
    __in_opt BOOLEAN GroupDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetGroupSecurityDescriptor(
    __in PSECURITY_DESCRIPTOR SecurityDescriptor,
    __out PSID *Group,
    __out PBOOLEAN GroupDefaulted
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlMakeSelfRelativeSD(
    __in PSECURITY_DESCRIPTOR AbsoluteSecurityDescriptor,
    __out_bcount(*BufferLength) PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    __inout PULONG BufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAbsoluteToSelfRelativeSD(
    __in PSECURITY_DESCRIPTOR AbsoluteSecurityDescriptor,
    __out_bcount_part_opt(*BufferLength, *BufferLength) PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    __inout PULONG BufferLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSelfRelativeToAbsoluteSD(
    __in PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    __out_bcount_part_opt(*AbsoluteSecurityDescriptorSize, *AbsoluteSecurityDescriptorSize) PSECURITY_DESCRIPTOR AbsoluteSecurityDescriptor,
    __inout PULONG AbsoluteSecurityDescriptorSize,
    __out_bcount_part_opt(*DaclSize, *DaclSize) PACL Dacl,
    __inout PULONG DaclSize,
    __out_bcount_part_opt(*SaclSize, *SaclSize) PACL Sacl,
    __inout PULONG SaclSize,
    __out_bcount_part_opt(*OwnerSize, *OwnerSize) PSID Owner,
    __inout PULONG OwnerSize,
    __out_bcount_part_opt(*PrimaryGroupSize, *PrimaryGroupSize) PSID PrimaryGroup,
    __inout PULONG PrimaryGroupSize
    );

// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlSelfRelativeToAbsoluteSD2(
    __inout PSECURITY_DESCRIPTOR SelfRelativeSecurityDescriptor,
    __inout PULONG BufferLength
    );

// Access masks

NTSYSAPI
BOOLEAN
NTAPI
RtlAreAllAccessesGranted(
    __in ACCESS_MASK GrantedAccess,
    __in ACCESS_MASK DesiredAccess
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlAreAnyAccessesGranted(
    __in ACCESS_MASK GrantedAccess,
    __in ACCESS_MASK DesiredAccess
    );

NTSYSAPI
VOID
NTAPI
RtlMapGenericMask(
    __inout PACCESS_MASK AccessMask,
    __in PGENERIC_MAPPING GenericMapping
    );

// ACLs

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateAcl(
    __out_bcount(AclLength) PACL Acl,
    __in ULONG AclLength,
    __in ULONG AclRevision
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlValidAcl(
    __in PACL Acl
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryInformationAcl(
    __in PACL Acl,
    __out_bcount(AclInformationLength) PVOID AclInformation,
    __in ULONG AclInformationLength,
    __in ACL_INFORMATION_CLASS AclInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetInformationAcl(
    __inout PACL Acl,
    __in_bcount(AclInformationLength) PVOID AclInformation,
    __in ULONG AclInformationLength,
    __in ACL_INFORMATION_CLASS AclInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG StartingAceIndex,
    __in_bcount(AceListLength) PVOID AceList,
    __in ULONG AceListLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteAce(
    __inout PACL Acl,
    __in ULONG AceIndex
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlGetAce(
    __in PACL Acl,
    __in ULONG AceIndex,
    __deref_out PVOID *Ace
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlFirstFreeAce(
    __in PACL Acl,
    __out PVOID *FirstFree
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessAllowedAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ACCESS_MASK AccessMask,
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessAllowedAceEx(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ACCESS_MASK AccessMask,
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessDeniedAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ACCESS_MASK AccessMask,
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessDeniedAceEx(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ACCESS_MASK AccessMask,
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAuditAccessAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ACCESS_MASK AccessMask,
    __in PSID Sid,
    __in BOOLEAN AuditSuccess,
    __in BOOLEAN AuditFailure
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAuditAccessAceEx(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ACCESS_MASK AccessMask,
    __in PSID Sid,
    __in BOOLEAN AuditSuccess,
    __in BOOLEAN AuditFailure
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessAllowedObjectAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ACCESS_MASK AccessMask,
    __in_opt PGUID ObjectTypeGuid,
    __in_opt PGUID InheritedObjectTypeGuid,
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAccessDeniedObjectAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ACCESS_MASK AccessMask,
    __in_opt PGUID ObjectTypeGuid,
    __in_opt PGUID InheritedObjectTypeGuid,
    __in PSID Sid
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddAuditAccessObjectAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ACCESS_MASK AccessMask,
    __in_opt PGUID ObjectTypeGuid,
    __in_opt PGUID InheritedObjectTypeGuid,
    __in PSID Sid,
    __in BOOLEAN AuditSuccess,
    __in BOOLEAN AuditFailure
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddCompoundAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in UCHAR AceType,
    __in ACCESS_MASK AccessMask,
    __in PSID ServerSid,
    __in PSID ClientSid
    );

#if (PHNT_VERSION >= PHNT_VISTA)
// rev
NTSYSAPI
NTSTATUS
NTAPI
RtlAddMandatoryAce(
    __inout PACL Acl,
    __in ULONG AceRevision,
    __in ULONG AceFlags,
    __in ULONG MandatoryPolicy,
    __in UCHAR AceType,
    __in PSID LabelSid
    );
#endif

// Security objects

NTSYSAPI
NTSTATUS
NTAPI
RtlNewSecurityObject(
    __in_opt PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt PSECURITY_DESCRIPTOR CreatorDescriptor,
    __out PSECURITY_DESCRIPTOR *NewDescriptor,
    __in BOOLEAN IsDirectoryObject,
    __in_opt HANDLE Token,
    __in PGENERIC_MAPPING GenericMapping
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlNewSecurityObjectEx(
    __in_opt PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt PSECURITY_DESCRIPTOR CreatorDescriptor,
    __out PSECURITY_DESCRIPTOR *NewDescriptor,
    __in_opt GUID *ObjectType,
    __in BOOLEAN IsDirectoryObject,
    __in ULONG AutoInheritFlags, // SEF_*
    __in_opt HANDLE Token,
    __in PGENERIC_MAPPING GenericMapping
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlNewSecurityObjectWithMultipleInheritance(
    __in_opt PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt PSECURITY_DESCRIPTOR CreatorDescriptor,
    __out PSECURITY_DESCRIPTOR *NewDescriptor,
    __in_opt GUID **ObjectType,
    __in ULONG GuidCount,
    __in BOOLEAN IsDirectoryObject,
    __in ULONG AutoInheritFlags, // SEF_*
    __in_opt HANDLE Token,
    __in PGENERIC_MAPPING GenericMapping
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteSecurityObject(
    __inout PSECURITY_DESCRIPTOR *ObjectDescriptor
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQuerySecurityObject(
     __in PSECURITY_DESCRIPTOR ObjectDescriptor,
     __in SECURITY_INFORMATION SecurityInformation,
     __out_opt PSECURITY_DESCRIPTOR ResultantDescriptor,
     __in ULONG DescriptorLength,
     __out PULONG ReturnLength
     );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetSecurityObject(
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR ModificationDescriptor,
    __inout PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    __in PGENERIC_MAPPING GenericMapping,
    __in_opt HANDLE Token
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetSecurityObjectEx(
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR ModificationDescriptor,
    __inout PSECURITY_DESCRIPTOR *ObjectsSecurityDescriptor,
    __in ULONG AutoInheritFlags, // SEF_*
    __in PGENERIC_MAPPING GenericMapping,
    __in_opt HANDLE Token
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlConvertToAutoInheritSecurityObject(
    __in_opt PSECURITY_DESCRIPTOR ParentDescriptor,
    __in PSECURITY_DESCRIPTOR CurrentSecurityDescriptor,
    __out PSECURITY_DESCRIPTOR *NewSecurityDescriptor,
    __in_opt GUID *ObjectType,
    __in BOOLEAN IsDirectoryObject,
    __in PGENERIC_MAPPING GenericMapping
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlNewInstanceSecurityObject(
    __in BOOLEAN ParentDescriptorChanged,
    __in BOOLEAN CreatorDescriptorChanged,
    __in PLUID OldClientTokenModifiedId,
    __out PLUID NewClientTokenModifiedId,
    __in_opt PSECURITY_DESCRIPTOR ParentDescriptor,
    __in_opt PSECURITY_DESCRIPTOR CreatorDescriptor,
    __out PSECURITY_DESCRIPTOR *NewDescriptor,
    __in BOOLEAN IsDirectoryObject,
    __in HANDLE Token,
    __in PGENERIC_MAPPING GenericMapping
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCopySecurityDescriptor(
    __in PSECURITY_DESCRIPTOR InputSecurityDescriptor,
    __out PSECURITY_DESCRIPTOR *OutputSecurityDescriptor
    );

// Misc. security

NTSYSAPI
VOID
NTAPI
RtlRunEncodeUnicodeString(
    __inout PUCHAR Seed,
    __in PUNICODE_STRING String
    );

NTSYSAPI
VOID
NTAPI
RtlRunDecodeUnicodeString(
    __in UCHAR Seed,
    __in PUNICODE_STRING String
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlImpersonateSelf(
    __in SECURITY_IMPERSONATION_LEVEL ImpersonationLevel
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAdjustPrivilege(
    __in ULONG Privilege,
    __in BOOLEAN Enable,
    __in BOOLEAN Client,
    __out PBOOLEAN WasEnabled
    );

#define RTL_ACQUIRE_PRIVILEGE_REVERT 0x00000001
#define RTL_ACQUIRE_PRIVILEGE_PROCESS 0x00000002

NTSYSAPI
NTSTATUS
NTAPI
RtlAcquirePrivilege(
    __in PULONG Privilege,
    __in ULONG NumPriv,
    __in ULONG Flags,
    __out PVOID *ReturnedState
    );

NTSYSAPI
VOID
NTAPI
RtlReleasePrivilege(
    __in PVOID StatePointer
    );

// Private namespaces

#if (PHNT_VERSION >= PHNT_VISTA)

// begin_rev

NTSYSAPI
PVOID
NTAPI
RtlCreateBoundaryDescriptor(
    __in PUNICODE_STRING Name,
    __in ULONG Flags
    );

NTSYSAPI
VOID
NTAPI
RtlDeleteBoundaryDescriptor(
    __in PVOID BoundaryDescriptor
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlAddSIDToBoundaryDescriptor(
    __inout PVOID *BoundaryDescriptor,
    __in PSID RequiredSid
    );

#if (PHNT_VERSION >= PHNT_WIN7)
NTSYSAPI
NTSTATUS
NTAPI
RtlAddIntegrityLabelToBoundaryDescriptor(
    __inout PVOID *BoundaryDescriptor,
    __in PSID IntegrityLabel
    );
#endif

// end_rev

#endif

// Version

NTSYSAPI
NTSTATUS
NTAPI
RtlGetVersion(
    __out PRTL_OSVERSIONINFOW lpVersionInformation
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlVerifyVersionInfo(
    __in PRTL_OSVERSIONINFOEXW VersionInfo,
    __in ULONG TypeMask,
    __in ULONGLONG ConditionMask
    );

// Thread pool (old)

NTSYSAPI
NTSTATUS
NTAPI
RtlRegisterWait(
    __out PHANDLE WaitHandle,
    __in HANDLE Handle,
    __in WAITORTIMERCALLBACKFUNC Function,
    __in PVOID Context,
    __in ULONG Milliseconds,
    __in ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeregisterWait(
    __in HANDLE WaitHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeregisterWaitEx(
    __in HANDLE WaitHandle,
    __in HANDLE Event
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlQueueWorkItem(
    __in WORKERCALLBACKFUNC Function,
    __in PVOID Context,
    __in ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlSetIoCompletionCallback(
    __in HANDLE FileHandle,
    __in APC_CALLBACK_FUNCTION CompletionProc,
    __in ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateTimerQueue(
    __out PHANDLE TimerQueueHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateTimer(
    __in HANDLE TimerQueueHandle,
    __out PHANDLE Handle,
    __in WAITORTIMERCALLBACKFUNC Function,
    __in PVOID Context,
    __in ULONG DueTime,
    __in ULONG Period,
    __in ULONG Flags
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlUpdateTimer(
    __in HANDLE TimerQueueHandle,
    __in HANDLE TimerHandle,
    __in ULONG DueTime,
    __in ULONG Period
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteTimer(
    __in HANDLE TimerQueueHandle,
    __in HANDLE TimerToCancel,
    __in HANDLE Event
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteTimerQueue(
    __in HANDLE TimerQueueHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteTimerQueueEx(
    __in HANDLE TimerQueueHandle,
    __in HANDLE Event
    );

// Registry access

// begin_wdm

#define RTL_REGISTRY_ABSOLUTE 0
#define RTL_REGISTRY_SERVICES 1 // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL 2 // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT 3 // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP 4 // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER 5 // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM 6
#define RTL_REGISTRY_HANDLE 0x40000000
#define RTL_REGISTRY_OPTIONAL 0x80000000

// end_wdm

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateRegistryKey(
    __in ULONG RelativeTo,
    __in PWSTR Path
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckRegistryKey(
    __in ULONG RelativeTo,
    __in PWSTR Path
    );

// begin_wdm

typedef NTSTATUS (NTAPI *PRTL_QUERY_REGISTRY_ROUTINE)(
    __in PWSTR ValueName,
    __in ULONG ValueType,
    __in PVOID ValueData,
    __in ULONG ValueLength,
    __in PVOID Context,
    __in PVOID EntryContext
    );

typedef struct _RTL_QUERY_REGISTRY_TABLE
{
    PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
    ULONG Flags;
    PWSTR Name;
    PVOID EntryContext;
    ULONG DefaultType;
    PVOID DefaultData;
    ULONG DefaultLength;
} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;

#define RTL_QUERY_REGISTRY_SUBKEY 0x00000001
#define RTL_QUERY_REGISTRY_TOPKEY 0x00000002
#define RTL_QUERY_REGISTRY_REQUIRED 0x00000004
#define RTL_QUERY_REGISTRY_NOVALUE 0x00000008
#define RTL_QUERY_REGISTRY_NOEXPAND 0x00000010
#define RTL_QUERY_REGISTRY_DIRECT 0x00000020
#define RTL_QUERY_REGISTRY_DELETE 0x00000040

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryRegistryValues(
    __in ULONG RelativeTo,
    __in PWSTR Path,
    __in PRTL_QUERY_REGISTRY_TABLE QueryTable,
    __in PVOID Context,
    __in_opt PVOID Environment
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlWriteRegistryValue(
    __in ULONG RelativeTo,
    __in PWSTR Path,
    __in PWSTR ValueName,
    __in ULONG ValueType,
    __in PVOID ValueData,
    __in ULONG ValueLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteRegistryValue(
    __in ULONG RelativeTo,
    __in PWSTR Path,
    __in PWSTR ValueName
    );

// end_wdm

// Debugging

NTSYSAPI
VOID
NTAPI
DbgUserBreakPoint();

// begin_wdm

NTSYSAPI
VOID
NTAPI
DbgBreakPoint();

NTSYSAPI
VOID
NTAPI
DbgBreakPointWithStatus(
    __in ULONG Status
    );

#define DBG_STATUS_CONTROL_C 1
#define DBG_STATUS_SYSRQ 2
#define DBG_STATUS_BUGCHECK_FIRST 3
#define DBG_STATUS_BUGCHECK_SECOND 4
#define DBG_STATUS_FATAL 5
#define DBG_STATUS_DEBUG_CONTROL 6
#define DBG_STATUS_WORKER 7

NTSYSAPI
ULONG
__cdecl
DbgPrint(
    __in_z __format_string PSTR Format,
    ...
    );

NTSYSAPI
ULONG
__cdecl
DbgPrintEx(
    __in ULONG ComponentId,
    __in ULONG Level,
    __in_z __format_string PSTR Format,
    ...
    );

NTSYSAPI
ULONG
NTAPI
vDbgPrintEx(
    __in ULONG ComponentId,
    __in ULONG Level,
    __in_z PCH Format,
    __in va_list arglist
    );

NTSYSAPI
ULONG
NTAPI
vDbgPrintExWithPrefix(
    __in_z PCH Prefix,
    __in ULONG ComponentId,
    __in ULONG Level,
    __in_z PCH Format,
    __in va_list arglist
    );

NTSYSAPI
NTSTATUS
NTAPI
DbgQueryDebugFilterState(
    __in ULONG ComponentId,
    __in ULONG Level
    );

NTSYSAPI
NTSTATUS
NTAPI
DbgSetDebugFilterState(
    __in ULONG ComponentId,
    __in ULONG Level,
    __in BOOLEAN State
    );

// end_wdm

NTSYSAPI
ULONG
NTAPI
DbgPrompt(
    __in PCH Prompt,
    __out_bcount(Length) PCH Response,
    __in ULONG Length
    );

// Misc.

NTSYSAPI
ULONG
NTAPI
RtlGetCurrentProcessorNumber();

NTSYSAPI
ULONG32
NTAPI
RtlComputeCrc32(
    __in ULONG32 PartialCrc,
    __in PVOID Buffer,
    __in ULONG Length
    );

NTSYSAPI
PVOID
NTAPI
RtlEncodePointer(
    __in PVOID Ptr
    );

NTSYSAPI
PVOID
NTAPI
RtlDecodePointer(
    __in PVOID Ptr
    );

NTSYSAPI
PVOID
NTAPI
RtlEncodeSystemPointer(
    __in PVOID Ptr
    );

NTSYSAPI
PVOID
NTAPI
RtlDecodeSystemPointer(
    __in PVOID Ptr
    );

#endif
