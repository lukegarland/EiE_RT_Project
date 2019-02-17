/**********************************************************************************************************************
File: rt_app.c

----------------------------------------------------------------------------------------------------------------------
To start a new task using this rt_app as a template:
 1. Copy both rt_app.c and rt_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "rt_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "RtApp" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "RT_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a rt_app.c file template

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void RtAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void RtAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_RtApp"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32RtAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "RtApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type RtApp_StateMachine;            /* The state machine function pointer */
//static u32 RtApp_u32Timeout;                      /* Timeout counter used across states */
static u16 RtApp_ReactTimer;
static u32 RtApp_AvgTime;
static u32 RtApp_TotalTime;
static u16 RtApp_WaitTime;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: RtAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  -
*/
void RtAppInitialize(void)
{


    PixelAddressType pa_WelcomeMessageLoc1 = {LCD_SMALL_FONT_LINE3, 0};
    PixelAddressType pa_WelcomeMessageLoc2 = {LCD_SMALL_FONT_LINE4, 0};

    LcdClearScreen();
    LcdLoadString("Press Button0 to run", LCD_FONT_SMALL, &pa_WelcomeMessageLoc1);
    LcdLoadString("reaction time test!", LCD_FONT_SMALL, &pa_WelcomeMessageLoc2);



  /* If good initialization, set state to Idle */
  if(1)
  {
    RtApp_StateMachine = RtAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    RtApp_StateMachine = RtAppSM_Error;
  }

} /* end RtAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function RtAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void RtAppRunActiveState(void)
{
  RtApp_StateMachine();

} /* end RtAppRunActiveState */

void RtApp_AllLedsOn(void)
{

}



/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void RtAppSM_Idle(void)
{
    if(WasButtonPressed(BUTTON0))
  {
    LcdClearScreen();
    RtApp_StateMachine = RtAppSM_WFT;
  }

} /* end RtAppSM_Idle() */



static void RtAppSM_WFT(void)
{

}

static void RtAppSM_WFR(void)
{

}

static void RtAppSM_ShowTime(void)
{

}

static void RtAppSM_CalcResults(void)
{

}

static void RtAppSM_DispResults(void)
{


}









/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void RtAppSM_Error(void)
{

} /* end RtAppSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
