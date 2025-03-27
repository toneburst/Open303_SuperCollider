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
		var notetrigger = NamedControl.tr(\notetrigger) * notetype; // Ensures 'noteevent' is only ever positive for 1 k-rate cycle!
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
// Note-stack
~noteStack = [];

// Start MIDI
MIDIClient.init;
MIDIIn.connectAll;

// MIDI functions
MIDIFunc.noteOn({ |vel, num|
	"SCLANG NOTEON % \n".postf(num);
	// Check if stack is empty
	if(~noteStack.size > 0) {
		// Stack not empty. This is a legato note
		// Set noteevent to 3 to tell the plugin we need to slide to the incoming note
		~bassline.set(\notetype, 3.0, \notenum, num, \notevel, vel, \notetrigger, 1.0);
	} {
		// Stack is empty. This is a non-legato note
		// Set noteevent to 2 to play the incoming note
		~bassline.set(\notetype, 2.0, \notenum, num, \notevel, vel, \notetrigger, 1.0);
	};
	// Push note onto stack
	~noteStack = ~noteStack.add(num);
});

MIDIFunc.noteOff({ |vel, num|
	"SCLANG NOTEOFF %\n".postf(num);
	// Remove note from the stack
	~noteStack.remove(num);
	//
	if(~noteStack.size > 0) {
		// Note stack is not empty. We need to slide back to a previous note
		~bassline.set(\notetype, 3.0, \notenum, ~noteStack.last, \notevel, 0.0, \notetrigger, 1.0);
	} {
		// Note stack is empty. Non-legato release
		~bassline.set(\notetype, 2.0, \notenum, num, \notevel, 0.0, \notetrigger, 1.0);
	};
});

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set(\waveform, val / 127);
}, ccNum:21);

MIDIFunc.cc({ |val, ccNum, chan, src|
	~bassline.set("cutoff", val / 127);
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
