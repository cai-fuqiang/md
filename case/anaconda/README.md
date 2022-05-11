pyanaconda/modules/network/initialization.py --> SetRealOnbootValuesFromKickstartTask
```
def name(self):
    return "Set real ONBOOT values from kickstart"
```

```
initialize_network
SetRealOnbootValuesFromKickstartWithTask
set_real_onboot_values_from_kickstart_with_task
SetRealOnbootValuesFromKickstartTask
```

```
def run_network_initialization_task(task_path):
    """Run network initialization task and log the result."""
    task_proxy = NETWORK.get_proxy(task_path)
    log.debug("Running task %s", task_proxy.Name)
    sync_run_task(task_proxy)
    result = get_native(task_proxy.GetResult())
    msg = "%s result: %s" % (task_proxy.Name, result)
    log.debug(msg)

def initialize_network():
    """Initialize networking."""
    if not conf.system.can_configure_network:
    ¦   return

    network_proxy = NETWORK.get_proxy()

    msg = "Initialization started."
    log.debug(msg)
    network_proxy.LogConfigurationState(msg)

    log.debug("Devices found: %s",
    ¦   ¦   ¦ [dev.device_name for dev in get_supported_devices()])

    run_network_initialization_task(network_proxy.ConsolidateInitramfsConnectionsWithTask())
    run_network_initialization_task(network_proxy.ApplyKickstartWithTask())
    run_network_initialization_task(network_proxy.DumpMissingIfcfgFilesWithTask())
    run_network_initialization_task(network_proxy.SetRealOnbootValuesFromKickstartWithTask())
```

```
def SetRealOnbootValuesFromKickstartWithTask(self) -> ObjPath:
¦   """Update ifcfg ONBOOT values according to kickstart configuration.

¦   So it reflects the --onboot option.

¦   This is needed because:
¦   1) For ifcfg files created in initramfs we use ONBOOT for --activate
¦   2) For kickstart applied in stage 2 we can't set the autoconnect
¦   ¦  setting of connection because the device would be activated immediatel

¦   :returns: DBus path of the task setting the values
¦   """
¦   return TaskContainer.to_object_path(
¦   ¦   self.implementation.set_real_onboot_values_from_kickstart_with_task()
¦   )

    def set_real_onboot_values_from_kickstart_with_task(self):
    ¦   """Update ifcfg ONBOOT values according to kickstart configuration.

    ¦   So it reflects the --onboot option.

    ¦   This is needed because:
    ¦   1) For ifcfg files created in initramfs we use ONBOOT for --activate
    ¦   2) For kickstart applied in stage 2 we can't set the autoconnect
    ¦   ¦  setting of connection because the device would be activated immediately.

    ¦   :returns: a task setting the values
    ¦   """
    ¦   supported_devices = [dev_info.device_name for dev_info in self.get_supported_devices()]
    ¦   task = SetRealOnbootValuesFromKickstartTask(self.nm_client,
    ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   self._original_network_data,
    ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   supported_devices,
    ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   self.bootif,
    ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   ¦   self.ifname_option_values)
    ¦   task.succeeded_signal.connect(lambda: self.log_task_result(task, check_result=True))
    ¦   return task

class SetRealOnbootValuesFromKickstartTask(Task):
    """Task for setting of real ONBOOT values from kickstart."""

    def __init__(self, nm_client, network_data, supported_devices, bootif, ifname_option_values):
    ¦   """Create a new task.

    ¦   :param nm_client: NetworkManager client used as configuration backend
    ¦   :type nm_client: NM.Client
    ¦   :param network_data: kickstart network data to be applied
    ¦   :type: list(NetworkData)
    ¦   :param supported_devices: list of names of supported network devices
    ¦   :type supported_devices: list(str)
    ¦   :param bootif: MAC addres of device to be used for --device=bootif specification
    ¦   :type bootif: str
    ¦   :param ifname_option_values: list of ifname boot option values
    ¦   :type ifname_option_values: list(str)
    ¦   """
    ¦   super().__init__()
    ¦   self._nm_client = nm_client
    ¦   self._network_data = network_data
    ¦   self._supported_devices = supported_devices
    ¦   self._bootif = bootif
    ¦   self._ifname_option_values = ifname_option_values

    @property
    def name(self):
    ¦   return "Set real ONBOOT values from kickstart"

    def for_publication(self):
    ¦   """Return a DBus representation."""
    ¦   return NetworkInitializationTaskInterface(self)

    @guard_by_system_configuration(return_value=[])
    def run(self):

```
