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
static u16      u16RtApp_ReactTime;
static u16      u16RtApp_AvgTime;
static u32      u32RtApp_TotalTime;
static u16      u16RtApp_WaitCount;
static u16      u16RtApp_WaitTime;
static u16      u16RtApp_STWait;
static u8       u8RtApp_Trials;
static s8       au8RtApp_TimeStr[15];
static s8       au8RtApp_AvgTimeStr[15];

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


    PixelAddressType pa_WelcomeMessageLoc1 = {LCD_SMALL_FONT_LINE2, 3};
    LcdClearScreen();
    LcdLoadString("Push BUTTON0 to start!", LCD_FONT_SMALL, &pa_WelcomeMessageLoc1);
    u32RtApp_TotalTime = 0;




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





/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

void RtApp_AllLedsOn(void)
{
    for(u8 i = 0; i < 12; i++)
    {
        // LedOn(0+i);
        LedPWM(0+i,LED_PWM_20);

    }
}
void RtApp_AllLedsOff(void)
{
      for(u8 i = 0; i < 12; i++)
        LedOff(0+i);

}




/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void RtAppSM_Idle(void)
{


     if (WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
        LcdClearScreen();
        RtApp_StateMachine = RtAppSM_RFT;
            u16RtApp_WaitCount = 0;

    }






} /* end RtAppSM_Idle() */

static void RtAppSM_RFT(void)
{

   srand(G_u32SystemTime1ms); // Seeding random number with system time.
    u16RtApp_WaitTime = rand() % (MAX_WAIT + 1 - MIN_WAIT) + MIN_WAIT;
        // Generates a random wait time between min and max times
        // (default 5s and 10s)

    static PixelAddressType pa_WFTLine1 = {LCD_SMALL_FONT_LINE2, 10};
    static PixelAddressType pa_WFTLine2 = {LCD_SMALL_FONT_LINE4, 24};

    LcdLoadString("Press BUTTON0 when",LCD_FONT_SMALL,&pa_WFTLine1);

    LcdLoadString("all LEDs flash",LCD_FONT_SMALL,&pa_WFTLine2);

    RtApp_StateMachine = RtAppSM_WFT;
}

static void RtAppSM_WFT(void)
{


    if(u16RtApp_WaitCount == u16RtApp_WaitTime)
    {

        u16RtApp_ReactTime = 0;
        RtApp_StateMachine = RtAppSM_WFR;
        u16RtApp_WaitCount = 0;

        RtApp_AllLedsOn();
    }else
    {
        u16RtApp_WaitCount++;
    }





}

static void RtAppSM_WFR(void)
{
    if(WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);

        RtApp_AllLedsOff();
        RtApp_StateMachine = RtAppSM_TimeToStr;

    }else u16RtApp_ReactTime++;
}
static void RtAppSM_TimeToStr(void)
{
    sprintf(au8RtApp_TimeStr,"%d.%d seconds",u16RtApp_ReactTime / 1000, u16RtApp_ReactTime % 1000);
    RtApp_StateMachine = RtAppSM_ShowTime;
    LcdClearScreen();
}

static void RtAppSM_ShowTime(void)
{

    static PixelAddressType pa_ST = {LCD_SMALL_FONT_LINE3, 30};
    LcdLoadString(au8RtApp_TimeStr, LCD_FONT_SMALL, &pa_ST);
    u16RtApp_STWait = 0;
    RtApp_StateMachine = RtAppSM_CalcResults;







}

static void RtAppSM_CalcResults(void)
{
     u16RtApp_STWait++;
     if(u16RtApp_STWait == SHOW_WAIT1)
    {
        LcdClearScreen();

        u8RtApp_Trials++;
        u32RtApp_TotalTime += u16RtApp_ReactTime;
        u16RtApp_ReactTime = 0;

        if(u8RtApp_Trials == NUMBER_OF_TRIALS)
        {
            u8RtApp_Trials = 0;

            RtApp_StateMachine = RtAppSM_ResultsToStr;

        }else RtApp_StateMachine = RtAppSM_RFT;
    }

}



static void RtAppSM_ResultsToStr(void)
{
     u16RtApp_AvgTime = u32RtApp_TotalTime/NUMBER_OF_TRIALS;
     sprintf(au8RtApp_AvgTimeStr,"%d.%d seconds",u16RtApp_AvgTime / 1000, u16RtApp_AvgTime % 1000);
     RtApp_StateMachine = RtAppSM_DispResults;

}


static void RtAppSM_DispResults(void)
{

    PixelAddressType pa_disp1 = {LCD_SMALL_FONT_LINE1,0};
    PixelAddressType pa_disp2 = {LCD_SMALL_FONT_LINE4,30};
    PixelAddressType pa_disp3 = {LCD_SMALL_FONT_LINE7, 12};
    LcdLoadString("Average reaction time", LCD_FONT_SMALL, &pa_disp1);
    LcdLoadString(au8RtApp_AvgTimeStr, LCD_FONT_SMALL, &pa_disp2);
    LcdLoadString("BUTTON0 to restart", LCD_FONT_SMALL, &pa_disp3);

    if(WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
    RtApp_StateMachine = RtAppInitialize;
    }
}









/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void RtAppSM_Error(void)
{

} /* end RtAppSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
