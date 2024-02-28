# kwatchdog-lin
Linux kernel-side watchdog sample

for FreeBSD: https://github.com/olevole/kwatchdog

# Usage

to resets the timer:
```
cat /proc/kwatchdog
```

to set/update timer interval (set 180 seconds instead of 60 (by default)):
```
echo '180' > /proc/kwatchdog
```

or freeze immediately:
```
echo '0' > /proc/kwatchdog
```
