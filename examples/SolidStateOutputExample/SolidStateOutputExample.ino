
#include "SolidStateOutput.h"

unsigned long OnTime = 3000;
unsigned long OffTime = 3000;

int CycleSeq = 0;
unsigned long OnTimeStart = 0;
unsigned long OffTimeStart = 0;

SolidStateOutput S1(4);
SolidStateOutput S2(5, 1000, 100, 30);
SolidStateOutput S3(6, 1000, 0, 100, 1000);

void setup()
{

  /* add setup code here */


}

void loop()
{
	
	S1.Process();
	S2.Process();
	S3.Process();

  /* add main program code here */

	switch (CycleSeq)
	{
	case 0:

		S1.State = true;
		S2.State = true;
		S3.State = true;
		OnTimeStart = millis();
		CycleSeq = 1; 

		break;
	case 1:
		if (millis() - OnTimeStart >= OnTime)
		{
			S1.State = false;
			S2.State = false;
			S3.State = false;
			OffTimeStart = millis();
			CycleSeq = 2;
		}
		break;
	case 2:
		if (millis() - OffTimeStart >= OffTime)
		{
			CycleSeq = 0;
		}
		break;
	}

}

