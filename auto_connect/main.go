package main

import (
	"fmt"
	"github.com/xthexder/go-jack"
	"strings"
	"time"
)

var Client *jack.Client
var Pair []string

func list(tport string) {
	tport_prefix := strings.Split(tport, ":")[0]
	search := "jack_mixer:" + tport_prefix
	ports := Client.GetPorts("", "", 0)
	for _, port := range ports {
		if port == search {
			fmt.Println("match", tport, port)
			Pair[0] = tport
			Pair[1] = port
		}
	}
}

func update(id jack.PortId, st bool) {
	if st {
		port := Client.GetPortById(id)
		list(port.String())
	}
}

func main() {
	client, _ := jack.ClientOpen("autoconnect", jack.NoStartServer)
	if client == nil {
		panic("connect error")
	}
	defer client.Close()
	Client = client
	list("")
	client.SetPortRegistrationCallback(update)
	if code := client.Activate(); code != 0 {
		panic("activate error")
	}
	Pair = make([]string, 2)
	for {
		time.Sleep(time.Second * 1)
		if Pair[0] != "" {
			fmt.Println("get", Pair[0], Pair[1])
			Client.Connect(Pair[0], Pair[1])
			Pair[0] = ""
		}
	}
}
