//Function called by main program for square root using launch tasks

task void calulateRoot_launchTasks(uniform double times, uniform float input[], uniform float output[], uniform float firstGuess, uniform int span){

    uniform int start = taskIndex * span;
    uniform int end = min((taskIndex+1) * span, (unsigned int)times);	
    foreach(i = start ... end){
        
        double x1 = -1.0;
        double x0 = -1.0;

        x0 = firstGuess;
        do{
            x1 = (x0 - (((x0 * x0) - input[i]) / (2 * x0)));
            x0 = x1;
        }while((x1 * x1) - input[i] > 1e-4);

        output[i] = x1;
    }
}

export void calulateRoot_ispc_tasks(uniform double times, uniform float input[], uniform float output[], uniform float firstGuess, uniform int nbThreads){
   uniform int span = (20e6)/nbThreads;

   launch[times/span] calulateRoot_launchTasks(times, input, output,firstGuess,span);
}