//Function called by main program for square root using ISPC (single core)

export void calulateRoot_ispc(uniform double times, uniform float input[], uniform float output[], uniform float firstGuess){
    foreach(i = 0 ... times){
        
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
