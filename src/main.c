#include <stdio.h>
#include "gps.h"

void update(struct gps_data_t *gps, char *str, size_t len)
{
    fprintf(stderr, "Received an update from gpsd\n");
    if (gps->set & LATLON_SET) {
        printf("Latitude:   % 6.3f\nLongitude: % 7.3f\n",
               gps->fix.latitude, gps->fix.longitude);
    }
    if (gps->set & TIME_SET) {
        printf("Time: %.1f\n", gps->fix.time);
    }
}

int main(int argc, char *argv[])
{
    fprintf(stderr, "gpsd streaming test\n");

    struct gps_data_t *gps;

    gps = gps_open("gpsd.mainframe.cx", "2947");
    if (!gps) {
        fprintf(stderr, "gps_open failed\n");
        return -1;
    }

//     gps_set_raw_hook(gps, update);
    
    gps_stream(gps, WATCH_ENABLE|WATCH_NEWSTYLE, NULL);
    
    while(!feof(stdin)) {
        if (gps_waiting(gps)) {
            fprintf(stderr, "+\r");
            gps_poll(gps);
            update(gps, NULL, 0);
        } else {
            fprintf(stderr, "-\r");
        }
    }
    
    gps_close(gps);
    fprintf(stderr, "Done\n");
}
