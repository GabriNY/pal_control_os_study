#!/usr/bin/env bash
rm -f /tmp/os_exam_fifo /tmp/pair_* /tmp/triple_* /tmp/quad_* /tmp/os_exam_all_fifo 2>/dev/null || true
# Remove SysV message queues owned by current user. Use with care in a lab VM/WSL.
if command -v ipcs >/dev/null && command -v ipcrm >/dev/null; then
  ipcs -q | awk -v u="$USER" '$3==u {print $2}' | while read -r id; do ipcrm -q "$id" 2>/dev/null || true; done
fi
