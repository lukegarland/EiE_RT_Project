/**********************************************************************************************************************
File: rt_app.h

----------------------------------------------------------------------------------------------------------------------
To start a new task using this rt_app as a template:
1. Follow the instructions at the top of rt_app.c
2. Use ctrl-h to find and replace all instances of "rt_app" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "RtApp" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "RT_APP" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for rt_app.c

**********************************************************************************************************************/

#ifndef __RT_APP_H
#define __RT_APP_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define NUMBER_OF_TRIALS (u8) 5  // DOES NOT INCLUDE THE SPOOFED TRIALS
#define SPOOF_OODS (u8) 3        // Odds for buzzer spoof (Ex. 1 in SPOOF_OODS)
// All times below are in ms.
#define MAX_WAIT (u16) 10000
#define MIN_WAIT (u16) 4000
#define SHOW_WAIT (u16) 2000
#define SPOOFPENALTY (u16) 1000
#define BUZZERTIME (u16) 200

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void RtAppInitialize(void);
void RtAppRunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/
void RtApp_AllLedsOn(void);
void RtApp_AllLedsOff(void);


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void RtAppSM_Idle(void);

static void RtAppSM_Error(void);
static void RtAppSM_AskMode(void);
static void RtAppSM_RFT(void);          // Ready for Trigger
static void RtAppSM_WFT(void);          // Wait for reaction trigger (sound or LED flash)
static void RtAppSM_WFR(void);          // Wait for the user's Reaction.
static void RtAppSM_TimeToStr(void);
static void RtAppSM_ShowTime(void);     // Shows the reaction timer to the user for 3 seconds
static void RtAppSM_CalcResults(void);  // Calculates mean reaction time
static void RtAppSM_ResultsToStr(void);
static void RtAppSM_DispResults(void);  // Displays Reaction time results
static void RtAppSM_WaitRestart(void);



static void RtAppSM_SpoofReact(void);
static void RtAppSM_SpoofDispMsg(void);


#endif /* __RT_APP_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
