/**********************************************************************************************************************
File: rt_app.c


Description:
This is an application to test one's reaction time to visual stimulus.

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
static u16      u16RtApp_ReactTimer;    // Running Reaction time
static u16      u16RtApp_ReactTimeP1;   // Player 1 Reaction time
static u16      u16RtApp_ReactTimeP2;   // Player 2 Reaction time

static u16      u16RtApp_AvgTimeP1;     // Player 1 Mean reaction time.
static u16      u16RtApp_AvgTimeP2;     // Player 2 Mean reaction time.

static u32      u32RtApp_TotalTimeP1;   // Player 1 total reaction time. Used to calculate mean time
static u32      u32RtApp_TotalTimeP2;   // Player 2  total reaction time. Used to calculate mean time

static u16      u16RtApp_WaitCount;     // Running count when waiting for trigger
static u16      u16RtApp_WaitTime;      // Time to wait until trigger (randomly generated)
static u16      u16RtApp_STWait;        // Running count to stall display when showing last reaction time
static u8       u8RtApp_Trials;              // Running count for number of trials
static char      au8RtApp_TimeStrP1[20];     // String to show time for player 1
static char      au8RtApp_TimeStrP2[20];     // String to show time for player 2.
static char      au8RtApp_TrialStr[14];      // String for "Trial __ of __ "
static char      au8RtApp_AvgTimeStrP1[23];  // String to show mean time for player 1.
static char      au8RtApp_AvgTimeStrP2[23];  // String to show mean time for player 2.



static bool b_button0Pressed; // = to TRUE when button 0 is pressed for the first time
static bool b_button1Pressed; // = to TRUE when button 1 is pressed for the first time
static bool b_2Players;  // TRUE if 2-player mode is selected by the user
static bool b_EasyMode;  // Forces b_SoundSpoof = 0 if TRUE.
                         // If TRUE, buzzer will never sound

static bool b_SoundSpoof;// If b_EasyMode == 0, then will randomly be set to TRUE


static u16 u16RtApp_SpoofMsgWait; //Running count to stall display when showing penalty screen
static u16 u16RtApp_SpoofWait; //Running count when sounding buzzer


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
Initializes the State Machine, and displays a Welcome Message

Requires:
  -

Promises:
  -
*/
void RtAppInitialize(void)
{
    ButtonAcknowledge(BUTTON0);
    ButtonAcknowledge(BUTTON1);

    PWMAudioSetFrequency(BUZZER1,A4);

    PixelAddressType pa_WelcomeMessage = {LCD_SMALL_FONT_LINE1, 20};
    PixelAddressType pa_WelcomeMessage2 = {LCD_SMALL_FONT_LINE2, 10};
    PixelAddressType pa_WelcomeMessage3 = {LCD_SMALL_FONT_LINE4, 20};
    PixelAddressType pa_WelcomeMessage4 = {LCD_SMALL_FONT_LINE5,10};
    LcdClearScreen();
    LcdLoadString("Push BUTTON 0 to", LCD_FONT_SMALL, &pa_WelcomeMessage);
    LcdLoadString("start 1 player mode", LCD_FONT_SMALL, &pa_WelcomeMessage2);
    LcdLoadString("Push BUTTON 1 to", LCD_FONT_SMALL, &pa_WelcomeMessage3);
    LcdLoadString("start 2 player mode", LCD_FONT_SMALL, &pa_WelcomeMessage4);

    u32RtApp_TotalTimeP1 = 0;
    u32RtApp_TotalTimeP2 = 0;




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
        LedPWM(RED0,LED_PWM_15);
        LedPWM(GREEN0,LED_PWM_15);
        LedPWM(GREEN1,LED_PWM_15);
        LedPWM(RED1,LED_PWM_15);
        LedPWM(RED2,LED_PWM_15);
        LedPWM(GREEN2,LED_PWM_15);
        LedPWM(RED3,LED_PWM_15);
        LedPWM(GREEN3,LED_PWM_15);


}
void RtApp_AllLedsOff(void)
{
    // TURN ALL LEDS OFF
      for(u8 i = 0; i < 12; i++)
        LedOff(0+i);

}




/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void RtAppSM_Idle(void)
// Waits for user to sleect either 1-Player or 2-Player mode
{


     if (WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
        LcdClearScreen();


        b_2Players = 0;
        RtApp_StateMachine = RtAppSM_AskMode;
    }

    if(WasButtonPressed(BUTTON1))
    {
        ButtonAcknowledge(BUTTON1);
        LcdClearScreen();


        b_2Players = 1;
        RtApp_StateMachine = RtAppSM_AskMode;

    }

} /* end RtAppSM_Idle() */

static void RtAppSM_AskMode(void)
//Asks if user wants to play with buzzer spoofing or not
{
    static PixelAddressType pa_AskModeLoc1 = {LCD_SMALL_FONT_LINE2, 0};
    static PixelAddressType pa_AskModeLoc2 = {LCD_SMALL_FONT_LINE3+5, 0};

     LcdLoadString("BUTTON0 for easy mode", LCD_FONT_SMALL, &pa_AskModeLoc1);
     LcdLoadString("BUTTON1 for hard mode", LCD_FONT_SMALL, &pa_AskModeLoc2);

     if(WasButtonPressed(BUTTON0))
     {
         b_EasyMode = 1;
         RtApp_StateMachine = RtAppSM_RFT;
     }
     if(WasButtonPressed(BUTTON1))
     {
         b_EasyMode = 0;
         RtApp_StateMachine = RtAppSM_RFT;

     }
}






static void RtAppSM_RFT(void)
// Gets the trigger ready, and determines if trigger will be LEDs or a buzzer spoof
{
   LcdClearScreen();
   srand(G_u32SystemTime1ms); // Seeding random number with system time.
   u16RtApp_WaitTime = rand() % (MAX_WAIT + 1 - MIN_WAIT) + MIN_WAIT;
        // Generates a random wait time between min and max times
        // (default 5s and 10s)

   if ( b_EasyMode || u16RtApp_WaitTime % (SPOOF_OODS) != 1) // If it is easy mode, never run Buzzer spoofing
       b_SoundSpoof = 0;
   else b_SoundSpoof = 1; //If in hard mode:  Run Buzzer Spoof 1 in SPOOF_ODDS times

    static PixelAddressType pa_WFTLine1 = {LCD_SMALL_FONT_LINE2, 10};
    static PixelAddressType pa_WFTLine2 = {LCD_SMALL_FONT_LINE4, 10};
    static PixelAddressType pa_WFTLine3 = {LCD_SMALL_FONT_LINE6, 24};
    static PixelAddressType pa_WFTLineP1 = {LCD_SMALL_FONT_LINE3, 30};
    static PixelAddressType pa_WTFTrialLoc = {0, 57};

    sprintf(au8RtApp_TrialStr, "Trial %d of %d", u8RtApp_Trials + 1, NUMBER_OF_TRIALS);
    LcdLoadString(au8RtApp_TrialStr, LCD_FONT_SMALL, &pa_WTFTrialLoc);

    if(b_2Players)
    {
        LcdLoadString("P1 : BUTTON0 ",LCD_FONT_SMALL,&pa_WFTLine1);
        LcdLoadString("P2 : BUTTON1 ",LCD_FONT_SMALL,&pa_WFTLine2);
        LcdLoadString("When LEDs flash",LCD_FONT_SMALL,&pa_WFTLine3);


    }else //SINGLE PLAYER
    {
        LcdLoadString("Press BUTTON0 ",LCD_FONT_SMALL,&pa_WFTLineP1);
        LcdLoadString("When LEDs flash",LCD_FONT_SMALL,&pa_WFTLine3);


    }
    RtApp_StateMachine = RtAppSM_WFT;
    u16RtApp_WaitCount = 0;



}

static void RtAppSM_WFT(void)
// State when waiting for the trigger to begin
{


    if(u16RtApp_WaitCount >= u16RtApp_WaitTime) // When time is up
    {
         // WFR and SpoofReact initialzations
        u16RtApp_ReactTimer = 0;
        u16RtApp_WaitCount = 0;
        b_button0Pressed = 0;
        b_button1Pressed = 0;
        ButtonAcknowledge(BUTTON0);
        ButtonAcknowledge(BUTTON1);

        if (!b_SoundSpoof) //If this trial is not a buzzer spoof, LEDS ON and go to Wait for Reaction state
        {
            RtApp_AllLedsOn();
            RtApp_StateMachine = RtAppSM_WFR;
        }
        else // If this trial is a buzzer spoof, BUZZER ON, and go to Spoof Reaction state
        {
            PWMAudioSetFrequency(BUZZER1,A4);
            u16RtApp_SpoofWait = 0;
            RtApp_StateMachine = RtAppSM_SpoofReact;
            PWMAudioOn(BUZZER1);



        }


    }else
    {
        u16RtApp_WaitCount++;
    }

}

static void RtAppSM_WFR(void)
{


    if(WasButtonPressed(BUTTON0) && b_button0Pressed ==0) //If BUTTON0 was pressed for the first time in this state
    {
        ButtonAcknowledge(BUTTON0);
        u16RtApp_ReactTimeP1 = u16RtApp_ReactTimer;
        b_button0Pressed = 1;
        if(!b_2Players) //If one player, move to next state
        {
            RtApp_AllLedsOff();
            RtApp_StateMachine = RtAppSM_TimeToStr;
        }
    }


    if (b_2Players && WasButtonPressed(BUTTON1) && b_button1Pressed == 0) //If BUTTON1 was pressed for the first time in this state
    {
        ButtonAcknowledge(BUTTON1);
        u16RtApp_ReactTimeP2 = u16RtApp_ReactTimer;
        b_button1Pressed = 1;

    }

    if(b_button0Pressed & b_button1Pressed) //If both buttons have been pressed, move to Time to String state (2-Player case)

    {
        RtApp_AllLedsOff();
        RtApp_StateMachine = RtAppSM_TimeToStr;
    }

    u16RtApp_ReactTimer++;

}


static void RtAppSM_TimeToStr(void)
// Converts the reaction time (in ms) to a char[] in order to display on screen
{
    if(b_2Players)
    {
        sprintf(au8RtApp_TimeStrP1,"P1 : %d.%d seconds",u16RtApp_ReactTimeP1 / 1000, u16RtApp_ReactTimeP1 % 1000);
        sprintf(au8RtApp_TimeStrP2,"P2 : %d.%d seconds",u16RtApp_ReactTimeP2 / 1000, u16RtApp_ReactTimeP2 % 1000);
    }
    else sprintf(au8RtApp_TimeStrP1,"   %d.%d seconds",u16RtApp_ReactTimeP1 / 1000, u16RtApp_ReactTimeP1 % 1000);


    RtApp_StateMachine = RtAppSM_ShowTime;
    LcdClearScreen();
}




static void RtAppSM_ShowTime(void)
//Shows the reaction time on screen
{

    static PixelAddressType pa_ShowP1 = {LCD_SMALL_FONT_LINE2, 10};
    static PixelAddressType pa_ShowP2 = {LCD_SMALL_FONT_LINE4, 10};

    if(b_2Players)
    {
        LcdLoadString(au8RtApp_TimeStrP1, LCD_FONT_SMALL, &pa_ShowP1);
        LcdLoadString(au8RtApp_TimeStrP2, LCD_FONT_SMALL, &pa_ShowP2);
    }else
    {
        LcdLoadString(au8RtApp_TimeStrP1, LCD_FONT_SMALL, &pa_ShowP1);

    }


    u16RtApp_STWait = 0;
    RtApp_StateMachine = RtAppSM_CalcResults;
}

static void RtAppSM_CalcResults(void)
// Clears the results of the previous reaction times after SHOW_WAIT ms, then updates running stats (Total reaction time, average reaction time, number of trials).
{
     u16RtApp_STWait++;
     if(u16RtApp_STWait == SHOW_WAIT)
    {

        LcdClearScreen();

        u8RtApp_Trials++;
        u32RtApp_TotalTimeP1 += u16RtApp_ReactTimeP1;

        if(b_2Players) u32RtApp_TotalTimeP2 += u16RtApp_ReactTimeP2;

        u16RtApp_ReactTimer = 0;

        if(u8RtApp_Trials == NUMBER_OF_TRIALS) //If last trial, calculate average times, and go to results to String state
        {

            u8RtApp_Trials = 0;

            u16RtApp_AvgTimeP1 = u32RtApp_TotalTimeP1/NUMBER_OF_TRIALS;
            u16RtApp_AvgTimeP2 = u32RtApp_TotalTimeP2/NUMBER_OF_TRIALS;

            RtApp_StateMachine = RtAppSM_ResultsToStr;

       }else RtApp_StateMachine = RtAppSM_RFT; // If not last trial, go to Ready for Trigger state
    }

}

static void RtAppSM_ResultsToStr(void)
//Converts the average reaction time (in ms) to a char[] to display on screen
{



     if(b_2Players)
     {
           sprintf(au8RtApp_AvgTimeStrP1,"P1 :   %d.%d seconds",u16RtApp_AvgTimeP1 / 1000, u16RtApp_AvgTimeP1 % 1000);
           sprintf(au8RtApp_AvgTimeStrP2,"P2 :   %d.%d seconds",u16RtApp_AvgTimeP2 / 1000, u16RtApp_AvgTimeP2 % 1000);
     } else   sprintf(au8RtApp_AvgTimeStrP1,"    %d.%d seconds",u16RtApp_AvgTimeP1 / 1000, u16RtApp_AvgTimeP1 % 1000);


     RtApp_StateMachine = RtAppSM_DispResults;

}

static void RtAppSM_DispResults(void)
// Displays final mean reaction times on screen, and determines a winner.
{

    PixelAddressType pa_disp1 = {LCD_SMALL_FONT_LINE0,10};
    PixelAddressType pa_disp3 = {LCD_SMALL_FONT_LINE4-3,0};
    PixelAddressType pa_disp5 = {LCD_SMALL_FONT_LINE7, 12};

    LcdLoadString("Mean reaction time", LCD_FONT_SMALL, &pa_disp1);

    if(b_2Players)
    {

        PixelAddressType pa_disp2 = {LCD_SMALL_FONT_LINE2,0};
        PixelAddressType pa_disp4 = {LCD_SMALL_FONT_LINE5+2,26};

        LcdLoadString(au8RtApp_AvgTimeStrP1, LCD_FONT_SMALL, &pa_disp2);
        LcdLoadString(au8RtApp_AvgTimeStrP2, LCD_FONT_SMALL, &pa_disp3);


        if(u16RtApp_AvgTimeP1 < u16RtApp_AvgTimeP2)

            LcdLoadString("PLAYER 1 WINS!", LCD_FONT_SMALL, &pa_disp4);

        else if(u16RtApp_AvgTimeP1 > u16RtApp_AvgTimeP2)

            LcdLoadString("PLAYER 2 WINS!", LCD_FONT_SMALL, &pa_disp4);

        else LcdLoadString("TIE!", LCD_FONT_SMALL, &pa_disp4);
    }
    else
        LcdLoadString(au8RtApp_AvgTimeStrP1, LCD_FONT_SMALL, &pa_disp3);





    LcdLoadString("BUTTON1 to restart", LCD_FONT_SMALL, &pa_disp5);
    ButtonAcknowledge(BUTTON1);
    RtApp_StateMachine = RtAppSM_WaitRestart;


}


static void RtAppSM_WaitRestart(void)
// Waits for user to restart
{
      if(WasButtonPressed(BUTTON1))
    {
        ButtonAcknowledge(BUTTON1);
    RtApp_StateMachine = RtAppInitialize;
    }

}


/*-------------------------------------------------------------------------------------------------------------------*/
/* SOUND SPOOF STATE MACHINES*/




static void RtAppSM_SpoofReact(void)
// Listens for player reaction after sounding the buzzer. Adds penalties to players that react to buzzer
{

    if(WasButtonPressed(BUTTON0) & (b_button0Pressed ==0)) //If BUTTON0 was pressed for the first time in this state add penalty to Player 1's total time
    {
        b_button0Pressed = 1;
        u32RtApp_TotalTimeP1 += SPOOFPENALTY;
    }
    if (WasButtonPressed(BUTTON1) &(b_button1Pressed ==0)) //If BUTTON1 was pressed for the first time in this state add penalty to Player 2's total time
    {
        b_button1Pressed = 1;
        u32RtApp_TotalTimeP2 += SPOOFPENALTY;
    }

    if (u16RtApp_SpoofWait >= 1000) // After 1000 ms, BUZZER OFF, and check if a button was pressed
    {
        LcdClearScreen();

        PWMAudioOff(BUZZER1);


        if(WasButtonPressed(BUTTON0) | WasButtonPressed(BUTTON1)) // If a button was presssed, go to display spoof message state.
        {
           u16RtApp_SpoofMsgWait = 0;
           RtApp_StateMachine = RtAppSM_SpoofDispMsg;
        }
        else // If neither button was pressed, go to Ready for Trigger state
        {
            RtApp_StateMachine = RtAppSM_RFT;

        }
    }else u16RtApp_SpoofWait++;

    if(u16RtApp_SpoofWait >= BUZZERTIME) // Audio off after 200 ms
          PWMAudioOff(BUZZER1);




}

static void RtAppSM_SpoofDispMsg(void)
//Displays a pentalty message to the user that reacted to a buzzer sound.
{
    PWMAudioOff(BUZZER1);

    static PixelAddressType SpoofDispMsgLoc = {LCD_SMALL_FONT_LINE2, 12};
    static PixelAddressType SpoofDispMsgLoc2 = {LCD_SMALL_FONT_LINE4, 40};
    static PixelAddressType SpoofDispMsgLoc3 = {LCD_SMALL_FONT_LINE5+5, 40};


    LcdLoadString("1 sec PENALTY TO:", LCD_FONT_SMALL, &SpoofDispMsgLoc);

    if(WasButtonPressed(BUTTON0))
    {
        ButtonAcknowledge(BUTTON0);
        LcdLoadString("PLAYER 1", LCD_FONT_SMALL, &SpoofDispMsgLoc2);
    }


    if(WasButtonPressed(BUTTON1) && b_2Players) // 2 PLayer calse
    {
        ButtonAcknowledge(BUTTON1);
        LcdLoadString("PLAYER 2", LCD_FONT_SMALL, &SpoofDispMsgLoc3);
    }


    if (u16RtApp_SpoofMsgWait >= SHOW_WAIT)
    {

        LcdClearScreen();
        u16RtApp_WaitCount = 0;

        RtApp_StateMachine = RtAppSM_RFT;

    }else u16RtApp_SpoofMsgWait++;

}



/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void RtAppSM_Error(void)
{

} /* end RtAppSM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
