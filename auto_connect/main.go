package main

import (
	"container/list"
	"fmt"
	"github.com/xthexder/go-jack"
	"strings"
	"time"
)

var Client *jack.Client
var Queue *list.List

func connect_to(tport string) (port string, rev bool) {
	port = ""
	rev = false
	x := strings.Split(tport, ":")
	switch x[0] {
	case "jack_mixer":
		switch x[1] {
		case "MAIN L":
			port = "system:playback_1"
		case "MAIN R":
			port = "system:playback_2"
		case "capture L":
			port = "system:capture_1"
			rev = true
		case "capture R":
			port = "system:capture_2"
			rev = true
		}
	case "nekobee DSSI plugin":
		port = "jack_mixer:nekobee"
	case "ffmpeg":
		switch x[1] {
		case "input_1":
			port = "jack_mixer:MAIN L"
			rev = true
		case "input_2":
			port = "jack_mixer:MAIN R"
			rev = true
		}
	case "alsa_in":
		switch x[1] {
		case "capture_1":
			port = "jack_mixer:alsa L"
		case "capture_2":
			port = "jack_mixer:alsa R"
		}
	default:
		port = "jack_mixer:" + x[0]
	}
	return
}

func portlist(tport string) {
	connect_to, rev := connect_to(tport)
	ports := Client.GetPorts("", "", 0)
	for _, port := range ports {
		if port == connect_to {
			fmt.Println("match", tport, port)
			if rev {
				Queue.PushBack([]string{port, tport})
			} else {
				Queue.PushBack([]string{tport, port})
			}
		}
	}
}

func update(id jack.PortId, st bool) {
	if st {
		port := Client.GetPortById(id)
		portlist(port.String())
	}
}

func main() {
	client, _ := jack.ClientOpen("autoconnect", jack.NoStartServer)
	if client == nil {
		panic("connect error")
	}
	defer client.Close()
	Client = client
	portlist("")
	client.SetPortRegistrationCallback(update)
	if code := client.Activate(); code != 0 {
		panic("activate error")
	}
	Queue = list.New()
	for {
		time.Sleep(time.Millisecond * 250)
		v := Queue.Front()
		if v != nil {
			Queue.Remove(v)
			x, _ := v.Value.([]string)
			fmt.Println("get", x)
			Client.Connect(x[0], x[1])
		}
	}
}
