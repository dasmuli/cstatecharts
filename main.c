

#include "stdio.h"
#include "stdlib.h"
#include "lc.h"

/* http://viz-js.com/ */

struct transition_data
{
  const char* ev;
  const char* target_name; 
};
typedef struct transition_data transition_data_t;

struct cs
{
  int event;
  lc_t lc;
  int timer;  /* idea: increase timer in milliseconds outside */
  const char* current_state_name;
  const char* parent_state_name;
  int execute_on_enter;
  int execute_on_exit;
  void* user_data;
  transition_data_t transition_data[100];
  int transition_count;
};

#define CS_PRINTF printf

#define CS_WAITING 0
#define CS_YIELDED 1
#define CS_EXITED  2
#define CS_ENDED   3

#define DOCUMENT 1

#if DOCUMENT == 1

#define STATE(cs,name)                \
  cs->current_state_name = #name;     \
  cs->transition_count = 0;               \
  CS_PRINTF("subgraph cluster_%s_%s {\n",cs->parent_state_name,#name); \
  CS_PRINTF("style = rounded\n");   \
  CS_PRINTF("label = %s\n",#name);    \
  CS_PRINTF("%s_%s_C [ style=invis, shape = plaintext, label=\"\", height=0, width=0 ];\n",\
            cs->parent_state_name,#name);

#define TRANSITION(cs,event,name)     \
   cs->transition_data[cs->transition_count].ev = #event; \
   cs->transition_data[cs->transition_count].target_name = #name; \
   cs->transition_count++;
  
            
#define ON_ENTER if( 0 )
#define ON_EXIT if( 0 )
#define INIT(cs)  
#define BEGIN(cs)  
#define END(cs)   
#define ENDSTATE(cs,name) CS_PRINTF("}\n");	\
    for(int i = 0; i < cs->transition_count; i++)   \
    CS_PRINTF("%s_%s_C -> %s_%s_C [ltail=cluster_%s_%s lhead=cluster_%s_%s label=\"%s\" ];\n",           \
            cs->parent_state_name,              \
            cs->current_state_name,             \
            cs->parent_state_name,              \
            cs->transition_data[i].target_name,     \
            cs->parent_state_name,              \
            cs->current_state_name,             \
            cs->parent_state_name,              \
            cs->transition_data[i].target_name,      \
            cs->transition_data[i].ev);             \
    cs->current_state_name = NULL;		
#define EXECUTE_BEGIN                           \
  CS_PRINTF("digraph D {\ncompound=true\n");    \
  CS_PRINTF("graph [pad=\"0.5\", nodesep=\"1\", ranksep=\"1.75\"];");
  
#define EXECUTE_END CS_PRINTF("}\n");
#define RUN( state_func, state_data )           \
  CS_PRINTF( "subgraph cluster_%s {\nstyle = dashed\n", #state_func );    \
  CS_PRINTF( "  label = %s\n", #state_func );\
  state_data.parent_state_name = #state_func;  \
  state_func( &state_data );                   \
  CS_PRINTF( "}\n" );   


#else

int __ev = 0;

#define STATE(cs,name)                          \
    state_##name:                               \
    LC_SET((cs)->lc);				\
    if(CS_YIELD_FLAG == 0) 			\
      return CS_YIELDED;			

#define TRANSITION(cs,ev,name)                  \
  do {                                          \
    if(cs->event == ev) {                       \
      if(cs->execute_on_exit == 0) {            \
        cs->execute_on_exit = 1;                \
      } else {                                  \
        cs->execute_on_enter=1;                 \
        cs->execute_on_exit = 0;                \
        goto state_##name;                      \
      }                                         \
    }                                           \
  } while (0)

#define ON_ENTER if( cs->execute_on_enter )
#define ON_EXIT if( cs->execute_on_exit )
#define INIT(cs)   LC_INIT((cs)->lc); (cs)->execute_on_enter = 1; (cs)->execute_on_exit = 0; 
#define BEGIN(cs)  { char CS_YIELD_FLAG = 1; LC_RESUME((cs)->lc)
#define END(cs)    LC_END((pt)->lc); CS_YIELD_FLAG = 0; \
                    INIT(cs); return CS_ENDED; }
#define ENDSTATE(cs,name)			\
    cs->execute_on_enter=0;                     \
    if(cs->execute_on_exit == 0)                \
      CS_YIELD_FLAG = 0;		        \
    goto state_##name; 				
#define EXECUTE_BEGIN  while(1) {
#define EXECUTE_END __ev = cs_get_next_event(); }
#define RUN( state_func, p_state_data )        \
  p_state_data.event = __ev;                   \
  state_func( &p_state_data );

#endif

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


