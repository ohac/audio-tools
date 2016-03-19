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
		case "audacious L":
			port = "audacious-jack_*:out_0"
			rev = true
		case "audacious R":
			port = "audacious-jack_*:out_1"
			rev = true
		}
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
			port := Client.GetPortByName(x[0])
			cons := port.GetConnections()
			found := false
			for _, c := range cons {
				if c == x[1] {
					found = true
				}
			}
			if !found {
				Client.Connect(x[0], x[1])
			}
		}
	}
}
