#!/bin/bash
# Enable P2P specific ACS bits on every device that supports it
PLATFORM=$(dmidecode --string system-product-name)
logger "PLATFORM=${PLATFORM}"

# must be root to access extended PCI config space
if [ "$EUID" -ne 0 ]; then
  echo "ERROR: $0 must be run as root"
  exit 1
fi

for BDF in `lspci -d "*:*:*" | awk '{print $1}'`; do
  # skip if it doesn't support ACS
  setpci -v -s ${BDF} ECAP_ACS+0x6.w > /dev/null 2>&1
  if [ $? -ne 0 ]; then
  # echo "${BDF} does not support ACS, skipping"
    continue
  fi
  logger "Enabling ACS on `lspci -s ${BDF}`"
  setpci -v -s ${BDF} ECAP_ACS+0x6.w
  setpci -v -s ${BDF} ECAP_ACS+0x6.w=0x5D
  setpci -v -s ${BDF} ECAP_ACS+0x6.w
  if [ $? -ne 0 ]; then
    logger "Error enabling ACS on ${BDF}"
    continue
  fi
  NEW_VAL=`setpci -v -s ${BDF} ECAP_ACS+0x6.w | awk '{print $NF}'`
  if [ "${NEW_VAL}" != "0x5D" ]; then
    logger "Failed to Enable ACS on ${BDF}"
    continue
  fi
done
exit 0
