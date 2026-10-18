
1. 
there is a proplem in the enum : Timer2_Prescaler_t
there are added numbers like _32 and _128 which are not even in the datasheet
so when you apply the number presalcer _1024 it was number 7  1 1 1 which is wrong
i changed it to number 5 which is 1 0 1 which is the correct one 
note : THIS IS WRONG ITS NOT LIKE IN THE DATASHEET TABLE 68 ITS 1 1 1 for 1024 

2. 

i set the CTC mode last thing cause i see if i run in ctc mode
there is a chance that the code runs with no OCR2 so there a cycle that happens that
didnt start from the 151 but from 0 
so to be safe i start it at the end

3. 

in the fucntion <mTimer2_DelayMs> in the description there are asked 2 things
start time and curret time and both ask the same fucntion so there is no reason for 2

4. 

in fucntion <mTimer2_ResetTick> i add also the <g_subSecondTicks> as 
its not asked in desicription