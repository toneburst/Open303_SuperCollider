( // SynthDef
s.waitForBoot {
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
		var notetrigger = NamedControl.tr(\notetrigger); // Ensures 'noteevent' is only ever positive for 1 k-rate cycle!
		// Create output
		var result;
		result = Open303.ar(notetrigger, notenum, notevel, notealloff, waveform, cutoff, resonance, envmod, decay, accent, volume);
		// Output output
		Out.ar(out, result);
	}).add;
};
)

( // Start it

// Create bassline synth object
~bassline = Synth(\AcidBass);

// Start MIDI
MIDIClient.init;
MIDIIn.connectAll;

// MIDI functions
MIDIFunc.noteOn({ |vel, num|
	"SCLANG NOTEON % \n".postf(num);
	// Check if stack is empty
	~bassline.set(\notenum, num, \notevel, vel);
	~bassline.set(\notetrigger, 1.0);
});

MIDIFunc.noteOff({ |vel, num|
	"SCLANG NOTEOFF %\n".postf(num);
	// Remove note from the stack
	~bassline.set(\notenum, num, \notevel, 0.0);
	~bassline.set(\notetrigger, 1.0);
});

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\waveform, val / 127);
}, ccNum:21);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\cutoff, val / 127);
}, ccNum:22);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\resonance, val / 127);
}, ccNum:23);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\envmod, val / 127);
}, ccNum:24);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\decay, val / 127);
}, ccNum:25);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\accent, val / 127);
}, ccNum:26);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\volume, val / 127);
}, ccNum:27);

)

// Stop it
~bassline.free;
MIDIdef.freeAll;
