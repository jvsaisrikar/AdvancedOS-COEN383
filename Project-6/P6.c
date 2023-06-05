#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>  // rand(), srand()
#include <time.h> 

#define RUN_DUR 30 //run for 30 seconds
#define READ_END 0
#define WRITE_END 1
#define SI 1500
#define DI 1000

void run_ch(int child_no, int pip[], struct timeval* st,
	       struct timeval* end, struct timeval* curr) {

   double timeElapsed = 0; // Used to track time for the child is alive
   char msg[SI];           

   srand(time(NULL) + child_no - 1); // seed for the random number generator
   close(pip[2*(child_no - 1)]);     

   // msg counter
   int msg_snt = 1;
      
   while(timeElapsed<RUN_DUR)
   {
      //usleep(rand() % 100000 + 1);
      // ^ use for testing different microsecond values; otherwise
      // they are pretty much zero.

      gettimeofday(end, NULL);
      timersub(end, st, curr);

      // copy 
      sprintf(msg, "%ld.%03ld: Child %d message %d\n",
	      curr->tv_sec, curr->tv_usec/1000, child_no, msg_snt);

      // Write msg o/p to the appropriate pipe. Exclude the null terminator.
      write(pip[2*(child_no-1) + 1], msg, strlen(msg));

      // Sleep for 0, 1, or 2 seconds
      int sleep_time = rand() % 3;
      sleep(sleep_time); //sleep process for 0,1,2 
      timeElapsed += sleep_time;

      // Maintain msg counter
      msg_snt++;
   }

   printf("Exiting child %d\n", child_no);
   
   
   close(pip[2*(child_no-1) + 1]);

   // close the process
   exit(0);	
}

int max_fd(int pip[], int ll) {
   int max_fd = 0;
   for (int i = 0; i < ll; i++) {
      if (pip[i] > max_fd) max_fd = pip[i];
   }
   return max_fd;
}

int main()
{
   int i = 0;
   struct timeval t_v, st, end, curr;
   int pip[10];
   //srand(time(NULL));	
	
   for(i=0; i<5; i++) 
   {
      pipe(pip + 2*i);
   }
		
   gettimeofday(&st, NULL);

   int pid;
   // CHILDREN CODE: The parent will ignore this because fork() never equals 0 for it.
   for (int i = 0; i < 5; i++) {
      if ((pid = fork()) == 0) {
	 // Code for child 5
   	 if (i == 4) {
   	    // Use select to timeout user input after so many
   	    // seconds
   	    char msg[SI];
   	    char tmp[DI];
   	    while (true) {
   	       memset(msg, 0, SI);
   	       fd_set set;
   	       FD_ZERO(&set);
   	       FD_SET(0, &set);

   	       gettimeofday(&end, NULL);
   	       timersub(&end, &st, &curr);
		     
   	       t_v.tv_sec = RUN_DUR - curr.tv_sec;
   	       t_v.tv_usec = 0;

   	       printf("Waiting %ld seconds for i/p: \n", t_v.tv_sec);

   	       int ready = select(1, &set, NULL, NULL, &t_v);

   	       if (ready == 0 || t_v.tv_sec == 0)
   		  break;
   	       else {
   		  if (FD_ISSET(0, &set)) {
   		     if (fgets(tmp, SI, stdin)) {
   			gettimeofday(&end, NULL);
   			timersub(&end, &st, &curr);

   			snprintf(msg, sizeof(tmp),"%ld.%03ld: Child %d message printing = %s",
   				curr.tv_sec, curr.tv_usec/1000, 5, tmp);

   			write(pip[9], msg, strlen(msg) + 1);
   		     }
		  }
   	       }
   	    }

	    // memset(msg, 0, 1);
	    // write(pip[9], msg, 1);
	    printf("Exiting child 5\n");
	    close(pip[9]);
   	    exit(0);
   	 }
	 // Code for children 1 - 4
   	 else {
   	    run_ch(i+1, pip, &st, &end, &curr);
	    exit(0);
   	 }
      }
   }

   // PARENT CODE:
   // Prepare to output to the file output.txt
   FILE* o_file = fopen("./output.txt", "w");
   fclose(o_file);
		  
   o_file = fopen("./output.txt", "a");
   int n = 0;
		  
   // Prepare select
   fd_set set;
   FD_ZERO(&set);
   for (int i = 0; i < 5; i++) {
      // Add read file descriptors to the set
      FD_SET(pip[2*i], &set);
      close(pip[2*i + 1]);
   }

   // Prepare select timeout
   t_v.tv_sec = RUN_DUR;
   t_v.tv_usec = 0;

   while (true) {
      char msg[SI];
      select(max_fd(pip, 10), &set, NULL, NULL, NULL /*&tv*/);
      int ch_exited = 0;
      
      // Check all fd's
      for (int i = 0; i <= 8; i+=2) {
	 if (FD_ISSET(pip[i], &set)) {
	    memset(msg, 0, SI);
	    // Write msgs to output.txt
	    n = read(pip[i], msg, sizeof(msg));

	    // If read returns 0 then we know a child has sent it's last message and will now be exiting.
	    if (n == 0) {
	       ch_exited++;
	       // Terminate the parent once all 5 children are done executing
	       if (ch_exited == 5) {
		  printf("Exiting parent now\n");
		  fclose(o_file);
		  exit(0);
	       }
	    }
	    else {
	       // Write msg to file
	       fprintf(o_file, "%s", msg);

	       // Write msg to stdout
	       printf("%s", msg);
	    }
	 }
      }

      // Re-prepare select
      FD_ZERO(&set);
      for (int i = 0; i < 5; i++) {
	 FD_SET(pip[2*i], &set);
      }
   }
   
	
   return 0;
}
