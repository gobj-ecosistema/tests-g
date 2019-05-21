/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include "/yuneta/development/yuneta/^gobj-ecosistema/ghelpers/src/11_time_helper2.h"

/***************************************************************************
 *      Constants
 ***************************************************************************/

/***************************************************************************
 *      Structures
 ***************************************************************************/

/***************************************************************************
 *      Prototypes
 ***************************************************************************/

/***************************************************************************
 *      Data
 ***************************************************************************/

/***************************************************************************
 *
 ***************************************************************************/

/***************************************************************************
 *
 ***************************************************************************/
int parse(char *date)
{
    char result[256];

    if(parse_date(date, result, sizeof(result))<0) {
        printf("ERROR parsing date '%s'\n", date);
        return -1;
    }
    printf("Parsing    %s ==> %s\n", date, result);

    timestamp_t timestamp;
    int offset;
    parse_date_basic(date, &timestamp, &offset);


    struct tm *tm;
    char stamp[164];
    tm = gmtime((time_t *)&timestamp);
    strftime(stamp, sizeof (stamp), "%Y-%m-%dT%H:%M:%S%z", tm);
    printf("gmtime   : %s, offset %d\n", stamp, offset);

    tm = localtime((time_t *)&timestamp);
    strftime(stamp, sizeof (stamp), "%Y-%m-%dT%H:%M:%S%z", tm);
    printf("localtime: %s, offset %d\n\n", stamp, offset);

    return 0;
}


/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    parse("2018-03-14T17:02:31.000Z");

    char current[120];
    current_timestamp(current, sizeof(current));
    parse(current);

    parse("2018-03-14T17:02:31");

    parse("2018-03-14 17:02:31");

    parse("2018/03/14 17:02:31");

    parse("14-03-2018 17:02:31");

    parse("01-03-18 17:02:31");

    return 0;
}

