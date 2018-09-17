# MIDI FALSE

A programmable LV2 MIDI filter

## Install

```
make
make install
```

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
    <Value>Tx8*7-&lt;3N%R%C\7+\R</Value>
   </CustomData>
```

Restart Carla and load it.

## Examples

* [add fifth note](./examples/fifth.rb)
* [ignore note off](./examples/ignore_noteoff.rb)
* [velocity offset](./examples/vel_offset.rb)
* [channel offset](./examples/ch_offset.rb)

## TODO

* state
* storage
* timer event
* CC, sustain pedal, pitch bend, mod wheel, ...
* program change
