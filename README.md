# Fake Microphone Wav Player

![powered by Pulseaudio](https://www.freedesktop.org/software/pulseaudio/logo.png)

![Powered by Pulseaudio](icons/powered_by_pulseaudio.png)


![Travis CI build status](https://travis-ci.org/SCOTT-HAMILTON/FakeMicWavPlayer.svg?branch=master)

Fake is a pulseaudio client that can inject wav files like a microphone to any recording app on linux.

# Meson Building 
  To build it, run : 
  ```sh
  meson setup build
  ninja -C build
  ninja -C install
  ```

# How does it work
 - Fake creates a virtual combined sink. Pulseaudio sinks are basically just the devices in which you can output sound, it can be your USB headphones, you speakers or some virtual stuff. The combined sink forwards its audio to its slave sinks. Furthermore, we connect the recording application to the monitor of this combined sink. This combination feature of pulseaudio is used to play the wav file both in the recording application and in "real hardware" sinks so that you can also hear what is played.
 - Now fake can just play the wav file to the combined sink.
 - To summarize :
![Summarized Schema](icons/schema.png "schema")

# Usage
## The GUI Way
If you prefer using a GUI, you can use Control's for Fake GUI frontend. [Control's for Fake link](https://github.com/SCOTT-HAMILTON/ControlsForFake) .
	
## The hard way
```
Usage: FakeMicWavPlayer [options] wav file process binary

Positional arguments:
wav file        The wav audio file to play.
process binary  The binary name of the app to send the sound to.

Optional arguments:
-h --help       show this help message and exit
-s --sink-list  The comma-sepatated list of sinks to play the wav file to.
```

  - The wav file is an audio file in PCM_S16_LE (PCM 16 bit integers little-endian)
  - The process binary is the binary-filename of your recording app. In the case of discord, on most linux distributions you will set it to `Discord` (some exceptions exist like in NixOS where the binary wrapping changes the binary name, you would set it to `.Discord-wrapped`)
  - The optional pulseaudio sinks is a comma separated list of pulseaudio sinks that you want Fake to play in alongside with the virtual sink. This option should be used if you want to hear the wav file played. To know which values you can put, plug the device you want to listen on (if it's not an internal device) and run `pactl list short sinks`. The second column of the table contains the names of all your devices. You just have to copy them. Here is an example : 
  
   > The output of pactl is for me : 
  
  ```
  1	alsa_output.pci-0000_00_1f.3.analog-stereo	module-alsa-card.c	s32le 2ch 44100Hz	SUSPENDED
  9	alsa_output.usb-Sennheiser_Sennheiser_SC60_for_Lync_55ef416267-00.analog-stereo module-alsa-card.c	s16le 2ch 44100Hz	SUSPENDED
  ```
  
   > So if I wan't to hear the wav on my sennheiser headsets and on my internal speakers, I would run : 
  
  `./FakeMicWavPlayer -s alsa_output.pci-0000_00_1f.3.analog-stereo,alsa_output.usb-Sennheiser_Sennheiser_SC60_for_Lync_55ef416267-00.analog-stereo MyWav.wav MyRecordingAppBinaryName `
  

### Requirements

Fake requires : 
 - libpulse  libpulse-simple: 
 - [FakeLib](https://github.com/SCOTT-HAMILTON/FakeLib)
 - [argpase](https://github.com/p-ranav/argparse)


### Help

Fake is just a little project, but feel free to fork, change, extend or correct the code.


License
----
Fake is delivered as it is under the well known MIT License


**References that helped**
 - [pulseaudio IRC] : <#pulseaudio>
 - [pulseaudio under the hood] : <https://gavv.github.io/articles/pulseaudio-under-the-hood/>
 - [pulseaudio documentation] : <https://freedesktop.org/software/pulseaudio/doxygen/>
 - [pulseaudio introspect API interface] :
   <https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html>

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)



   [pulseaudio IRC]: <#pulseaudio>
   [pulseaudio under the hood]: <https://gavv.github.io/articles/pulseaudio-under-the-hood/>
   [pulseaudio documentation]: <https://freedesktop.org/software/pulseaudio/doxygen/>
   [pulseaudio introspect API interface]:
   <https://freedesktop.org/software/pulseaudio/doxygen/introspect_8h.html>
