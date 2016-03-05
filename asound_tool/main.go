package main

/*
#cgo LDFLAGS: -lasound
#include <stdlib.h>
extern void set_volume(const char *card, const char *name, long volume);
*/
import "C"
import (
	"fmt"
	"unsafe"
)

func set_volume(card, name string, vol int) {
	ccard := C.CString(card)
	defer C.free(unsafe.Pointer(ccard))
	cname := C.CString(name)
	defer C.free(unsafe.Pointer(cname))
	C.set_volume(ccard, cname, C.long(vol))
}

func main() {
	set_volume("hw:1", "Master", 0)
	// set_volume("hw:0", "PCM", 0)
	// set_volume("default", "Headphone", 0)
}
