If you give input a sample file containing "Yes" or "No". This project can say whether it is "Yes" or "No".

Steps
__________________________________

1. Calculated DC shift taking silence.
2. Calculated normalization factor.
3. Read input file, for each sample subtracted DC shift and multiply the normalization factor.
then calculated energy and ZCR taking 100 samples in a frame.(for this step ignored first 200-300 samples as they majorly contain silence). 
Stored these values in out.txt file.
4. Calculated base energy of the input file taking first 5 frames. Iterated over the calculated energy values to find when there is a 5x increase of energy than base energy to find start of the word. 
Iterated over the calculated energy values(from the end) to find when there is a 5x increase of energy than base energy to find end of the word. 
5.Now checked for how many frames ZCR is greater than or equal to 15 within start and energy range of the input.
If the count is more than 30% of the word then it is yes, else no.


Run
__________________________________
1. Download the project. Run the Assignment2.sln in visual studio 2010. There are few input files present in the given folder. 
You can use those files as input or you can record your own "yes" or "no" using any cool edit software store it as .txt file in the project folder and give it as input.






