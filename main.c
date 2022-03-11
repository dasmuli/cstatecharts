

#include "stdio.h"
#include "stdlib.h"
#include "cstate.h"

static struct cs cs1,cs2, cs_nested;

static int statemachine_nested(struct cs* cs)
{
  BEGIN(cs)

  STATE(cs,first)
    TRANSITION(cs,'7', second);
    ON_ENTER
    {
      printf("NESTED state 6\n");
    }
    ON_EXIT
    {
      printf("NESTED left state 6\n");
    }
  ENDSTATE(cs,first);
  
  STATE(cs,second)
    TRANSITION(cs,'6', first);
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


static int statemachine1(struct cs* cs)
{
  BEGIN(cs)

  STATE(cs,first)
    TRANSITION(cs,'2', second);
    ON_ENTER
    {
      printf("state 1\n");
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


static int statemachine2(struct cs* cs)
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

int cs_get_next_event()
{
  char ev;
  scanf( " %c", &ev );
  return (int) ev;
}

int main(int argc, char* argv[])
{
  INIT(&cs1);
  INIT(&cs2);
  INIT(&cs_nested);

  EXECUTE_BEGIN 
    
    RUN( statemachine1, cs1 );
    RUN( statemachine2, cs2 );

  EXECUTE_END
}


