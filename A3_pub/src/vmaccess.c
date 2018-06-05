/**
 * @file vmaccess.c
 * @author Prof. Dr. Wolfgang Fohl, HAW Hamburg
 * @date 2010
 * @brief The access functions to virtual memory.
 */

#include "vmaccess.h"
#include <sys/ipc.h>
#include <sys/shm.h>

#include "syncdataexchange.h"
#include "vmem.h"
#include "debug.h"

/*
 * static variables
 */

static struct vmem_struct *vmem = NULL; //!< Reference to virtual memory

/**
 * The progression of time is simulated by the counter g_count, which is incremented by 
 * vmaccess on each memory access. The memory manager will be informed by a command, whenever 
 * a fixed period of time has passed. Hence the memory manager must be informed, whenever 
 * g_count % TIME_WINDOW == 0. 
 * Based on this information, memory manager will update aging information
 */

static int g_count = 0;    //!< global acces counter as quasi-timestamp - will be increment by each memory access
#define TIME_WINDOW   20

/**
 *****************************************************************************************
 *  @brief      This function setup the connection to virtual memory.
 *              The virtual memory has to be created by mmanage.c module.
 *
 *  @return     void
 ****************************************************************************************/
static void vmem_init(void) {
    /* Create System V shared memory */
    // convert a pathname and a project identifier to a System V IPC key
    key_t key = ftok(SHMKEY, SHMPROCID);
    TEST_AND_EXIT_ERRNO(key == -1, "Fail to generate key");

    /* We are only using the shm, don't set the IPC_CREAT flag */
    // get an XSI shared memory segment
    int shm_id = shmget (key, SHMSIZE, 0666); // 0666 flag to grant access to all groups.
    TEST_AND_EXIT_ERRNO(shm_id == -1, "Fail to get an XSI shared memory segment");

    /* attach shared memory to vmem */
    // XSI attach operation. NULL-> in first available address. 0-> read and write permissions
    vmem = shmat(shm_id, NULL, 0);

    //setupSyncDataExchangeInternal(false);
}

/**
 *****************************************************************************************
 *  @brief      This function puts a page into memory (if required). Ref Bit of page table
 *              entry will be updated.
 *              If the time window handle by g_count has reached, the window window message
 *              will be send to the memory manager. 
 *              To keep conform with this log files, g_count must be increased before 
 *              the time window will be checked.
 *              vmem_read and vmem_write call this function.
 *
 *  @param      address The page that stores the contents of this address will be 
 *              put in (if required).
 * 
 *  @return     void
 ****************************************************************************************/
static void vmem_put_page_into_mem(int address) {
    // get pageNo
    int pageNo = address / VMEM_PAGESIZE;
    TEST_AND_EXIT(pageNo < 0, (stderr, "pageNo out of range\n"));
    TEST_AND_EXIT(pageNo >= VMEM_NPAGES, (stderr, "pageNo out of range\n"));

    // check g_count if passed TIME_WINDOW if so memory manager will update aging
    if(g_count % TIME_WINDOW == 0) {
        struct msg message;
        message.cmd = CMD_TIME_INTER_VAL;
        message.g_count = g_count;

        sendMsgToMmanager (message);
    }

    // if page not present
    if ((vmem->pt[pageNo].flags & PTF_PRESENT) == 0) {
        struct msg message;

        message.cmd = CMD_PAGEFAULT;

        sendMsgToMmanager (message);
    }

}

int vmem_read(int address) {
    // if memory does not exist yet.
    if (vmem == NULL) {
        vmem_init ();
    }

    vmem_put_page_into_mem (address);

    // get pageNo
    int pageNo = address / VMEM_PAGESIZE;
    TEST_AND_EXIT(pageNo < 0, (stderr, "pageNo out of range\n"));
    TEST_AND_EXIT(pageNo >= VMEM_NPAGES, (stderr, "pageNo out of range\n"));

    // get offset from pageNo
    int offset = address % VMEM_PAGESIZE;

    //update flag
    vmem->pt[pageNo].flags |= PTF_REF;
    g_count++;

    return vmem->mainMemory[(vmem->pt[pageNo].frame * VMEM_PAGESIZE) + offset];
}

void vmem_write(int address, int data) {
    // if memory does not exist yet.
    if (vmem == NULL) {
        vmem_init();
    }

    //timer increment
    vmem_put_page_into_mem (address);

    // get pageNo
    int pageNo = address / VMEM_PAGESIZE;
    TEST_AND_EXIT(pageNo < 0, (stderr, "pageNo out of range\n"));
    TEST_AND_EXIT(pageNo >= VMEM_NPAGES, (stderr, "pageNo out of range\n"));

    // get offset from pageNo
    int offset = address % VMEM_PAGESIZE;

    // update flags
    vmem->pt[pageNo].flags |= PTF_DIRTY;
    vmem->pt[pageNo].flags |= PTF_REF;
    g_count++;

    //write data
    vmem->mainMemory[(vmem->pt[pageNo].frame * VMEM_PAGESIZE) + offset] = data;
}

void
vmem_close() {
    key_t key = ftok (SHMKEY, SHMPROCID);
    int shm_id = shmget (key, SHMSIZE, 0666);
    shmctl (shm_id, IPC_RMID, NULL);
    free(vmem);
}
// EOF
