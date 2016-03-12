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
	if strings.Index(x[0], "audacious-") == 0 {
		switch x[1] {
		case "out_0":
			port = "jack_mixer:audacious L"
		case "out_1":
			port = "jack_mixer:audacious R"
		}
		return
	}
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
		case "alsa L":
			port = "alsa_in:capture_1"
			rev = true
		case "alsa R":
			port = "alsa_in:capture_2"
			rev = true
		case "gtklick":
			port = "gtklick:out"
			rev = true
		case "qsynth L":
			port = "qsynth:l_00"
			rev = true
		case "qsynth R":
			port = "qsynth:r_00"
			rev = true
		case "audacious L":
			port = "audacious-jack_*:out_0"
			rev = true
		case "audacious R":
			port = "audacious-jack_*:out_1"
			rev = true
		case "nekobee":
			port = "nekobee DSSI plugin:nekobee DSSI plugin out_1"
			rev = true
		case "M1 L":
			port = "M1:output_1"
			rev = true
		case "M1 R":
			port = "M1:output_2"
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
	case "M1":
		switch x[1] {
		case "output_1":
			port = "jack_mixer:M1 L"
		case "output_2":
			port = "jack_mixer:M1 R"
		}
	default:
		port = "jack_mixer:" + x[0]
	}
	return
}

func portlist(tport string) (ports []string) {
	connect_to, rev := connect_to(tport)
	ports = Client.GetPorts("", "", 0)
	if connect_to == "" {
		return
	}
	xx := strings.Split(connect_to, ":")
	for _, port := range ports {
		x := strings.Split(port, ":")
		y1 := false
		y2 := false
		if strings.HasSuffix(xx[0], "*") {
			y1 = strings.Index(x[0], strings.TrimSuffix(xx[0], "*")) == 0
		} else {
			y1 = x[0] == xx[0]
		}
		if strings.HasSuffix(xx[1], "*") {
			y2 = strings.Index(x[1], strings.TrimSuffix(xx[1], "*")) == 0
		} else {
			y2 = x[1] == xx[1]
		}
		if y1 && y2 {
			fmt.Println("match", tport, port)
			if rev {
				Queue.PushBack([]string{port, tport})
			} else {
				Queue.PushBack([]string{tport, port})
			}
		}
	}
	return
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
	Queue = list.New()
	ports := portlist("")
	for _, tport := range ports {
		portlist(tport)
	}
	client.SetPortRegistrationCallback(update)
	if code := client.Activate(); code != 0 {
		panic("activate error")
	}
	for {
		time.Sleep(time.Millisecond * 250)
		v := Queue.Front()
		if v != nil {
			Queue.Remove(v)
			x, _ := v.Value.([]string)
			Client.Connect(x[0], x[1])
		}
	}
}
