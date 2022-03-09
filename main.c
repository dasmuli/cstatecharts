

#include "stdio.h"
#include "stdlib.h"
#include "lc.h"

struct state_data
{
  int transition;
};

struct cs
{
  int transition;
  lc_t lc;
  int timer;  /* idea: increase timer in milliseconds outside */
  const char* current_state_name;
  int print;
};

#define CS_PRINTF printf

#define CS_WAITING 0
#define CS_YIELDED 1
#define CS_EXITED  2
#define CS_ENDED   3

#define STATE(cs,name)                          \
    state_##name:                               \
    LC_SET((cs)->lc);				\
    if(CS_YIELD_FLAG == 0) 			\
      return CS_YIELDED;			

#define TRANSITION(cs,event,name)               \
  do {                                          \
    if(cs->transition == event)                 \
      goto state_##name;                        \
  } while (0)

#define ON_ENTER if( 1 )
#define END_STATE(name) sd.transition = getchar();  goto state_##name

#define INIT(cs)   LC_INIT((cs)->lc) 
#define BEGIN(cs)  { char CS_YIELD_FLAG = 1; LC_RESUME((cs)->lc)
#define END(cs)    LC_END((pt)->lc); CS_YIELD_FLAG = 0; \
                      INIT(cs); return CS_ENDED; }
#define ENDSTATE(cs,name)			\
    CS_YIELD_FLAG = 0;				\
    goto state_##name; 				

static int statemachine1(struct cs* cs)
{
  BEGIN(cs)

  STATE(cs,first)
    TRANSITION(cs,'2', second);
    ON_ENTER
    {
      printf("state 1\n");
    }
  ENDSTATE(cs,first);
  
  STATE(cs,second)
    TRANSITION(cs,'1', first);
    ON_ENTER
    {
      printf("state 2\n");
    }
  ENDSTATE(cs,second);

  END(cs)
}

static struct cs cs1,cs2;

int main(int argc, char* argv[])
{
  printf("Hello state\n");

  INIT(&cs1);

  while(1)
  {
    statemachine1(&cs1);
    cs1.transition = getchar();
  }
}


