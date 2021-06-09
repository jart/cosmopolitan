#include <nwthread.h>
#include <nwerrno.h>

/*******************************/
/*  Interupt handler		   */
/*******************************/

int NLM_mainThreadGroupID;
int NLM_threadCnt = 0;
int NLM_exiting = FALSE;

#pragma off(unreferenced);
void NLM_SignalHandler(int sig)
#pragma on(unreferenced);
{
	int handlerThreadGroupID;

	switch(sig)
	{
	case SIGTERM:
		NLM_exiting = TRUE;
		handlerThreadGroupID = GetThreadGroupID();
		SetThreadGroupID(NLM_mainThreadGroupID);

		/* NLM SDK functions may be called here */

		while (NLM_threadCnt != 0) 
			ThreadSwitchWithDelay();
		SetThreadGroupID(handlerThreadGroupID);
		break;
	case SIGINT:
		signal(SIGINT, NLM_SignalHandler);
		break;
	}
	return;
}

void NLMsignals(void)
{
    ++NLM_threadCnt;
    NLM_mainThreadGroupID = GetThreadGroupID();
    signal(SIGTERM, NLM_SignalHandler);
    signal(SIGINT, NLM_SignalHandler);
}

void NLMexit(void)
{
   --NLM_threadCnt;
}
