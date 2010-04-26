#include <libudev.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void udev_device_add_event(struct udev_device*);
void udev_device_remove_event(struct udev_device*);

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

      device = udev_monitor_receive_device(monitor);
      if (device == NULL) {
        fprintf(stderr, "DEBUG: device == NULL\n");
        continue;
      }

      const char *action = udev_device_get_action(device);
      if (strcmp(action, "add") == 0)
        udev_device_add_event(device);
      else if (strcmp(action, "remove") == 0)
        udev_device_remove_event(device);

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

void udev_device_add_event(struct udev_device *device) {
  printf("in udev_device_add_event\n");
  const char *product = udev_device_get_sysattr_value(device, "product");
  const char *driver = udev_device_get_driver(device);
  const char *devnode = udev_device_get_devnode(device);

  printf("Dev Node: %s\n", devnode);
  printf("Product: %s\n", product);
  printf("Driver: %s\n", driver);

}

void udev_device_remove_event(struct udev_device *device) {
  printf("in udev_device_remove_event\n");
}
