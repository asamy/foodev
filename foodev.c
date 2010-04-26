#include <libudev.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  struct udev *udev;
  struct udev_monitor *monitor;

  /* create contexts */
  udev = udev_new();
  monitor = udev_monitor_new_from_netlink(udev, "udev");
  /* enumerate = udev_enumerate_new(udev); */

  /* Listen for events */
  udev_monitor_filter_add_match_subsystem_devtype(monitor, "usb", NULL); 

  /* Listen to events */
  udev_monitor_enable_receiving(monitor);

  printf("Entering poll loop -- ^D to end\n");
  while (1) {
    struct pollfd pfd[2];
    struct udev_device *device;

    /* poll on udev's socket */
    pfd[0].fd = udev_monitor_get_fd(monitor);
    pfd[0].events = POLLIN;


    /* poll on STDIN so we can quit */
    pfd[1].fd = STDIN_FILENO;
    pfd[1].events = POLLIN;

    /* got nothin', keep going */
    if (poll(pfd, 2, -1) < 1)
      continue;

    /* found a udev event */
    if (pfd[0].revents & POLLIN) {
      printf("inside if\n");

      device = udev_monitor_receive_device(monitor);
      if (device == NULL) {
        fprintf(stderr, "DEBUG: device == NULL\n");
        continue;
      }

      printf("%s (%s)\n",
              udev_device_get_action(device),
              udev_device_get_devnode(device));
      udev_device_unref(device);
    }

    /* console input, we're done  */
    if (pfd[1].revents & POLLIN)
      break;
  }

  /* deallocate */
  udev_monitor_unref(monitor);
  udev_unref(udev);

  return 0;

}
