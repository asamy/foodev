#include <libudev.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
  struct udev *udev;
  struct udev_monitor *monitor;

  /*
  struct udev_enumerate *enumerate;
  struct udev_list_entry *list_entry;
  */

  /* create a new context of udev and get a monitor ref */
  udev = udev_new();
  monitor = udev_monitor_new_from_netlink(udev, "udev");

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
      const char *cap;

      device = udev_monitor_receive_device(monitor);
      if (device == NULL)
        continue;

      cap = udev_device_get_property_value(device, "ID_VENDOR");
      if (cap == NULL)
        continue;

      printf("%s (%s) (%s)\n",
        udev_device_get_property_value(device, "ID_VENDOR"),
        udev_device_get_devnode(device),
        udev_device_get_action(device));
      udev_device_unref(device);
    }

    /* console input, we're done */
    if (pfd[1].revents & POLLIN)
      break;
  }

  /* deallocate */
  udev_monitor_unref(monitor);
  udev_unref(udev);

  return 0;

}
