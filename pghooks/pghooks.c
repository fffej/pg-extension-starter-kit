#include "postgres.h"
#include "fmgr.h"
#include "executor/executor.h"
#include "commands/event_trigger.h"
#include "utils/guc.h"
#include "utils/elog.h"
#include "tcop/utility.h"
#include "access/xact.h"
#include "utils/builtins.h"

PG_MODULE_MAGIC;

void _PG_init(void);
void _PG_fini(void);

// Hook storage - keeping the previous hook values 
static ExecutorStart_hook_type prev_executor_start = NULL;
static ProcessUtility_hook_type prev_process_utility = NULL;
static ExecutorCheckPerms_hook_type prev_executor_check_perms = NULL;
static ExecutorEnd_hook_type prev_executor_end = NULL;

// Hook implementation functions 
static void hook_executor_start(QueryDesc *queryDesc, int eflags);
static void hook_process_utility(PlannedStmt *pstmt, const char *queryString, bool readOnlyTree,
                               ProcessUtilityContext context, ParamListInfo params,
                               QueryEnvironment *queryEnv,
                               DestReceiver *dest, QueryCompletion *qc);
static bool hook_executor_check_perms(List *rangeTabls, List* rtePermInfos, bool abort);
static void hook_executor_end(QueryDesc *queryDesc);

// Transaction callback 
static void hook_xact_callback(XactEvent event, void *arg);

// Extension initialization 
void _PG_init(void)
{
    elog(NOTICE, "pg_hook_demo: initializing extension");

    RegisterXactCallback(hook_xact_callback, NULL);

    // Save original hook values and set our hooks 
    prev_executor_start = ExecutorStart_hook;
    ExecutorStart_hook = hook_executor_start;

    prev_process_utility = ProcessUtility_hook;
    ProcessUtility_hook = hook_process_utility;

    prev_executor_check_perms = ExecutorCheckPerms_hook;
    ExecutorCheckPerms_hook = hook_executor_check_perms;

    prev_executor_end = ExecutorEnd_hook;
    ExecutorEnd_hook = hook_executor_end;
}

// Unload extension
void _PG_fini(void)
{
    // Restore saved extensions
    ExecutorStart_hook = prev_executor_start;
    ProcessUtility_hook = prev_process_utility;
    ExecutorCheckPerms_hook = prev_executor_check_perms;
    ExecutorEnd_hook = prev_executor_end;

    UnregisterXactCallback(hook_xact_callback, NULL);
    elog(NOTICE, "pg_hook_demo: unloaded");
}

/* ExecutorStart hook - called at the start of query execution */
static void hook_executor_start(QueryDesc *queryDesc, int eflags)
{
    elog(NOTICE, "pg_hook_demo: executor start hook called for query: %s", 
         queryDesc->sourceText);

    // Call the previous hook or standard function 
    if (prev_executor_start)
        prev_executor_start(queryDesc, eflags);
    else
        standard_ExecutorStart(queryDesc, eflags);
}

// ProcessUtility hook - called for utility statements (CREATE, ALTER, etc.) 
static void hook_process_utility(PlannedStmt *pstmt, const char *queryString, bool readOnlyTree,
                   ProcessUtilityContext context, ParamListInfo params,
                   QueryEnvironment *queryEnv,
                   DestReceiver *dest, QueryCompletion *qc)
{
    elog(NOTICE, "pg_hook_demo: process utility hook called for: %s", queryString);

    /* Call the previous hook or standard function */
    if (prev_process_utility)
        prev_process_utility(pstmt, queryString, readOnlyTree, context, params, queryEnv, dest, qc);
    else
        standard_ProcessUtility(pstmt, queryString, readOnlyTree, context, params, queryEnv, dest, qc);
}

// ExecutorCheckPerms hook - security check for queries 
static bool hook_executor_check_perms(List *rangeTabls, List* rtePermInfos, bool abort)
{
    elog(NOTICE, "pg_hook_demo: permission check hook called");

    /* Call the previous hook if it exists, otherwise return true (abort) */
    if (prev_executor_check_perms)
        return prev_executor_check_perms(rangeTabls, rtePermInfos, abort);
    else
        return true;
}

// ExecutorEnd hook - called at the end of query execution 
static void hook_executor_end(QueryDesc *queryDesc)
{
    elog(NOTICE, "pg_hook_demo: executor end hook called");

    // Call the previous hook or standard function 
    if (prev_executor_end)
        prev_executor_end(queryDesc);
    else
        standard_ExecutorEnd(queryDesc);
}

// Transaction callback 
static void hook_xact_callback(XactEvent event, void *arg)
{
    switch (event)
    {
        case XACT_EVENT_COMMIT:
            elog(NOTICE, "Commit");
            break;        
        case XACT_EVENT_PARALLEL_COMMIT:
            elog(NOTICE, "Parallel Commit");
            break;        
        case XACT_EVENT_ABORT:
            elog(NOTICE, "Abort");
            break;        
        case XACT_EVENT_PARALLEL_ABORT:
            elog(NOTICE, "Parallel Abort");
            break;        
        case XACT_EVENT_PREPARE:
            elog(NOTICE, "Prepare");
            break;        
        case XACT_EVENT_PRE_COMMIT:
            elog(NOTICE, "Precommit");
            break;        
        case XACT_EVENT_PARALLEL_PRE_COMMIT:
            elog(NOTICE, "Parallel Pre Commit");
            break;        
        case XACT_EVENT_PRE_PREPARE:
            elog(NOTICE, "PrePrepare");
            break;
        default:
            break;
    }
}

// SQL-callable function for testing that the extension is loaded 
PG_FUNCTION_INFO_V1(pg_hook_demo_test);
Datum
pg_hook_demo_test(PG_FUNCTION_ARGS)
{
    elog(NOTICE, "pg_hook_demo: test function called");
    PG_RETURN_TEXT_P(cstring_to_text("pg_hook_demo is working!"));
}