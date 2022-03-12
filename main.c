

#include "stdio.h"
#include "stdlib.h"
#include "cstatechart.h"
#include <sys/poll.h>

static cs_t cs1,cs2, cs_nested;

static int statemachine_nested(cs_t* cs)
{
  BEGIN(cs)

  STATE(cs,first)
    TIME_TRANSITION(cs,0.5, second);
    ON_ENTER
    {
      printf("NESTED state 6, timer: %f\n",cs->timer);
    }
    ON_EXIT
    {
      printf("NESTED left state 6, timer: %f\n,",cs->timer);
    }
  ENDSTATE(cs,first);
  
  STATE(cs,second)
    TIME_TRANSITION(cs,2.5, first);
    ON_ENTER
    {
      printf("NESTED state 7\n");
    }
    ON_EXIT
    {
      printf("NESTED left state 7\n");
    }
  ENDSTATE(cs,second);

  END(cs)
}


static int statemachine1(cs_t* cs)
{
  BEGIN(cs)

  STATE(cs,first)
    TRANSITION(cs,'2', second);
    ON_ENTER
    {
      printf("state 1, parameter: %i\n", *((int*)cs_get_event_parameter()) );
    }
    ON_EXIT
    {
      printf("Left state 1\n");
    }
  ENDSTATE(cs,first);
  
  STATE(cs,second)
    TRANSITION(cs,'1', first);
    ON_ENTER
    {
      printf("state 2\n");
    }
    ON_EXIT
    {
      printf("Left state 2\n");
    }
    RUN(statemachine_nested, cs_nested);
  ENDSTATE(cs,second);

  END(cs)
}


static int statemachine2(cs_t* cs)
{
  BEGIN(cs)

  STATE(cs,first)
    TRANSITION(cs,'3', second);
    ON_ENTER
    {
      printf("m2 state 3\n");
    }
    ON_EXIT
    {
      printf("m2 Left state 3\n");
    }
  ENDSTATE(cs,first);
  
  STATE(cs,second)
    TRANSITION(cs,'4', third);
    ON_ENTER
    {
      printf("m2 state 4\n");
    }
    ON_EXIT
    {
      printf("m2 Left state 4\n");
    }
  ENDSTATE(cs,second);
  
  STATE(cs,third)
    TRANSITION(cs,'5', first);
    ON_ENTER
    {
      printf("m2 state 5\n");
    }
    ON_EXIT
    {
      printf("m2 Left state 5\n");
    }
  ENDSTATE(cs,third);

  END(cs)
}


int poll_stdin(void)
{
        struct pollfd fds;
        int ret;
        fds.fd = 0; /* this is STDIN */
        fds.events = POLLIN;
        ret = poll(&fds, 1, 0);
        if(ret == 1)
                return 1;
        else
                return 0;
        return 0;
}

static int counter = 0;
void get_next_key_event()
{
  char ev;
  if(poll_stdin())
  {
    do {
      scanf( "%c", &ev );
      /* Add event to statechart. */
      cs_add_event_with_parameter( (int) ev, (char*)&counter ); 
      counter++;
    } while(ev != '\n');
  }
}

int main(int argc, char* argv[])
{
  INIT(&cs1);
  INIT(&cs2);
  INIT(&cs_nested);

  EXECUTE_BEGIN 
    
    RUN( statemachine1, cs1 );
    RUN( statemachine2, cs2 );
    
    get_next_key_event();

  EXECUTE_END
}


