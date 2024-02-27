# kwatchdog-lin
Linux kernel-side watchdog sample

for FreeBSD: https://github.com/olevole/kwatchdog

# Usage

resets the timer:
```
cat /proc/kwatchdog
```

set new interval:
```
echo "180" > /proc/kwatchdog
```
