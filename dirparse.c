#include <stdio.h>
#include <stdlib.h>

#define MAX_SAMPLES (10000000)
#define HEADER (9)

int direction_from_state(unsigned int prev_state, unsigned int state, int current_direction);


int main(int argc, char *argv[])
{
    int rc, idx, direction=0, direction_prev=0;
    unsigned int sigA, sigB, ipestate=0, ipestate_prev=0;
    size_t linelen;
    double time;
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
        printf("%s", lineptr);
    }

    for(idx=0; idx <  MAX_SAMPLES; idx++)
    {
        // READ: one line of AD trace
        rc=fscanf(fin, "%lf, %u, %u", &time, &sigA, &sigB);

        if(rc < 0)
        {
            // STOP parsing if we encounter an EOF
            if(rc == EOF)
                break;
            else
            {
                perror("fscanf");
                exit(-1);
            }
        }
        

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
            printf("Signal STATE ERROR, sigA=%u, sigB=%u\n", sigA, sigB);
            exit(-1);
        }

	// one time case to set previous to current on first loop
	if(idx==0) ipestate_prev=ipestate;

        //printf("@ time=%lf, sigA=%d, sigB=%d\n", time, sigA, sigB);

        if(ipestate != ipestate_prev)
        {
            // STATE HAS CHANGED: determine direction
            direction = direction_from_state(ipestate_prev, ipestate, direction_prev);

            //printf("direction is %d: @ time=%lf, sigA=%d, sigB=%d\n", direction, time, sigA, sigB);
            ipestate_prev = ipestate;

            if(direction != direction_prev)
            {
                printf("DIRECTION CHANGE: @ time=%lf, from %d to %d\n", time, direction_prev, direction);
            }

            direction_prev = direction;
        }


        // NOTHING HAS CHANGED - consume data and continue
        else
        {
            //printf(".");
            ipestate_prev = ipestate;
            direction_prev = direction;
        }

    }

    fclose(fin);
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

