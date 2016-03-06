package main

/*
#cgo LDFLAGS: -lasound
#include <stdlib.h>
extern void set_volume(const char *card, const char *name, long volume);
extern int connect_midi(const char* sender_name, const char* dest_name);
*/
import "C"
import (
	"unsafe"
)

func set_volume(card, name string, vol int) {
	ccard := C.CString(card)
	defer C.free(unsafe.Pointer(ccard))
	cname := C.CString(name)
	defer C.free(unsafe.Pointer(cname))
	C.set_volume(ccard, cname, C.long(vol))
}

func connect_midi(sender, dest string) {
	csender := C.CString(sender)
	defer C.free(unsafe.Pointer(csender))
	cdest := C.CString(dest)
	defer C.free(unsafe.Pointer(cdest))
	C.connect_midi(csender, cdest)
}

func main() {
	set_volume("hw:1", "Master", 50)
	// set_volume("hw:0", "PCM", 0)
	// set_volume("default", "Headphone", 0)
	connect_midi("microKORG XL MIDI 2", "nekobee DSSI plugin")
}
