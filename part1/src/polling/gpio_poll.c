#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>

int main() {
    // Enable gpio17 and gpio27
    int fd_export = open("/sys/class/gpio/export", O_WRONLY);
    write(fd_export, "17", 2);
    write(fd_export, "27", 2);
    close(fd_export);

    // Set gpio27 as input and gpio17 as output
    int fd_direction27 = open("/sys/class/gpio/gpio27/direction", O_WRONLY);
    int fd_direction17 = open("/sys/class/gpio/gpio17/direction", O_WRONLY);
    write(fd_direction27, "in", 2);
    write(fd_direction17, "out", 3);
    close(fd_direction27);
    close(fd_direction17);

    // Set gpio27 interrupt
    int fd_edge = open("/sys/class/gpio/gpio27/edge", O_WRONLY);
    write(fd_edge, "both", 4);  // Trigger on both rising and falling edges
    close(fd_edge);

    struct pollfd pfd;
    int fd_value27 = open("/sys/class/gpio/gpio27/value", O_RDONLY);
    pfd.fd = fd_value27;
    pfd.events = POLLPRI;  // We are interested in urgent data (edge triggered)
    
    for(int i=0; i<20000; i++) {
        lseek(fd_value27, 0, SEEK_SET);  // Reset the file pointer due to the read
        char buf[2] = {0};  // Initialize buffer to store the read value
        read(fd_value27, buf, 2);  // Dummy read to clear the interrupt
        
        int ret = poll(&pfd, 1, -1);  // Wait indefinitely for an event
        if(ret > 0) {
            lseek(fd_value27, 0, SEEK_SET);  // Reset the file pointer for a new read
            read(fd_value27, buf, 2);  // Read the new value
            int fd_value17 = open("/sys/class/gpio/gpio17/value", O_WRONLY);
            write(fd_value17, buf, 1);  // Copy the state to the output pin
            close(fd_value17);
        } else {
            printf("Error polling gpio27.\n");
        }
    }

    close(fd_value27);

    // Disable gpio17 and gpio27
    int fd_unexport = open("/sys/class/gpio/unexport", O_WRONLY);
    write(fd_unexport, "17", 2);
    write(fd_unexport, "27", 2);
    close(fd_unexport);

    return 0;
}
