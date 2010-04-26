/* Copyright (c) 2010 <d@falconindy.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <libudev.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void udev_device_add_event(struct udev_device*);
void udev_device_remove_event(struct udev_device*);

int main(void) {
  struct udev *udev;
  struct udev_monitor *monitor;

  /* create contexts */
  udev = udev_new();
  monitor = udev_monitor_new_from_netlink(udev, "udev");

  /* Listen for events */
  udev_monitor_filter_add_match_subsystem_devtype(monitor, "usb", NULL); 
  udev_monitor_enable_receiving(monitor);

  printf("Entering poll loop -- ^D to end\n");
  while (1) {
    struct pollfd pfd[2];
    struct udev_device *device;

    /* poll on udev's socket */
    pfd[0].fd = udev_monitor_get_fd(monitor);
    pfd[0].events = POLLIN;

    /* poll on STDIN so we can quit on key input */
    pfd[1].fd = STDIN_FILENO;
    pfd[1].events = POLLIN;

    /* block until we get an event */
    if (poll(pfd, 2, -1) < 1)
      continue;

    /* If we're here, we received an event. Find out who sent it by checking
     * each struct pollfd for revents
     */

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

    /* input on STDIN, we're done  */
    if (pfd[1].revents & POLLIN)
      break;
  }

  /* deallocate */
  udev_monitor_unref(monitor);
  udev_unref(udev);

  return 0;

}

void udev_device_add_event(struct udev_device *device) {
  printf("in udev_device_add_event\n"); /* hello! */

  const char *product = udev_device_get_sysattr_value(device, "product");
  const char *driver = udev_device_get_driver(device);
  const char *devnode = udev_device_get_devnode(device);

  printf("Dev Node: %s\n", devnode);
  printf("Product: %s\n", product);
  printf("Driver: %s\n", driver);

}

void udev_device_remove_event(struct udev_device *device) {
  printf("in udev_device_remove_event\n"); /* hello! */
}
