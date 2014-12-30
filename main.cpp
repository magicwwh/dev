#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int core[2];
static unsigned long trials;
static void* hijack_addr;
static volatile int hijacked = 0;
cpu_set_t cpuset;



void *ChildThread(void *retaddr_ptr)
{
    // get the current thread.
    pthread_t child_thread = pthread_self();

    // confine the child thread to Core #2
    //    CPU_ZERO(&cpuset);
    //CPU_SET(core[1], &cpuset);
    int s = pthread_setaffinity_np(child_thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
        abort();

    // Repeatedly write <hijack_addr> into the main thread's return address slot.
    // The write is attempted <trials> times.
    asm volatile ("movl %0, %%eax"
                  :
                  : "m" (retaddr_ptr)
                  :);
    asm volatile ("movl %0, %%ebx"
                  :
                  : "m" (hijack_addr)
                  : "eax");
    asm volatile ("movl %0, %%ecx"
                  :
                  : "m" (trials)
                  : "eax", "ebx");
    asm volatile ("L: movl %ebx, (%eax)\n\t"
                  "sfence\n\t"
                  "loop L");

    // If the hijack was successful, the main thread sets the global "hijacked" variable to 1;
    if (!hijacked)
    {
        printf("All trials complete. Hijack unsuccessful.\n");
        fflush(stdout);
        exit(0);
    }
    return NULL;
}


int main()
{
    int s, i, j;
    pthread_t thread1, thread2;
    unsigned int cores_available = 0;
    void* esp_addr = malloc(sizeof(void*));

    thread1 = pthread_self();

    /* Set affinity mask to include CPUs 0 to 4 */
    /*
    CPU_ZERO(&cpuset);
    for (j = 0; j < 2; j++)
        CPU_SET(j, &cpuset);

    s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
        abort();
    */

    // check the actual affinity mask assigned to the thread

    s = pthread_getaffinity_np(thread1, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
        abort();

    printf("Set returned by pthread_getaffiity_np() contains:\n");
    for (j = 0; j < CPU_SETSIZE; j++)
    {
        if (CPU_ISSET(j, &cpuset))
        {
            printf("\tCPU %d\n", j+1);
            ++cores_available;
        }
    }

    // Ask the user to select two available cores.
    core[0]= core[1] = 0;
    if (cores_available > 2)
        for (i=0; i < 2; ++i)
            while (1)
            {
                printf("Select core #%u: ", i+1);
                scanf("%u", &core[i]);
                core[i]--;
                if (!CPU_ISSET(core[i], &cpuset))
                    printf("Core %u not available. Choose another.\n", core[i]+1);
                else
                {
                    CPU_CLR(core[i], &cpuset);
                    break;
                }
            }
    else
        for (i=0; i<2; ++i)
            {
                core[i] = i;
                CPU_CLR(core[i], &cpuset);
                printf("Auto-selected core #%u = %u\n", i+1, core[i]+1);
            }

    fflush(stdout);
    if (cores_available < 2)
    {
        printf("Not enough cores to run a multicore test.\n");
        return 1;
    }

    // Ask the user to specify how many times the child thread should attempt to
    // hijack the main thread.  I find that 100 trials is usually enough when
    // running both threads on the same physical core (different virtual cores),
    // and 100000 is usually enough when running on separate cores.
    printf("Enter number of trials:\n");

    scanf("%lu", &trials);


    asm volatile ("movl $HIJACK, %0"
                  : "=r" (hijack_addr)
                  :
                  :);
    asm volatile ("mfence");
    asm ("clflush %0" : : "m" (hijack_addr));
    //    asm volatile ("leal -4(%%esp), %%eax"
    //asm volatile  "movl %%eax, %0"
    //              : "=r" (esp_addr)
    //              :
    //              : "a");

    asm volatile ("leal -4(%%esp), %%eax"
                  : "=a" (esp_addr)
                  :
                  :);


    int rc =  pthread_create(&thread2, NULL, ChildThread, esp_addr);
    if (rc) abort();

    // confine the main thread to Core #1
    CPU_ZERO(&cpuset);
    CPU_SET(core[0], &cpuset);
    s = pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpuset);
    if (s != 0)
       abort();


    // Repeat a guarded return in the tightest possible infinite loop.
    asm volatile ("movl $L2, %eax");
    asm volatile ("L1: call L3");
    asm volatile ("L2: jmp L1");
    asm volatile ("L3: movl %eax, (%esp)");

    asm volatile ("ret");

    printf("Infinite loop ended normally---impossible!\n");
    //abort();

    // If the hijack is successful, the "ret" above will jump here.
    asm volatile ("HIJACK:");

    hijacked = 1;
    asm volatile ("mfence");    // Commit the "hijacked=1" to main memory.
    asm ("clflush %0" : : "m" (hijacked));
    asm volatile ("push $0");    // The child thread might still be running, so push a dummy return
                                     // address for it to continue hijacking.
    printf("Hijack successful!\n");
    fflush(stdout);
    s = pthread_join(thread2, NULL);    // Wait for the child thread to terminate normally.
    if (s != 0)
        abort();
    asm volatile ("pop %eax");    // Pop the dummy return address.

    return 0;
}
