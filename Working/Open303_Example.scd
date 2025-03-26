//"https://doc.sccode.org/Guides/UsingMIDI.html"
MIDIClient.init;
MIDIIn.connectAll;

(
// Init bassline synth object
var bassline;
var m_on, m_off, m_waveform, m_cutoff, m_resonance, m_envmod, m_decay, m_accent, m_volume;

SynthDef.new("AcidBass", {
	arg out,
	notenum = 60.0,
	notevel = 64.0,
	notealloff = 0.0,
	waveform = 0.85,
	cutoff = 0.229,
	resonance = 0.5,
	envmod = 0.25,
	decay = 0.5,
	accent = 0.5,
	volume = 0.9;
	// Declare vars
	var noteevent = NamedControl.tr(\noteevent); // Ensures 'noteevent' is only ever positive for 1 k-rate cycle!
	// Create output
	var result;
    result = Open303.ar(noteevent, notenum, notevel, notealloff, waveform, cutoff, resonance, envmod, decay, accent, volume);
	// Output output
	Out.ar(out, result);
}).add;

// Instantiate Open303 synth
bassline = Synth("AcidBass");
bassline.set(\noteevent, 0.0);

// MIDI functions
m_on = MIDIFunc.noteOn({ |vel, num, chan, src|
	"SCLANG NOTEON % \n".postf(num);
	bassline.set(\notenum, num);
	bassline.set(\notevel, vel);
	bassline.set(\noteevent, 1.0);
});

m_off = MIDIFunc.noteOff({ |vel, num, chan, src|
	"SCLANG NOTEOFF %\n".postf(num);
	bassline.set(\notenum, num);
	bassline.set(\notevel, 0.0);
	bassline.set(\noteevent, 1.0);
});

m_waveform = MIDIFunc.cc({ |val, ccNum, chan, src|
    bassline.set(\waveform, val / 127);
}, ccNum:21);

m_cutoff = MIDIFunc.cc({ |val, ccNum, chan, src|
	bassline.set("cutoff", val / 127);
}, ccNum:22);

m_resonance = MIDIFunc.cc({ |val, ccNum, chan, src|
    bassline.set(\resonance, val / 127);
}, ccNum:23);

m_envmod = MIDIFunc.cc({ |val, ccNum, chan, src|
    bassline.set(\envmod, val / 127);
}, ccNum:24);

m_decay = MIDIFunc.cc({ |val, ccNum, chan, src|
    bassline.set(\decay, val / 127);
}, ccNum:25);

m_accent = MIDIFunc.cc({ |val, ccNum, chan, src|
    bassline.set(\accent, val / 127);
}, ccNum:26);

m_volume = MIDIFunc.cc({ |val, ccNum, chan, src|
    bassline.set(\volume, val / 127);
}, ccNum:27);

)

bassline.free;
s.free;
