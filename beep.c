#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <math.h>

// works !
// source : https://zserge.com/posts/beep/
// gcc -o beep beep.c -lm -lasound && valgrind --leak-check=full --show-leak-kinds=all ./beep 2> error.txt

//int beep(int freq, int ms) {
int main() {
  
    float freq = 440.0;
    int dur = 800; // = number of samples

    float acumulator_duration = 0;
    int number_of_tests = 2;

    snd_pcm_t *pcm = NULL;

    if (pcm == NULL) {
        if (snd_pcm_open(&pcm, "default", 0, 0)) {
            return -1;
        }
        snd_pcm_set_params(pcm, 1, 3, 1, 8000, 1, 10000);
    }

    // min = 768
    unsigned char buf[dur]; // seems to be the duration 
    /*
    (environ)
    768 = 64 ou après d'autres tests 56 ms, 55936.421875 us, ou 56304.300781 us exactement avec 1000 iterations, 56210.191406 us avec 100
    1000 = 80 ms, ou 70575.367188 us avec 1000 iterations, 71499.398438 us avec 100
    2000 = 210 ms, ou 197221.296875 us. avec 1000 iterations, 197107.593750 us avec 100 itération
    10000 = 1200 ms, 1202361.250000 us. avec 744 iterations, 1200505.875000 us avec 100 itération
    // hypothèse : plus la carte est solicitée, plus le nombre de second par éléments augmente

    20000 = 2455 ms 2451018.250000 us with 100 tests
    76800 = 9556 ms 9547842.000000 us with 100 tests
    */

    float amplitude = 127.0;
    float two_pi = 6.28318530718;
    float pi = 3.14159265359;

    for (int test_iterator = 0; test_iterator < number_of_tests; test_iterator++) {
        snd_pcm_prepare(pcm);
        //struct timeval stop, start;
        struct timespec start, end;

        /*
        for (int i = 0; i < sizeof(buf); i++) buf[i] = rand();
        for (int t=0; t < 8000; t++) {
            int i = t % sizeof(buf);
            int j = (t+1) % sizeof(buf);
            buf[i] = (buf[i] + buf[j])/2;
        }
        */
        
        for (int t = 0; t < sizeof(buf); t++) {
            //gettimeofday(&start, NULL);
            //clock_gettime(CLOCK_MONOTONIC_RAW, &start);

            if (freq > 0) {
                //float w = two_pi * freq;
                //float wt = w * t;
                //buf[t] = amplitude * (t * freq / 8000); // original

                // with the help of :
                // https://zserge.com/posts/etude-in-c/
                //buf[t] = amplitude * (sin(wt/255.0*2*3.14) + 128); // sound without noise
                // after playing around
                //buf[t] = amplitude * (sin((t * freq) / (255.0*two_pi)) + 128); // sound without noise

                //float x = t * freq;
                // https://dsp.stackexchange.com/questions/46598/mathematical-equation-for-the-sound-wave-that-a-piano-makes
                //buf[t] = amplitude * (-14 * sin(3 * pi * x) + 14 * sin(pi * x) + (sqrt(3) / 2 * cos(pi * x)));

                buf[t] = amplitude * (sin((t * freq) / (255.0*two_pi)) + 128);

                //buf[t] += amplitude * ((sin((t * freq) / (255.0*two_pi)) + 128)/2);
                //buf[t] += amplitude * (sin((3 * t * freq) / (255.0*two_pi)) + 128);
                //buf[t] += amplitude * (sin((4 * t * freq) / (255.0*two_pi)) + 128);
                //buf[t] += amplitude * (sin((5 * t * freq) / (255.0*two_pi)) + 128);
                //buf[t] += amplitude * (sin((6 * t * freq) / (255.0*two_pi)) + 128);

                //buf[t] *= buf[t];
            }
            
            else {
                buf[t] = 0;
            }

            //buf[t] = freq > 0 ? (255 * t * freq / 8000) : 0;

            //gettimeofday(&stop, NULL);
            //clock_gettime(CLOCK_MONOTONIC_RAW, &end);


            //printf("took %lu us\n", (&stop - &start) * 1000000 + stop.tv_usec - start.tv_usec); 
            //printf("took %lu us\n", (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000);
            //buf[t] = freq; //> 0 ? (300 * t * freq / 8000) : 0;
        }

        //clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        //gettimeofday(&start, NULL);
        int r = snd_pcm_writei(pcm, buf, sizeof(buf)); // the sound is created here
        //gettimeofday(&stop, NULL);
        //clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        //float duration = (float)((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000);

        //printf("took %f us\n", duration);

        //printf("took %lu us\n", (&stop - &start) * 1000000 + stop.tv_usec - start.tv_usec); 
        //acumulator_duration = acumulator_duration + duration;


        //printf("r = %d\n", r);

        //printf("time / r = %f us per sample\n", duration / r);

        if (r < 0) {
            snd_pcm_recover(pcm, r, 0);
        }
    
        //snd_pcm_close(pcm);
        
        //float average_time = acumulator_duration / (test_iterator + 1);

        //printf("\nAfter %d iterations, the average time is : %f us.\n", test_iterator, average_time);
        //printf("\nAfter %d iterations, the average time is : %f us per sample.\n", test_iterator, average_time / dur);
    }
    
    // freeing pcm
    snd_pcm_close(pcm);
    snd_config_update_free_global();

    //float average_time = acumulator_duration / number_of_tests;

    //printf("\nThe average time is : %f us.\n", average_time);
    //printf("\nThe average time is : %f us per sample.\n", average_time / dur);


    return 0;
}
