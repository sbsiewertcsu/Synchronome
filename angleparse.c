#include <stdio.h>
#include <stdlib.h>

#define MAX_SAMPLES (10000000)
#define HEADER (9)

#define IPE_STEP_DEGREES (0.17578125) // or [360.0 degress]/[2048 increments]

int direction_from_state(unsigned int prev_state, unsigned int state, int current_direction);
int read_trace_line(FILE *fin, double *start_time, unsigned int *sigA, unsigned int *sigB);


int main(int argc, char *argv[])
{
    int rc, idx, direction=0, direction_prev=0;
    unsigned int sigA, sigB, ipestate=0, ipestate_prev=0;
    size_t linelen;
    double trace_time, start_time, angle_off_start, time=0.0, angle=0.0;
    char *lineptr = (char *)0;
    FILE *fin, *fout;

    if((fin = fopen(argv[1], "r")) == (FILE *)0)
    {
        printf("Error opening AD trace file %s\n", argv[1]);
    }

    printf("Header:\n");

    for(idx=0; idx < HEADER; idx++)
    {
        getline(&lineptr, &linelen, fin);
        if(idx < HEADER-2) printf("%s", lineptr);
    }

    // Column header for new output
    printf("time, angle, A, B\n");

    // read the very first data line to get start time
    rc=read_trace_line(fin, &start_time, &sigA, &sigB);
    trace_time = start_time;
    angle_off_start = 0.0;

    for(idx=0; idx <  MAX_SAMPLES; idx++)
    {
        // see if we need to break here based on last trace read
        if(rc == EOF) break;

        // PARSE: signal output into one of 4 states
        if((sigA == 0) && (sigB == 0))
            ipestate=0;
        else if((sigA == 1) && (sigB == 0))
            ipestate=2;
        else if((sigA == 1) && (sigB == 1))
            ipestate=3;
        else if((sigA == 0) && (sigB == 1))
            ipestate=1;

        // INPUT ERROR: signal invalid value (must be 0 or 1 only)
        else
        {
            printf("Signal STATE ERROR\n"); exit(-1);
        }

        if(ipestate != ipestate_prev)
        {
            // STATE CHANGED so DETERMINE DIRECTION and add in ANGLE INCREMENT
            direction = direction_from_state(ipestate_prev, ipestate, direction_prev);
            angle_off_start = angle_off_start + ((double)direction * IPE_STEP_DEGREES);
            ipestate_prev = ipestate;
            direction_prev = direction;

            printf("%lf, %lf, %u, %u\n", (trace_time-start_time), angle_off_start, sigA, sigB);
        }

        // NO STATE CHANGE - consume data and continue
        else
        {
            ipestate_prev = ipestate;
            direction_prev = direction;
        }

        // READ: next line of AD trace for next parsing loop
        rc=read_trace_line(fin, &trace_time, &sigA, &sigB);
    }

    fclose(fin);
}


int read_trace_line(FILE *fin, double *start_time, unsigned int *sigA, unsigned int *sigB)
{
    int rc=0;

    rc=fscanf(fin, "%lf, %u, %u", start_time, sigA, sigB);

    if(rc < 0)
    {
        // STOP parsing if we encounter an EOF
        if(rc == EOF)
            return EOF;
        else
        {
            perror("fscanf");
            exit(-1);
        }
    }

    return rc;
}


int direction_from_state(unsigned int prev_state, unsigned int state, int current_direction)
{
    // no direction
    int direction=0;
 
    // determine direction based on transition for current state to new state
    if(prev_state == 0)
    {
        if(state == 2)
        {
            direction=1;
        }
        else if(state == 1)
        {
            direction=-1;
        }
        else
        {
            printf("STATE TRANSITION ERROR\n");
            exit(-1);
        }
    }
    else if(prev_state == 2) 
    {
        if(state == 3)
        {
            direction=1;
        }
        else if(state == 0)
        {
            direction=-1;
        }
        else
        {
            printf("STATE TRANSITION ERROR\n");
            exit(-1);
        }
    }
    else if(prev_state == 3) 
    {
        if(state == 1)
        {
            direction=1;
        }
        else if(state == 2)
        {
            direction=-1;
        }
        else
        {
            printf("STATE TRANSITION ERROR\n");
            exit(-1);
        }
    }
    else if(prev_state == 1) 
    {
        if(state == 0)
        {
            direction=1;
        }
        else if(state == 3)
        {
            direction=-1;
        }
        else
        {
            printf("STATE TRANSITION ERROR\n");
            exit(-1);
        }
    }
    else
    {
        printf("STATE TRANSITION ERROR\n");
        exit(-1);
    }

    return direction;

}

