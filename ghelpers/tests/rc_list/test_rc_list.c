/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <ghelpers.h>

/***************************************************************************
 *      Constants
 ***************************************************************************/
#define MIN_BLOCK           32                  /* smaller memory block */
#define MAX_BLOCK           256*1024LL          /* largest memory block */
#define SUPERBLOCK          512*1024LL          /* super-block size */
#define MAX_SYSTEM_MEMORY   64*1024LL*1024LL    /* maximum core memory */

/***************************************************************************
 *      Structures
 ***************************************************************************/
typedef struct myresource_s { // Sample of rc_resource_t
    RC_RESOURCE_HEADER
    // user data
    char data[256];
} myresource_t;

/***************************************************************************
 *      Prototypes
 ***************************************************************************/

/***************************************************************************
 *      Data
 ***************************************************************************/

/***************************************************************************
 *
 ***************************************************************************/
static inline struct timespec ts_diff (struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

/***************************************************************************
 *
 ***************************************************************************/
void _delete_resource(void *rc)
{
    rc_delete_resource(rc, gbmem_free);
}

/***************************************************************************
 *
 ***************************************************************************/
int cb_walking_fn(
    rc_instance_t *instance,
    rc_resource_t *resource_,
    void *user_data, void *user_data2, void *user_data3)
{
    myresource_t *resource = (myresource_t *)resource_;
    uint64_t count = (uint64_t)(size_t)user_data;
    if(count < 2) {
        printf("-> %s\n", resource->data);
    }
    if(strcmp(resource->data, "1")==0) {
        return 1;
    }
    return 0;
}

/***************************************************************************
 *
 ***************************************************************************/
static inline void test(int flags, uint64_t cnt)
{
#define DUMP_SIZE 512

    struct timespec st, et, dt;
    char mensaje[256];

    switch(flags) {
    case 0:
        strcpy(mensaje, "test");
        break;
    }

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/
    log_startup(
        "test",             // application name
        "1.0.0",            // applicacion version
        "test_glogger"     // executable program, to can trace stack
    );
    log_add_handler("test_stdout", "stdout", LOG_OPT_ALL|LOG_HND_OPT_TRACE_STACK, 0);

    static uint32_t mem_list[] = {0};
    // You also need link with ghelpersd library
    gbmem_trace_alloc_free(0, mem_list);

    if(0) {
        gbmem_startup( /* Create memory core */
            MIN_BLOCK,
            MAX_BLOCK,
            SUPERBLOCK,
            MAX_SYSTEM_MEMORY,
            NULL,               /* system memory functions */
            0
        );
        gbmem_enable_log_info(1);
        json_set_alloc_funcs(
            gbmem_malloc,
            gbmem_free
        );
    }

    init_ghelpers_library("test");

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);

            for(uint64_t count=0; count<cnt; count++) {
                /*
                 *  Create resource tree
                 */
                dl_list_t *dl_myresource_list = rc_init_iter(0);

                for(int i=0; i<3; i++) {
                    rc_instance_t *parent_resource_i = rc_add_instance(
                        dl_myresource_list,
                        0, sizeof(myresource_t)
                    );
                    myresource_t *parent_resource = rc_get_resource(parent_resource_i);
                    snprintf(parent_resource->data, sizeof(parent_resource->data), "%d", i);

                    for(int j=0; j<3; j++) {
                        rc_instance_t *child_resource_i = rc_add_child(
                            (rc_resource_t *)parent_resource,
                            0,
                            sizeof(myresource_t)
                        );
                        myresource_t *child_resource = rc_get_resource(child_resource_i);
                        snprintf(child_resource->data, sizeof(child_resource->data), "%d-%d", i, j);
                    }
                }

                /*
                 *  Display resource list
                 */
                if(count < 2) printf("====> List >\n");
                rc_walk_by_list(
                    dl_myresource_list,
                    WALK_FIRST2LAST,
                    cb_walking_fn,
                    (void *)(size_t)count,
                    0,
                    0
                );
                if(count < 2) printf("====> List <\n");
                rc_walk_by_list(
                    dl_myresource_list,
                    WALK_LAST2FIRST,
                    cb_walking_fn,
                    (void *)(size_t)count,
                    0,
                    0
                );
                /*
                 *  Display resource tree
                 */
                if(count < 2) printf("====> Tree level ->\n");
                rc_walk_by_tree(
                    dl_myresource_list,
                    WALK_BYLEVEL|WALK_FIRST2LAST,
                    cb_walking_fn,
                    (void *)(size_t)count,
                    0,
                    0
                );
                if(count < 2) printf("====> Tree level <-\n");
                rc_walk_by_tree(
                    dl_myresource_list,
                    WALK_BYLEVEL|WALK_LAST2FIRST,
                    cb_walking_fn,
                    (void *)(size_t)count,
                    0,
                    0
                );

                if(count < 2) printf("====> Tree top-bottom\n");
                rc_walk_by_tree(
                    dl_myresource_list,
                    WALK_TOP2BOTTOM,
                    cb_walking_fn,
                    (void *)(size_t)count,
                    0,
                    0
                );

                if(count < 2) printf("====> Tree bottom-top\n");
                rc_walk_by_tree(
                    dl_myresource_list,
                    WALK_BOTTOM2TOP,
                    cb_walking_fn,
                    (void *)(size_t)count,
                    0,
                    0
                );

                /*
                 *  Libera
                 */
                rc_free_iter(dl_myresource_list, TRUE, _delete_resource);
            }

            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/
    gbmem_shutdown();
    log_end();

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    printf("# %-36s (%12" PRIu64 "): %lu.%lus\n",
        mensaje,
        cnt,
        dt.tv_sec, dt.tv_nsec);
}


/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    if(argc == 1) {
        test(0, 1LL);
    } else if(argc == 2) {
        test(0, 100000LL);
    } else if(argc == 3) {
        test(0, 1000000LL);
    } else if(argc == 3) {
        test(0, 10000000LL);
    }
    return 0;
}

