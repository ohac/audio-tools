# MIDI FALSE

A programmable LV2 MIDI filter

## Install

```
make
make install
```

## Usage (for jalv)

```
jalv-gtk http://lv2.sighash.info/midifalse
```

Input script and parameters then hit enter key.

## Usage (for Carla)

Refresh LV2

```
carla-database
```

Add Plugin. Search FALSE and Add it.

```
carla
```

Save Carla state to file. (e.g. `~/carla_state.carxp`)

```
vi ~/carla_state.carxp
```

Edit Value in CustomData.

```
   <CustomData>
    <Type>http://lv2plug.in/ns/ext/atom#String</Type>
    <Key>http://lv2.sighash.info/midifalse#onoff</Key>
    <Value>Tx8*7-&lt;3N%R%C\7+\</Value>
   </CustomData>
```

Restart Carla and load it.

## Script examples

* [add fifth note](./examples/fifth.rb)
* [ignore note off](./examples/ignore_noteoff.rb)
* [velocity offset](./examples/vel_offset.rb)
* [channel offset](./examples/ch_offset.rb)
* [drum pad](./examples/drum_pad.rb)

## TODO

* state
* storage
* timer event
* CC, sustain pedal, pitch bend, mod wheel, ...
* program change
