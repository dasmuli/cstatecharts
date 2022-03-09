

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

#define DOCUMENT 1

#if DOCUMENT == 1

#define STATE(cs,name) cs->current_state_name = #name;                          
#define TRANSITION(cs,event,name)     \
        CS_PRINTF("%s -> %s\n",cs->current_state_name, #name);
#define ON_ENTER if( 0 )
#define END_STATE(name) 
#define INIT(cs)  
#define BEGIN(cs)  
#define END(cs)   
#define ENDSTATE(cs,name)			


#else

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

#endif

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
  INIT(&cs1);

#if DOCUMENT == 1
  CS_PRINTF("digraph D {\n");
  statemachine1(&cs1);
  CS_PRINTF("}\n");
#else  
  while(1)
  {
    statemachine1(&cs1);
    cs1.transition = getchar();
  }
#endif
}


