package main

import (
	"fmt"
	"os"
	"time"

	dbus "github.com/godbus/dbus/v5"
	"k8s.io/klog/v2"
)

type dbusInhibiter interface {
	CurrentInhibitDelay() (time.Duration, error)
	InhibitShutdown() (InhibitLock, error)
	ReleaseInhibitLock(lock InhibitLock) error
	ReloadLogindConf() error
	MonitorShutdown() (<-chan bool, error)
	OverrideInhibitDelay(inhibitDelayMax time.Duration) error
}

var systemDbus = func() (dbusInhibiter, error) {
	bus, err := dbus.SystemBus()
	if err != nil {
		return nil, err
	}
	return &DBusCon{SystemBus: bus}, nil
}

func example1() {
	systemBus, err := systemDbus()
	if err != nil {
		klog.Errorf("create dbus failed:%v", err)
		return
	}
	dbusCon := systemBus

	currentInhibitDelay, err := dbusCon.CurrentInhibitDelay()
	if err != nil {
		klog.Errorf("Current Inhibit Delay failed:%v, %v", err, currentInhibitDelay)
		return
	}
	klog.Errorf("Current Inhibit Delay:%v", currentInhibitDelay)
	inhibitlock, err := dbusCon.InhibitShutdown()
	if err != nil {
		klog.Errorf("Inhibit shutdown failed:%v", err)
		return
	}

	events, err := dbusCon.MonitorShutdown()
	if err != nil {
		releaseErr := dbusCon.ReleaseInhibitLock(inhibitlock)
		if releaseErr != nil {
			klog.Errorf(fmt.Sprintf("failed releasing inhibitLock: %v and failed monitoring shutdown: %v", releaseErr, err))
			return
		}
		klog.Errorf(fmt.Sprintf("failed to monitor shutdown: %v", err))
		return
	}
	klog.Infof("start monitor event")
	for {
		select {
		case isShuttingDown := <-events:
			klog.Infof("manager detected new shutdown event, isNodeShuttingDownNow: %t", isShuttingDown)

			if isShuttingDown {
				// Update node status and ready condition
				klog.Infof("manager is shuttingdown")
			} else {
				klog.Infof("not shuttingdown")
			}
		}
	}
}

func simulateWorkload() {
	// Got signal, sleep a bit then exit
	fmt.Println("Preparing to shutdown")
	for i := 0; i < 20; i = i + 1 {
		fmt.Printf(".")
		time.Sleep(time.Second)
	}
	fmt.Println("")
	fmt.Println("All saved, exiting")
	os.Exit(0)
}

// need to tweak InhibitDelayMaxSec= in /etc/systemd/logind.conf
func example2() error {
	conn, err := dbus.SystemBus()
	if err != nil {
		return err
	}
	defer conn.Close()
	var fd int

	obj := conn.Object("org.freedesktop.login1", "/org/freedesktop/login1")
	what := "shutdown"
	who := "test-app"
	why := "because"
	mode := "delay"
	call := obj.Call("org.freedesktop.login1.Manager.Inhibit", 0, what, who, why, mode)
	if call.Err != nil {
		return call.Err
	}
	err = call.Store(&fd)

	f := os.NewFile(uintptr(fd), "inhibit fd")
	defer f.Close()

	err = conn.AddMatchSignal(
		dbus.WithMatchInterface("org.freedesktop.login1.Manager"),
		dbus.WithMatchMember("PrepareForShutdown"),
		dbus.WithMatchObjectPath("/org/freedesktop/login1"),
	)
	if err != nil {
		return err
	}

	c := make(chan *dbus.Signal, 1)
	conn.Signal(c)

	for v := range c {
		r, _ := v.Body[0].(bool)
		if r {
			go simulateWorkload()
		}

	}
	return nil
}

func main() {
	example1()
}
