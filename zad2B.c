#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
int numCh;
int fd[2*numCh][2]; //parent+child pipe
int i, j, len, fpos=0, val, count=0, total=0;
pid_t pid;
int nums = 1000;
FILE * file;

printf("How many children to use: ");
scanf("%d", &numCh);
printf("\nWill use %d child process(es).\n", numCh);

// create all pipes
for (i=0; i<2*numCh; i++)
{
    pipe(fd[i]);
}

for (i=0; i<numCh; i++)
{
    if((pid = fork()) == 0) // child process
    {
        pid = getpid();

        // read from parent
        len = read(fd[i][0], &fpos, sizeof(fpos));
        if (len > 0)
        {
            file = fopen("file1.dat", "r");
            fseek (file, fpos, SEEK_SET);
            count = 0;
            total = 0;

            printf("Child(%d): Recieved position: %d\n", pid, fpos);

            // read from file starting at fpos
            // add values read to a total value
            while (count < (nums/numCh))
            {
                fscanf(file, "%i", &val);
                total += val;
                count++;
            }
            //write to parent
            write(fd[i+numCh][1], &total, sizeof(total));
            printf("Child(%d): Sent %d to parent.\n", pid, total);
        }
        else
        {
            printf("Child(%d): Error with len\n", pid);
        }

        _exit;
    }

    // parent process
    pid = getpid();

    fpos = ((i*nums*5)/numCh); // 5 is the offset of the file values

    // write to child process
    printf("Parent(%d): Sending file position to child\n", pid);
    write(fd[i][1], &fpos, sizeof(fpos));

    // wait for child responce
    len = read(fd[i+numCh][0], &total, sizeof(total));
    if (len > 0)
    {
        printf("Parent(%d): Recieved %d from child.\n", pid, total);
        total += total;
        printf("Parent(%d): Total: %d\n", pid, total);
    }
    else
    {
        printf("Parent(%d): Error with len\n", pid);
    }
}
}
