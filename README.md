# Fake Microphone Wav Player

![powered by Pulseaudio](https://www.freedesktop.org/software/pulseaudio/logo.png)

![Powered by Pulseaudio](icons/powered_by_pulseaudio.png)

![Travis CI build status](https://travis-ci.org/SCOTT-HAMILTON/FakeMicWavPlayer.svg?branch=master)

Fake is a pulseaudio client that can inject ogg files like a microphone to any recording app on linux.

## Meson Building 
  To build it, run : 
  ```sh
  meson setup build
  ninja -C build
  ninja -C install
  ```

## How does it work
 Fake creates a pipeline structure of virtual sink, virtual combined sinks and loopback module to perform
 its magical work. This structure allows separate volume control of the played audio file both to
 the recording app and to your personnal headphones/headsets/... It also allows you to be kept recorded
 while the audio is playing; you can still talk to your friends while streaming them music ! This structure
 is however not cpu costless and all those intermediates streaming devices could decrease your processing 
 performances a bit. Although this structure is not CPU-costless, most of the experienced performance drop
 originates from the recording app.

 - If the structure of the software interests you, here is a summarized view of it :
![Summarized Schema](icons/schema.png "schema")

## Usage
### The GUI Way
If you prefer using a GUI, you can use Control's for Fake GUI frontend. [Control's for Fake link](https://github.com/SCOTT-HAMILTON/ControlsForFake) .
	
### The hard way
```shell_session
Usage: FakeMicWavPlayer [options] <Ogg File> <Source> <Process Binary>

Positional arguments:
<Ogg File>              The ogg audio file to play.
<Source>                The source name to keep recording with
<Process Binary>        The binary name of the app to send the sound to.

Optional arguments:
-h --help               show this help message and exit
-s --sink-list          The comma-sepatated list of sinks to play the ogg file to.
```

  - The ogg file is an audio file in ogg vorbis format
  - The source is the name of the pulseaudio source to record. This options allows to keep recording your voice while playing the audio. To find out the name of your microphone, headset or other audio input devices, you can use `pactl list short sources`. The data you're interested in is the second column. In my case, running this command outputs : 
  > Output of `pactl list short sources` : 

  ```
  0       alsa_output.pci-0000_00_1f.3.analog-stereo.monitor      module-alsa-card.c      s16le 2ch 48000Hz       SUSPENDED
  1       alsa_input.pci-0000_00_1f.3.analog-stereo       module-alsa-card.c      s16le 2ch 44100Hz       SUSPENDED
  ```
  > So if I'd want to keep my voice recorded with the internal mic, I'd use :

  `./FakeMicWavPlayer MyMusic.ogg alsa_input.pci-0000_00_1f.3.analog-stereo MyRecordingAppBinaryName`

  - The process binary is the binary-filename of your recording app. In the case of discord, on most linux distributions you will set it to `Discord` (some exceptions exist like in NixOS where the binary wrapping changes the binary name, you would set it to `.Discord-wrapped`)
  - The optional pulseaudio sinks is a comma separated list of pulseaudio sinks that you want Fake to play in alongside with the virtual sink. This option should be used if you want to hear the wav file played. To know which values you can put, plug the device you want to listen on (if it's not an internal device) and run `pactl list short sinks`. The second column of the table contains the names of all your devices. You just have to copy them. Here is an example : 
  
   > The output of pactl is for me : 
  
  ```
  1	alsa_output.pci-0000_00_1f.3.analog-stereo	module-alsa-card.c	s32le 2ch 44100Hz	SUSPENDED
  9	alsa_output.usb-Sennheiser_Sennheiser_SC60_for_Lync_55ef416267-00.analog-stereo module-alsa-card.c	s16le 2ch 44100Hz	SUSPENDED
  ```
  
   > So if I wan't to hear the wav on my sennheiser headsets and on my internal speakers, I would run : 
  
  `./FakeMicWavPlayer -s alsa_output.pci-0000_00_1f.3.analog-stereo,alsa_output.usb-Sennheiser_Sennheiser_SC60_for_Lync_55ef416267-00.analog-stereo MyMusic.ogg <source> MyRecordingAppBinaryName `
  

## Requirements
Fake requires : 
 - libpulse  libpulse-simple: 
 - libvorbis libogg
 - [FakeLib](https://github.com/SCOTT-HAMILTON/FakeLib)
 - [argparse](https://github.com/p-ranav/argparse)

## Help
Fake is just a little project, but feel free to fork, change, extend or correct the code.

## Warning
**Fake Mic Wav Player unloads all module-null-sink, module-combined-sink and module-combine-sink in existence as a cleanup precaution. This shouldn't bother most of the users except those who hack with pulseaudio.**

## License
Fake is delivered as it is under the well known MIT License

**References that helped**
 - [pulseaudio irc] : <#pulseaudio>
 - [pulseaudio under the hood] : <https://gavv.github.io/articles/pulseaudio-under-the-hood/>
 - [pulseaudio documentation] : <https://freedesktop.org/software/pulseaudio/doxygen/>
 - [pulseaudio introspect api interface] :
   <https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html>

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)

   [pulseaudio irc]: <#pulseaudio>
   [pulseaudio under the hood]: <https://gavv.github.io/articles/pulseaudio-under-the-hood/>
   [pulseaudio documentation]: <https://freedesktop.org/software/pulseaudio/doxygen/>
   [pulseaudio introspect api interface]:
   <https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html>
